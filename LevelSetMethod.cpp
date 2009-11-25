#include "LevelSetMethod.h"
#include <Logging/Logger.h>

LevelSetMethod::LevelSetMethod(ITextureResourcePtr inputTex)
    : inputTex(inputTex) {
    
    int w = inputTex->GetWidth(),
        h = inputTex->GetHeight();
    

    sdfTex = EmptyTextureResource::Create(w,h,8);


}

void LevelSetMethod::Run() {
    
    while (run) {
        Thread::Sleep(1000000);
               
    }        
}
