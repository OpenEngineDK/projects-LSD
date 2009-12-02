// main
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------



// OpenEngine stuff
#include <Meta/Config.h>
#include <Logging/Logger.h>
#include <Logging/StreamLogger.h>
#include <Core/Engine.h>

// SimpleSetup
#include <Utils/SimpleSetup.h>

#include <Resources/ResourceManager.h>
#include <Resources/DirectoryManager.h>
#include <Resources/ITextureResource.h>
#include <Resources/CairoResource.h>

#include <Renderers/TextureLoader.h>
#include <Geometry/FaceSet.h>
#include <Scene/GeometryNode.h>
#include <Scene/TransformationNode.h>
#include <Scene/SceneNode.h>
#include <Math/Vector.h>
#include <Math/Exceptions.h>


#include <Utils/CairoTextTool.h>
#include <Display/SDLEnvironment.h>

#include "LockedQueue.h"
#include "EmptyTextureResource.h"
#include "LevelSetMethod.h"

#include "Producer.h"
#include "Consumer.h"

// name spaces that we will be using.
// this combined with the above imports is almost the same as
// fx. import OpenEngine.Logging.*; in Java.
using namespace OpenEngine;
using namespace OpenEngine::Logging;
using namespace OpenEngine::Core;
using namespace OpenEngine::Utils;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Scene;
using namespace OpenEngine::Geometry;
using namespace OpenEngine::Math;
using namespace OpenEngine::Renderers;
using namespace OpenEngine::Renderers::OpenGL;
using namespace OpenEngine::Display;


static TransformationNode* CreateTextureBillboard(ITextureResourcePtr texture,
                                                  float scale) {
    unsigned int textureHosisontalSize = texture->GetWidth();
    unsigned int textureVerticalSize = texture->GetHeight();

    logger.info << "w x h = " << texture->GetWidth()
                << " x " << texture->GetHeight() << logger.end;
    float fullxtexcoord = 1;
    float fullytexcoord = 1;
  
    FaceSet* faces = new FaceSet();

    float horisontalhalfsize = textureHosisontalSize * 0.5;
    Vector<3,float>* lowerleft = new Vector<3,float>(horisontalhalfsize,0,0);
    Vector<3,float>* lowerright = new Vector<3,float>(-horisontalhalfsize,0,0);
    Vector<3,float>* upperleft = new Vector<3,float>(horisontalhalfsize,textureVerticalSize,0);
    Vector<3,float>* upperright = new Vector<3,float>(-horisontalhalfsize,textureVerticalSize,0);

    FacePtr leftside = FacePtr(new Face(*lowerleft,*lowerright,*upperleft));

    /*
      leftside->texc[1] = Vector<2,float>(1,0);
      leftside->texc[0] = Vector<2,float>(0,0);
      leftside->texc[2] = Vector<2,float>(0,1);
    */
    leftside->texc[1] = Vector<2,float>(0,fullytexcoord);
    leftside->texc[0] = Vector<2,float>(fullxtexcoord,fullytexcoord);
    leftside->texc[2] = Vector<2,float>(fullxtexcoord,0);
    leftside->norm[0] = leftside->norm[1] = leftside->norm[2] = Vector<3,float>(0,0,1);
    leftside->CalcHardNorm();
    leftside->Scale(scale);
    faces->Add(leftside);

    FacePtr rightside = FacePtr(new Face(*lowerright,*upperright,*upperleft));
    /*
      rightside->texc[2] = Vector<2,float>(0,1);
      rightside->texc[1] = Vector<2,float>(1,1);
      rightside->texc[0] = Vector<2,float>(1,0);
    */
    rightside->texc[2] = Vector<2,float>(fullxtexcoord,0);
    rightside->texc[1] = Vector<2,float>(0,0);
    rightside->texc[0] = Vector<2,float>(0,fullytexcoord);
    rightside->norm[0] = rightside->norm[1] = rightside->norm[2] = Vector<3,float>(0,0,1);
    rightside->CalcHardNorm();
    rightside->Scale(scale);
    faces->Add(rightside);

    MaterialPtr m = leftside->mat = rightside->mat = MaterialPtr(new Material());
    m->texr = texture;

    GeometryNode* node = new GeometryNode();
    node->SetFaceSet(faces);
    TransformationNode* tnode = new TransformationNode();
    tnode->AddNode(node);
    return tnode;
}


