//Lab5
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
float n;
float f;

Matrix4f Mp;
Matrix4f Morth;
Matrix4f Mvrot;
Matrix4f xyz_1s;
Matrix4f Mv;
Matrix4f M;

Vector3f eyePos;
Vector3f gaze;
Vector3f viewUp;
Vector3f U;
Vector3f V;
Vector3f W;
Vector3f txW;

Canvas canvas;

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
  -.5f, .5f, -.5f,
  .5f, .5f, -.5f, //upper half of top square
  -.5f, .5f, .5f,
  .5f, .5f, .5f,
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

/*R[0][0] =  cos(rot);\
R[0][1] =  sin(rot);\
R[1][0] = -sin(rot);\
R[1][1] =  cos(rot);\*/

const char * vshader_square = "\
#version 330 core \n\
in vec3 vpoint; \
uniform float rotation;\
uniform mat4 view;\
uniform mat4 pers;\
uniform mat4 orth;\
uniform mat4 final_Matrix;\
\
mat4 RotationMatrix(float rot){\
  mat3 R = mat3(1);\
  R[0][0] = 1;\
  R[1][1] = cos(rot);\
  R[1][2] = -sin(rot);\
  R[2][1] = sin(rot);\
  R[2][2] = cos(rot);\
  return mat4(R); } \
void main() { \
  gl_Position = pers*view * vec4(vpoint, 1); \
}";
//Fragment shader
const char * fshader_square = "\
#version 330 core \n \
out vec3 color;\
void main() { color = vec3(1, 0, 0);}";

float Rotation = 0;
float RotatingSpeed = 0.02;

GLuint VertexArrayID = 0;
GLuint ProgramID = 0;
GLuint RotBindingID = 0;

void InitializeGL()
{
  //Generate Vertex Array Object
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  //Now want to upload triangle data onto Graphics card
  //Vertex Buffer Object
  GLuint vertexBufferID;
  glGenBuffers(1, &vertexBufferID);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
  glBufferData(GL_ARRAY_BUFFER,
                    sizeof(vpoint),
                    vpoint,
                    GL_STATIC_DRAW);
  //Shaders
  ProgramID = compile_shaders(vshader_square, fshader_square);
  glUseProgram(ProgramID);
  GLuint vpoint_id = glGetAttribLocation(ProgramID, "vpoint"); //vpoint_id is an address
  glEnableVertexAttribArray(vpoint_id); //enables that input slot
  //glBindBuffer(GL_ARRAY_BUFFER, vertexbufferID); //This line doesn't actually belong here cause done above
  glVertexAttribPointer(vpoint_id,
                        3,
                        GL_FLOAT,
                        false,
                        0,
                        0);

                        Mp << 1, 0, 0, 0,
                              0, 1, 0, 0,
                              0, 0, (n+f)/n, -f,
                              0, 0, 1/n, 0;
                        //View positions

                        float t = 1.0f;
                        float r = 1.0f;
                        float b = -1.0f;
                        float l = -1.0f;
                        Morth << 2/(r-l), 0, 0, -((r+l)/2),
                                0, 2/(t-b), 0, -((t+b)/2),
                                0, 0, 2/(n-f), -((n+f)/(n-f)),
                                0, 0, 0, 1;
    cout << "HERE" << endl;
    cout << Morth << endl;
    cout << Mp << endl;
  RotBindingID = glGetUniformLocation(ProgramID, "rotation");


}

void MouseMove(double x, double y) {
  //the pointer has moved
  vppos_x = (float)(x) / 256 - 1;
  vppos_y = 1 - (float)(y) / 256;
}

void MouseButton(MouseButtons mouseButton, bool press) {
}

void KeyPress(char keychar) {
}

void OnPaint() {
  glClear(GL_COLOR_BUFFER_BIT);
  //Context
  glUseProgram(ProgramID);
  glBindVertexArray(VertexArrayID);

  GLuint m_view = glGetUniformLocation(ProgramID, "view");
  GLuint m_pers = glGetUniformLocation(ProgramID, "pers");
  GLuint m_orth = glGetUniformLocation(ProgramID, "orth");
  glUniformMatrix4fv(m_view, 1, false, Mv.data());
  glUniformMatrix4fv(m_pers, 1, false, Mp.data());
  glUniformMatrix4fv(m_orth, 1, false, Morth.data());


  GLuint fin_matrix = glGetUniformLocation(ProgramID, "final_Matrix");
  glUniformMatrix4fv(fin_matrix, 1, false, M.data());
  //Draw
  glUniform1f(RotBindingID, Rotation);
  glDrawArrays(GL_TRIANGLES, 0, 12*3); //6 vertices cause 2 triangle
  //cleanup
  glUseProgram(0);
  glBindVertexArray(0);
}

void OnTimer() {
  eyePos << 0, 0, 15;
  Rotation += RotatingSpeed;
  //Nomalize eyePos
  gaze = -(eyePos.normalized());
  //Setup W, U, V Vectors
  W = -(gaze.normalized());
  txW = viewUp.cross(W);
  U = (txW).normalized();
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
  M = Morth*Mp*Mv;
  cout << "Mv" << Mv << endl;
  glUseProgram(ProgramID);
}

int main(int, char **){
  //Create the Perspective Matrix
  n = -1.0f;
  f = -50.0f;
  viewUp << 0, 1, 0;



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
