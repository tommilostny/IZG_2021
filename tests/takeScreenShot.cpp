#include <tests/takeScreenShot.hpp>
#include <tests/renderMethodFrame.hpp>
#include <framework/application.hpp>

#include <libs/stb_image/stb_image_write.h>

#include <SDL.h>
#include <string>

void takeScreenShot(std::string const&groundTruthFile,std::string const&modelFile){
  uint32_t width = 500;
  uint32_t height = 500;


  auto frame = renderMethodFrame(width,height,modelFile);

  for(uint32_t y=0;y<height/2;++y)
    for(uint32_t x=0;x<width;++x){
      for(uint32_t c=0;c<3;++c){
        auto z=frame.at((y*width+x)*4+c);
        frame.at((y*width+x)*4+c) = frame.at(((height-1-y)*width+x)*4+c);
        frame.at(((height-1-y)*width+x)*4+c) = z;
      }
      frame.at(((         y)*width+x)*4+3)=255;
      frame.at(((height-1-y)*width+x)*4+3)=255;
    }

  stbi_write_png(groundTruthFile.c_str(),width,height,4,frame.data(),0);

  //auto surface = SDL_CreateRGBSurface(0, width, height, 24,0,0,0,0);

  //copyToSDLSurface(surface,frame.data(),width,height);

  std::cerr << "storing screenshot to: \"" << groundTruthFile << "\"" << std::endl;

  //SDL_Surface* rgb = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGB24, 0);
  //SDL_SaveBMP(rgb, groundTruthFile.c_str());
  //SDL_FreeSurface(rgb);
  //SDL_FreeSurface(surface);

}
