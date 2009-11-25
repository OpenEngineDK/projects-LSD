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
      dx(1),
      dy(1),
      phi(Tex<float>(width,height)),
      phiT(Tex<float>(width,height)),      
      vf(Tex<Vector<2,float> >(width,height)),
      grad(Tex<Vector<2,float> >(width,height)),
      sdfTex(EmptyTextureResource::Create(width,height,8)),
      vfTex(EmptyTextureResource::Create(width,height,24)),
      gradTex(EmptyTextureResource::Create(width,height,24)),
      phiTTex(EmptyTextureResource::Create(width,height,8)),
      run(true)
{
    sdfTex->Load();
    vfTex->Load();
    gradTex->Load();
    phiTTex->Load();
    // Lets generate the SDF
    BuildSDF();
    phi.ToTexture(sdfTex);

    BuildVF();
    vf.ToTexture(vfTex);
    
    BuildGradient();
    grad.ToTexture(gradTex);
}


void LevelSetMethod::BuildGradient() {    

#warning TODO: Vi burde fixe edge cases!111etetet

    for(unsigned int x=0; x<width-1;x++)
        for(unsigned int y=0; y<height-1;y++) {
            Vector<2,float> g = Gradient(x,y);

            grad(x,y) = g;
            
        }

}
void LevelSetMethod::BuildVF() {
    // make vector field V
    
    const unsigned int Y = inputTex->GetHeight();
    const unsigned int X = inputTex->GetWidth();

    Vector<2,float> gradient[X][Y];
    float dx = 1;
    float dy = 1;
    float cdX, cdY;
    for (unsigned int x=0; x<X; x++)
        for (unsigned int y=0; y<Y; y++) {
      
            //lower left corner
            if (x == 0 && y == 0) {
                cdX = (phi(x, y) - phi(x+1, y)) / dx;
                cdY = (phi(x, y) - phi(x, y+1)) / dy;

            } 
            //upper right corner
            else if (x == X - 1 && y == 0) {
                cdX = (phi(x, y) - phi(x-1, y)) / dx;
                cdY = (phi(x, y) - phi(x, y+1)) / dy;

            }
            //upper left corner
            else if (x == 0 && y == Y - 1) {
                cdX = (phi(x, y) - phi(x+1, y)) / dx;
                cdY = (phi(x, y) - phi(x, y-1)) / dy;

            }      
            //lower right corner
            else if (x == X - 1 && y == Y - 1) {
                cdX = (phi(x, y) - phi(x-1, y)) / dx;
                cdY = (phi(x, y) - phi(x, y-1)) / dy;

            }

            // upper border
            else if (y == 0 && (x > 0 && x < X - 1)) {
                cdX = (phi(x-1, y) - phi(x+1, y)) / 2 * dx;
                cdY = (phi(x, y)   - phi(x, y+1)) / dy;

            }       
            // lower border
            else if (y == Y - 1 && (x > 0 && x < X - 1)) {
                cdX = (phi(x-1, y) - phi(x+1, y)) / 2 * dx;
                cdY = (phi(x, y)   - phi(x, y-1)) / dy;

            }
            // left border
            else if (x == 0 && (y > 0 && y < Y - 1)) {
                cdX = (phi(x, y)   - phi(x+1, y)) / dx;
                cdY = (phi(x, y-1) - phi(x, y+1)) / 2 * dy;

            }
            // right border
            else if (x == X - 1 && (y > 0 && y < Y - 1)) {
                cdX = (phi(x, y)   - phi(x-1, y)) / dx;
                cdY = (phi(x, y-1) - phi(x, y+1)) / 2 * dy;

            }
            // Normal case
            else {
	
                // central differences
                cdX = (phi(x-1, y) - phi(x+1, y)) / 2 * dx;
                cdY = (phi(x, y-1) - phi(x, y+1)) / 2 * dy;
            }

            gradient[x][y] = vf(x,y) = Vector<2, float>(cdX, cdY);

        }

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

void LevelSetMethod::ProcessImage() {
    logger.info << "Process" << logger.end;
       
    for(unsigned int x=0; x<width;x++) {
        for(unsigned int y=0; y<height;y++) {
            phiT(x,y) = x;

        }
    }

}

void LevelSetMethod::Handle(ProcessEventArg arg) {
    phiT.ToTexture(phiTTex);
    phiTTex->RebindTexture();
    
}

void LevelSetMethod::Run() {

    while (run) {
        Thread::Sleep(1000000);
        ProcessImage();
    }
}

float LevelSetMethod::GetValue(unsigned int i, unsigned int j) {
    return phi(i,j);
}


void LevelSetMethod::Godunov(unsigned int i, unsigned int j,  float a,
                             float & dx2, float & dy2) {

    float diffXPositive = (phi(i, j)   - phi(i-1, j  )) / dx;
    float diffXNegative = (phi(i, j)   - phi(i+1, j  )) / dx;
    float diffYPositive = (phi(i, j)   - phi(i  , j-1)) / dy;
    float diffYNegative = (phi(i, j)   - phi(i  , j+1)) / dy;

    if (a > 0) {
        dx2 = max( pow(max(diffXNegative,0.0f),2), pow(min(diffXPositive,0.0f),2) );
        dy2 = max( pow(max(diffYNegative,0.0f),2), pow(min(diffYPositive,0.0f),2) );
    } else {
        dx2 = max( pow(min(diffXNegative,0.0f),2), pow(max(diffXPositive,0.0f),2) );
        dy2 = max( pow(min(diffYNegative,0.0f),2), pow(max(diffYPositive,0.0f),2) );
    }
}

Vector<2, float> LevelSetMethod::Gradient(unsigned int i, unsigned int j) {
    // Upwind
    float ddx, ddy;
    
    Vector<2,float> v = vf(i,j);

    ddx = ( v[0] < 0.0f ) 
        ? (phi(i, j)   - phi(i-1, j    )) / dx
        : (phi(i, j)   - phi(i+1, j    )) / dx;
    ddy = ( v[1] < 0.0f ) 
        ? (phi(i, j)   - phi(i  , j-1  )) / dx 
        : (phi(i, j)   - phi(i  , j+1  )) / dy;

    return Vector<2, float>(ddx, ddy);
        
}

Tex<float> LevelSetMethod::Union(Tex<float> sdf1, Tex<float> sdf2) {
	unsigned int height = sdf1.GetHeight();
	unsigned int width = sdf1.GetWidth();
	
	Tex<float> res(width,height);
	
	for (unsigned int y=0; y<height; y++) {
		for (unsigned int x=0; x<width; x++) {
			res(x,y) = min(sdf1(x,y),sdf2(x,y));
		}
	}
	return res;
}

Tex<float> LevelSetMethod::Intersection(Tex<float> sdf1, Tex<float> sdf2) {
	unsigned int height = sdf1.GetHeight();
	unsigned int width = sdf1.GetWidth();
	
	Tex<float> res(width,height);
	
	for (unsigned int y=0; y<height; y++) {
		for (unsigned int x=0; x<width; x++) {
			res(x,y) = max(sdf1(x,y),sdf2(x,y));
		}
	}
	return res;
}

Tex<float> LevelSetMethod::Subtract(Tex<float> sdf1, Tex<float> sdf2) {
	unsigned int height = sdf1.GetHeight();
	unsigned int width = sdf1.GetWidth();
	
	Tex<float> res(width,height);
	
	for (unsigned int y=0; y<height; y++) {
		for (unsigned int x=0; x<width; x++) {
			res(x,y) = max(sdf1(x,y),-sdf2(x,y));
		}
	}
	return res;
}
