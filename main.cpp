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
#include <Math/Exceptions.h>

#include "LockedQueue.h"
#include "EmptyTextureResource.h"

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

/**
 * Main method for the first quarter project of CGD.
 * Corresponds to the
 *   public static void main(String args[])
 * method in Java.
 */

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

// @todo template parameter specifing number of dimensions
template <typename T>
class Tex {
private:
    unsigned int width, height;
    T* data;
public:
    Tex(unsigned int width, unsigned int height)
      :  width(width), height(height) {
      data = new T[height*width];
  }

  T* operator[](const unsigned int iy) {
      return data+ iy*width;
  }

  T& operator()(const unsigned int ix, const unsigned int iy) {
      
      return data[ix+iy*width];
  }

  // look up pixel by interpolation
  T operator()(const float fx, const float fy) {
#if OE_SAFE
    if (fx < 0) throw Math::IndexOutOfBounds(floor(fx),0,(unsigned int)-1);
    if (fy < 0) throw Math::IndexOutOfBounds(floor(fy),0,(unsigned int)-1);
#endif

    unsigned int ix_below = floor(fx);
    unsigned int ix_above = ceil(fx);
    unsigned int iy_below = floor(fy);
    unsigned int iy_above = ceil(fy);

    float wx_below =  fx - ix_below;
    float wx_above =  1 - wx_below;

    float wy_below =  fy - iy_below;
    float wy_above =  1 - wy_below;

    float dy_above = 
      (*this)(ix_above,iy_above) * wx_above +
      (*this)(ix_below,iy_above) * wx_below;

    float dy_below = 
      (*this)(ix_above,iy_below) * wx_above +
      (*this)(ix_below,iy_below) * wx_below;

    T v = dy_above * wy_above + dy_below * wy_below;
    return v;
  }
};

#define HEIGHT 400
#define WIDTH 640


struct Point
{
    int dx, dy;

    int DistSq() const { return dx*dx + dy*dy; }
};

struct Grid
{
    Point grid[HEIGHT][WIDTH];
};

Point Get(Grid &g, int x, int y) {
	Point* other = new Point();
	other->dx = g.grid[y][x].dx;
	other->dy = g.grid[y][x].dy;
	return *other;
}

void Put(Grid &g, int x, int y, Point p) {
	g.grid[y][x] = p;
}


void Compare( Grid &g, Point &p, int x, int y, int offsetx, int offsety )
{
	Point other = Get(g,x+offsetx,y+offsety);
    other.dx += offsetx;
    other.dy += offsety;

	if (other.DistSq() < p.DistSq()) {
        p = other;
	}
}


void GenerateSDF( Grid &g )
{
    // Pass 0
    for (int y=0;y<HEIGHT;y++)
    {
        for (int x=0;x<WIDTH;x++)
        {
            Point p = Get( g, x, y );
			if(x>0)
				Compare( g, p, x, y, -1,  0 );
			if(y>0)
				Compare( g, p, x, y,  0, -1 );
			if(x>0 && y>0)
				Compare( g, p, x, y, -1, -1 );
			if(x<WIDTH-1 && y>0)
				Compare( g, p, x, y,  1, -1 );
            Put( g, x, y, p );
        }

        for (int x=WIDTH-1;x>=0;x--)
        {
            Point p = Get( g, x, y );
			if(x<WIDTH-1)
				Compare( g, p, x, y, 1, 0 );
            Put( g, x, y, p );
        }
    }

    // Pass 1
    for (int y=HEIGHT-1;y>=0;y--)
    {
        for (int x=WIDTH-1;x>=0;x--)
        {
            Point p = Get( g, x, y );
			if(x<WIDTH-1)
				Compare( g, p, x, y,  1,  0 );
			if(y<HEIGHT-1)
				Compare( g, p, x, y,  0,  1 );
			if(x>0 && y<HEIGHT-1)
				Compare( g, p, x, y, -1,  1 );
			if(x<WIDTH-1 && y<HEIGHT-1)
				Compare( g, p, x, y,  1,  1 );
            Put( g, x, y, p );
        }

        for (int x=0;x<WIDTH;x++)
        {
            Point p = Get( g, x, y );
			if(x>0)
				Compare( g, p, x, y, -1, 0 );
            Put( g, x, y, p );
        }
    }
}



