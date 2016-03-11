#include "Canvas.h"
#include <math.h>

const GLfloat vpoint[] = {
  //Near side
  //-.5f, -.5f, 0.0f,
  //0.5f, -.5f, 0.0f,
  //-.5f, 0.5f, 0.0f, //upper half of the near square
  //0.5f, -.5f, 0.0f,
  //0.5f, 0.5f, 0.0f,
  //-.5f, 0.5f, 0.0f //lower half of the near square
  -.5f, -.5f, 0.5f,
  -.5f, .5f, 0.5f,
  .5f, .5f, 0.5f, //upper half of the near square
  -.5f, -.5f, 0.5f,
  .5f, -.5f, 0.5f,
  .5f, .5f, 0.5f, //lower half of the near square
  //Left side
  -.5f, .5f, 0.5f,
  -.5f, -.5f, 0.5f,
  -.5f, -.5f, -.5f, //lower half of the left square
  -.5f, .5f, 0.5f,
  -.5f, .5f, -0.5f,
  -.5f, -.5f, -.5f, //upper half of the left square
  //Right side
  .5f, .5f, .5f,
  .5f, -.5f, .5f,
  .5f, .5f, -.5f, //upper half of the right square
  .5f, .5f, -.5f,
  .5f, -.5f, .5f,
  .5f, -.5f, -.5f, //lower half of the right square
  //Top side
  -.5f, .5f, .5f,
  -.5f, .5f, -,5,
  .5f, .5f, -.5f //upper half of top square
  -.5f, .5f, .5f,
  .5f, .5f, ,5,
  .5f, .5f, -.5f, //lower half of top square
  //Bottom side
  -.5f, -.5f, -.5f,
  -.5f, -.5f, .5f,
  .5f, -.5f, .5f, //upper half of bottom square
  -.5f, -.5f, -.5f,
  .5f, -.5f, -.5f,
  .5f, -.5f, .5f, //lower half of bottom square
  //Back side
  -.5f, -.5f, -.5f,
  -.5f, .5f, -.5f,
  .5f, .5f, -.5f, //upper half of back square
  -.5f, -.5f, -.5f,
  .5f, -.5f, -.5f,
  .5f, .5f, -.5f //lower half of back square
};

void InitializeGL()
{
}

void MouseMove(double x, double y)
{
}

void MouseButton(MouseButtons mouseButton, bool press)
{
}

void KeyPress(char keychar)
{
}

void OnPaint()
{
}

void OnTimer()
{
}

int main(int, char **){
    //Link the call backs
    canvas.SetMouseMove(MouseMove);
    canvas.SetMouseButton(MouseButton);
    canvas.SetKeyPress(KeyPress);
    canvas.SetOnPaint(OnPaint);
    canvas.SetTimer(0.1, OnTimer);
    //Show Window
    canvas.Initialize(width, height, "Image Kit Demo");
    //Do our initialization
    InitializeGL();
    canvas.Show();
    return 0;
}
