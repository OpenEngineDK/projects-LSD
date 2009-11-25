#include "LevelSetMethod.h"
#include <Logging/Logger.h>



LevelSetMethod::LevelSetMethod(ITextureResourcePtr inputTex)
    : inputTex(inputTex), 
      width(inputTex->GetWidth()),
      height(inputTex->GetHeight()),
      phi(Tex<float>(width,height)),
      sdfTex(EmptyTextureResource::Create(width,height,8))
 {
    

    

}

void LevelSetMethod::Run() {
    
    while (run) {
        Thread::Sleep(1000000);
               
    }        
}
