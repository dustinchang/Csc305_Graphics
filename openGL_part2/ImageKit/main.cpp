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
float p_dist = 5;
float initial_clickX = 0;
float initial_clickY = 0;
float initial1 = 0;
float initial2 = 0;
float r_y_pressed;
float end_angle_click1 = 0;
float end_angle_click2 = 0;
float end_x_val;
float end_y_val;
float end_z_val;
float angle1 = 0;
float angle2 = 0;
float x_val = p_dist*sin(initial2)*sin(initial1);
float y_val = p_dist*cos(initial2);
float z_val = p_dist*sin(initial2)*cos(initial1);
bool starting = true;
bool leftButtonPressed = false;
bool rightButtonPressed = false;
bool pressed2 = false;
bool pressedR = false;
bool change = false;

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
  -0.5f,-0.5f,-0.5f,
  -0.5f,-0.5f, 0.5f,
  -0.5f, 0.5f, 0.5f,
  0.5f, 0.5f,-0.5f,
  -0.5f,-0.5f,-0.5f,
  -0.5f, 0.5f,-0.5f,

  0.5f,-0.5f, 0.5f,
  -0.5f,-0.5f,-0.5f,
  0.5f,-0.5f,-0.5f,
  0.5f, 0.5f,-0.5f,
  0.5f,-0.5f,-0.5f,
  -0.5f,-0.5f,-0.5f,

  -0.5f,-0.5f,-0.5f,
  -0.5f, 0.5f, 0.5f,
  -0.5f, 0.5f,-0.5f,
  0.5f,-0.5f, 0.5f,
  -0.5f,-0.5f, 0.5f,
  -0.5f,-0.5f,-0.5f,

  -0.5f, 0.5f, 0.5f,
  -0.5f,-0.5f, 0.5f,
  0.5f,-0.5f, 0.5f,
  0.5f, 0.5f, 0.5f,
  0.5f,-0.5f,-0.5f,
  0.5f, 0.5f,-0.5f,

  0.5f,-0.5f,-0.5f,
  0.5f, 0.5f, 0.5f,
  0.5f,-0.5f, 0.5f,
  0.5f, 0.5f, 0.5f,
  0.5f, 0.5f,-0.5f,
  -0.5f, 0.5f,-0.5f,

  0.5f, 0.5f, 0.5f,
  -0.5f, 0.5f,-0.5f,
  -0.5f, 0.5f, 0.5f,
  0.5f, 0.5f, 0.5f,
  -0.5f, 0.5f, 0.5f,
  0.5f,-0.5f, 0.5f
};

const GLfloat vtexcoord[] = {
  1.0f, 1.0f,
  0.0f, 1.0f,
  0.0f, 0.0f, //upper half of the square

  0.0f, 0.0f,
  1.0f, 1.0f,
  1.0f, 0.0f, //lower half of the square

  0.0f, 0.0f,
  1.0f, 1.0f,
  1.0f, 0.0f,

  0.0f, 0.0f,
  0.0f, 1.0f,
  1.0f, 1.0f,

  1.0f, 1.0f,
  0.0f, 0.0f,
  1.0f, 0.0f,

  0.0f, 0.0f,
  0.0f, 1.0f,
  1.0f, 1.0f,

  0.0f, 0.0f,
  0.0f, 1.0f,
  1.0f, 1.0f,

  0.0f, 0.0f,
  1.0f, 1.0f,
  1.0f, 0.0f,

  1.0f, 1.0f,
  0.0f, 0.0f,
  0.0f, 1.0f,

  0.0f, 0.0f,
  0.0f, 1.0f,
  1.0f, 1.0f,

  0.0f, 0.0f,
  1.0f, 1.0f,
  1.0f, 0.0f,

  1.0f, 0.0f,
  0.0f, 0.0f,
  1.0f, 1.0f,
};

const GLfloat vnormal[] = {
  0, 0, 1,
  0, 0, 1,
  0, 0, 1,
  0, 0, 1,
  0, 0, 1,
  0, 0, 1, //all points have the same normal!
  0, 0, 1,
  0, 0, 1,
  0, 0, 1,
  0, 0, 1,
  0, 0, 1,
  0, 0, 1, //all points have the same normal!
  0, 0, 1,
  0, 0, 1,
  0, 0, 1,
  0, 0, 1,
  0, 0, 1,
  0, 0, 1, //all points have the same normal!
  0, 0, 1,
  0, 0, 1,
  0, 0, 1,
  0, 0, 1,
  0, 0, 1,
  0, 0, 1, //all points have the same normal!
  0, 0, 1,
  0, 0, 1,
  0, 0, 1,
  0, 0, 1,
  0, 0, 1,
  0, 0, 1, //all points have the same normal!
  0, 0, 1,
  0, 0, 1,
  0, 0, 1,
  0, 0, 1,
  0, 0, 1,
  0, 0, 1 //all points have the same normal!
};

