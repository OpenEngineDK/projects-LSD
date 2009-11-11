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
#include <Renderers/TextureLoader.h>
#include <Geometry/FaceSet.h>
#include <Scene/GeometryNode.h>
#include <Scene/TransformationNode.h>
#include <Math/Vector.h>

#include "LockedQueue.h"

#include "Producer.h"

// name spaces that we will be using.
// this combined with the above imports is almost the same as
// fx. import OpenEngine.Logging.*; in Java.
using namespace OpenEngine::Logging;
using namespace OpenEngine::Core;
using namespace OpenEngine::Utils;
using namespace OpenEngine::Resources;
using namespace OpenEngine::Scene;
using namespace OpenEngine::Geometry;
using namespace OpenEngine::Math;

/**
 * Main method for the first quarter project of CGD.
 * Corresponds to the
 *   public static void main(String args[])
 * method in Java.
 */

   static TransformationNode* CreateTextureBillboard(ITextureResourcePtr texture, float scale) {
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



int main(int argc, char** argv) {
    // Create simple setup
    SimpleSetup* setup = new SimpleSetup("Example Project Title");
 

    setup->GetRenderer().SetBackgroundColor(Vector<4,float>(0.0,0.0,0.0,1.0));

    ISceneNode* rootNode = setup->GetScene();

    DirectoryManager::AppendPath("./projects/LSD/data/");
    ITextureResourcePtr image = ResourceManager<ITextureResource>::Create("au-logo.png");
    image->Load();
    setup->GetTextureLoader().Load(image);

    TransformationNode* imageNode = CreateTextureBillboard(image,0.1);
    imageNode->SetScale(Vector<3,float>(1.0,-1.0,1.0));
    rootNode->AddNode(imageNode);

    setup->GetCamera()->SetPosition(Vector<3,float>(0.0,-20.0,77.99375739));
    setup->GetCamera()->LookAt(Vector<3,float>(0.0,-20.0,0.0));

 


    //LockedQueue<float> q;// = new LockedQueue<float>();
    LockedQueue<float> *q = new LockedQueue<float>();
    //q->Put(42.0);

    Producer p(q);


    p.Start();

    // Start the engine.
    setup->GetEngine().Start();

    p.run = false;
    p.Wait();

    // Return when the engine stops.
    return EXIT_SUCCESS;
}


