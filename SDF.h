#ifndef _LS_SDF_H_
#define _LS_SDF_H_

#include <Resources/ITextureResource.h>
#include <Math/Vector.h>
#include "Tex.h"


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
    Tex<float> phi0;
    Tex<Vector<2,float> > gradient;

    void BuildSDF();
    void BuildGradient();
    
    int S(Tex<float>& field, unsigned int x, unsigned int y);
    void SDFToTexture(Tex<float> p, EmptyTextureResourcePtr t);

public:
    SDF(ITextureResourcePtr);
    
    void Reinitialize();

    // Returns the isosurface?
    EmptyTextureResourcePtr GetOutputTexture() {return outputTexture;} 
    EmptyTextureResourcePtr GetPhiTexture() {return phiTexture;}
    EmptyTextureResourcePtr GetGradientTexture() {return gradientTexture;}


    float operator()(unsigned int, unsigned int);

    Vector<2,float> Gradient(unsigned int, unsigned int);

};


#endif
