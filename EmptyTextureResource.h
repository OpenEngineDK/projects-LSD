// Empty texture resource.
// -------------------------------------------------------------------
// Copyright (C) 2007 OpenEngine.dk (See AUTHORS) 
// 
// This program is free software; It is covered by the GNU General 
// Public License version 2 or any later version. 
// See the GNU General Public License for more details (see LICENSE). 
//--------------------------------------------------------------------

#ifndef _EMPTY_TEXTURE_RESOURCE_H_
#define _EMPTY_TEXTURE_RESOURCE_H_

#include <Resources/ITextureResource.h>

#include <boost/serialization/weak_ptr.hpp>

using namespace OpenEngine::Resources;

//namespace OpenEngine {
//    namespace Resources {

class EmptyTextureResource;

typedef boost::shared_ptr<EmptyTextureResource> EmptyTextureResourcePtr;

        class EmptyTextureResource : public ITextureResource {
        private:
            unsigned int width;
            unsigned int height;
            unsigned int depth;
            unsigned char* data;
            int id;
            ColorFormat format;

            boost::weak_ptr<EmptyTextureResource> weak_this;

            EmptyTextureResource(unsigned int w, unsigned int h, unsigned int d) 
                : width(w), height(h), depth(d), data(NULL), id(0) {
                switch (depth){
                case 8: format = LUMINANCE; break;
                case 24: format = RGB; break;
                case 32: format = RGBA; break;
                }
            }
        public:
            static EmptyTextureResourcePtr Create(unsigned int w,
                                                  unsigned int h,
                                                  unsigned int d) {
                EmptyTextureResourcePtr ptr(new EmptyTextureResource(w,h,d));
                ptr->weak_this = ptr;
                return ptr;
            }
            
            ~EmptyTextureResource() { delete data; }
            void Load() { if (!data) data = new unsigned char[width * height * depth / 8]; }
            void Unload() { delete data; }
            int GetID() { return id; }
            void SetID(int id) { this->id = id; }
            unsigned int GetWidth() { return width; }
            unsigned int GetHeight() { return height; }
            unsigned int GetDepth() { return depth; }
            unsigned char* GetData() { return data; }
            ColorFormat GetColorFormat() { return format; }

            void RebindTexture() {
                changedEvent.Notify(TextureChangedEventArg(EmptyTextureResourcePtr(weak_this)));
            }

            unsigned char& operator()(const unsigned int x,
                                      const unsigned int y,
                                      const unsigned int component) {
                return data[y*width*depth/8+x*depth/8+component];
            }
        };
//    }
//}

#endif