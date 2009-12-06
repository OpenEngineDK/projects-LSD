#include "SDF.h"

struct Point {
    int dx, dy;

    int DistSq() const { return dx*dx + dy*dy; }
};

class Grid {
    
    unsigned int width;
    unsigned int height;
    
    Point *grid;//[HEIGHT][WIDTH];

public:
    Grid(unsigned int w, 
         unsigned int h) : width(w), height(h) {
        grid = new Point[width*height];
    }
    Point Get(unsigned int x,
               unsigned int y) {
        return grid[y*width+x];
    }
    void Put(unsigned int x,
             unsigned int y,
             Point v) {
        grid[y*width+x] = v;
    }

    Point& operator()(unsigned int x,
                      unsigned int y) {
        return grid[y*width+x];
    }
};

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


// ^- This will go away soon


SDF::SDF(ITextureResourcePtr input) 
  : inputTexture(input)
  , width(inputTexture->GetWidth())
  , height(inputTexture->GetHeight())
  , phiTexture(EmptyTextureResource::Create(width,height,8))
  , phi0Texture(EmptyTextureResource::Create(width,height,8))
  , outputTexture(EmptyTextureResource::Create(width,height,8))
  , gradientTexture(EmptyTextureResource::Create(width,height,24))
  , phi(width,height)
  , phi0(width,height)
  , gradient(width,height)
 {
     phiTexture->Load();
     phi0Texture->Load();
     outputTexture->Load();
     gradientTexture->Load();
     
     BuildSDF();
     BuildGradient();
}

void SDF::BuildSDF() {
    //Tex<Vector<2,float> > gridInner;
    //Tex<Vector<2,float> > gridOuter;
    
    ITextureResourcePtr in = inputTexture;
    

    const unsigned int Y = in->GetHeight();
    const unsigned int X = in->GetWidth();
    const unsigned char* bw = in->GetData();
    const unsigned int depth = in->GetDepth()/8;


    //Tex<float> pphi(X,Y);

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

    phi.ToTexture(phiTexture);
    updateQueue.Put(phiTexture);

    phi0 = phi;
    phi0.ToTexture(phi0Texture);
    updateQueue.Put(phi0Texture);


    SDFToTexture(phi,outputTexture);
    updateQueue.Put(outputTexture);
}


void SDF::BuildGradient() {   
    const unsigned int Y = inputTexture->GetHeight();
    const unsigned int X = inputTexture->GetWidth();

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

            gradient(x,y)  = Vector<2, float>(cdX, cdY);

        }
    gradient.ToTexture(gradientTexture);
    updateQueue.Put(gradientTexture);

}


Vector<2, float> SDF::Gradient(unsigned int i, unsigned int j) {
    
    return gradient(i,j);
    
    // Upwind
    // float ddx, ddy;
    
    // Vector<2,float> v = vf(i,j);

    // ddx = ( v[0] < 0.0f ) 
    //     ? (phi(i, j)   - phi(i-1, j    )) / dx
    //     : (phi(i, j)   - phi(i+1, j    )) / dx;
    // ddy = ( v[1] < 0.0f ) 
    //     ? (phi(i, j)   - phi(i  , j-1  )) / dx 
    //     : (phi(i, j)   - phi(i  , j+1  )) / dy;

    // return Vector<2, float>(ddx, ddy);
        
}

// Fortegns funktion fra bogen.
int SDF::S(unsigned int x, unsigned int y) {
    float phi = phi0(x,y);

    //7.5
    //return (phi > 0 ? 1 : -1);

    Vector<2,float> grad = gradient(x,y);
    //7.6
    return phi / sqrt(phi*phi + grad.GetLengthSquared());
}


void SDF::Reinitialize(unsigned int iterations) {
    // Equation 7.4
    while (iterations--) {
        for (unsigned int y=1; y<height-1 ; y++) {
            for (unsigned int x=1; x<width-1; x++) { 
             
                Vector<2,float> g = gradient(x,y);
                phi(x,y) = phi0(x,y) +  S(x, y) * (g.GetLength() - 1.0);
            
                if (isnan(g.GetLength()))
                    logger.info << g << logger.end;
            }
        }

#warning Edge cases fixed here too
        for (unsigned int y=0; y<height ; y++) {
            phi(0,y) = phi(1,y);
            phi(width-1,y) = phi(width-2,y);
        }
        for (unsigned int x=0; x<width; x++) { 
            phi(x,0) = phi(x,1);
            phi(x,height-1) = phi(x,height-2);
        }
    }
}


void SDF::SDFToTexture(Tex<float> p, EmptyTextureResourcePtr t) {
    for (unsigned int x=0;x<p.GetWidth();x++) {
        for (unsigned int y=0;y<p.GetHeight();y++) {
            //logger.info << p(x,y) << logger.end;
            if (p(x,y) < 0 )
                (*t)(x,y,0) = 0;
            else 
                (*t)(x,y,0) = -1;
        }
    }
}

Tex<float> SDF::GetPhi() {
    //return phi;

    Tex<float> cp(width,height);
    cp = phi;
    return cp;
}

void SDF::SetPhi(Tex<float> phiT) {
    phi0 = phi;
    phi = phiT;

    phi.ToTexture(phiTexture);
    updateQueue.Put(phiTexture);

    phi0.ToTexture(phi0Texture);
    updateQueue.Put(phi0Texture);
}


void SDF::Refresh() {
    SDFToTexture(phi,outputTexture);
    updateQueue.Put(outputTexture);

    while(!updateQueue.IsEmpty()) {
        EmptyTextureResourcePtr t = updateQueue.Get();
        t->RebindTexture();
    }

}