const char * vshader_square = "\
#version 330 core \n\
in vec3 vnormal;\
in vec3 vpoint; \
in vec2 vtexcoord; \
out vec3 object_pos;\
out vec3 normal;\
out vec2 uv; \
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
  uv = vtexcoord; \
  gl_Position = final_Matrix * vec4(vpoint, 1); \
  object_pos = gl_Position.xyz;\
  normal = vnormal;\
}";
//Fragment shader
const char * fshader_square = "\
#version 330 core \n \
in vec2 uv;\
in vec3 object_pos;\
in vec3 normal;\
uniform vec3 light_pos;\
uniform vec3 camera_pos;\
uniform vec3 specular_color;\
out vec3 color; \
uniform sampler2D tex;\
void main() {\
  vec3 diffuse_color = texture(tex, uv).rgb;\
  vec3 ambient_color = diffuse_color * 0.1;\
  vec3 light_vec = light_pos - object_pos;\
  light_vec = normalize(light_vec);\
  float ndotl = dot(normal, light_vec);\
  if(ndotl > 0) {\
    vec3 eye_vec = camera_pos - object_pos;\
    eye_vec = normalize(eye_vec);\
    vec3 h = eye_vec + light_vec;\
    h = normalize(h);\
    float hdotn= dot(h, normal);\
    float specular_term = pow(hdotn, 36);\
    color = ambient_color + diffuse_color * ndotl + specular_color * specular_term;\
  }\
  else color = ambient_color;\
}\
";

float Rotation = 0;
float RotatingSpeed = 0.02;
float RotatingAngle = M_PI * 0.3;

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
  RotBindingID = glGetUniformLocation(ProgramID, "rotation");

  //Texture part
  GLuint vtexcoordbuffer;
  glGenBuffers(1, &vtexcoordbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vtexcoordbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vtexcoord), vtexcoord, GL_STATIC_DRAW);
  GLuint vtexcoord_bindingPosition = glGetAttribLocation(ProgramID, "vtexcoord");
  glEnableVertexAttribArray(vtexcoord_bindingPosition);
  glVertexAttribPointer(vtexcoord_bindingPosition, 2, GL_FLOAT, false, 0, 0);
  /// --- Upload normals, Bind the normals, for specular part
  GLuint vnormalbuffer;
  glGenBuffers(1, &vnormalbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vnormalbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vnormal), vnormal, GL_STATIC_DRAW);
  GLuint vnormal_id = glGetAttribLocation(ProgramID, "vnormal");
  glEnableVertexAttribArray(vnormal_id);
  glVertexAttribPointer(vnormal_id, 3, GL_FLOAT, GL_FALSE, 0, 0); //3 because of 3 coordinate vector this time

  //Load Texture
  Texture teximage = LoadPNGTexture("/Volumes/dustinch/Csc305/Csc305_Graphics/openGL_part2/ImageKit/code.png");
  //Upload this image onto GPU
  GLuint texobject;
  glGenTextures(1, &texobject);
  glBindTexture(GL_TEXTURE_2D, texobject);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //the i at the end means to integer
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, teximage.width, teximage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, teximage.dataptr);
  //Activate the texture image
  GLuint sampler_ID = glGetUniformLocation(ProgramID, "tex");
  glUniform1i(sampler_ID, 0);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texobject);
  glEnable(GL_CULL_FACE);

  //Upload light_pos, camera_pos, and specular_color
  GLfloat Light_Pos[3];
  Light_Pos[0] = 2; //just random place chosen
  Light_Pos[1] = 2;
  Light_Pos[2] = 5;
  GLuint light_pos_id = glGetUniformLocation(ProgramID, "light_pos");
  glUniform3f(light_pos_id, Light_Pos[0], Light_Pos[1], Light_Pos[2]);
  //camera_pos
  GLfloat Camera_Pos[3];
  Camera_Pos[0] = 0;
  Camera_Pos[1] = 0;
  Camera_Pos[2] = 1;
  GLuint camera_pos_id = glGetUniformLocation(ProgramID, "camera_pos");
  glUniform3f(camera_pos_id, Camera_Pos[0], Camera_Pos[1], Camera_Pos[2]);
  //specular_color
  GLfloat Specular_Color[3];
  Specular_Color[0] = 1;
  Specular_Color[1] = 1;
  Specular_Color[2] = 1;
  GLuint specular_pos_id = glGetUniformLocation(ProgramID, "specular_color");
  glUniform3f(specular_pos_id, Specular_Color[0], Specular_Color[1], Specular_Color[2]);

  //Matrix calcualtions
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
}

