#include<framework/textureData.hpp>

#include<libs/stb_image/stb_image.h>

#include <iostream>

TextureData loadTexture(std::string const&fileName){
  TextureData res;

  int32_t w,h,channels;
  uint8_t* data = stbi_load(fileName.c_str(),&w,&h,&channels,0);
  if(!data){
    std::cerr << "Cannot open image file: "<<fileName<<std::endl;
    return res;
  }
  //std::cerr << "w: " << w << " h: " << h << " c: " << channels << std::endl;
  res.data.resize(w*h*channels);

  for(int32_t y=0;y<h;++y)
    for(int32_t x=0;x<w;++x)
      for(int32_t c=0;c<channels;++c){
        res.data[(y*w+x)*channels+c] = data[((h-y-1)*w+x)*channels+c];
      }

  res.channels = channels;
  res.height = h;
  res.width = w;
  stbi_image_free(data);
  return res;
}
