#include <tests/saveFrame.hpp>
#include <framework/application.hpp>
#include <SDL.h>

void saveFrame(std::string const&file,Frame const&frame){
  auto surface = SDL_CreateRGBSurface(0, frame.width, frame.height, 24,0,0,0,0);

  copyToSDLSurface(surface,frame.color,frame.width,frame.height);

  SDL_Surface* rgb = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGB24, 0);
  SDL_SaveBMP(rgb, file.c_str());
  SDL_FreeSurface(rgb);
  SDL_FreeSurface(surface);
}