struct Wall {
    pair<ITextureResourcePtr,string> tex[9];
    TextureLoader& loader;

    Wall(TextureLoader& l) : loader(l) {        
    }

    pair<ITextureResourcePtr,string>& operator()(int x, int y) {
        return tex[x*3+y];
    }
    ISceneNode* MakeScene() {
        SceneNode *sn = new SceneNode();
        CairoTextTool textTool;
        
        for (int x=0;x<3;x++) {
            for (int y=0;y<3;y++) {
                pair<ITextureResourcePtr,string> itm = (*this)(x,y);
                ITextureResourcePtr t = itm.first;
                if (t) {
                    loader.Load(t,TextureLoader::RELOAD_QUEUED);
                    TransformationNode* node = CreateTextureBillboard(t,0.05);
                    node->SetScale(Vector<3,float>(1.0,-1.0,1.0));
                    node->Move(x*35-35,y*25-25,0);

                    CairoResourcePtr textRes = CairoResource::Create(128,32);
                    textRes->Load();

                    ostringstream out;
                    out << "(" << x << "," << y << ") " << itm.second;

                    textTool.DrawText(out.str(), textRes);

                    loader.Load(textRes);
                    TransformationNode* textNode = CreateTextureBillboard(textRes,0.1);
                    textNode->SetScale(Vector<3,float>(1.0,-1.0,1.0));                    
                    textNode->Move(0,22.0,0);


                    node->AddNode(textNode);
                    //sn->AddNode(textNode);
                    sn->AddNode(node);
                }
            }
        }

        return sn;
    }
};


/**
 * Main method for the first quarter project of CGD.
 * Corresponds to the
 *   public static void main(String args[])
 * method in Java.
 */

int main(int argc, char** argv) {
    // Create simple setup
    

    SDLEnvironment* env = new SDLEnvironment(1024,768);
    

    SimpleSetup* setup = new SimpleSetup("LevelSet Method",NULL,env);
 

    setup->GetRenderer().SetBackgroundColor(Vector<4,float>(0.0,0.0,0.0,1.0));

    ISceneNode* rootNode = setup->GetScene();

    DirectoryManager::AppendPath("./projects/LSD/data/");
    ITextureResourcePtr auLogo = ResourceManager<ITextureResource>::Create("au-logo.png");
    ITextureResourcePtr circle = ResourceManager<ITextureResource>::Create("circle.png");

    circle->Load();
    auLogo->Load();

    LevelSetMethod& method = *(new LevelSetMethod(circle,auLogo));
    setup->GetEngine().ProcessEvent().Attach(method);


    Wall wall(setup->GetTextureLoader());
    
    wall(0,0) = make_pair<>(auLogo,"AU Logo");
    wall(2,0) = make_pair<>(circle,"Circle");

    wall(0,1) = make_pair<>(method.GetDFSTexture(),"Phi");
    wall(1,1) = make_pair<>(method.GetPhiTTexture(),"Phi_0");

    wall(0,2) = make_pair<>(method.GetTestTexture(),"Test");
    wall(1,2) = make_pair<>(method.GetGradientTexture(),"Gradient");

    wall(2,2) = make_pair<>(method.GetVFTexture(),"VF");

    setup->SetScene((*wall.MakeScene()));

    float h = -25/2;

    setup->GetCamera()->SetPosition(Vector<3,float>(0.0,h,100));
    setup->GetCamera()->LookAt(Vector<3,float>(0.0,h,0.0));


    method.Start();

    // Start the engine.
    setup->GetEngine().Start();

    method.run  = false;
    
    method.Wait();

    // Return when the engine stops.
    return EXIT_SUCCESS;
}


