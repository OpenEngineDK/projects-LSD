#ifndef _LSD_LEVEL_SET_METHOD_H_
#define _LSD_LEVEL_SET_METHOD_H_

#include "LockedQueue.h"
#include <Core/Thread.h>
#include "Tex.h"

#include "EmptyTextureResource.h"

using namespace OpenEngine::Core;


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


class LevelSetMethod : public Thread {

    

    
    //LockedQueue<EmptyTextureResourcePtr>* texQueue;

    ITextureResourcePtr inputTex;



    unsigned int width;
    unsigned int height;

    Tex<float> phi;

    EmptyTextureResourcePtr sdfTex;


    void BuildSDF();
public:

    LevelSetMethod(ITextureResourcePtr inputTex);



    bool run;

    virtual void Run();

    EmptyTextureResourcePtr GetDFSTexture() {return sdfTex;}

    Tex<float> GetPhi() {return phi;}

};

#endif
