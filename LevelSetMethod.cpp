#include "LevelSetMethod.h"
#include <Logging/Logger.h>



LevelSetMethod::LevelSetMethod(ITextureResourcePtr inputTex, ITextureResourcePtr inputTex2)
    : sdf1(new SDF(inputTex)),
      inputTex(inputTex),
      inputTex2(inputTex2),
      width(inputTex->GetWidth()),
      height(inputTex->GetHeight()),
      dx(1),
      dy(1),
      // phi(*(new Tex<float>(width,height))),
      // phi0(*(new Tex<float>(width,height))),
      // phiT(*(new Tex<float>(width,height))),
      //phi(width,height),
      //phi0(width,height),
      //phiT(width,height),
      //vf(width,height),
      //grad(width,height),
      sdfTex(EmptyTextureResource::Create(width,height,8)),
      vfTex(EmptyTextureResource::Create(width,height,24)),
      gradTex(EmptyTextureResource::Create(width,height,24)),
      phiTTex(EmptyTextureResource::Create(width,height,8)),
      testTex(EmptyTextureResource::Create(width,height,8)),
      run(true)
{
    sdfTex->Load();
    vfTex->Load();
    gradTex->Load();
    phiTTex->Load();
    testTex->Load();
    // Lets generate the SDF    
    //BuildSDF();
    //phi.ToTexture(sdfTex);

    //BuildVF();
    //vf.ToTexture(vfTex);
    
    //BuildGradient();
    //grad.ToTexture(gradTex);

    
    // Testing
    //Tex<float> phiTest = Subtract(phi,BuildPhi(inputTex2));
    
    //SDFToTexture(phiTest,testTex);

    //phiTest.ToTexture(testTex);

    //phiT = phi;
    
    
    // for(unsigned int x=0; x<width-1;x++)
    //     for(unsigned int y=0; y<height-1;y++) {
    //         Vector<2,float> g = vf(x,y);
            
    //         (*testTex)(x,y,0) = g.GetLength()*100.0;
            
    //     }


    

}


// void LevelSetMethod::BuildVF() {
//     // make vector field V
    
//     const unsigned int Y = inputTex->GetHeight();
//     const unsigned int X = inputTex->GetWidth();

//     Vector<2,float> gradient[X][Y];
//     float dx = 1;
//     float dy = 1;
//     float cdX, cdY;
//     for (unsigned int x=0; x<X; x++)
//         for (unsigned int y=0; y<Y; y++) {
      
//             //lower left corner
//             if (x == 0 && y == 0) {
//                 cdX = (phi(x, y) - phi(x+1, y)) / dx;
//                 cdY = (phi(x, y) - phi(x, y+1)) / dy;

//             } 
//             //upper right corner
//             else if (x == X - 1 && y == 0) {
//                 cdX = (phi(x, y) - phi(x-1, y)) / dx;
//                 cdY = (phi(x, y) - phi(x, y+1)) / dy;

//             }
//             //upper left corner
//             else if (x == 0 && y == Y - 1) {
//                 cdX = (phi(x, y) - phi(x+1, y)) / dx;
//                 cdY = (phi(x, y) - phi(x, y-1)) / dy;

//             }      
//             //lower right corner
//             else if (x == X - 1 && y == Y - 1) {
//                 cdX = (phi(x, y) - phi(x-1, y)) / dx;
//                 cdY = (phi(x, y) - phi(x, y-1)) / dy;

//             }

//             // upper border
//             else if (y == 0 && (x > 0 && x < X - 1)) {
//                 cdX = (phi(x-1, y) - phi(x+1, y)) / 2 * dx;
//                 cdY = (phi(x, y)   - phi(x, y+1)) / dy;

//             }       
//             // lower border
//             else if (y == Y - 1 && (x > 0 && x < X - 1)) {
//                 cdX = (phi(x-1, y) - phi(x+1, y)) / 2 * dx;
//                 cdY = (phi(x, y)   - phi(x, y-1)) / dy;

