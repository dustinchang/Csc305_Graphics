//This is a simple ray tracing example showing how to use the Image 
#pragma once
#include "Image.h"
#include <math.h>

struct Vector3
{
	float x, y, z;
	Vector3(float x_in, float y_in, float z_in)
	{
		x = x_in;
		y = y_in;
		z = z_in;
	}
};

Vector3 Add(Vector3 a, Vector3 b) {	return Vector3(a.x + b.x, a.y + b.y, a.z + b.z); }
Vector3 Minus(Vector3 a, Vector3 b) { return Vector3(a.x - b.x, a.y - b.y, a.z - b.z); }
Vector3 MultiplyScalar(Vector3 a, float b) { return Vector3(a.x * b, a.y * b, a.z * b); }
float DotProduct(Vector3 a, Vector3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
Vector3 Normalize(Vector3 a)
{
	float length = a.x * a.x + a.y * a.y + a.z * a.z;
	length = sqrt(length);
	return Vector3(a.x / length, a.y / length, a.z / length);
}

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

void RayTraceSphere(Image * pImage)
{
	//The Image is assumed to be 512 x 512
	//We put the camera at 256, 256, -200
	//and the sphere at 256, 256, 450, with radius 400.
	//The light will be at 128, 128, 0,
	//to give a bright spot on the upper-left corner of the sphere
	
	Vector3 Camera(256, 256, -200);
	Vector3 SphereCenter(256, 256, 450);
	float SphereRadius = 400;
	Vector3 Light(128, 128, 0);

	//These light colours will give the sphere a red appearance
	Vector3 AmbientColour(20, 10, 10);
	Vector3 DiffuseColour(255, 128, 128);
	Vector3 BackgroundColor(64, 64, 128);	
	
	for (int i = 0; i < 512; ++ i)
		for (int j = 0; j < 512; ++j)
		{
			Pixel px;
			Vector3 PixelPosition((float)i, (float)j, 0);
			Vector3 Direction = Minus(PixelPosition, Camera);
			Direction = Normalize(Direction);

			//Ray-sphere intersection, please refer to the textbook
			//for the notations
			Vector3 OC = Minus(Camera, SphereCenter);			
			float D_Dot_OC = DotProduct(Direction, OC);
			float RadiusSquare = SphereRadius * SphereRadius;
			float OCSquare = DotProduct(OC, OC);
            float discriminant = RadiusSquare - OCSquare + D_Dot_OC * D_Dot_OC;
			float t = -1;

			if (discriminant > 0){
				float discRoot = sqrt(discriminant);
				float B = (-1) * DotProduct(Direction, OC);
                float t1 = B + discRoot;
                float t2 = B - discRoot;
				//determine which one is the real intersection point

				if (t1 > 0){
					if (t2 > 0){
						t = t2;
					}
					else { //Camera is in the sphere
						t = t1;
					}
				}
			}//if discrimiant > 0

			if (t > 0)
			{
				Vector3 Intersection = MultiplyScalar(Direction, t);
				Intersection = Add(Intersection, Camera);
				Vector3 Normal = Minus(Intersection, SphereCenter);
				Normal = Normalize(Normal);
				Vector3 LightVector = Minus(Light, Intersection);
				LightVector = Normalize(LightVector);

				float DiffuseTerm = DotProduct(LightVector, Normal);
				Vector3 PixelColour = AmbientColour;
				if (DiffuseTerm > 0)
				{
					Vector3 PixelDiffuseColour = MultiplyScalar(DiffuseColour, DiffuseTerm);
					PixelColour = Add(PixelColour, PixelDiffuseColour);
				}
				SetColor(px, PixelColour);
			}//if t > 0
			else //No Intersection, set background colour
			{
				SetColor(px, BackgroundColor);
			}
			 
			(*pImage)(i, j) = px;
		}
}
