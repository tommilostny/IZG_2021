#include <tests/catch.hpp>

#include <iostream>
#include <string.h>

#include <algorithm>
#include <numeric>

#include <student/gpu.hpp>
#include <framework/method.hpp>
#include <framework/framebuffer.hpp>

#include <tests/testCommon.hpp>

using namespace tests;

#include <glm/gtc/matrix_transform.hpp>

SCENARIO("24"){
  std::cerr << "24 - clipping - CW triangle behind near plane" << std::endl;
  uint32_t w = 100;
  uint32_t h = 100;
  GPUContext ctx;
  auto framebuffer = std::make_shared<Framebuffer>(w,h);
  ctx.frame = framebuffer->getFrame();

  ctx.prg.vertexShader = vertexShaderInject;
  ctx.prg.fragmentShader = fragmentShaderDump;


  OutVertex a;
  outVertices.clear();
  inFragments.clear();
  //near 1, far 2
  outVertices.push_back({{},glm::vec4(-1.f,-1.f,-7.f,-1.f)});
  outVertices.push_back({{},glm::vec4(+1.f,-1.f,-7.f,-1.f)});
  outVertices.push_back({{},glm::vec4(-1.f,+1.f,-7.f,-1.f)});

  drawTriangles(ctx,3);

  REQUIRE(inFragments.size() == 0);
}


SCENARIO("25"){
  std::cerr << "25 - clipping - CCW triangle behind near plane" << std::endl;
  uint32_t w = 100;
  uint32_t h = 100;
  GPUContext ctx;
  auto framebuffer = std::make_shared<Framebuffer>(w,h);
  ctx.frame = framebuffer->getFrame();

  ctx.prg.vertexShader = vertexShaderInject;
  ctx.prg.fragmentShader = fragmentShaderDump;


  OutVertex a;
  outVertices.clear();
  inFragments.clear();
  //near 1, far 2
  outVertices.push_back({{},glm::vec4(-1.f,-1.f,-7.f,-1.f)});
  outVertices.push_back({{},glm::vec4(-1.f,+1.f,-7.f,-1.f)});
  outVertices.push_back({{},glm::vec4(+1.f,-1.f,-7.f,-1.f)});

  drawTriangles(ctx,3);

  REQUIRE(inFragments.size() == 0);
}


SCENARIO("26"){
  std::cerr << "26 - clipping - 1 vertex behind near plane" << std::endl;
  uint32_t w = 100;
  uint32_t h = 100;
  GPUContext ctx;
  auto framebuffer = std::make_shared<Framebuffer>(w,h);
  ctx.frame = framebuffer->getFrame();

  ctx.prg.vertexShader = vertexShaderInject;
  ctx.prg.fragmentShader = fragmentShaderDump;


  OutVertex a;
  outVertices.clear();
  inFragments.clear();
  
  float nnear = 1.f;
  float ffar  = 2.f;
  float e = - (ffar + nnear) / (ffar - nnear);
  float f = - 2.f * ffar * nnear / (ffar - nnear);
  float z = 2.f/3.f;

  outVertices.push_back({{},glm::vec4(-2.f,-2.f,+2.f,+2.f)});
  outVertices.push_back({{},glm::vec4(+2.f,-2.f,+2.f,+2.f)});
  outVertices.push_back({{},glm::vec4(-z  ,+z  ,-z*e+f,+z)});

  drawTriangles(ctx,3);

  uint32_t const expectedCount = 100*100/8*3;
  uint32_t const err           = 2*100;

  if(expectedCount < inFragments.size())
    REQUIRE(inFragments.size() <= expectedCount + err);
  else
    REQUIRE(inFragments.size() >= expectedCount - err);
}

SCENARIO("27"){
  std::cerr << "27 - clipping - 2 vertices behind near plane" << std::endl;
  uint32_t w = 100;
  uint32_t h = 100;
  GPUContext ctx;
  auto framebuffer = std::make_shared<Framebuffer>(w,h);
  ctx.frame = framebuffer->getFrame();

  ctx.prg.vertexShader = vertexShaderInject;
  ctx.prg.fragmentShader = fragmentShaderDump;


  OutVertex a;
  outVertices.clear();
  inFragments.clear();
  
  float nnear = 1.f;
  float ffar  = 2.f;
  float e = - (ffar + nnear) / (ffar - nnear);
  float f = - 2.f * ffar * nnear / (ffar - nnear);
  float z = 2.f/3.f;

  outVertices.push_back({{},glm::vec4(-z,-z,-z*e+f,+z)});
  outVertices.push_back({{},glm::vec4(+z,-z,-z*e+f,+z)});
  outVertices.push_back({{},glm::vec4(-2,+2,+2,+2)});

  drawTriangles(ctx,3);

  uint32_t const expectedCount = 100*100/8*1;
  uint32_t const err           = 2*100;

  if(expectedCount < inFragments.size())
    REQUIRE(inFragments.size() <= expectedCount + err);
  else
    REQUIRE(inFragments.size() >= expectedCount - err);
}

