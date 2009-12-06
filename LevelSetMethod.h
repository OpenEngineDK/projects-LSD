#ifndef _LSD_LEVEL_SET_METHOD_H_
#define _LSD_LEVEL_SET_METHOD_H_

#include "LockedQueue.h"
#include <Core/Thread.h>
#include <Math/Vector.h>
#include <Core/EngineEvents.h>

#include "Tex.h"

#include "SDF.h"

#include "EmptyTextureResource.h"

using namespace OpenEngine::Core;
using namespace std;


class LevelSetMethod : public Thread, public IListener<ProcessEventArg> {

    SDF* sdf1;
    SDF* sdf2;
    SDF* testSDF;

    ITextureResourcePtr inputTex;
    ITextureResourcePtr inputTex2;


    unsigned int width;
    unsigned int height;

    int dx, dy;

    LockedQueue<EmptyTextureResourcePtr> updateQueue;



    void ProcessImage();

public:

    LevelSetMethod(ITextureResourcePtr inputTex,
                   ITextureResourcePtr inputTex2);

    SDF* GetSDF1() {return sdf1;}
    SDF* GetSDF2() {return sdf2;}
    SDF* GetTestSDF() {return testSDF;}

    void Handle(ProcessEventArg arg);

    bool run;

    virtual void Run();


    
    float GetValue(unsigned int i, unsigned int j);        
    Vector<2, float> Godunov(unsigned int i, unsigned int j, float a);

    SDF* Union(SDF* sdf1, SDF* sdf2);
	SDF* Intersection(SDF* sdf1, SDF* sdf2);
	SDF* Subtract(SDF* sdf1, SDF* sdf2);


};

#endif
