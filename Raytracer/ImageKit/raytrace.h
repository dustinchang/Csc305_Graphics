//This is a simple ray tracing example showing how to use the Image
#pragma once
#include "Image.h"
#include <vector>
#include "sphere.h"
#include "floor.h"
#include "plane.h"
#include <iostream>

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
Vector3 DiffuseColour(200, 200, 200);//Might be too high
Vector3 BackgroundColor(173,255,47);
Vector3 Light(-128, -800, -200);
//******WARNING: THIS CODE MAKES EVERYTHING IN THE SCENE LOOKS THE SAME****
//SIMPLY COPY THIS CODE DOES NOT MEET THE REQUIREMENT FOR ASSIGNMENT 1
//YOU ARE EXPECTED TO HAVE DIFFERENT SURFACE MATERIALS FOR DIFFERENT OBJECTS
//IN YOUR SCENE.
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
        Vector3 PixelDiffuseColour = MultiplyScalar(colors, DiffuseTerm);
        PixelColour = Add(PixelColour, PixelDiffuseColour);
    }
    SetColor(shade, PixelColour);
    return shade;
}

void RayTraceSphere(Image * pImage)
{
    std::vector<Object *> pObjectList;
    Floor floor;
    Plane plane(Vector3(0, 850, 0), Vector3(0, -3, 1), Vector3(102, 205, 170));
    Sphere sphere(Vector3(256, 256, 450), //center
                  250, Vector3(235, 69, 30));//radius
    //pObjectList.push_back(&floor);
    //pObjectList.push_back(&plane);
    pObjectList.push_back(&sphere);
    pObjectList.push_back(&plane);

    Vector3 Camera(256, 256, -200);	//Was z=-400

    for (int i = 0; i < 512; ++ i)
        for (int j = 0; j < 512; ++j)
		{
            //Set up the ray we're tracing: R = O + tD;
            Pixel px;
            Vector3 PixelPosition((float)i, (float)j, 0);
			      Vector3 Direction = Minus(PixelPosition, Camera);
			      Direction = Normalize(Direction);

            float t_min = 999999;
            Vector3 Normal_min;
            Vector3 colors;
            bool HasIntersection = false;
            bool objHasIntersection = false;

            //Intersect with the list of objects
            for (int k = 0; k < pObjectList.size(); ++ k)
            {
                float t;
                Vector3 normal;
                Vector3 col;
                bool DoesIntersect = pObjectList[k]->Intersect(Camera, Direction,
                                                             &t, &normal, &col);
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

            if (HasIntersection)
			         {
                Vector3 Intersection = MultiplyScalar(Direction, t_min);
                Intersection = Add(Intersection, Camera);


                Vector3 Intersection2 = Add(Intersection, Camera);
                //Intersection2 = Normalize(Intersection2);

                //Vector3 IL = Minus(Light, Intersection2);
                //IL = Normalize(IL);

                //Put shadow feelers here
                for(int l = 0; l< pObjectList.size(); ++ l) {
                  float t;
                  Vector3 normal;
                  Vector3 col;
                  float v1;
                  Vector3 v2;
                  Vector3 v3;
                  bool objectIntersects = pObjectList[l]->Intersect(Intersection2, Normalize(Minus(Light, Intersection2)), &v1, &v2, &v3);

                    if(objectIntersects) {
                        colors = Minus(colors, Vector3(50, 50, 50));//20, 147);
                    }
                }//End of Shadow for loop


                px = DiffuseShade(Intersection, Normal_min, colors);
			           }//if t > 0
			          else //No Intersection, set background colour
			          {
				          SetColor(px, BackgroundColor);
			          }

			(*pImage)(i, j) = px;
		}
}