ITextureResourcePtr processImage(ITextureResourcePtr tex,
                                 EmptyTextureResourcePtr recv,
                                 EmptyTextureResourcePtr gradTex) {
    // load initial data fields
    const unsigned int Y = tex->GetHeight();
    const unsigned int X = tex->GetWidth();
    const unsigned char* bw = tex->GetData();
    const unsigned int depth = tex->GetDepth()/8;

    Tex<float> t(X,Y);
    float pixel = t(10.11f,19.28f);

    // hacking teh PHI!
    Grid* grid = new Grid();
    Grid* gridInner = new Grid();
    Grid* gridOuter = new Grid();

    for (int y=0; y<HEIGHT; y++) {
		for (int x=0; x<WIDTH; x++) {
            unsigned int gray = 0;
            for (unsigned int i=0;i<depth;i++)
                gray += bw[y*X*depth+x*depth+i]; // dummy copy

            gray = (gray > 256)?255:0;

            if(!gray){
				gridInner->grid[y][x].dx = 0;
				gridInner->grid[y][x].dy = 0;
				gridOuter->grid[y][x].dx = 10000;
				gridOuter->grid[y][x].dy = 10000;
			} else {
				gridInner->grid[y][x].dx = 10000;
				gridInner->grid[y][x].dy = 10000;
				gridOuter->grid[y][x].dx = 0;
				gridOuter->grid[y][x].dy = 0;
			}
		}
	}

    GenerateSDF(*gridInner);
	GenerateSDF(*gridOuter);

    int result[HEIGHT][WIDTH];

    //unsigned char buffer[WIDTH*HEIGHT];
    //cout << "bah" << endl;

    Tex<float> phi(X,Y);

	int min = WIDTH*HEIGHT+1;
	int max = -(WIDTH*HEIGHT+1);

	int dist1 = 0, dist2 = 0, dist = 0;
	for (int y=0; y<Y; y++) {
		for (int x=0; x<X; x++) {
			dist1 = (int)( sqrt( (double)Get( *gridInner, x, y ).DistSq() ) );
            dist2 = (int)( sqrt( (double)Get( *gridOuter, x, y ).DistSq() ) );
            dist = -dist2 + dist1;
			
			result[y][x] = dist;

			if(min > dist)
				min = dist;
			if(max < dist)
				max = dist;
			
			//cout << dist;// << " and " << (dist / (sqrt(200.0)))*256 << " - ";
		}
		//cout << endl;
	}

    logger.info << "MAX = " << max << logger.end;
    logger.info << "MIN = " << min << logger.end;

	for (unsigned int y=0; y<Y; y++) {
		for (unsigned int x=0; x<X; x++) {
			//buffer[y*WIDTH + x] = (char)(((float)result[y][x] / (float)max)*256.0f);
            phi(x,y) = (char)(((float)result[y][x] / (float)max)*256.0f);
            (*recv)(x,y,0) = phi(x,y);

            

            //cout << phi(x,y);
			//cout << (char)(((float)result[y][x] / (float)max)*256.0f) << " ";
		}
		//cout << endl;
	}


    // // make signed distence field phi from bw image (tex)
    // 
    // for (unsigned int x=0; x<X; x++)
    //     for (unsigned int y=0; y<Y; y++) {
    //         unsigned int gray = 0;
    //         //unsigned int i = 3;
    //         for (unsigned int i=0;i<depth;i++)
    //             gray += bw[y*X*depth+x*depth+i]; // dummy copy
          
    //         gray = (gray > 256)?255:0;
          
    //         phi(x,y) = gray;

    //         (*recv)(x,y,0) = phi(x,y);
          
    //         //out[y*X+x] = phi(x,y);
    //     }

    // make vector field V
    Vector<2,float> gradient[X][Y];
    float dx = 1;
    float dy = 1;
    float cdX, cdY;
    for (unsigned int x=0; x<X; x++)
        for (unsigned int y=0; y<Y; y++) {
      
            //lower left corner
            if (x == 0 && y == 0) {
                cdX = (phi(x, y) + phi(x+1, y)) / dx;
                cdY = (phi(x, y) + phi(x, y+1)) / dy;

            } 
            //upper right corner
            else if (x == X - 1 && y == 0) {
                cdX = (phi(x, y) + phi(x-1, y)) / dx;
                cdY = (phi(x, y) + phi(x, y+1)) / dy;

            }
            //upper left corner
            else if (x == 0 && y == Y - 1) {
                cdX = (phi(x, y) + phi(x+1, y)) / dx;
                cdY = (phi(x, y) + phi(x, y-1)) / dy;

            }      
            //lower right corner
            else if (x == X - 1 && y == Y - 1) {
                cdX = (phi(x, y) + phi(x-1, y)) / dx;
                cdY = (phi(x, y) + phi(x, y-1)) / dy;

            }

            // upper border
            else if (y == 0 && (x > 0 && x < X - 1)) {
                cdX = (phi(x-1, y) + phi(x+1, y)) / 2 * dx;
                cdY = (phi(x, y) + phi(x, y+1)) / dy;

            }       
            // lower border
            else if (y == Y - 1 && (x > 0 && x < X - 1)) {
                cdX = (phi(x-1, y) + phi(x+1, y)) / 2 * dx;
                cdY = (phi(x, y) + phi(x, y-1)) / dy;

            }
            // left border
            else if (x == 0 && (y > 0 && y < Y - 1)) {
                cdX = (phi(x, y) + phi(x+1, y)) / dx;
                cdY = (phi(x, y-1) + phi(x, y+1)) / 2 * dy;

            }
            // right border
            else if (x == X - 1 && (y > 0 && y < Y - 1)) {
                cdX = (phi(x, y) + phi(x-1, y)) / dx;
                cdY = (phi(x, y-1) + phi(x, y+1)) / 2 * dy;

            }
            // Normal case
            else {
	
                // central differences
                cdX = (phi(x-1, y) + phi(x+1, y)) / 2 * dx;
                cdY = (phi(x, y-1) + phi(x, y+1)) / 2 * dy;
            }

            gradient[x][y] = Vector<2, float>(cdX, cdY);

            (*gradTex)(x,y,0) = 0;//gradient[x][y][0];
            (*gradTex)(x,y,1) = 0;//gradient[x][y][1];
            (*gradTex)(x,y,2) = gradient[x][y].GetLength();
            //logger.info << "x=" << x << " y=" << y << logger.end;
        }

    // solve the equations
    // unsigned char phi_plus[X][Y];
    // for (unsigned int i=0; i<1; i++) {
    //     for (unsigned int x=0; x<X; x++)
    //         for (unsigned int y=0; y<Y; y++)             
    //             phi_plus[x][y] = 1;//phi[x+(int)(v[x][y][0])][y+(int)(v[x][y][1])];
        
    //     // swap
    //     //unsigned char** temp = phi_plus;
    //     //phi_plus = phi;
    //     //phi = temp;
    // }
    return tex;
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

    logger.info << "Image Depth = " << image->GetDepth() << logger.end;
    logger.info << "Image Width = " << image->GetWidth() << logger.end;
    logger.info << "Image Height = " << image->GetHeight() << logger.end;


    EmptyTextureResourcePtr empty =
        EmptyTextureResource::Create(image->GetWidth(),
                                     image->GetHeight(),
                                     8);
    empty->Load();
    setup->GetTextureLoader().Load(empty);

    EmptyTextureResourcePtr empty2 =
        EmptyTextureResource::Create(image->GetWidth(),
                                     image->GetHeight(),
                                     24);
    empty2->Load();
    setup->GetTextureLoader().Load(empty2);

    

    processImage(image,empty,empty2);


    TransformationNode* imageNode = CreateTextureBillboard(image,0.1);
    imageNode->SetScale(Vector<3,float>(1.0,-1.0,1.0));
    imageNode->Move(35,-25,0);
    rootNode->AddNode(imageNode);

    TransformationNode* emptyNode = CreateTextureBillboard(empty,0.1);
    emptyNode->SetScale(Vector<3,float>(1.0,-1.0,1.0));
    emptyNode->Move(-35,-25,0);
    rootNode->AddNode(emptyNode);

    TransformationNode* emptyNode2 = CreateTextureBillboard(empty2,0.1);
    emptyNode2->SetScale(Vector<3,float>(1.0,-1.0,1.0));
    emptyNode2->Move(-35,25,0);
    rootNode->AddNode(emptyNode2);


    setup->GetCamera()->SetPosition(Vector<3,float>(0.0,-20.0,140));
    setup->GetCamera()->LookAt(Vector<3,float>(0.0,-20.0,0.0));


    //LockedQueue<float> q;// = new LockedQueue<float>();
    LockedQueue<float> *q = new LockedQueue<float>();
    //q->Put(42.0);

    Producer p(q);
    Consumer c(q);

    p.Start();
    c.Start();

    // Start the engine.
    setup->GetEngine().Start();

    p.run = c.run = false;
    
    p.Wait();
    c.Wait();

    // Return when the engine stops.
    return EXIT_SUCCESS;
}


