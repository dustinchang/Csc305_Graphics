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
bool pressed2 = false;
bool pressedR = false;
bool x_dir = true;
bool y_dir = true;
bool z_dir = true;
float r_y_pressed; //For when the RightButton is clicked set the y of vppos_y for starting point
float initial1 = 0;
float initial2 = 0;
float p_dist = 10;
float x_val = p_dist*sin(initial2)*sin(initial1);
float y_val = p_dist*cos(initial2);
float z_val = p_dist*sin(initial2)*cos(initial1);
float end_x_val;
float end_y_val;
float end_z_val;
float end_angle_click1 = 0;
float end_angle_click2 = 0;
float init_click1 = 0;
float init_click2 = 0;
float angle1 = 0;
float angle2 = 0;

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
  if (mouseButton == LeftButton) {
      if (press == true) {
        leftButtonPressed = true;
        init_click1 = asin(vppos_x/p_dist); //To get the starting initial angle when the click happens
        init_click2 = acos(vppos_y/p_dist);
        cout << "In HERE!!!" << endl;
      }
      else leftButtonPressed = false;
  }
  if(mouseButton == RightButton) {
    if(press == true) {
      init_click1 = asin(vppos_x/p_dist); //To get the starting initial angle when the click happens
      init_click2 = acos(vppos_y/p_dist);
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

void OnPaint()
{
  canvas.Clear(); //Clears canvas
  for(int i=0; i < 24; i+=2) {
    p = M*Vertices4d[i];
    q = M*Vertices4d[i+1];
    float ph = p(2)/n;
    float qh = q(2)/n;

    canvas.AddLine(p(0)/ph, p(1)/ph, q(0)/qh, q(1)/qh);
  }
}

//Tutorial opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/
void OnTimer() {
  //http://mathinsight.org/spherical_coordinates; CAREFUL: Had to change to their coordinate systems
  //eyePos << 10*sin(initial1)*sin(initial2), 10*cos(initial1), 10*sin(initial1)*cos(initial2); //0, 0, 15; //0, 0, 2;
  initial1 = asin(vppos_x/p_dist);
  initial2 = acos(vppos_y/p_dist);

  if(initial1 != init_click1 && (pressed2==true || pressedR==true)) {
    angle1 = (initial1 - init_click1) + end_angle_click1;
    angle2 = (initial2 - init_click2) + end_angle_click2;
    cout << "IN IF!!!" << endl;
  } else {
    angle1 = /*initial1 + */end_angle_click1;
    angle2 = /*initial2 + */end_angle_click2;
    cout << "IN ELSE!!!" << endl;
  }

  if(leftButtonPressed) {
    x_val = p_dist*sin(angle2)*sin(angle1);//p_dist*sin(initial2)*sin(initial1);
    y_val = p_dist*cos(angle2);//p_dist*cos(initial2);
    z_val = p_dist*sin(angle2)*cos(angle1);//p_dist*sin(initial2)*cos(initial1);

    eyePos << x_val, -y_val, -z_val;
    change = true;

    if(initial1 != init_click1) {
      pressed2 = true;
    }
    pressedR = false;
  } else if(rightButtonPressed) {
    p_dist += (r_y_pressed-vppos_y)*.2;  //TODO in mouseButton as soon as right mouseButton pressed, set a value of that y and have vppos_y +- to that to possibly get a more accurate inc/dec by moving mouse up/down
    x_val = p_dist*sin(angle2)*sin(angle1);//p_dist*sin(initial2)*sin(initial1);
    y_val = p_dist*cos(angle2);//p_dist*cos(initial2);
    z_val = p_dist*sin(angle2)*cos(angle1);//p_dist*sin(initial2)*cos(initial1);
    eyePos << x_val, -y_val, -z_val;
    change = true;
    pressedR = true;
  } else {
    if(pressed2 || pressedR) {
          end_angle_click1 = angle1; //Should be angle1/2
          end_angle_click2 = angle2;
          end_x_val = x_val;//p_dist*sin(end_angle_click2)*sin(end_angle_click1);
          end_y_val = -y_val;//p_dist*cos(end_angle_click2);
          end_z_val = -z_val;//p_dist*sin(end_angle_click2)*cos(end_angle_click1);
    }
    if(starting) {
      end_angle_click1 = 0.000867082;//0.00115745;
      end_angle_click2 = 1.61556;//1.6061;
      x_val = p_dist*sin(end_angle_click2)*sin(end_angle_click1);
      y_val = p_dist*cos(end_angle_click2);
      z_val = p_dist*sin(end_angle_click2)*cos(end_angle_click1);
      eyePos << x_val, -y_val, -z_val;//0, 0, 10;
      starting = false;
    } else if(change) {
      eyePos << x_val, -y_val, -z_val;
    } else {
      eyePos << x_val, y_val, z_val;
    }
    pressed2 = false;
    pressedR = false;
  }

  cout << "initial1=" << initial1 << endl;
  cout << "initial2=" << initial2 << endl;
  cout << "init_click1=" << init_click1 << endl;
  cout << "init_click2=" << init_click2 << endl;
  cout << "x_val=" << x_val << endl;
  cout << "y_val=" << y_val << endl;
  cout << "z_val=" << z_val << endl;
  cout << "end_x_val=" << end_x_val << endl;
  cout << "end_y_val=" << end_y_val << endl;
  cout << "end_z_val=" << end_z_val << endl;
  cout << "end_angle_click1=" << end_angle_click1 << endl;
  cout << "end_angle_click2=" << end_angle_click2 << endl;

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
