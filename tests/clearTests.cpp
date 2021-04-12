#include <tests/catch.hpp>

#include <iostream>
#include <string.h>

#include <algorithm>
#include <numeric>

#include <student/gpu.hpp>
#include <framework/framebuffer.hpp>

#include <glm/gtc/matrix_transform.hpp>

SCENARIO("00"){
  std::cerr << "00 - clear function should properly clear framebuffer" << std::endl;

  uint32_t w = 100;
  uint32_t h = 100;
  GPUContext ctx;
  auto framebuffer = std::make_shared<Framebuffer>(w,h);
  ctx.frame = framebuffer->getFrame();


  for(int i=0;i<4;++i)
  {
    for(uint32_t i=0;i<w*h;++i)ctx.frame.depth[i] = -1337.f;
    if(i==0)clear(ctx,1.f,0.f,0.f,0.f);
    if(i==1)clear(ctx,0.f,1.f,0.f,0.f);
    if(i==2)clear(ctx,0.f,0.f,1.f,0.f);
    if(i==3)clear(ctx,0.f,0.f,0.f,1.f);

    uint8_t expectedValue[4] = {0};
    expectedValue[i] = 255;

    bool success = true;
    for(uint32_t i=0;i<w*h;++i){
      success &= (ctx.frame.color[i*4+0] == expectedValue[0]);
      success &= (ctx.frame.color[i*4+1] == expectedValue[1]);
      success &= (ctx.frame.color[i*4+2] == expectedValue[2]);
      success &= (ctx.frame.color[i*4+3] == expectedValue[3]);
      success &= (ctx.frame.depth[i] > 1.f);
    }

    char const*colorNames[] = {
      "červená","zelená","modrá","alpha"
    };

    if(!success){
      std::cerr << R".(
      Funkce clear() čístí obsah framebufferu.
      Když se zavolá funkce: clear(ctx,)."<< (int)(i==0) << "," << (int)(i==1) << "," << (int)(i==2) << "," << (int)(i==3) << R".(); měl by se vyšistit framebuffer.
      Framebuffer se nachází ve struktuře "Frame", která je obsažena ve skruktuře "GPUContext" v parametru ctx.
      Po zavolání funkce v tomto tvaru, by měla mít )."<< colorNames[i] << R".( komponenta barvy hodnotu 255 a zbytek hodnotu 0.
      Hloubka by měla být nastavena na takovou hodnotu, která umožňuje rasterizaci v rámci pohledového jehlanu.

      Funkce clear odpovídá přibližně funkcím glClearColor(), glClearDepth() a glClear() z OpenGL, které slouží pro vyčištění framebufferu.
      ).";
      REQUIRE(false);
    }
  }

}

