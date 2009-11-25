#ifndef _LSD_LEVEL_SET_METHOD_H_
#define _LSD_LEVEL_SET_METHOD_H_

#include "LockedQueue.h"
#include <Core/Thread.h>

#include "EmptyTextureResource.h"

using namespace OpenEngine::Core;

class LevelSetMethod : public Thread {

    
    //LockedQueue<EmptyTextureResourcePtr>* texQueue;

    ITextureResourcePtr inputTex;

    EmptyTextureResourcePtr sdfTex;

public:

    LevelSetMethod(ITextureResourcePtr inputTex);

    bool run;

    virtual void Run();

    EmptyTextureResourcePtr GetDFSTexture() {return sdfTex;}

    

};

#endif
