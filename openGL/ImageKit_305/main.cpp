#include "Canvas.h"
#include <math.h>

unsigned int width = 512;
unsigned int height = 512;

float vppos_x = 0;
float vppos_y = 0;
bool leftButtonPressed = false;
//Rotation
bool rotateCW = true; //Whether we're rotating clockwise or not
float rotateAngle = 0;
float rotateSpeed = 0.07;    //was 0.2

Canvas canvas;

struct Vector2 {
  float x, y;
  Vector2(float x_in, float y_in) {
    x = x_in; y = y_in;
  }
  Vector2() {x = y = 0;}
};

//To create square now
std::vector<Vector2> SquareVertices;

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
      Vector2 startPoint;
      //If do nothing at all, but we want to rotate that point so we add all this formula from tutorial/textbook
      startPoint.x = SquareVertices[i].x * cos(rotateAngle) - SquareVertices[i].y * sin(rotateAngle); //Change to -1 if you want to expand
      startPoint.y = SquareVertices[i].x * sin(rotateAngle) + SquareVertices[i].y * cos(rotateAngle);
      Vector2 endPoint;
      endPoint.x = SquareVertices[i + 1].x * cos(rotateAngle) - SquareVertices[i + 1].y * sin(rotateAngle);
      endPoint.y = SquareVertices[i + 1].x * sin(rotateAngle) + SquareVertices[i + 1].y * cos(rotateAngle);
      canvas.AddLine(startPoint.x, startPoint.y, endPoint.x, endPoint.y);
    }
}

void OnTimer()
{
  //Now create animation
  if(rotateCW) rotateAngle -= rotateSpeed;
  else rotateAngle += rotateSpeed;

}

int main(int, char **){
  //Init of square coordinate
  SquareVertices.push_back(Vector2(-0.5, -0.5));
  SquareVertices.push_back(Vector2(0.5, -0.5));
  SquareVertices.push_back(Vector2(0.5, 0.5));
  SquareVertices.push_back(Vector2(-0.5, 0.5));
  SquareVertices.push_back(Vector2(-0.5, -0.5));
  
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
