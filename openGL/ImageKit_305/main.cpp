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
Matrix4d Mvrot;
Matrix4d Mv;
//Left Bottom Near (lbn)
Vector3d lbn;
Vector3d rbn;
Vector3d ltn;
Vector3d rtn;
Vector3d lbf;
Vector3d rbf;
Vector3d ltf;
Vector3d rtf;

Vector3d eyePos;
Vector3d gaze;
Vector3d viewUp;

Vector3d W;
Vector3d U;
Vector3d V;
Vector3d txW;

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

void DrawCube(float x_center, float y_center, float z_center) {
  //canvas.AddLine(x_center + lbn(0), y_center + lbn(1), z_center + lbn(2), x_center + rbn(0), y_center + rbn(1), z_center + rbn(2));
  //canvas.AddLine(x_center + lbn(0), y_center + lbn(1), z_center + lbn(2), x_center + ltn(0), y_center + ltn(1), z_center + ltn(2));
  //canvas.AddLine(x_center + lbn(0), y_center + lbn(1), z_center + lbn(2), x_center + lbf(0), y_center + lbf(1), z_center + lbf(2));

  //canvas.AddLine(0, 0, 0, 0.3, 0, 0);
  //canvas.AddLine(0, 0, 0, 0, 0.5, 0);
}

void OnPaint()
{
  canvas.Clear(); //Clears canvas
  //if (leftButtonPressed == true) {
    DrawCube(0,0,0);//vppos_x, vppos_y);
  //}

}

void OnTimer()
{
  //Now create animation
  if(rotateCW) rotateAngle -= rotateSpeed;
  else rotateAngle += rotateSpeed;

}

int main(int, char **){
  //Create the Perspective Matrix
  int n = 1;
  int f = 2;
  perspecMat << 1, 0, 0, 0,
                0, 1, 0, 0,
                0, 0, (n+f)/n, -f,
                0, 0, (1/n), 0;

  //Vertices
  lbn << -0.5, 0.5, 0.5;
  rbn << 0.5, 0.5, 0.5;
  ltn << -0.5, -0.5, 0.5;
  rtn << 0.5, -0.5, 0.5;
  lbf << -0.7, 0.7, -0.7;
  rbf << 0.7, 0.7, -0.7;
  ltf << -0.7, -0.7, -0.7;
  rtf << 0.7, -0.7, -0.7;
  //View positions
  eyePos << 0, 0, 0;
  gaze << 0, 0, 1;
  viewUp << 0, 1, 0;
  //Setup W, U, and V Vectors
  W = gaze / sqrt(pow(gaze(0), 2) + pow(gaze(1), 2) + pow(gaze(2), 2));
  txW = viewUp.cross(W);
  U = txW / sqrt(pow(txW(0), 2) + pow(txW(1), 2) + pow(txW(2), 2));
  V = W.cross(U);

  Mvrot << U(0), U(1), U(2), 0,
          V(0), V(1), V(2), 0,
          W(0), W(1), W(2), 0,
          0, 0, 0, 1;
  Mv = Mvrot*perspecMat;


  cout << "W=" << W << endl;
  cout << "txW=" << txW << endl;
  cout << "U=" << U << endl;
  cout << "V=" << V << endl;
  cout << "Mvrot=" << Mvrot << endl;
  cout << "Mv=" << Mv << endl;

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
