#include "Renderer.hpp"
#include "SDL_opengles2.h"
#include <cmath>

#define STRINGIFY(version,A) "#version " #version "\n" #A


//##################### SHADER #####################
GLchar vShaderStr[]=STRINGIFY(110,
attribute vec4 vPosition;
uniform mat4 Projection;
uniform mat4 Model;
void main()
{
  gl_Position = Projection*Model*vPosition;
});


GLchar fShaderStr[] = STRINGIFY(110,
//precision mediump float;

uniform int iter[64*64];
uniform ivec4 parameters;
uniform int oversampling;
vec3 get_color(int value)
{
  vec3 start;
  vec3 end;
  vec3 count = vec3(500.0);
  vec3 weight;
  vec3 pos;

  int val = int(mod(float(value), count.x+1.0));

  // calculate gradiant value
  vec3 color;
  if(val <= int(count.x*0.15)) {
    start  = vec3(1.0, 1.0, 1.0);
    end    = vec3(1.0, 0.8, 0.0);
    weight = vec3(count.x*0.15);
    pos    = vec3(0);
  } else if (val <= int(count.x*0.49)) {
    start  = vec3(1.0, 0.8, 0.0);
    end    = vec3(0.53, 0.12, 0.075);
    weight = vec3(count.x*0.34);
    pos    = vec3(count.x*0.15);
  } else if (val <= int(count.x*0.67)) {
    start  = vec3(0.53, 0.12, 0.075);
    end    = vec3(0.0, 0.0, 0.6);
    weight = vec3(count.x*0.18);
    pos    = vec3(count.x*0.49);
  } else if (val <= int(count.x*0.85)) {
    start  = vec3(0.0, 0.0, 0.6);
    end    = vec3(0.0, 0.4, 1.0);
    weight = vec3(count.x*0.18);
    pos    = vec3(count.x*0.67);
  } else if (val <= int(count.x)) {
    start  = vec3(0.0, 0.4, 1.0);
    end    = vec3(1.0, 1.0, 1.0);
    weight = vec3(count.x*0.15);
    pos    = vec3(count.x*0.85);
  }

  color = start - ((start-end)/weight) * (vec3(val)-pos);
  return color;
}

vec3 get_sampled_color(int x, int y) {

  vec3 color = vec3(0.0, 0.0, 0.0);

  //TODO does not work...
  // Oversampling has to be seen local here
  // Offset nötig???
  x *= oversampling;
  y *= oversampling;
  for(int sampleX=0; sampleX<oversampling; ++sampleX) {
    for(int sampleY=0; sampleY<oversampling; ++sampleY) {
      int value = iter[(x+sampleX)+(y+sampleY)*64];
      if(value != parameters.z) {
        color += pow(get_color(value), vec3(2.2));        
      }
    }
  }
  color /= vec3(oversampling*oversampling);
  color = pow(color, vec3(1.0/2.2));
  return color;
}
void main()
{
  // FragCoord is always the midpoint -> 0.5. int() rounds up
  int x = int(gl_FragCoord.x)-parameters.x;
  int y = parameters.w-int(gl_FragCoord.y)-parameters.y-1;
  
  gl_FragColor = vec4(get_sampled_color(x,y),1.0);
//  gl_FragColor = vec4(gl_FragCoord.x,gl_FragCoord.y,0.0,1.0);
}
);
//##################################################

Renderer::Renderer(Logic *l)
{
  logic = l;
  gammaValue = 2.2;
}

Renderer::~Renderer()
{
}

void Renderer::Reset()
{

}

// TODO
GLuint LoadShader(GLenum type, const GLchar *shaderSrc)
{
  GLuint shader;
  GLint compiled;
  // Create the shader object
  shader = glCreateShader(type);
  if(shader == 0)
    return 0;
  // Load the shader source
  glShaderSource(shader, 1, &shaderSrc, NULL);
  // Compile the shader
  glCompileShader(shader);
  // Check the compile status
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if(!compiled) {
    GLint infoLen = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
    if(infoLen > 1) {
      char* infoLog = new char[infoLen];
      glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
      std::cout << "Error compiling shader: " << infoLog << std::endl;
      delete[] infoLog;
    }
    glDeleteShader(shader);
    return 0;
  }
  return shader;
}

