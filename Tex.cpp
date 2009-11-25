#include "Tex.h"
#include <Math/Vector.h>

using namespace OpenEngine::Math;

template<> void Tex<float>::ToTexture(EmptyTextureResourcePtr texture) {
        
        float min = 0;
        float max = 0;

        for(unsigned int x=0;x<width;x++) {
            for(unsigned int y=0;y<height;y++) {
                float pix = operator()(x,y);
                
                min = (min<pix)?min:pix;
                max = (max>pix)?max:pix;
                
            }
        }

        for(unsigned int x=0;x<width;x++) {
            for(unsigned int y=0;y<height;y++) {
            
                float pix = operator()(x,y);
                if (pix < 0)
                    (*texture)(x,y,0) = (unsigned char)(pix/min * 256);
                else
                    (*texture)(x,y,0) = (unsigned char)(pix/max * 256);
            
            }
        }
    }

using namespace std;

template<> void Tex<Vector<2,float> >::ToTexture(EmptyTextureResourcePtr texture) {
        
        float _min = 0;
        float _max = 0;

        for(unsigned int x=0;x<width;x++) {
            for(unsigned int y=0;y<height;y++) {
                Vector<2,float> pix = operator()(x,y);
                
                _min = min(_min,min(pix[0],pix[1]));
                _max = max(_max,max(pix[0],pix[1]));

                // min = (min<pix[0])?min:pix[0];
                // min = (min<pix[1])?min:pix[1];
                // max = (max>pix[0])?max:pix[0];
                // max = (max>pix[1])?max:pix[1];
                
            }
        }

        for(unsigned int x=0;x<width;x++) {
            for(unsigned int y=0;y<height;y++) {

                (*texture)(x,y,2) = 0;
                (*texture)(x,y,3) = -1;

            
                Vector<2,float> pix = operator()(x,y);
                if (pix[0] < 0) {
                    (*texture)(x,y,0) = (unsigned char)(pix[0]/_min * 256);
                    //(*texture)(x,y,2) = -1;
                }
                else
                    (*texture)(x,y,0) = (unsigned char)(pix[0]/_max * 256);

                if (pix[1] < 0)
                    (*texture)(x,y,1) = (unsigned char)(pix[1]/_min * 256);
                else
                    (*texture)(x,y,1) = (unsigned char)(pix[1]/_max * 256);

            
            }
        }
    }

