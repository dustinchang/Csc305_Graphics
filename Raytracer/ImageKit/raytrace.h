//This is a simple ray tracing example showing how to use the Image
#pragma once
#include <vector>
#include <iostream>
#include <algorithm>
#include "Image.h"
#include "light.h"
#include "object.h"
#include "plane.h"
#include "sphere.h"
using namespace std;

#define LIGHTS 2
#define PHONGPOW 2
#define SPHERES 4

std::vector<Object *> ObjVec;
std::vector<Light *> LightsVec;

int refl_obj;

//16 rays/px anit-aliasing, done for each 2x2 section
Pixel Sect1, Sect2, Sect3, Sect4;

void SetColor(Pixel & px, Vector3 CalculatedColor)
{
    if (CalculatedColor.x < 0) px.R = 0;
    else if (CalculatedColor.x > 255) px.R = 255;
    else px.R = (unsigned char)CalculatedColor.x;

    if (CalculatedColor.y < 0) px.G = 0;
    else if (CalculatedColor.y > 255) px.G = 255;
    else px.G = (unsigned char)CalculatedColor.y;

    if (CalculatedColor.z < 0) px.B = 0;
    else if (CalculatedColor.z > 255) px.B = 255;
    else px.B = (unsigned char)CalculatedColor.z;

    px.A = 255;
}

Vector3 AmbientColour(20, 20, 20);
Vector3 DiffuseColour(200, 200, 200);
Vector3 BackgroundColor(25,25,25); //0,153,153
//Switched to obj LIGHTS    Vector3 Light(-128, -500, -128); //-128, -500, -575
//Switched to obj LIGHTS    Vector3 Light2(-384, -500, -300);

/*Provide Diffuse Shading, while also passing in other colors*/
/*Pixel DiffuseShade(Vector3 Surface, Vector3 Normal, Vector3 colors)
{
    //These colours will give the sphere a red appearance
    Pixel shade;
    Vector3 LightVector = Minus(Light, Surface);
    LightVector = Normalize(LightVector);
    float DiffuseTerm = DotProduct(LightVector, Normal);
    Vector3 PixelColour = AmbientColour;
    if (DiffuseTerm > 0)
    {
        Vector3 PixelDiffuseColour = MultiplyScalar(colors, DiffuseTerm);//Changing diffuse color
        PixelColour = Add(PixelColour, PixelDiffuseColour);
    }
    SetColor(shade, PixelColour);
    return shade;
}//End of DiffuseShade*/

//If there is an Intersection set appropriate calculations, whether there is no intersection, an intersection, or an intersection that creates a shadow
bool ifIntersection(Vector3 *Intersection, bool HasIntersection, Vector3 Direction, float t_min, Vector3 Camera, Vector3 Light, std::vector<Object *>& pObjectList, Vector3 *colors) {
  //Multiple shadows from different lights
  if (HasIntersection) {
    *Intersection = MultiplyScalar(Direction, t_min);
    *Intersection = Add(*Intersection, Camera);
    Vector3 Intersection2 = Add(*Intersection, Camera);
    Vector3 Light_Direction = Normalize(Minus(Light,Intersection2));
    //Shadow feelers
    for(int l = 0; l < pObjectList.size(); ++ l) {
      float v1;
      Vector3 v2;
      Vector3 v3;
      bool objectIntersects = pObjectList[l]->Intersect(Intersection2, Light_Direction, &v1, &v2, &v3);
      if(objectIntersects) {
          *colors = Minus(*colors, Vector3(70, 70, 70));
      }
    }//End of Shadow for loop
    return true;
  } else {
    return false;
  }
}

//Return lighter color
Vector3 LighterCol(Vector3 MaxCol, Vector3 OutCol) {
  int mc = MaxCol.x + MaxCol.y + MaxCol.z;
  int oc = OutCol.x + OutCol.y + OutCol.z;
  if(mc < oc) { return OutCol; }
  else { return MaxCol; }
}

//Add Pixel colors and prevent going higher than 255
Pixel AddColors(Pixel px, Pixel refl){
    int x,y,z;
    Pixel a;
    x = px.R + refl.R;
    y = px.G + refl.G;
    z = px.B + refl.B;
    if(x > 255) x = 255;
    if(y > 255) y = 255;
    if(z > 255) z = 255;
    a.R = x; a.G = y; a.B = z;
    return a;
}