void Renderer::ApplyOrtho(float left, float right, float bottom, float top, float near, float far) const
{

  float a = 2.0f / (right - left);
  float b = 2.0f / (top - bottom);
  float c = -2.0f / (far - near);

  float tx = - (right + left)/(right - left);
  float ty = - (top + bottom)/(top - bottom);
  float tz = - (far + near)/(far - near);

  float ortho[16] = {
    a, 0, 0, 0,
    0, b, 0, 0,
    0, 0, c, 0,
    tx, ty, tz, 1
  };

  GLint projectionUniform = glGetUniformLocation(programObject, "Projection");
  glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, &ortho[0]);
}

void Renderer::ApplyTrans(float x, float y, float z) const
{
  float trans[16] = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    x, y, z, 1
  };

  GLint projectionUniform = glGetUniformLocation(programObject, "Model");
  glUniformMatrix4fv(projectionUniform, 1, GL_FALSE, &trans[0]);
}

void Renderer::InitRenderer(uint32_t w, uint32_t h)
{
  width = w;
  height = h;
  Reset();

  GLuint vertexShader;
  GLuint fragmentShader;
  GLint linked;
  // Load the vertex/fragment shaders
  vertexShader = LoadShader ( GL_VERTEX_SHADER, vShaderStr );
  fragmentShader = LoadShader ( GL_FRAGMENT_SHADER, fShaderStr );

  // Create the program object
  programObject = glCreateProgram();

  if ( programObject == 0 )
    return;

  glAttachShader ( programObject, vertexShader );
  glAttachShader ( programObject, fragmentShader );

  // Bind vPosition to attribute 0
  glBindAttribLocation ( programObject, 0, "vPosition" );

  // Link the program
  glLinkProgram ( programObject );

  // Check the link status
  glGetProgramiv ( programObject, GL_LINK_STATUS, &linked );

  if ( !linked ) {
    GLint infoLen = 0;

    glGetProgramiv ( programObject, GL_INFO_LOG_LENGTH, &infoLen );

    if ( infoLen > 1 ) {
      char* infoLog = new char[infoLen];

      glGetProgramInfoLog ( programObject, infoLen, NULL, infoLog );
      std::cout << "Error linking program: " << infoLog << std::endl;

      delete[] infoLog;
    }

    glDeleteProgram ( programObject );
    return;
  }

  glClearColor( 0.0f, 1.0f, 0.0f, 0.0f );
  glViewport( 0, 0, width, height );
  glUseProgram ( programObject );
  ApplyOrtho(0.0, w, h, 0.0, -1, 1);
}

void Renderer::Render()
{
  /*
    std::vector<iter_t> iterations;
    logic->Background()->PrepareVector(iterations);
    for(uint32_t idx = 0; idx<pixel.size(); idx++) {
      if(pixel[idx] == limitPixel) {
        uint32_t x,y;
        TransformIndex(idx,x,y);
        bool finished = logic->Background()->IterationForPixel(x,y,iterations);
        float r = 0.0f;
        float g = 0.0f;
        float b = 0.0f;
        if(finished) {
          for (uint32_t j=0; j<iterations.size(); j++) {
            iter_t n = iterations[j];
            if(n < IS_INSIDE) {
              RGBf color = palette->get(n);
              r += std::pow(color.red,gammaValue);
              g += std::pow(color.green,gammaValue);
              b += std::pow(color.blue,gammaValue);
            }
          }
          r/=iterations.size();
          g/=iterations.size();
          b/=iterations.size();
          r=std::pow(r,1.0f/gammaValue);
          g=std::pow(g,1.0f/gammaValue);
          b=std::pow(b,1.0f/gammaValue);
          uint8_t rb = r * 255.0f;
          uint8_t gb = g * 255.0f;
          uint8_t bb = b * 255.0f;
          uint8_t ab = 0;

          uint32_t color = ((ab) << 24)
            | ((bb) << 16)
            | ((gb) << 8)
            | ((rb) << 0);
          pixel[idx] = color;
        }
      }
    }
  */
  glClear ( GL_COLOR_BUFFER_BIT );

  GLint param_iter = glGetUniformLocation(programObject, "oversampling");
  GLint over =  logic->Background()->Oversampling();
  glUniform1i(param_iter, over);

  //check this also in the shader
  uint32_t data_size = 64;
  uint32_t offset=data_size/(over*over);
  GLfloat h = offset;
  GLfloat w = offset;
  iter_t *data = new iter_t[data_size*data_size];

  // a little box
  GLfloat vVertices[] = {
    0.0f, 0.0f, 0.0f,
    0.0f,    h, 0.0f,
       w,    h, 0.0f,
       w,    h, 0.0f,
       w, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f
  };
  GLuint positionAttribute = glGetAttribLocation(programObject, "vPosition");
  glEnableVertexAttribArray(positionAttribute);
  glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0, vVertices);

