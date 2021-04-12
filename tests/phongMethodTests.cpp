#include <tests/catch.hpp>

#include <iostream>
#include <string.h>

#include <algorithm>
#include <numeric>

#include <glm/gtc/matrix_transform.hpp>

#include <student/gpu.hpp>
#include <tests/testCommon.hpp>
#include <tests/renderPhongFrame.hpp>
#include <SDL.h>


void phong_VS(OutVertex&outVertex,InVertex const&inVertex,Uniforms const&uniforms);
void phong_FS(OutFragment&outFragment,InFragment const&inFragment,Uniforms const&uniforms);

SCENARIO("phongMethod vertex shader should forward world space position and normal"){
  std::cerr << "30 - phongMethod - vertex shader should forward attributes" << std::endl;
  Uniforms uniforms;
  OutVertex outVertex;
  for(uint32_t i=0;i<maxAttributes;++i)
    outVertex.attributes[i].v4 = glm::vec4(0.f);

  InVertex inVertex;

  inVertex.attributes[0].v3 = glm::vec3(1.f,2.f,3.f);
  inVertex.attributes[1].v3 = glm::vec3(0,0,1.f);

  uniforms.uniform[0].m4 = glm::translate(glm::mat4(1.f),glm::vec3(10.f,20.f,30.f));
  uniforms.uniform[1].m4 = glm::translate(glm::mat4(1.f),glm::vec3(30.f,40.f,50.f));


  phong_VS(outVertex,inVertex,uniforms);

  auto const& a0 = outVertex.attributes[0].v3;
  auto const& a1 = outVertex.attributes[1].v3;
  REQUIRE(equalFloats(a0[0] , 1.f));
  REQUIRE(equalFloats(a0[1] , 2.f));
  REQUIRE(equalFloats(a0[2] , 3.f));

  REQUIRE(equalFloats(a1[0] , 0.f));
  REQUIRE(equalFloats(a1[1] , 0.f));
  REQUIRE(equalFloats(a1[2] , 1.f));
}

SCENARIO("vertex shader should compute correct gl_Position according to transformation matrices"){
  std::cerr << "31 - phongMethod - vertex shader should compute correct gl_Position using matrices" << std::endl;
  Uniforms u;
  OutVertex outVertex;
  outVertex.gl_Position = glm::vec4(0.f);

  InVertex inVertex;
  inVertex.attributes[0].v3 = glm::vec3(1.f,2.f,3.f);

  u.uniform[0].m4 = glm::translate(glm::mat4(1.f),glm::vec3(1.f,3.f,4.f));
  u.uniform[1].m4 = glm::scale    (glm::mat4(1.f),glm::vec3(1.f,2.f,1.f));

  phong_VS(outVertex,inVertex,u);

  REQUIRE(equalFloats (outVertex.gl_Position[0] ,  2.f));
  REQUIRE(equalFloats (outVertex.gl_Position[1] , 10.f));
  REQUIRE(equalFloats (outVertex.gl_Position[2] ,  7.f));
  REQUIRE(equalFloats (outVertex.gl_Position[3] ,  1.f));
}

SCENARIO("fragment shader should compute correct color for vertical normals"){
  std::cerr << "32 - phongMethod - fragment shader should compute correct color for vertical normals" << std::endl;
  Uniforms u;
  OutFragment outFragment;
  outFragment.gl_FragColor = glm::vec4(.1f,.2f,.3f,.4f);

  InFragment inFragment;
  inFragment.attributes[0].v3 = glm::vec3(0.f,0.f,0.f);
  inFragment.attributes[1].v3 = glm::vec3(0.f,1.f,0.f);

  u.uniform[2].v3 = glm::vec3(0.f,1.f,0.f);
  u.uniform[3].v3 = glm::vec3(1.f,0.f,0.f);

  phong_FS(outFragment,inFragment,u);

  REQUIRE(equalFloats(outFragment.gl_FragColor[0] , 1.f));
  REQUIRE(equalFloats(outFragment.gl_FragColor[1] , 1.f));
  REQUIRE(equalFloats(outFragment.gl_FragColor[2] , 1.f));
}

glm::vec3 getTextureColor(float x,float y){
  Uniforms u;

  OutFragment outFragment;
  InFragment inFragment;

  float freq = 10.f;

  outFragment.gl_FragColor = glm::vec4(.1f,.2f,.3f,.4f);
  inFragment.attributes[0].v3 = glm::vec3(x,y,0);//pos
  inFragment.attributes[1].v3 = glm::vec3(1,0,0);//nor
  u.uniform[2].v3 = glm::vec3(x+1.f,y,0);//lig
  u.uniform[3].v3 = glm::vec3(x,y,1);//cam

  phong_FS(outFragment,inFragment,u);

  return glm::vec3(outFragment.gl_FragColor);
}

