#ifndef _LS_SDF_H_
#define _LS_SDF_H_

#include <Resources/ITextureResource.h>
#include <Math/Vector.h>
#include "Tex.h"
#include "LockedQueue.h"

using namespace OpenEngine::Resources;
using namespace OpenEngine::Math;

/**
 * Short description.
 *
 * @class SDF SDF.h ts/LSD/SDF.h
 */
class SDF {
private:   

    ITextureResourcePtr inputTexture;

    unsigned int width,height;

    EmptyTextureResourcePtr phiTexture;   
    EmptyTextureResourcePtr outputTexture;
    EmptyTextureResourcePtr gradientTexture;

    Tex<float> phi;
    Tex<Vector<2,float> > gradient;

    void BuildSDF();
    void BuildGradient();
    
    void SDFToTexture(Tex<float> p, EmptyTextureResourcePtr t);
    //int S(unsigned int x, unsigned int y);

    LockedQueue<EmptyTextureResourcePtr> updateQueue;

public:
    SDF(ITextureResourcePtr);
    SDF(unsigned int,unsigned int);
    
    void Reinitialize(unsigned int);

    // Returns the isosurface?
    //ITextureResourcePtr GetInputTexture() {return inputTexture;}
    EmptyTextureResourcePtr GetOutputTexture() {return outputTexture;} 
    EmptyTextureResourcePtr GetPhiTexture() {return phiTexture;}
    EmptyTextureResourcePtr GetGradientTexture() {return gradientTexture;}

    Tex<float> GetPhi();
    void SetPhi(Tex<float>);
    
    unsigned int GetWidth() {return width;}
    unsigned int GetHeight() {return height;}

    float operator()(unsigned int x, unsigned int y) {return phi(x,y);}

    Vector<2,float> Gradient(unsigned int, unsigned int);

    void Refresh();

};


#endif
