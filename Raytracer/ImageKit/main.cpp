#include "Image.h"
#include "raytrace.h"

int main(int, char**){
	Image raytraceImage(512, 512);
  RayTrace_Image(&raytraceImage);
	std::cout << "Got out of raytrace" << endl;
  raytraceImage.show("Ray Tracing Sphere Demo");
}
