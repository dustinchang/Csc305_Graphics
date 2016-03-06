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
bool rightButtonPressed = false;
//Rotation
bool rotateCW = true; //Whether we're rotating clockwise or not
float rotateAngle = 0;
float rotateSpeed = 0.05;//0.005;    //was 0.2
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
bool starting = true;
bool change = false;
float r_y_pressed; //For when the RightButton is clicked set the y of vppos_y for starting point
float initial1 = 0;
float initial2 = 0;
float p_dist = 10;
float x_val = p_dist*sin(initial2)*sin(initial1);
float y_val = p_dist*cos(initial2);
float z_val = p_dist*sin(initial2)*cos(initial1);

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
  if(mouseButton == RightButton) {
    if(press == true) {
      rightButtonPressed = true;
      r_y_pressed = vppos_y;  //Set right mouseButton start point
    }
    else rightButtonPressed = false;
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
    //cout << "Vertices4d[i]=" << Vertices4d[i] << endl;
    //cout << "Vertices4d[i+1]=" << Vertices4d[i+1] << endl;
    //cout << "M=" << M << endl;
    p = M*Vertices4d[i];
    q = M*Vertices4d[i+1];
    //cout << "p=" << p << endl;
    //cout << "q=" << q << endl;
    float ph = p(2)/n;
    float qh = q(2)/n;
    //cout << "ph=" << ph << endl;
    //cout << "qh=" << qh << endl;

    canvas.AddLine(p(0)/ph, p(1)/ph, q(0)/qh, q(1)/qh);
    //canvas.AddLine(0,0,.8,0); //Goes right
    //canvas.AddLine(0,0,0,.8); //Goes up
  }
}

void OnTimer()
{
  //http://mathinsight.org/spherical_coordinates
  //Careful: Had to change to their coordinate systems
  //eyePos << 10*sin(initial1)*sin(initial2), 10*cos(initial1), 10*sin(initial1)*cos(initial2); //0, 0, 15; //0, 0, 2;
  //MAYBE use a start and end angle
  if(leftButtonPressed) {
    x_val = p_dist*sin(initial2)*sin(initial1);
    y_val = p_dist*cos(initial2);
    z_val = p_dist*sin(initial2)*cos(initial1);
    //eyePos << 10*sin(initial1)*sin(initial2), 10*cos(initial1), 10*sin(initial1)*cos(initial2);
    eyePos << x_val, -y_val, -z_val;
    change = true;
    //eyePos << p_dist*sin(initial2)*sin(initial1), p_dist*cos(initial2), p_dist*sin(initial2)*cos(initial1);  //Works for x and y direction but inverted
  } else if(rightButtonPressed) {
    p_dist -= (r_y_pressed-vppos_y)*.2;  //TODO in mouseButton as soon as right mouseButton pressed, set a value of that y and have vppos_y +- to that to possibly get a more accurate inc/dec by moving mouse up/down
    x_val = p_dist*sin(initial2)*sin(initial1);
    y_val = p_dist*cos(initial2);
    z_val = p_dist*sin(initial2)*cos(initial1);
    //eyePos << p_dist*sin(initial2)*sin(initial1), p_dist*cos(initial2), p_dist*sin(initial2)*cos(initial1);
    eyePos << x_val, y_val, z_val;
    change = false;
    std::cout << "IN HERE!!!!!!!!!!" << std::endl;
  } else {
    if(starting) {
      eyePos << 0, 0, 10;
    } else if(change) {
      eyePos << x_val, -y_val, -z_val;
    } else {
      eyePos << x_val, y_val, z_val;
    }
  }
  //eyePos << 10*sin(initial1)*sin(initial2), 10*cos(initial1), 10*sin(initial1)*cos(initial2);
  //eyePos << sin(vppos_x)*8, cos(vppos_y)*8, cos(vppos_x)*8;
  //eyePos << 0, cos(vppos_y)*8, sin(vppos_y)*8; //JUST Y
  //eyePos << sin(vppos_x)*8, 0, cos(vppos_x)*8; //JUST X

  cout << "10*sin(initial1)*sin(initial2)=" << 10*sin(initial1)*sin(initial2) << endl;
  cout << "10*cos(initial1)=" << 10*cos(initial1) << endl;
  cout << "10*sin(initial1)*cos(initial2)=" << 10*sin(initial1)*cos(initial2) << endl;
  cout << "vppos_x=" << vppos_x << endl;
  cout << "vppos_y=" << vppos_y << endl;
  cout << "initial1=" << initial1 << endl;
  cout << "initial2=" << initial2 << endl;
  cout << "x_val=" << x_val << endl;
  cout << "y_val=" << y_val << endl;
  cout << "z_val=" << z_val << endl;

  //Normalize eyePos
  gaze = -(eyePos.normalized());//(eyePos / sqrt(pow(eyePos(0), 2) + pow(eyePos(1), 2) + pow(eyePos(2), 2)))*-1;
  //gaze << sin(rot_val), 0, cos(rot_val);

  //Setup W, U, and V Vectors
  W = -(gaze.normalized()); /// sqrt(pow(gaze(0), 2) + pow(gaze(1), 2) + pow(gaze(2), 2));
  txW = viewUp.cross(W);
  U = (txW).normalized(); /// sqrt(pow(txW(0), 2) + pow(txW(1), 2) + pow(txW(2), 2));
  V = (W.cross(U)).normalized();
  //Setup for Mv
  Mvrot << U(0), U(1), U(2), 0,
          V(0), V(1), V(2), 0,
          W(0), W(1), W(2), 0,
          0, 0, 0, 1;
  xyz_1s << 1, 0, 0, -eyePos(0),
            0, 1, 0, -eyePos(1),
            0, 0, 1, -eyePos(2),
            0, 0, 0, 1;
  Mv = Mvrot*xyz_1s; //Mcam
  M = Mo*Mp*Mv;
  //cout << "rot_val" << rot_val << endl;
  //Testing for rotation direction around y axis
  if (change == false && rot_val < 1) {
    rot_val += rotateSpeed;
  } else {
    rot_val -= rotateSpeed;
    change = true;
  }
  initial1 = asin(vppos_x/p_dist);
  initial2 = acos(vppos_y/p_dist);
}

