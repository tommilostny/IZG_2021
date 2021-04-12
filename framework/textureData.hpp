#pragma once

#include<vector>
#include<cstdint>
#include<string>
#include<student/fwd.hpp>

class TextureData{
  public:
    std::vector<uint8_t>data;
    uint32_t width    = 0;
    uint32_t height   = 0;
    uint32_t channels = 0;
    TextureData(){}
    TextureData(uint32_t w,uint32_t h,uint32_t c):width(w),height(h),channels(c){
      data.resize((size_t)w*h*c,0);
    }
    Texture getTexture(){
      Texture res;
      res.data = data.data();
      res.width = width;
      res.height = height;
      res.channels = channels;
      return res;
    }
};

TextureData loadTexture(std::string const&fileName);
