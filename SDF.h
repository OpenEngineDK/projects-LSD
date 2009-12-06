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

    Tex<float> phi;
    Tex<float> phi0;
    Tex<Vector<2,float> > gradient;

    void BuildSDF();
    void BuildGradient();
    
    int S(Tex<float>& field, unsigned int x, unsigned int y);

public:
    SDF(ITextureResourcePtr);
    
    void Reinitialize();

    EmptyTextureResourcePtr GetOutputTexture(); // Returns the isosurface?
    EmptyTextureResourcePtr GetPhiTexture() {return phiTexture;}
    
    float operator()(unsigned int, unsigned int);

    Vector<2,float> Gradient(unsigned int, unsigned int);

};


#endif
