#ifndef _LSD_LEVEL_SET_METHOD_H_
#define _LSD_LEVEL_SET_METHOD_H_

#include "LockedQueue.h"
#include <Core/Thread.h>
#include <Math/Vector.h>
#include <Core/EngineEvents.h>

#include "Tex.h"

#include "EmptyTextureResource.h"

using namespace OpenEngine::Core;
using namespace std;


struct Point {
    int dx, dy;

    int DistSq() const { return dx*dx + dy*dy; }
};

class Grid {
    
    unsigned int width;
    unsigned int height;
    
    Point *grid;//[HEIGHT][WIDTH];

public:
    Grid(unsigned int w, 
         unsigned int h) : width(w), height(h) {
        grid = new Point[width*height];
    }
    Point Get(unsigned int x,
               unsigned int y) {
        return grid[y*width+x];
    }
    void Put(unsigned int x,
             unsigned int y,
             Point v) {
        grid[y*width+x] = v;
    }

    Point& operator()(unsigned int x,
                      unsigned int y) {
        return grid[y*width+x];
    }
};


class LevelSetMethod : public Thread, public IListener<ProcessEventArg> {

    ITextureResourcePtr inputTex;


    unsigned int width;
    unsigned int height;

    int dx, dy;

    Tex<float> phi;
    Tex<float> phiT;

    Tex<Vector<2,float> > vf;
    Tex<Vector<2,float> > grad;
    


    EmptyTextureResourcePtr sdfTex,vfTex,gradTex,phiTTex;


    void BuildSDF();
    void BuildVF();
    void BuildGradient();

    void ProcessImage();
public:

    LevelSetMethod(ITextureResourcePtr inputTex);

    void Handle(ProcessEventArg arg);

    bool run;

    virtual void Run();

    EmptyTextureResourcePtr GetDFSTexture() {return sdfTex;}
    EmptyTextureResourcePtr GetVFTexture() {return vfTex;}
    EmptyTextureResourcePtr GetGradientTexture() {return gradTex;}
    EmptyTextureResourcePtr GetPhiTTexture() {return phiTTex;}

    static void SDFToTexture(Tex<float> p, EmptyTextureResourcePtr t);
    
    float GetValue(unsigned int i, unsigned int j);        
    Vector<2, float> Godunov(unsigned int i, unsigned int j, float a);
    Vector<2, float> Gradient(unsigned int i, unsigned j);

    Tex<float> GetPhi() {return phi;}

    Tex<float> Union(Tex<float> sdf1, Tex<float> sdf2);
	Tex<float> Intersection(Tex<float> sdf1, Tex<float> sdf2);
	Tex<float> Subtract(Tex<float> sdf1, Tex<float> sdf2);


};

#endif