int main(int, char **){
  //Create the Perspective Matrix
  n = -1.0f;
  f = -10.0f;
  rot_val = 0;
  Mp << n, 0, 0, 0,
        0, n, 0, 0,
        0, 0, (n+f), -(f*n),
        0, 0, 1, 0;
  //Vertices
  lbn << -1, -1, 1, 1,
  rbn << 1, -1, 1, 1,
  ltn << -1, 1, 1, 1,
  rtn << 1, 1, 1, 1,
  lbf << -1, -1, -1, 1,
  rbf << 1, -1, -1, 1,
  ltf << -1, 1, -1, 1,
  rtf << 1, 1, -1, 1;

  //View positions
  viewUp << 0, 1, 0;



  float t = 1.0f;
  float r = 1.0f;
  float b = -1.0f;
  float l = -1.0f;
  Morth << 2/(r-l), 0, 0, -((r+l)/(r-l)),
          0, 2/(t-b), 0, -((t+b)/(t-b)),
          0, 0, 2/(n-f), -((n+f)/(n-f)),
          0, 0, 0, 1;
  /*Morth << 2/(.5-(-.5)), 0, 0, -((.5+(-.5)) / (.5-(-.5))),
           0, 2/(.5-(-.5)), 0, -((.5+(-.5)) / (.5-(-.5))),
           0, 0, 2/(-.25-.25), -((-.25+.25) / (-.25-.25)),
           0, 0, 0, 1;*/
  /*Mvp << -.25/2, 0, 0, (-.25-1)/2,
         0, -.5/2, 0, (-.5-1)/2,
         0, 0, 1, 0,
         0, 0, 0, 1;*/
  Mo = Morth;

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
