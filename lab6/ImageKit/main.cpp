#include "Canvas.h"
#include <math.h>

unsigned int width = 512;
unsigned int height = 512;

float Rotation = 0;
float RotatingSpeed = 0.02;
Canvas canvas;

const GLfloat vpoint[] = {
       -.5f, -.5f, 0.0f,
       0.5f, -.5f, 0.0f,
       -.5f, 0.5f, 0.0f, //upper half of the square
       0.5f, -.5f, 0.0f,
       0.5f, 0.5f, 0.0f,
       -.5f, 0.5f, 0.0f //lower half of the squre
       };

const GLfloat vtexcoord[] = {
  0, 1,
  1, 1,
  0, 0, //upper half of the square
  1, 1,
  1, 0,
  0, 0, //lower half of the square
};

const char * vshader_square = " \
        #version 330 core \n\
        in vec3 vpoint; \
        in vec2 vtexcoord; \
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
        } \
        ";

const char * fshader_square = " \
        #version 330 core \n\
        out vec3 color; \
        in vec2 uv; \
        uniform sampler2D tex; \
        void main() { color = texture(tex, uv).rgb * texture(tex, uv).a;} //Set pixel to red \
        ";
        //Adding  * texture(tex, uv).a is the line above takes away color from behind

//OpenGL context variables
GLuint programID = 0;
GLuint VertexArrayID = 0;
GLuint RotBindingID = 0;

void InitializeGL()
{
    //Compile the shaders
    programID = compile_shaders(vshader_square, fshader_square);

    //Generate Vertex Array and bind the vertex buffer data
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    ///--- Generate memory for vertexbuffer
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
    GLuint vtexcoordbuffer;
    glGenBuffers(1, &vtexcoordbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vtexcoordbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vtexcoord), vtexcoord, GL_STATIC_DRAW);
    glUseProgram(programID); //Don't need to do this here again, but it's safe
    GLuint vtexcoord_binding = glGetAttribLocation(programID, "vtexcoord");
    glEnableVertexAttribArray(vtexcoord_binding);
    glVertexAttribPointer(vtexcoord_binding,
                          2,
                          GL_FLOAT,
                          false,
                          0, 0);

    //Load texture
    Texture teximage = LoadPNGTexture("texture.png");
    //Upload this image onto GPU
    GLuint texobject;
    glGenTextures(1, &texobject);
    glBindTexture(GL_TEXTURE_2D, texobject);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); //the i at the end means to integer
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                teximage.width,
                teximage.height,
                0,
                GL_RGBA, GL_UNSIGNED_BYTE, teximage.dataptr);

    // -- Activate the texture image
    GLuint sampler_ID = glGetUniformLocation(programID, "tex");
    glUniform1i(sampler_ID, 0); //Means? If tex is a uniform with type int, then that api call will set this int to 0, but it's a sampler2D, then we tell it to usee the first sampler
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texobject);



    //Find the binding point for the uniform variable
    //"rotation" in vertex shader
    RotBindingID = glGetUniformLocation(programID, "rotation");
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
    //Binding the openGL context
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(programID);
    glBindVertexArray(VertexArrayID);
        //upload current rotation to GPU
        glUniform1f(RotBindingID, Rotation);
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