//  int c = 0;
  
  // paint by drawing little boxes
  for(uint32_t chunk = 0; chunk<(width*height)/(w*h); chunk++) {
//    c++;
    // move the box around the screen, to paint the parts
    int x = (chunk*offset)%width;
    int y = ((chunk*offset)/width)*h;

    ApplyTrans(x,y,0);

    // put some important values to the shader
    int parameters[4] = {x, y, IS_INSIDE, int(height)};
    GLint param_iter = glGetUniformLocation(programObject, "parameters");
    glUniform4iv(param_iter, 1, &parameters[0] );

    //fetch the data from RAM
    //append data
    for(uint32_t idx=0; idx<data_size; idx++) {
      uint32_t v = (x*over)+((y*over)+idx)*(width*over);
//      std::cout << x << "x" << y+idx << "=" << v << std::endl;
      iter_t *ramData = &(logic->Background()->FullScreen()[v]);
      std::copy(ramData, ramData+data_size, data+(data_size*idx));
    }

    GLint iterUni = glGetUniformLocation(programObject, "iter");
    glUniform1iv(iterUni, offset*offset, data);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }
  
//  std::cout << c << std::endl;
 
  delete[] data;
  glDisableVertexAttribArray(positionAttribute);
}
/*
void Renderer::TGASaveHeader(std::ofstream &file, int16_t width, int16_t height, uint8_t pixelDepth) const {
  uint8_t cGarbage = 0, type;
  int16_t iGarbage = 0;

  if ((pixelDepth == 24) || (pixelDepth == 32)) {
    type = 2;
  } else {
    type = 3;
  }
  file.write((char*)&cGarbage, sizeof(uint8_t));
  file.write((char*)&cGarbage, sizeof(uint8_t));

  file.write((char*)&type, sizeof(uint8_t));

  file.write((char*)&iGarbage, sizeof(int16_t));
  file.write((char*)&iGarbage, sizeof(int16_t));
  file.write((char*)&cGarbage, sizeof(uint8_t));
  file.write((char*)&iGarbage, sizeof(int16_t));
  file.write((char*)&iGarbage, sizeof(int16_t));

  file.write((char*)&width, sizeof(int16_t));
  file.write((char*)&height, sizeof(int16_t));
  file.write((char*)&pixelDepth, sizeof(uint8_t));

  file.write((char*)&cGarbage, sizeof(uint8_t));
}


void Renderer::MakeScreenshot() {
  std::cout << "speichere TGA" << std::endl << std::flush;
  std::ofstream file;
  file.open("Ergebnis.tga", std::ios::binary | std::ios::trunc );
  if (file.is_open()) {
    TGASaveHeader(file, width, height, 24);

    for(uint32_t idx = 0; idx<pixel.size(); idx++) {
      uint32_t color = pixel[idx];
      uint8_t rb = (color);
      color=color>>8;
      uint8_t gb = (color);;
      color=color>>8;
      uint8_t bb = (color);;

      file.write((char*)&bb, sizeof(uint8_t));
      file.write((char*)&gb, sizeof(uint8_t));
      file.write((char*)&rb, sizeof(uint8_t));
    }
  }
  file.close();
  std::cout << "TGA gespeichert" << std::endl << std::flush;
}
*/

uint32_t Renderer::TransformCoord(uint32_t x, uint32_t y) const
{
  return x+y*width;
}

void Renderer::TransformIndex(uint32_t index, uint32_t &x, uint32_t &y) const
{
  x = index%width;
  y = index/width;
}