//Minus Pixel colors and prevent going lower than 0
Pixel MinusColors(Pixel px, Pixel refl){
  int x,y,z;
  Pixel px2;
  x = px.R - refl.R;
  y = px.G - refl.G;
  z = px.B - refl.B;
  if(x < 0) x = 0;
  if(y < 0) y = 0;
  if(z < 0) z = 0;
  px2.R = x;
  px2.G = y;
  px2.B = z;
  return px2;
}

//Returns Avg Color of 4 Pixels
Pixel AveragePixel(Pixel a, Pixel b, Pixel c, Pixel d){
    Pixel x;
    x.R = (a.R + b.R + c.R + d.R )/4;
    x.G = (a.G + b.G + c.G + d.G )/4;
    x.B = (a.B + b.B + c.B + d.B )/4;
    return x;
}

Pixel Shade(Vector3 SurfCol, Vector3 Normal, Vector3 Direction, Vector3 Intersection, float LightIntensity) {
  Pixel px;
  Vector3 MaxCol(0,0,0);
  bool inShadow = false;
  bool inShade[LIGHTS] = {0};

  for(int i=0; i<LightsVec.size(); i++) {
    //Labertain
    Vector3 L = Normalize(Minus(LightsVec[i]->Point, Intersection));
    //cout << "DotProduct=" << DotProduct(Normal, L) << endl;
    float Intensity_Max = LightIntensity * max((float)0, DotProduct(Normal, L));
    Vector3 OutCol = MultiplyScalar(SurfCol, Intensity_Max);

    //Phong part
    Vector3 H = Normalize(Add(L, Direction));
    float SMax = max((float)0, DotProduct(Normal, H));
    float temp = SMax;
    for(int j=0; j<PHONGPOW; j++) { SMax += temp; }
    SMax *= LightIntensity;
    OutCol = Add(OutCol, MultiplyScalar(Vector3(200,200,200), SMax));
    SetColor(px, OutCol);
    MaxCol = LighterCol(MaxCol, OutCol);

    //Intersection pt adjustments to avoid black plague
    Intersection = Add(Intersection, MultiplyScalar(L, 0.2));

    //Shadow
    float t_min = 999999;
    bool HasIntersection = false;
    //Intersection with list of objects
    for(int k=0; k<ObjVec.size() ; k++) {
      float t;
      Vector3 norm;
      Vector3 col;
      bool DoesIntersect = ObjVec[k]->Intersect(Intersection, L, &t, &norm, &col);
      if(DoesIntersect) {
        inShade[i] = true;
        inShadow = true;
        break;
      }
    }//End of k for
  }//End of i for
  if(inShadow) {
    for(int m=0; m<LIGHTS; m++) {
      if(inShade[m]) MaxCol = MultiplyScalar(MaxCol, 0.5);
    }
  }
  SetColor(px, MaxCol);
  return px;
}//End of Shade

Pixel DetermineRay(Vector3 StartPoint, Vector3 Direction, float LightIntensity, int j) {
  Pixel px;
  float t_min = 999999;
  Vector3 Normal_min;
  Vector3 colors;
  bool HasIntersection = false;
  int obj_hit;

  //Intersect with the list of objects
  for (int k = 0; k < ObjVec.size(); ++ k) {
    float t;
    Vector3 normal;
    Vector3 col;
    bool DoesIntersect = ObjVec[k]->Intersect(StartPoint, Direction, &t, &normal, &col);
    //Determine the min t if an intersection
    if (DoesIntersect)
    {
        HasIntersection = true;
        if (t_min > t)
        {
            t_min = t;
            Normal_min = normal;
            colors = col;
            obj_hit = k;
        }
    }
  }
  if (HasIntersection) {
    Vector3 Intersection = MultiplyScalar(Direction, t_min);
    Intersection = Add(Intersection, StartPoint);
    px = Shade(colors, Normal_min, Direction, Intersection, LightIntensity);

    //Make chosen object reflective
    if(obj_hit == refl_obj ){
        //r = d - 2(d . n)n
        Vector3 N = Normal_min;
        float TwoD_Dot_N = DotProduct(Direction,N) * 2;
        Vector3 N_Time = MultiplyScalar(N, TwoD_Dot_N);
        Vector3 ReflRay = Minus(Direction, N_Time);
        ReflRay = Normalize(ReflRay);
        Pixel refl = DetermineRay(Intersection, ReflRay, 0.9, j);
        px = AddColors(refl,px);
    }
    //If Hit bottom/floor plane
    if(obj_hit == 4){
        //Give stripe texture
        Vector3 stripecolor(120, 120, 120);
        Pixel h;
        if(sin(M_PI*j/8)>0){
            SetColor(h, stripecolor);
            px = MinusColors(px,h);
        }
    }
  }//if t > 0
  else //No Intersection, set background colour
  {
    SetColor(px, BackgroundColor);
  }
  return px;
}//End of DetermineRay

