#include "Canvas.h"
#include <math.h>

const GLfloat vpoint[] = {
       -.5f, -.5f, 0.0f,
       0.5f, -.5f, 0.0f,
       -.5f, 0.5f, 0.0f, //upper half of the square
       0.5f, -.5f, 0.0f,
       0.5f, 0.5f, 0.0f,
       -.5f, 0.5f, 0.0f //lower half of the squre
       };

const GLfloat vtexcoord[] = { 0.0f, 0.0f,
							  1.0f, 0.0f,
							  0.0f, 1.0f, //upper half of the square
							  1.0f, 0.0f,
							  1.0f, 1.0f,
							  0.0f, 1.0f }; //lower half of the square

const GLfloat vnormal[] = {
  0, 0, 1,
  0, 0, 1,
  0, 0, 1,
  0, 0, 1,
  0, 0, 1,
  0, 0, 1 //all points have the same normal!
};

const char * vshader_square = " \
    #version 330 core \n\
    in vec3 vnormal;\
    in vec3 vpoint; \
		in vec2 vtexcoord; \
    out vec3 object_pos;\
    out vec3 normal;\
		out vec2 uv; \
    uniform float rotation; \
    \
    mat4 RotationMatrix(float rot){\
        mat3 R = mat3(1);\
        R[0][0] =  cos(rot);\
        R[0][1] =  sin(rot);\
        R[1][0] = -sin(rot);\
        R[1][1] =  cos(rot);\
        return mat4(R);\
    }\
    void main() { \
		    uv = vtexcoord; \
        gl_Position = RotationMatrix(rotation) * vec4(vpoint, 1); \
        object_pos = gl_Position.xyz;\
        normal = vnormal;\
    } \
    ";

const char * fshader_square = " \
    #version 330 core \n\
		in vec2 uv; \
    in vec3 object_pos;\
    in vec3 normal;\
    uniform vec3 light_pos;\
    uniform vec3 camera_pos;\
    uniform vec3 specular_color;\
    out vec3 color; \
		uniform sampler2D tex;\
		uniform float texZoom; \
    void main() { \
		vec3 diffuse_color = texture(tex, uv).rgb;\
    vec3 ambient_color = diffuse_color * 0.1;\
    vec3 light_vec = light_pos - object_pos;\
    light_vec = normalize(light_vec);\
    float ndotl = dot(normal, light_vec);\
    if(ndotl>0) {\
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
    //ndotl = diffuse term
    //h = half vector

unsigned int width = 1000;
unsigned int height = 1000;

float Rotation = 0;
float RotatingSpeed = -0.1f;
float TexZoom = 1;
Canvas canvas;

//OpenGL context variables
GLuint programID = 0;
GLuint VertexArrayID = 0;
GLuint RotBindingID = 0;
GLuint TexZoomID = 0;

bool leftButtonPressed = false;
double lastYPosition = 0;

