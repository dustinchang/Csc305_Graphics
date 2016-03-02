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
float rotateSpeed = 0.07;    //was 0.2
float rot_val = 0;

Canvas canvas;

//Use Eigen to produce the Perspective Matrix
//n = dist to near plane
//f = dist to far plane
Matrix4d Mp;
Matrix4d Mvrot;
Matrix4d Mv;
Matrix4d Mo;
Matrix4d Mvp;
Matrix4d Morth;
Matrix4d xyz_1s;
Matrix4d M;

Vector4d homogenous;
Vector4d MpHomo;
Vector4d p;
Vector4d q;
//Left Bottom Near (lbn)
Vector4d lbn;
Vector4d rbn;
Vector4d ltn;
Vector4d rtn;
Vector4d lbf;
Vector4d rbf;
Vector4d ltf;
Vector4d rtf;

Vector3d eyePos;
Vector3d gaze;
Vector3d viewUp;

Vector3d W;
Vector3d U;
Vector3d V;
Vector3d txW;

float n;
float f;

std::vector<Vector4d> Vertices4d;

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
    //DrawCube(0,0,0);//vppos_x, vppos_y);
  //}
  for(int i=0; i < 24; i+=2) {
    p = M*Vertices4d[i];
    q = M*Vertices4d[i+1];
    // cout << "M=" << M << endl;
    // cout << "p=" << p << endl;
    // cout << "q=" << q << endl;
    // cout << "Vertices4d[i]=" << Vertices4d[i] << endl;
    // cout << "Vertices4d[i+1]=" << Vertices4d[i+1] << endl;

    //canvas.AddLine(Vertices4d[i](0), Vertices4d[i](1), Vertices4d[i+1](0), Vertices4d[i+1](1));
    canvas.AddLine(p(0), p(1), q(0), q(1));
    //canvas.AddLine(0,0,.8,0); //Goes right
    //canvas.AddLine(0,0,0,.8); //Goes up
  }

}

void OnTimer()
{
  //Now create animation
  if(rotateCW) rotateAngle -= rotateSpeed;
  else rotateAngle += rotateSpeed;
  //Was in main
  eyePos << sin(rot_val), 0, cos(rot_val); //0, 0, 2;
  //Normalize eyePos
  gaze = (eyePos / sqrt(pow(eyePos(0), 2) + pow(eyePos(1), 2) + pow(eyePos(2), 2)))*-1;
  //gaze << sin(rot_val), 0, cos(rot_val);

  //Setup W, U, and V Vectors
  W = gaze / sqrt(pow(gaze(0), 2) + pow(gaze(1), 2) + pow(gaze(2), 2));
  txW = viewUp.cross(W);
  U = txW / sqrt(pow(txW(0), 2) + pow(txW(1), 2) + pow(txW(2), 2));
  V = W.cross(U);

  Mvrot << U(0), U(1), U(2), 0,
          V(0), V(1), V(2), 0,
          W(0), W(1), W(2), 0,
          0, 0, 0, 1;
  Mv = Mvrot*xyz_1s; //*Mp;

  homogenous << 0, 0, 0, 1;
  MpHomo = Mp*homogenous;
  cout << "MpHomo1=" << MpHomo << endl;
  MpHomo /= (0/n);
  cout << "Mv=" << Mv << endl;
  cout << "MpHomo=" << MpHomo << endl;
  M = /*Mo**/Mv*Mp;//Mv*MpHomo; //was Mv*Mp;  Mv*MpHomo doens't work cause it produces a Vector4d


  cout << "rot_val" << rot_val << endl;
  rot_val += rotateSpeed;

}

int main(int, char **){
  //Create the Perspective Matrix
  n = -1;
  f = -1;
  rot_val = 0;
  Mp << 1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, (n+f)/n, -f,
        0, 0, (1/n), 0;
  //Vertices
  lbn << -0.5, 0.5, -0.25, 1;
  rbn << 0.5, 0.5, -0.25, 1;
  ltn << -0.5, -0.5, -0.25, 1;
  rtn << 0.5, -0.5, -0.25, 1;
  lbf << -0.5, 0.5, -0.75, 1;//lbf << -0.7, 0.7, -0.7, 1;
  rbf << 0.5, 0.5, -0.75, 1;//rbf << 0.7, 0.7, -0.7, 1;
  ltf << -0.5, -0.5, -0.75, 1;//ltf << -0.7, -0.7, -0.7, 1;
  rtf << 0.5, -0.5, -0.75, 1;//rtf << 0.7, -0.7, -0.7, 1;
  //View positions


  viewUp << 0, 1, 0;
  //Setup for Mv
  xyz_1s << 1, 0, 0, -eyePos(0),
            0, 1, 0, -eyePos(1),
            0, 0, 1, -eyePos(2),
            0, 0, 0, 1;
  Morth << 2/(.5-(-.5)), 0, 0, -((.5+(-.5)) / (.5-(-.5))),
           0, 2/(.5-(-.5)), 0, -((.5+(-.5)) / (.5-(-.5))),
           0, 0, 2/(-.25-.25), -((-.25+.25) / (-.25-.25)),
           0, 0, 0, 1;
  Mvp << -.25/2, 0, 0, (-.25-1)/2,
         0, -.5/2, 0, (-.5-1)/2,
         0, 0, 1, 0,
         0, 0, 0, 1;
  Mo = Mvp*Morth;





  //cout << "W=" << W << endl;
  //cout << "txW=" << txW << endl;
  //cout << "U=" << U << endl;
  //cout << "V=" << V << endl;
  //cout << "Mvrot=" << Mvrot << endl;
  //cout << "Mv=" << Mv << endl;
  //cout << "M=" << M << endl;

  //Vertices4d.push_back(lbn, rbn);
  //Vertices4d.push_back(lbn, ltn);
  //Vertices4d.push_back(lbn, lbf);
  //Vertices4d.push_back(rbn, rtn);
  //Vertices4d.push_back(rbn, rbf);
  //Vertices4d.push_back(ltn, rtn);
  //Vertices4d.push_back(ltn, ltf);
  //Vertices4d.push_back(rtn, rtf);
  //Vertices4d.push_back(lbf, rbf);
  //Vertices4d.push_back(lbf, ltf);
  //Vertices4d.push_back(rbf, rtf);
  //Vertices4d.push_back(ltf, rtf);
  Vertices4d.push_back(lbn);
  Vertices4d.push_back(rbn);
  Vertices4d.push_back(lbn);
  Vertices4d.push_back(ltn);
  Vertices4d.push_back(lbn);
  Vertices4d.push_back(lbf);
  Vertices4d.push_back(rbn);
  Vertices4d.push_back(rtn);
  Vertices4d.push_back(rbn);
  Vertices4d.push_back(rbf);
  Vertices4d.push_back(ltn);
  Vertices4d.push_back(rtn);
  Vertices4d.push_back(ltn);
  Vertices4d.push_back(ltf);
  Vertices4d.push_back(rtn);
  Vertices4d.push_back(rtf);
  Vertices4d.push_back(lbf);
  Vertices4d.push_back(rbf);
  Vertices4d.push_back(lbf);
  Vertices4d.push_back(ltf);
  Vertices4d.push_back(rbf);
  Vertices4d.push_back(rtf);
  Vertices4d.push_back(ltf);
  Vertices4d.push_back(rtf);

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
