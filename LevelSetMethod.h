#ifndef _LSD_LEVEL_SET_METHOD_H_
#define _LSD_LEVEL_SET_METHOD_H_

#include <Core/LockedQueue.h>
#include <Core/Thread.h>
#include <Math/Vector.h>
#include <Core/EngineEvents.h>

#include <Resources/Tex.h>

#include <LevelSet/SDF.h>

#include <Resources/EmptyTextureResource.h>

using namespace OpenEngine::Core;
using namespace std;
using namespace OpenEngine::LevelSet;

class LevelSetMethod : public Thread, public IListener<ProcessEventArg> {

    Strategy* strategy;

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
    
    void VFExpand(float a, SDF* sdf);
    void VFShrink(float a, SDF* sdf);
    void VFMeanCurvature(float a, SDF* sdf);
    void VFMorph(float a, SDF* sdf, SDF* sdf2);
    

public:

    LevelSetMethod(ITextureResourcePtr inputTex,
                   ITextureResourcePtr inputTex2,
                   Strategy* s = NULL);

    SDF* GetSDF1() {return sdf1;}
    SDF* GetSDF2() {return sdf2;}
    SDF* GetTestSDF() {return testSDF;}

    void Handle(ProcessEventArg arg);

    bool run;

    virtual void Run();
    
    /**
     * Set run to false, and wait for thread to end.
     */    

    void Start();
    void Stop(); 
    
    
    float GetValue(unsigned int i, unsigned int j);        
    Vector<2, float> Godunov(unsigned int i, unsigned int j, float a);

    SDF* Union(SDF* sdf1, SDF* sdf2);
	SDF* Intersection(SDF* sdf1, SDF* sdf2);
	SDF* Subtract(SDF* sdf1, SDF* sdf2);


};

#endif
