#include "LevelSetMethod.h"
#include <Logging/Logger.h>



LevelSetMethod::LevelSetMethod(ITextureResourcePtr inputTex, ITextureResourcePtr inputTex2)
    : sdf1(new SDF(inputTex)),
      sdf2(new SDF(inputTex2)),
      inputTex(inputTex),
      inputTex2(inputTex2),
      width(inputTex->GetWidth()),
      height(inputTex->GetHeight()),
      dx(1),
      dy(1),      
      run(true)
{
    //testSDF = sdf1;//Subtract(sdf2,sdf1);
    testSDF = Subtract(sdf2,sdf1);
    
    testSDF->Refresh();
    testSDF->Reinitialize(width/2);

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

void LevelSetMethod::ProcessImage() {

    SDF* sdf = testSDF;

    static unsigned int count = 0;
    logger.info << "Process (iteration " << count++ << ")" << logger.end;
       
    sdf->Reinitialize(30);

    Tex<float> phi = sdf->GetPhi();
    Tex<float> phi2 = sdf2->GetPhi();
    //unsigned char* u0 = inputTex->GetData();
    //EmptyTextureResourcePtr ut_tex = EmptyTextureResource::Clone(inputTex);
    //unsigned char* ut = ut_tex->GetData();

    for(unsigned int x=1; x<width-1; x++) {
        for(unsigned int y=1; y<height-1; y++) {
            // //Vector<2,float> godunov = Godunov(x,y,a);
            Vector<2,float> g = sdf->Gradient(x,y);
 
            /*          
            // //float phiX = sqrt(godunov[0]);
            // //float phiY = sqrt(godunov[1]);
            
            Vector<2,float> v;
            v[0] = g[0] / g.GetLength();
            v[1] = g[1] / g.GetLength();

            //phi(x,y) += a*(v*g);
            */


            // formula 6.1 page 55
            //float a = -0.7, dt = 0.48;
            //phi(x,y) += -a * g.GetLength() * dt; //shrink

            //phi(x,y) += (phi(x,y) - phi2(x,y)) * -0.1; //morph


            // using formula 1.9 on page 12 and
            const float dx = 1.0;
            float phi_xx = (phi(x+1,y) - 2*phi(x,y) + phi(x-1,y))/(dx*dx);
            float phi_yy = (phi(x,y+1) - 2*phi(x,y) + phi(x,y-1))/(dx*dx);
            // using formula 2.7 on page 21.
            float kappa = phi_xx + phi_yy;
            // using formula 4.11 on page 45
            const float b = 1.0, dt = 0.48;
            phi(x,y) += kappa * g.GetLength() * b * dt; //mean curvature

            /*
            float u0_xx = u0[(x+1)+y*width] - 2*u0[x+y*width] +
                u0[(x-1)+y*width];
            float u0_yy = u0[x+(y+1)*width] - 2*u0[x+y*width] + 
                u0[x+(y-1)*width];
            float Lu0 = u0_xx + u0_yy;
            float dt = 0.2;
            u0[x+y*width] = u0[x+y*width] + Lu0*dt;
            */
        }
    }

    sdf->SetPhi(phi);

    

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

void LevelSetMethod::Handle(ProcessEventArg arg) {
    
    testSDF->Refresh();
    sdf1->Refresh();
    sdf2->Refresh();

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
        //sdf1->Reinitialize(10);
        ProcessImage();
        //Thread::Sleep(1000000);
    }
}


// Vector<2,float> LevelSetMethod::Godunov(unsigned int i, unsigned int j,  float a) {
//     float diffXPositive = (phi(i, j)   - phi(i-1, j  )) / dx;
//     float diffXNegative = (phi(i, j)   - phi(i+1, j  )) / dx;
//     float diffYPositive = (phi(i, j)   - phi(i  , j-1)) / dy;
//     float diffYNegative = (phi(i, j)   - phi(i  , j+1)) / dy;
    //[Osher153] Eq 6.3 / 6.4 
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



SDF* LevelSetMethod::Union(SDF* s1, SDF* s2) {
	unsigned int height = s1->GetHeight();
	unsigned int width = s1->GetWidth();
	
    SDF* ns = new SDF(width,height);
    
	Tex<float> res = ns->GetPhi();
	
	for (unsigned int y=0; y<height; y++) {
		for (unsigned int x=0; x<width; x++) {            
			res(x,y) = min((*s1)(x,y),(*s2)(x,y));
		}
	}
	ns->SetPhi(res);
    return ns;    
}


SDF* LevelSetMethod::Intersection(SDF* s1, SDF* s2) {
	unsigned int height = s1->GetHeight();
	unsigned int width = s1->GetWidth();
	
    SDF* ns = new SDF(width,height);
    
	Tex<float> res = ns->GetPhi();
	
	for (unsigned int y=0; y<height; y++) {
		for (unsigned int x=0; x<width; x++) {            
			res(x,y) = max((*s1)(x,y),(*s2)(x,y));
		}
	}
	ns->SetPhi(res);
    return ns;    
}


SDF* LevelSetMethod::Subtract(SDF* s1, SDF* s2) {
	unsigned int height = s1->GetHeight();
	unsigned int width = s1->GetWidth();
	
    SDF* ns = new SDF(width,height);
    
	Tex<float> res = ns->GetPhi();
	
	for (unsigned int y=0; y<height; y++) {
		for (unsigned int x=0; x<width; x++) {
			res(x,y) = max((*s1)(x,y),-(*s2)(x,y));
		}
	}
	ns->SetPhi(res);
    return ns;    
}