//             }
//             // left border
//             else if (x == 0 && (y > 0 && y < Y - 1)) {
//                 cdX = (phi(x, y)   - phi(x+1, y)) / dx;
//                 cdY = (phi(x, y-1) - phi(x, y+1)) / 2 * dy;

//             }
//             // right border
//             else if (x == X - 1 && (y > 0 && y < Y - 1)) {
//                 cdX = (phi(x, y)   - phi(x-1, y)) / dx;
//                 cdY = (phi(x, y-1) - phi(x, y+1)) / 2 * dy;

//             }
//             // Normal case
//             else {
	
//                 // central differences
//                 cdX = (phi(x-1, y) - phi(x+1, y)) / 2 * dx;
//                 cdY = (phi(x, y-1) - phi(x, y+1)) / 2 * dy;
//             }

//             gradient[x][y] = vf(x,y) = Vector<2, float>(cdX, cdY);

//         }

// }


void LevelSetMethod::ProcessImage() {
    logger.info << "Process" << logger.end;
       
    float a = 1.0;

#warning Oh fail, flere kant tilfælde...!!shift-en

    // for(unsigned int x=1; x<width-1;x++) {
    //     for(unsigned int y=1; y<height-1;y++) {
            
    //         //Vector<2,float> godunov = Godunov(x,y,a);
    //         //Vector<2,float> g = Gradient(x,y);
    //         Vector<2,float> g = vf(x,y);
 
    //         //float phiX = sqrt(godunov[0]);
    //         //float phiY = sqrt(godunov[1]);
            
    //         Vector<2,float> v;
    //         v[0] = g[0] / g.GetLength();
    //         v[1] = g[1] / g.GetLength();

    //         phiT(x,y) += -v*g;
    //         //phiT(x,y) += -1.1;
    //     }
    // }

    // int iterations = 4; //width/2;

    // while(iterations--) {
    //     //logger.info << "Reinitialize - iteration: " << x << logger.end;
    //     ReInitialize();
    // }
    
    
    //phi0 = phiT;

    //phi0.SetTex(phi);
    //phi.SetTex(phiT);
    
    // phi.ToTexture(sdfTex,true);
    // phi0.ToTexture(phiTTex);

    // updateQueue.Put(sdfTex);
    // updateQueue.Put(phiTTex);

    
}

void LevelSetMethod::SDFToTexture(Tex<float> p, EmptyTextureResourcePtr t) {
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

void LevelSetMethod::Handle(ProcessEventArg arg) {

    while(!updateQueue.IsEmpty()) {
        EmptyTextureResourcePtr t = updateQueue.Get();
        t->RebindTexture();
    }

    //phiT.ToTexture(phiTTex);
    //SDFToTexture(phiT, phiTTex);
    //phiTTex->RebindTexture();
    
}

void LevelSetMethod::Run() {

    while (run) {

        ProcessImage();
        //Thread::Sleep(1000000);
    }
}


// Vector<2,float> LevelSetMethod::Godunov(unsigned int i, unsigned int j,  float a) {
//     float diffXPositive = (phi(i, j)   - phi(i-1, j  )) / dx;
//     float diffXNegative = (phi(i, j)   - phi(i+1, j  )) / dx;
//     float diffYPositive = (phi(i, j)   - phi(i  , j-1)) / dy;
//     float diffYNegative = (phi(i, j)   - phi(i  , j+1)) / dy;

//     float dx2,dy2;

//     if (a > 0) {
//         dx2 = max( pow(max(diffXNegative,0.0f),2), pow(min(diffXPositive,0.0f),2) );
//         dy2 = max( pow(max(diffYNegative,0.0f),2), pow(min(diffYPositive,0.0f),2) );
//     } else {
//         dx2 = max( pow(min(diffXNegative,0.0f),2), pow(max(diffXPositive,0.0f),2) );
//         dy2 = max( pow(min(diffYNegative,0.0f),2), pow(max(diffYPositive,0.0f),2) );
//     }

//     return Vector<2,float>(dx2,dy2);
// }




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