//Add the directions
void addDirections(Vector3 Camera, Vector3 PixelPosition, Vector3 D1, Vector3 D2, Vector3 D3, Vector3 D4, int sectNum, float LightIntensity, int j) {
  Vector3 Direction1, Direction2, Direction3, Direction4;
  //Calculates the sections
  Direction1 = Add(PixelPosition,D1);
  Direction1 = Minus(Direction1,Camera);
  Direction1 = Normalize(Direction1);
  Direction2 = Add(PixelPosition,D2);
  Direction2 = Minus(Direction2,Camera);
  Direction2 = Normalize(Direction2);
  Direction3 = Add(PixelPosition,D3);
  Direction3 = Minus(Direction3,Camera);
  Direction3 = Normalize(Direction3);
  Direction4 = Add(PixelPosition,D4);
  Direction4 = Minus(Direction4,Camera);
  Direction4 = Normalize(Direction4);

  switch (sectNum) {
    case 1:
      Sect1 = AveragePixel(DetermineRay(Camera, Direction1, LightIntensity, j),DetermineRay(Camera, Direction2, LightIntensity, j), DetermineRay(Camera, Direction3, LightIntensity, j),DetermineRay(Camera, Direction4, LightIntensity, j));
      break;
    case 2:
      Sect2 = AveragePixel(DetermineRay(Camera, Direction1, LightIntensity, j),DetermineRay(Camera, Direction2, LightIntensity, j), DetermineRay(Camera, Direction3, LightIntensity, j),DetermineRay(Camera, Direction4, LightIntensity, j));
      break;
    case 3:
      Sect3 = AveragePixel(DetermineRay(Camera, Direction1, LightIntensity, j),DetermineRay(Camera, Direction2, LightIntensity, j), DetermineRay(Camera, Direction3, LightIntensity, j),DetermineRay(Camera, Direction4, LightIntensity, j));
      break;
    case 4:
      Sect4 = AveragePixel(DetermineRay(Camera, Direction1, LightIntensity, j),DetermineRay(Camera, Direction2, LightIntensity, j), DetermineRay(Camera, Direction3, LightIntensity, j),DetermineRay(Camera, Direction4, LightIntensity, j));
      break;
      default:
        cout << "Shouldn't reach here" << endl;
  }
}


