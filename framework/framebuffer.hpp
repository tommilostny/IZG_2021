#pragma once

#include <student/gpu.hpp>

#include<memory>
#include<vector>

/**
 * @brief This class represents framebuffer.
 * Student dont have to not be concerned about this.
 */
class Framebuffer{
  public:
    Framebuffer(uint32_t w = 500,uint32_t h = 500){
      resize(w,h);
    }
    void resize(uint32_t w,uint32_t h){
      width = w;
      height = h;
      auto const nofPixes = w*h;
      auto const bytesPerPixel = 4;
      color.resize((size_t)nofPixes*bytesPerPixel,0);
      for(size_t i=0;i<w*h;++i)color.at(i*bytesPerPixel+3)=255;
      depth.resize(nofPixes,1.f);
    }
    std::vector<uint8_t>color;
    std::vector<float  >depth;
    uint32_t width;
    uint32_t height;
    Frame getFrame(){
      Frame frame;
      frame.color  = color.data();
      frame.depth  = depth.data();
      frame.width  = width;
      frame.height = height;
      return frame;
    }
};