SCENARIO("fragment shader should compute correct texture - green stripes"){
  std::cerr << "33a - phongMethod - green stripes" << std::endl;

  float freq = 10.f;

  for(float x=0.01f/freq;x<1.f;x+=2.f/freq){
    auto color = getTextureColor(x,0.f);
    REQUIRE(equalFloats (color[0] , 0.0f));
    REQUIRE(equalFloats (color[1] , 0.5f));
    REQUIRE(equalFloats (color[2] , 0.0f));
  }

  for(float x=0.99f/freq;x<1.f;x+=2.f/freq){
    auto color = getTextureColor(x,0.f);
    REQUIRE(equalFloats (color[0] , 0.0f));
    REQUIRE(equalFloats (color[1] , 0.5f));
    REQUIRE(equalFloats (color[2] , 0.0f));
  }
}

SCENARIO("fragment shader should compute correct texture - yellow stripes"){
  std::cerr << "33b - phongMethod - yellow stripes" << std::endl;

  float freq = 10.f;

  for(float x=1.01f/freq;x<1.f;x+=2.f/freq){
    auto color = getTextureColor(x,0.f);
    REQUIRE(equalFloats (color[0] , 1.0f));
    REQUIRE(equalFloats (color[1] , 1.0f));
    REQUIRE(equalFloats (color[2] , 0.0f));
  }

  for(float x=1.99f/freq;x<1.f;x+=2.f/freq){
    auto color = getTextureColor(x,0.f);
    REQUIRE(equalFloats (color[0] , 1.0f));
    REQUIRE(equalFloats (color[1] , 1.0f));
    REQUIRE(equalFloats (color[2] , 0.0f));
  }
}

SCENARIO("fragment shader should compute correct texture - green sinus stripes"){
  std::cerr << "33c - phongMethod - green sin stripes" << std::endl;

  float freq = 10.f;

  glm::vec3 color;

  for(float x=1.01f/freq;x<1.f;x+=2.f/freq){
    color = getTextureColor(x, 3.f*glm::half_pi<float>()/freq);
    REQUIRE(equalFloats (color[0] , .0f));
    REQUIRE(equalFloats (color[1] , .5f));
    REQUIRE(equalFloats (color[2] , .0f));
  }
  for(float x=1.99f/freq;x<1.f;x+=2.f/freq){
    color = getTextureColor(x, 3.f*glm::half_pi<float>()/freq);
    REQUIRE(equalFloats (color[0] , .0f));
    REQUIRE(equalFloats (color[1] , .5f));
    REQUIRE(equalFloats (color[2] , .0f));
  }

  for(float x=0.51f/freq;x<1.f;x+=2.f/freq){
    color = getTextureColor(x, (3.f*glm::half_pi<float>() + 2.f*glm::half_pi<float>()/3.f)/freq);
    REQUIRE(equalFloats (color[0] , .0f));
    REQUIRE(equalFloats (color[1] , .5f));
    REQUIRE(equalFloats (color[2] , .0f));
  }
  for(float x=1.49f/freq;x<1.f;x+=2.f/freq){
    color = getTextureColor(x, (3.f*glm::half_pi<float>() + 2.f*glm::half_pi<float>()/3.f)/freq);
    REQUIRE(equalFloats (color[0] , .0f));
    REQUIRE(equalFloats (color[1] , .5f));
    REQUIRE(equalFloats (color[2] , .0f));
  }

}

SCENARIO("fragment shader should compute correct texture - yellow sin stripes"){
  std::cerr << "33c - phongMethod - yellow sin stripes" << std::endl;

  float freq = 10.f;

  glm::vec3 color;

  for(float x=0.01f/freq;x<1.f;x+=2.f/freq){
    color = getTextureColor(x, 3.f*glm::half_pi<float>()/freq);
    REQUIRE(equalFloats (color[0] , 1.0f));
    REQUIRE(equalFloats (color[1] , 1.0f));
    REQUIRE(equalFloats (color[2] , 0.0f));
  }
  for(float x=0.99f/freq;x<1.f;x+=2.f/freq){
    color = getTextureColor(x, 3.f*glm::half_pi<float>()/freq);
    REQUIRE(equalFloats (color[0] , 1.0f));
    REQUIRE(equalFloats (color[1] , 1.0f));
    REQUIRE(equalFloats (color[2] , 0.0f));
  }

  for(float x=1.51f/freq;x<1.f;x+=2.f/freq){
    color = getTextureColor(x, (3.f*glm::half_pi<float>() + 2.f*glm::half_pi<float>()/3.f)/freq);
    REQUIRE(equalFloats (color[0] , 1.0f));
    REQUIRE(equalFloats (color[1] , 1.0f));
    REQUIRE(equalFloats (color[2] , 0.0f));
  }
  for(float x=2.49f/freq;x<1.f;x+=2.f/freq){
    color = getTextureColor(x, (3.f*glm::half_pi<float>() + 2.f*glm::half_pi<float>()/3.f)/freq);
    REQUIRE(equalFloats (color[0] , 1.0f));
    REQUIRE(equalFloats (color[1] , 1.0f));
    REQUIRE(equalFloats (color[2] , 0.0f));
  }

}