//Main raytrace function
void RayTrace_Image(Image * pImage)
{
  //Creation of Lights
  Light L1(Vector3(120,120,0));
  LightsVec.push_back(&L1);
  Light L2(Vector3(200,256,0));
  LightsVec.push_back(&L2);
  float LightIntensity = 1.0;
  //Camera
  Vector3 Camera(256, 256, -200);	//Was z=-400
  //SPHERES
  Sphere s1(Vector3(500, 384, 200), //center
                90,//radius
                Vector3(220, 20, 60));//Color
  Sphere s2(Vector3(200,40,120), 40, Vector3(120,170,210));
  Sphere s3(Vector3(300,40,100), 38, Vector3(142,39,38));
  Sphere s4(Vector3(400,50,160), 50, Vector3(80,186,168));
  ObjVec.push_back(&s1);
  ObjVec.push_back(&s2);
  ObjVec.push_back(&s3);
  ObjVec.push_back(&s4);

  //Planes for Cornell box
  Plane bottom(Vector3(0,1,0), Vector3(256,0,0), Vector3(252,251,250));
  Plane Left(Vector3(1,0,0), Vector3(0,256,0), Vector3(136,141,147));
  Plane Right(Vector3(-1,0,0), Vector3(512,256,0), Vector3(213,77,48));
  Plane Top(Vector3(0,-1,0), Vector3(256,512,0), Vector3(1,56,75));
  Plane Back(Vector3(0,0,-1), Vector3(0,256,220), Vector3(136,141,147));

  //Put planes into ObjVec
  ObjVec.push_back(&bottom);
  ObjVec.push_back(&Left);
  ObjVec.push_back(&Right);
  ObjVec.push_back(&Top);
  ObjVec.push_back(&Back);

  refl_obj = 0;

  //Cycle through image pixels
  for (int i = 0; i < 512; ++ i) {
    for (int j = 0; j < 512; ++j)
    {
      Vector3 PixelPosition((float)i, (float)j, 0);
      //Add all sections
      addDirections(Camera, PixelPosition, Vector3(-0.375, 0.375, 0), Vector3(-0.125, 0.375, 0), Vector3(-0.375, 0.125, 0), Vector3(-0.125, 0.125, 0), 1, LightIntensity, j);
      addDirections(Camera, PixelPosition, Vector3(0.125, 0.375, 0), Vector3(0.375, 0.375, 0), Vector3(0.125, 0.125, 0), Vector3(0.375, 0.125, 0), 1, LightIntensity, j);
      addDirections(Camera, PixelPosition, Vector3(-0.375, -0.125, 0), Vector3(-0.125, -0.125, 0), Vector3(-0.375, -0.375, 0), Vector3(-0.125, -0.375, 0), 1, LightIntensity, j);
      addDirections(Camera, PixelPosition, Vector3(0.125, -0.125, 0), Vector3(0.375, -0.125, 0), Vector3(0.125, -0.375, 0), Vector3(0.375, -0.375, 0), 1, LightIntensity, j);

      //Get Average of the 4 section colours
      (*pImage)(i, 511-j) = AveragePixel(Sect1,Sect2, Sect3, Sect4);
    }
  }//End of outer for loop




    //From master
  ///  Plane plane(Vector3(0, 775, 0), Vector3(0, -3, 1), Vector3(102, 205, 170));
  ///  Sphere sphere(Vector3(500, 384, 200), //center
  ///                90,//radius
  ///                Vector3(220, 20, 60));//Color
  ///  Sphere sphere2(Vector3(50, 250, 375), 150, Vector3(0, 153, 153));
  ///  Sphere sphere3(Vector3(0, 425, 165), 75, Vector3(255, 153, 153));
  ///  pObjectList.push_back(&sphere);
  ///  pObjectList.push_back(&sphere2);
  ///  pObjectList.push_back(&sphere3);
  ///  pObjectList.push_back(&plane);
    //End of master

    //Start of modified
    /*Floor floor(Vector3(0, 512, 0), Vector3(100, 256, 0), Vector3(535, 512, 0), Vector3(204, 204, 0), Vector3(256, 350, 450));
    Floor floor2(Vector3(100, 256, 0), Vector3(400, 256, 0), Vector3(512, 512, 0), Vector3(204, 204, 0), Vector3(256, 350, 450));
    Plane plane(Vector3(0, 850, 0), Vector3(0, -3, 1), Vector3(102, 205, 170));
    Sphere sphere(Vector3(250, 225, -50), //center
                  50,//radius
                  Vector3(220, 20, 60));//Color
    Sphere sphere2(Vector3(50, 250, 375), 150, Vector3(0, 153, 153));
    pObjectList.push_back(&floor);
    pObjectList.push_back(&floor2);
    //pObjectList.push_back(&plane);
    pObjectList.push_back(&sphere);
    //pObjectList.push_back(&sphere2);
    //pObjectList.push_back(&plane);
    *///End of modified



/*

    //Cycle through image pixels
    for (int i = 0; i < 512; ++ i)
        for (int j = 0; j < 512; ++j)
        {
            //Set up the ray we're tracing: R = O + tD;
            Vector3 PixelPosition((float)i, (float)j, 0);
			      //Vector3 Direction = Minus(PixelPosition, Camera);
			      //Direction = Normalize(Direction);





















            //Intersect with the list of objects
            for (int k = 0; k < pObjectList.size(); ++ k)
            {
                float t;
                Vector3 normal;
                Vector3 col;
                bool DoesIntersect = pObjectList[k]->Intersect(Camera, Direction, &t, &normal, &col);
                //Determine the mine t if an intersection
                if (DoesIntersect)
                {
                    HasIntersection = true;
                    if (t_min > t)
                    {
                        t_min = t;
                        Normal_min = normal;
                        colors = col;
                    }
                }
            }

            //Multiple shadows from different lights
            Vector3 Intersection;
            //1st Light
            if(ifIntersection(&Intersection, HasIntersection, Direction, t_min, Camera, Light, pObjectList, &colors)) {
              px = DiffuseShade(Intersection, Normal_min, colors);
            } else {
              SetColor(px, BackgroundColor);
            }
            //2nd Light
            if(ifIntersection(&Intersection, HasIntersection, Direction, t_min, Camera, Light2, pObjectList, &colors)) {
              px = DiffuseShade(Intersection, Normal_min, colors);
            } else {
              SetColor(px, BackgroundColor);
            }

            */

      //Set the pixel image
			///(*pImage)(i, j) = px;
      //Get Average of the 4 section colours
    //  (*pImage)(i, 511-j) = AveragePixel(Section1,Section2, Section3, Section4);
		//}
}//End of RayTrace_Image
