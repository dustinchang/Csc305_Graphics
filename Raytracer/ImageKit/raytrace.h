//This is a simple ray tracing example showing how to use the Image
#pragma once
#include <vector>
#include <iostream>
#include "Image.h"
#include "light.h"
#include "object.h"
#include "plane.h"
#include "sphere.h"

std::vector<Object *> ObjVec;
std::vector<Light *> lightsVec;

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
Vector3 Light(-128, -500, -128); //-128, -500, -575
Vector3 Light2(-384, -500, -300);

/*Provide Diffuse Shading, while also passing in other colors*/
Pixel DiffuseShade(Vector3 Surface, Vector3 Normal, Vector3 colors)
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
}

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

//Add the directions
void addDirections(Vector3 Camera, Vector3 D1, Vector3 D2, Vector3 D3, Vector3 D4, int sectNum, float LightIntensity, ) {
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
      Sect1 = AveragePixel(ComputeRay(Camera, Direction1, LightIntensity, j),ComputeRay(Camera, Direction2, LightIntensity, j), ComputeRay(Camera, Direction3, LightIntensity, j),ComputeRay(Camera, Direction4, LightIntensity, j));
      break;
    case 2:
      Sect2 =
      break;
  }

}


//Main raytrace function
void RayTrace_Image(Image * pImage)
{
  //Creation of Lights
  Light L1(Vector3(120,120,0));
  LightList.push_back(&L1);
  Light L2(Vector3(200,256,0));
  LightList.push_back(&L2);
  float LightIntensity = 1.0;
  /////Creation of objects
  ///std::vector<Object *> pObjectList;
  //Camera
  Vector3 Camera(256, 256, -200);	//Was z=-400
  //Planes for Cornell box
  Plane bottom(Vector3(0,1,0), Vector3(256,0,0), Vector3(252,251,250));

  //Put planes into ObjVec
  ObjVec.push_back(&bottom);




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



    //Cycle through image pixels
    for (int i = 0; i < 512; ++ i)
        for (int j = 0; j < 512; ++j)
        {
            //Set up the ray we're tracing: R = O + tD;
            Pixel px;
            Vector3 PixelPosition((float)i, (float)j, 0);
			      //Vector3 Direction = Minus(PixelPosition, Camera);
			      //Direction = Normalize(Direction);



















            float t_min = 999999;
            Vector3 Normal_min;
            Vector3 colors;
            bool HasIntersection = false;

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

      //Set the pixel image
			(*pImage)(i, j) = px;
		}
}