void InitializeGL()
{
    //Compile the shaders
    programID = compile_shaders(vshader_square, fshader_square);

    //Generate Vertex Array and bind the vertex buffer data
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    ///--- Bind the vertex position
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    /// The subsequent commands will affect the specified buffer
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    /// Pass the vertex positions to OpenGL
    glBufferData(GL_ARRAY_BUFFER, sizeof(vpoint), vpoint, GL_STATIC_DRAW);

    ///--- find the binding point in the shader:
    /// "vpoint" in the vertex shader
    glUseProgram(programID);
    GLuint vpoint_id = glGetAttribLocation(programID, "vpoint");
    glEnableVertexAttribArray(vpoint_id);
    glVertexAttribPointer(vpoint_id,
                          3, //size per vertex (3 floats for cord)
                          GL_FLOAT,
                          false, //don't normalize
                          0, //stride = 0
                          0); //offset = 0

	/// --- Bind the texture coordinate
	GLuint texcoordbuffer;
	glGenBuffers(1, &texcoordbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, texcoordbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vtexcoord), vtexcoord, GL_STATIC_DRAW);
	GLuint texcoordBindingPosition = glGetAttribLocation(programID, "vtexcoord");
	glEnableVertexAttribArray(texcoordBindingPosition);
	glVertexAttribPointer(texcoordBindingPosition, 2, GL_FLOAT,
		GL_FALSE, 0, (void *)0);

  /// --- Upload normals, Bind the normals
  GLuint vnormalbuffer;
  glGenBuffers(1, &vnormalbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vnormalbuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vnormal), vnormal, GL_STATIC_DRAW);
  GLuint vnormal_id = glGetAttribLocation(programID, "vnormal");
  glEnableVertexAttribArray(vnormal_id);
  glVertexAttribPointer(vnormal_id, 3, GL_FLOAT, GL_FALSE, 0, 0); //3 because of 3 coordinate vector this time

	/// --- Load the texture image
	Texture teximage = LoadPNGTexture("texture.png");

	/// --- Set openGL texture parameters
	GLuint texobject;
	glGenTextures(1, &texobject);
	glBindTexture(GL_TEXTURE_2D, texobject);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, teximage.width,
		teximage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
		teximage.dataptr);

	/// --- Activate the loaded texture in channel 0
	GLuint tex_bindingpoint = glGetUniformLocation(programID, "tex");
	glUniform1i(tex_bindingpoint, 0 /*GL_TEXTURE0*/);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texobject);

    //Find the binding point for the uniform variable
    //"rotation" in vertex shader
    RotBindingID = glGetUniformLocation(programID, "rotation");
	TexZoomID = glGetUniformLocation(programID, "texZoom");

  //Upload light_pos, camera_pos, and specular_color
  GLfloat Light_Pos[3];
  Light_Pos[0] = 2; //just random place chosen
  Light_Pos[1] = 2;
  Light_Pos[2] = 5;
  GLuint light_pos_id = glGetUniformLocation(programID, "light_pos");
  glUniform3f(light_pos_id, Light_Pos[0], Light_Pos[1], Light_Pos[2]);
  //camera_pos
  GLfloat Camera_Pos[3];
  Camera_Pos[0] = 0;
  Camera_Pos[1] = 0;
  Camera_Pos[2] = 1;
  GLuint camera_pos_id = glGetUniformLocation(programID, "camera_pos");
  glUniform3f(camera_pos_id, Camera_Pos[0], Camera_Pos[1], Camera_Pos[2]);
  //specular_color
  GLfloat Specular_Color[3];
  Specular_Color[0] = 1;
  Specular_Color[1] = 1;
  Specular_Color[2] = 1;
  GLuint specular_pos_id = glGetUniformLocation(programID, "specular_color");
  glUniform3f(specular_pos_id, Specular_Color[0], Specular_Color[1], Specular_Color[2]);


}

void MouseMove(double x, double y)
{
	if (leftButtonPressed == true)
	{
		TexZoom += (y - lastYPosition) * 0.002f;
		if (TexZoom >= 1) TexZoom = 1;
		if (TexZoom < 0.001) TexZoom = 0.001;
	}
	lastYPosition = y;
}

void MouseButton(MouseButtons mouseButton, bool press)
{
	if (mouseButton == LeftButton)
	{
		if (press == true) leftButtonPressed = true;
		else leftButtonPressed = false;
	}
}

void KeyPress(char keychar)
{

}

void OnPaint()
{
    //Binding the openGL context
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(programID);
    glBindVertexArray(VertexArrayID);
        //upload current rotation to GPU
        glUniform1f(RotBindingID, Rotation);
		glUniform1f(TexZoomID, TexZoom);
        glDrawArrays(GL_TRIANGLES, 0 /*buffer offset*/, 6 /*#vertices*/);
    //Clean up the openGL context for other drawings
    glUseProgram(0);
    glBindVertexArray(0);
}

void OnTimer()
{
    Rotation += RotatingSpeed;
}

int main(int, char **){
    //Link the call backs
    canvas.SetMouseMove(MouseMove);
    canvas.SetMouseButton(MouseButton);
    canvas.SetKeyPress(KeyPress);
    canvas.SetOnPaint(OnPaint);
    canvas.SetTimer(0.05, OnTimer);
    //Show Window
    canvas.Initialize(width, height, "Rotating Square Demo");
    //Do our initialization
    InitializeGL();
    canvas.Show();

    return 0;
}