SCENARIO("fragment shader should compute correct color for backfacing triangles"){
  std::cerr << "34 - phongMethod - fragment shader backfacing triangles" << std::endl;
  Uniforms u;
  
  OutFragment outFragment;
  InFragment inFragment;

  outFragment.gl_FragColor = glm::vec4(.1f,.2f,.3f,.4f);
  inFragment.attributes[0].v3 = glm::vec3(0,0,0);//pos
  inFragment.attributes[1].v3 = glm::vec3(1,0,0);//nor

  u.uniform[2].v3 = glm::vec3(0,1,0);//lig
  u.uniform[3].v3 = glm::vec3(1,0,0);//cam

  phong_FS(outFragment,inFragment,u);

  REQUIRE(equalFloats(outFragment.gl_FragColor[0] , 0.f));
  REQUIRE(equalFloats(outFragment.gl_FragColor[1] , 0.f));
  REQUIRE(equalFloats(outFragment.gl_FragColor[2] , 0.f));
}


SCENARIO("fragment shader should compute correct color for specular reflections"){
  std::cerr << "35 - phongMethod - fragment shader specular reflections" << std::endl;
  Uniforms u;
  
  OutFragment outFragment;
  InFragment inFragment;

  outFragment.gl_FragColor = glm::vec4(.1f,.2f,.3f,.4f);
  inFragment.attributes[0].v3 = glm::vec3(0,0,0);
  inFragment.attributes[1].v3 = glm::vec3(1,0,0);

  u.uniform[2].v3 = glm::vec3(1,0,0);
  u.uniform[3].v3 = glm::vec3(1,0,0);

  phong_FS(outFragment,inFragment,u);

  REQUIRE(equalFloats(outFragment.gl_FragColor[0] , 1.f));
  REQUIRE(equalFloats(outFragment.gl_FragColor[1] , 1.f));
  REQUIRE(equalFloats(outFragment.gl_FragColor[2] , 1.f));
}


SCENARIO("fragment shader should not compute specular reflection for backfacing triangles"){
  std::cerr << "36 - phongMethod - fragment shader backfacing specular reflections" << std::endl;
  Uniforms u;
  
  OutFragment outFragment;
  InFragment inFragment;

  inFragment.attributes[0].v3 = glm::vec3(0,0,0);//pos
  inFragment.attributes[1].v3 = glm::vec3(1,0,0);//nor
  u.uniform[2].v3 = glm::vec3(1,0,0);//lig
  u.uniform[3].v3 = glm::vec3(-1,0,0);//cam
  outFragment.gl_FragColor = glm::vec4(.1f,.2f,.3f,.4f);

  phong_FS(outFragment,inFragment,u);

  auto color = getTextureColor(0,0);

  REQUIRE(equalFloats(outFragment.gl_FragColor[0] , color[0]));
  REQUIRE(equalFloats(outFragment.gl_FragColor[1] , color[1]));
  REQUIRE(equalFloats(outFragment.gl_FragColor[2] , color[2]));
}


SCENARIO("fragment shader should not compute specular reflection for black triangles"){
  std::cerr << "37 - phongMethod - fragment shader black triangles" << std::endl;
  Uniforms u;
  
  OutFragment outFragment;
  InFragment inFragment;

  inFragment.attributes[0].v3 = glm::vec3(0,0,0);//pos
  inFragment.attributes[1].v3 = glm::vec3(0,-1,0);//nor
  u.uniform[2].v3 = glm::vec3(0,1,0);//lig
  u.uniform[3].v3 = glm::vec3(-1,0,0);//cam
  outFragment.gl_FragColor = glm::vec4(.1f,.2f,.3f,.4f);

  phong_FS(outFragment,inFragment,u);

  REQUIRE(equalFloats(outFragment.gl_FragColor[0] , 0.f));
  REQUIRE(equalFloats(outFragment.gl_FragColor[1] , 0.f));
  REQUIRE(equalFloats(outFragment.gl_FragColor[2] , 0.f));
}

