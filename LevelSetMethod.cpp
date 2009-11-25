#include "LevelSetMethod.h"
#include <Logging/Logger.h>

void Compare( Grid &g, Point &p, int x, int y, int offsetx, int offsety ) {
	Point other = g.Get(x+offsetx,y+offsety);
    other.dx += offsetx;
    other.dy += offsety;

	if (other.DistSq() < p.DistSq()) {
        p = other;
	}
}



void GenerateSDF( Grid &g, int width, int height ) {
    // Pass 0
    for (int y=0;y<height;y++) {
        for (int x=0;x<width;x++) {
            Point p = g.Get(x, y );
			if(x>0)
				Compare( g, p, x, y, -1,  0 );
			if(y>0)
				Compare( g, p, x, y,  0, -1 );
			if(x>0 && y>0)
				Compare( g, p, x, y, -1, -1 );
			if(x<width-1 && y>0)
				Compare( g, p, x, y,  1, -1 );
            g.Put( x, y, p );
        }

        for (int x=width-1;x>=0;x--) {            
            Point p = g.Get( x, y );
			if(x<width-1)
				Compare( g, p, x, y, 1, 0 );
            g.Put( x, y, p );
        }
    }

    // Pass 1
    for (int y=height-1;y>=0;y--) {
        for (int x=width-1;x>=0;x--) {
            Point p = g.Get( x, y );
			if(x<width-1)
				Compare( g, p, x, y,  1,  0 );
			if(y<height-1)
				Compare( g, p, x, y,  0,  1 );
			if(x>0 && y<height-1)
				Compare( g, p, x, y, -1,  1 );
			if(x<width-1 && y<height-1)
				Compare( g, p, x, y,  1,  1 );
            g.Put( x, y, p );
        }

        for (int x=0;x<width;x++) {
            Point p = g.Get( x, y );
			if(x>0)
				Compare( g, p, x, y, -1, 0 );
            g.Put( x, y, p );
        }
    }
}


LevelSetMethod::LevelSetMethod(ITextureResourcePtr inputTex)
    : inputTex(inputTex), 
      width(inputTex->GetWidth()),
      height(inputTex->GetHeight()),
      phi(Tex<float>(width,height)),
      sdfTex(EmptyTextureResource::Create(width,height,8))
{
    
    // Lets generate the SDF
    BuildSDF();
    

}


void LevelSetMethod::BuildSDF() {
    
    const unsigned int Y = inputTex->GetHeight();
    const unsigned int X = inputTex->GetWidth();
    const unsigned char* bw = inputTex->GetData();
    const unsigned int depth = inputTex->GetDepth()/8;


    Grid* gridInner = new Grid(X,Y);
    Grid* gridOuter = new Grid(X,Y);

    for (unsigned int y=0; y<Y; y++) {
		for (unsigned int x=0; x<X; x++) {
            unsigned int gray = 0;
            for (unsigned int i=0;i<depth;i++)
                gray += bw[y*X*depth+x*depth+i]; // dummy copy

            gray = (gray > 256)?255:0;

            if(!gray){
				(*gridInner)(x,y).dx = 0;
				(*gridInner)(x,y).dy = 0;
				(*gridOuter)(x,y).dx = X;
				(*gridOuter)(x,y).dy = Y;
			} else {
				(*gridInner)(x,y).dx = X;
				(*gridInner)(x,y).dy = Y;
				(*gridOuter)(x,y).dx = 0;
				(*gridOuter)(x,y).dy = 0;
			}
		}
	}

    GenerateSDF(*gridInner,X,Y);
	GenerateSDF(*gridOuter,X,Y);

	int min = X*Y+1;
	int max = -(X*Y+1);

	int dist1 = 0, dist2 = 0, dist = 0;
	for (unsigned int y=0; y<Y; y++) {
		for (unsigned int x=0; x<X; x++) {
			dist1 = (int)( sqrt( (double)gridInner->Get(x,y).DistSq() ) );
            dist2 = (int)( sqrt( (double)gridOuter->Get(x,y).DistSq() ) );
            dist = -dist2 + dist1;
			
			//result[y][x] = dist;
            phi(x,y) = dist;

			if(min > dist)
				min = dist;
			if(max < dist)
				max = dist;
			
		}
	}



}


void LevelSetMethod::Run() {
    
    while (run) {
        Thread::Sleep(1000000);
               
    }        
}
