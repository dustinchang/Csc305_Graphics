#include "Image.h"
//#include "GLFW\include\glew.h" For Windows
#include "raytrace.h"


int main(int, char**){

    Image image(512, 512);

    for (int i = 0; i < 512; ++ i)
    {
        for (int j = 0; j < 512; ++ j)
        {
            Pixel px;
            //Set the pixel with some colour....
            px.R = i / 2;
            px.G = j / 2;
            px.B = (i + j) / 4;
            px.A = 255;

            image(i, j) = px;
        }
    }

    RayTraceSphere(&image);
    image.show("GLFW3+Libpng Image Window Demo");
    image.save("/Users/dustin/Desktop/demo.png");

    Pixel Black(0, 0, 0);
    Pixel White(255, 255, 255);
    int CheckerSize = 128;

    for (int i = 0; i<512; ++i) {
        for (int j = 0; j<512; ++j) {
            bool BlackOrWhite;
            //SOME CALCS
            int iOffset = i - (i / CheckerSize) * CheckerSize;
            int jOffset = j - (j / CheckerSize) * CheckerSize;

            int ibit = iOffset / (CheckerSize / 2);
            int jbit = jOffset / (CheckerSize / 2);

            BlackOrWhite = ibit ^ jbit;

            if (BlackOrWhite) image(i, j) = Black;
            else image(i, j) = White;
        }
    }

    image.show("Checker Board Example");
    image.save("/Users/dustin/Desktop/CheckerBoard.png");
}