SCENARIO("fragment shader should normalize normals"){
  std::cerr << "38 - phongMethod - fragment shader should normalize normals" << std::endl;
  Uniforms u;
  
  OutFragment outFragment;
  InFragment inFragment;

  inFragment.attributes[0].v3 = glm::vec3(0,0,0);//pos
  inFragment.attributes[1].v3 = glm::vec3(.1f,0,0);//nor
  u.uniform[2].v3 = glm::vec3(1,0,0);//lig
  u.uniform[3].v3 = glm::vec3(0,1,0);//cam
  outFragment.gl_FragColor = glm::vec4(.1f,.2f,.3f,.4f);

  phong_FS(outFragment,inFragment,u);

  auto color = getTextureColor(0,0);

  REQUIRE(equalFloats(outFragment.gl_FragColor[0] , color[0]));
  REQUIRE(equalFloats(outFragment.gl_FragColor[1] , color[1]));
  REQUIRE(equalFloats(outFragment.gl_FragColor[2] , color[2]));
}


SCENARIO("fragment shader lighting should not depend on the distance to the viewer"){
  std::cerr << "39 - phongMethod - distance to the viewer" << std::endl;
  Uniforms u;
  
  OutFragment outFragment;
  InFragment inFragment;

  inFragment.attributes[0].v3 = glm::vec3(0,0,0);
  inFragment.attributes[1].v3 = glm::vec3(1,0,0);
  u.uniform[2].v3 = glm::vec3(1.f,0,0);
  u.uniform[3].v3 = glm::vec3(.1f,0,0);
  outFragment.gl_FragColor = glm::vec4(.1f,.2f,.3f,.4f);

  phong_FS(outFragment,inFragment,u);

  REQUIRE(equalFloats(outFragment.gl_FragColor[0] , 1.f));
  REQUIRE(equalFloats(outFragment.gl_FragColor[1] , 1.f));
  REQUIRE(equalFloats(outFragment.gl_FragColor[2] , 1.f));
}

SCENARIO("fragment shader lighting should not depend on the distance to the light"){
  std::cerr << "40 - phongMethod - distance to the light" << std::endl;
  Uniforms u;
  
  OutFragment outFragment;
  InFragment inFragment;

  inFragment.attributes[0].v3 = glm::vec3(0,0,0);
  inFragment.attributes[1].v3 = glm::vec3(1,0,0);
  u.uniform[2].v3 = glm::vec3(.1f,0,0);
  u.uniform[3].v3 = glm::vec3(1.f,0,0);
  outFragment.gl_FragColor = glm::vec4(.1f,.2f,.3f,.4f);

  phong_FS(outFragment,inFragment,u);

  REQUIRE(equalFloats(outFragment.gl_FragColor[0] , 1.f));
  REQUIRE(equalFloats(outFragment.gl_FragColor[1] , 1.f));
  REQUIRE(equalFloats(outFragment.gl_FragColor[2] , 1.f));
}

std::string extern groundTruthFile;

SCENARIO("phong method should render correct image"){
  std::cerr << "41 - phongMethod - image to image comparison" << std::endl;
  uint32_t width = 500;
  uint32_t height = 500;
  auto frame = renderPhongFrame(width,height);



  SDL_Surface* groundTruth = SDL_LoadBMP(groundTruthFile.c_str());

  if (groundTruth == nullptr) {
    std::cerr << "ERROR: reference image: \"" << groundTruthFile
              << "\" is corrupt!" << std::endl;
    REQUIRE(groundTruth != nullptr);
  }


  uint32_t bitsPerByte = 8;
  uint32_t const swizzleTable[] = {
      groundTruth->format->Rshift / bitsPerByte,
      groundTruth->format->Gshift / bitsPerByte,
      groundTruth->format->Bshift / bitsPerByte,
  };


  if (width == groundTruth->w && height == groundTruth->h) {
    float meanSquareError = 0;
    for (uint32_t y = 0; y < height; ++y)
      for (uint32_t x = 0; x < width; ++x){
        for (uint32_t c = 0; c < 3; ++c) {
          uint8_t ucol = frame[((height-y-1)*width+x)*4+c];
          uint8_t gcol = ((uint8_t*)groundTruth->pixels)[y * groundTruth->pitch + x*groundTruth->format->BytesPerPixel + swizzleTable[c]];
          float diff = glm::abs((float)ucol - (float)gcol);
          diff *= diff;
          meanSquareError += diff;
        }
      }

    meanSquareError /= (float)(width * height * 3);
    SDL_FreeSurface(groundTruth);

    REQUIRE(meanSquareError < 40.f);
  }

}
