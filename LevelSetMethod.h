#ifndef _LSD_LEVEL_SET_METHOD_H_
#define _LSD_LEVEL_SET_METHOD_H_

#include "LockedQueue.h"
#include <Core/Thread.h>
#include "Tex.h"

#include "EmptyTextureResource.h"

using namespace OpenEngine::Core;

class LevelSetMethod : public Thread {

    
    //LockedQueue<EmptyTextureResourcePtr>* texQueue;

    ITextureResourcePtr inputTex;



    int width;
    int height;

    Tex<float> phi;

    EmptyTextureResourcePtr sdfTex;


public:

    LevelSetMethod(ITextureResourcePtr inputTex);

    bool run;

    virtual void Run();

    EmptyTextureResourcePtr GetDFSTexture() {return sdfTex;}

    

};

#endif