void MouseMove(double x, double y) {
  //the pointer has moved
  vppos_x = (float)(x) / 256 - 1;
  vppos_y = 1 - (float)(y) / 256;
}

void MouseButton(MouseButtons mouseButton, bool press) {
  if(mouseButton == LeftButton) {
    if(press == true) {
      leftButtonPressed = true;
      initial_clickX = asin(vppos_x/p_dist);
      initial_clickY = acos(vppos_y/p_dist);
    } else {
      leftButtonPressed = false;
    }
  }
  if(mouseButton == RightButton) {
    if(press == true) {
      rightButtonPressed = true;
      initial_clickX = asin(vppos_x/p_dist); //To get the starting initial angle when the click happens
      initial_clickY = acos(vppos_y/p_dist);
      r_y_pressed = vppos_y; //Set right mouseButton start point
    } else {
      rightButtonPressed = false;
    }
  }
}

void KeyPress(char keychar) {
}

void OnPaint() {
  glClear(GL_COLOR_BUFFER_BIT);
  //Context
  glUseProgram(ProgramID);
  glBindVertexArray(VertexArrayID);

  //Additional matrices
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
  //glDrawArrays again after adding a new matrix with different translations HERE
  //Chap6 transformations



  //cleanup
  glUseProgram(0);
  glBindVertexArray(0);
}

void OnTimer() {
  //Top part just for rotation
  initial1 = asin(vppos_x/p_dist);
  initial2 = acos(vppos_y/p_dist);
  //Cases for getting the current angles
  if(initial1 != initial_clickX && (pressed2==true || pressedR==true)) {
    angle1 = (initial1 - initial_clickX) + end_angle_click1;
    angle2 = (initial2 - initial_clickY) + end_angle_click2;
  } else {
    angle1 = end_angle_click1;
    angle2 = end_angle_click2;
  }
  //Button press cases
  if(leftButtonPressed) {
    x_val = p_dist*sin(angle2)*sin(angle1);
    y_val = p_dist*cos(angle2);
    z_val = p_dist*sin(angle2)*cos(angle1);
    eyePos << x_val, -y_val, -z_val;
    change = true;
    if(initial1 != initial_clickX) {
      pressed2 = true;
    }
    pressedR = false;
  } else if(rightButtonPressed) {
    p_dist += (r_y_pressed-vppos_y)*.2;
    x_val = p_dist*sin(angle2)*sin(angle1);
    y_val = p_dist*cos(angle2);
    z_val = p_dist*sin(angle2)*cos(angle1);
    eyePos << x_val, -y_val, -z_val;
    change = true;
    pressedR = true;
  } else {
    if(pressed2 || pressedR) {
          end_angle_click1 = angle1;
          end_angle_click2 = angle2;
          end_x_val = x_val;
          end_y_val = -y_val;
          end_z_val = -z_val;
    }
    if(starting) {
      end_angle_click1 = 0.000867082;
      end_angle_click2 = 1.61556;
      x_val = p_dist*sin(end_angle_click2)*sin(end_angle_click1);
      y_val = p_dist*cos(end_angle_click2);
      z_val = p_dist*sin(end_angle_click2)*cos(end_angle_click1);
      eyePos << x_val, -y_val, -z_val;
      starting = false;
    } else if(change) {
      eyePos << x_val, -y_val, -z_val;
    } else {
      eyePos << x_val, y_val, z_val;
    }
    pressed2 = false;
    pressedR = false;
  }

  //Calculations for matrices
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
  glUseProgram(ProgramID);
}

int main(int, char **){
  //Create the Perspective Matrix
  n = -1.0f;
  f = -10.0f;
  viewUp << 0, 1, 0;

    //Link the call backs
    canvas.SetMouseMove(MouseMove);
    canvas.SetMouseButton(MouseButton);
    canvas.SetKeyPress(KeyPress);
    canvas.SetOnPaint(OnPaint);
    canvas.SetTimer(0.01, OnTimer);
    //Show Window
    canvas.Initialize(width, height, "OpenGL Intro Demo");
    //Do our initialization
    InitializeGL();
    canvas.Show();
    return 0;
}
