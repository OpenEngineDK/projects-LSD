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

    ITextureResourcePtr inputTex;
    ITextureResourcePtr inputTex2;


    unsigned int width;
    unsigned int height;

    int dx, dy;

    // Tex<float> phi;
    // Tex<float> phi0;
    // Tex<float> phiT;

    //Tex<Vector<2,float> > vf;
    //Tex<Vector<2,float> > grad;
    
    LockedQueue<EmptyTextureResourcePtr> updateQueue;

    EmptyTextureResourcePtr sdfTex,vfTex,gradTex,phiTTex,testTex;


    void BuildSDF();
    void BuildVF();
    void BuildGradient();

    void ProcessImage();

public:

    LevelSetMethod(ITextureResourcePtr inputTex,
                   ITextureResourcePtr inputTex2);

    SDF* GetSDF1() {return sdf1;}

    void Handle(ProcessEventArg arg);

    bool run;

    virtual void Run();

    EmptyTextureResourcePtr GetDFSTexture() {return sdfTex;}
    EmptyTextureResourcePtr GetVFTexture() {return vfTex;}
    EmptyTextureResourcePtr GetGradientTexture() {return gradTex;}
    EmptyTextureResourcePtr GetPhiTTexture() {return phiTTex;}
    EmptyTextureResourcePtr GetTestTexture() {return testTex;}

    static void SDFToTexture(Tex<float> p, EmptyTextureResourcePtr t);
    
    float GetValue(unsigned int i, unsigned int j);        
    Vector<2, float> Godunov(unsigned int i, unsigned int j, float a);
    Vector<2, float> Gradient(unsigned int i, unsigned j);
    //void ReInitialize();



    //Tex<float> GetPhi() {return phi;}

    Tex<float> Union(Tex<float> sdf1, Tex<float> sdf2);
	Tex<float> Intersection(Tex<float> sdf1, Tex<float> sdf2);
	Tex<float> Subtract(Tex<float> sdf1, Tex<float> sdf2);


};

#endif
