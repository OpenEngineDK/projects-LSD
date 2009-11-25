#ifndef _LSD_TEX_H_
#define _LSD_TEX_H_


#include <cmath>
#include "EmptyTextureResource.h"
#include <Math/Math.h>
#include <Math/Exceptions.h>
#include <Logging/Logger.h>

using namespace OpenEngine;
using namespace OpenEngine::Math;
using namespace std;


// @todo template parameter specifing number of dimensions
template <typename T>
class Tex {
private:
    unsigned int width, height;
    T* data;
public:
    Tex(unsigned int width, unsigned int height)
      :  width(width), height(height) {
      data = new T[height*width];
    }

    T* operator[](const unsigned int iy) {
        return data+ iy*width;
    }

    T& operator()(const unsigned int ix, const unsigned int iy) {
      
        return data[ix+iy*width];
    }
    void ToTexture(EmptyTextureResourcePtr t, bool dbg=false) ; 


  // look up pixel by interpolation
  T operator()(const float fx, const float fy) {
#if OE_SAFE
    if (fx < 0) throw Math::IndexOutOfBounds(floor(fx),0,(unsigned int)-1);
    if (fy < 0) throw Math::IndexOutOfBounds(floor(fy),0,(unsigned int)-1);
#endif

    unsigned int ix_below = floor(fx);
    unsigned int ix_above = ceil(fx);
    unsigned int iy_below = floor(fy);
    unsigned int iy_above = ceil(fy);

    float wx_below =  fx - ix_below;
    float wx_above =  1 - wx_below;

    float wy_below =  fy - iy_below;
    float wy_above =  1 - wy_below;

    float dy_above = 
      (*this)(ix_above,iy_above) * wx_above +
      (*this)(ix_below,iy_above) * wx_below;

    float dy_below = 
      (*this)(ix_above,iy_below) * wx_above +
      (*this)(ix_below,iy_below) * wx_below;

    T v = dy_above * wy_above + dy_below * wy_below;
    return v;
  }
};


#endif
