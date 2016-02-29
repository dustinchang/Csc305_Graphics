#include "Canvas.h"
#include <math.h>
#include <iostream>
#include "Eigen/Dense"
using namespace std;
using namespace Eigen;

unsigned int width = 512;
unsigned int height = 512;

float vppos_x = 0;
float vppos_y = 0;
bool leftButtonPressed = false;
//Rotation
bool rotateCW = true; //Whether we're rotating clockwise or not
float rotateAngle = 0;
float rotateSpeed = 0.001;    //was 0.2

Canvas canvas;

//Use Eigen to produce the Perspective Matrix
//n = dist to near plane
//f = dist to far plane
Matrix4d perspecMat;
//Left Bottom Near (lbn)
Vector3d lbn;
Vector3d rbn;
Vector3d ltn;
Vector3d rtn;
Vector3d lbf;
Vector3d rbf;
Vector3d ltf;
Vector3d rtf;


void InitializeGL()
{

}

void MouseMove(double x, double y)
{
  //the pointer has moved
  vppos_x = (float)(x) / 256 - 1;
  vppos_y = 1 - (float)(y) / 256;
}

void MouseButton(MouseButtons mouseButton, bool press)
{
  //What to do with the keys?
  if (mouseButton == LeftButton)
  {
      if (press == true) leftButtonPressed = true;
      else leftButtonPressed = false;
  }
}

void KeyPress(char keychar)
{
  //A key is pressed! print a message
  fprintf(stderr, "The \"%c\" key is pressed!\n", keychar);
  //Rotate other way
  if(keychar == 'R') rotateCW = !rotateCW;
}

void OnPaint()
{
  canvas.Clear(); //Clears canvas
  //Now actually going to draw square
    for(int i = 0; i < 4; ++ i) {
      //Vector2 startPoint;

    }
}

void OnTimer()
{
  //Now create animation
  if(rotateCW) rotateAngle -= rotateSpeed;
  else rotateAngle += rotateSpeed;

}

int main(int, char **){
  //Create the Perspective Matrix
  perspecMat << 1, 0, 0, 0,
                0, 1, 0, 0,
                0, 1, 0, 0,  //0, 0, (n+f)/n, -f,
                0, 1, 0, 0;  //0, 0, (1/n), 0;

  lbn << 0.5, 0.5, 0.5;
  rbn << 0.5, 0.5, 0.5;
  ltn << 0.5, 0.5, 0.5;
  rtn << 0.5, 0.5, 0.5;
  lbf << 0.5, 0.5, 0.5;
  rbf << 0.5, 0.5, 0.5;
  ltf << 0.5, 0.5, 0.5;
  rtf << 0.5, 0.5, 0.5;
  cout << "lbn=" << lbn << endl;
  cout << "perspecMat=" << perspecMat << endl;

    //Link the call backs
    canvas.SetMouseMove(MouseMove);
    canvas.SetMouseButton(MouseButton);
    canvas.SetKeyPress(KeyPress);
    canvas.SetOnPaint(OnPaint);
    canvas.SetTimer(0.05, OnTimer);
    //Show Window
    canvas.Initialize(width, height, "OpenGL Intro Demo");
    //Do our initialization
    InitializeGL();
    canvas.Show();

    return 0;
}
