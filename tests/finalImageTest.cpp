#include <tests/catch.hpp>

#include <iostream>
#include <string.h>

#include <algorithm>
#include <numeric>

#include <student/gpu.hpp>
#include <tests/testCommon.hpp>
#include <tests/renderMethodFrame.hpp>
#include <framework/textureData.hpp>

std::string extern groundTruthFile;
std::string extern modelFile;

SCENARIO("38"){
  std::cerr << "38 - image to image comparison" << std::endl;
  uint32_t width = 500;
  uint32_t height = 500;
  auto frame = renderMethodFrame(width,height,modelFile);

  auto ref = loadTexture(groundTruthFile);

  if (ref.data.size() == 0) {
    std::cerr << "ERROR: reference image: \"" << groundTruthFile
              << "\" is corrupt!" << std::endl;
    REQUIRE(false);
  }

  if(width != ref.width || height != ref.height){
    std::cerr << "ERROR: reference image does not have the dimension: " << width<< "x"<< height<< std::endl;
    REQUIRE(false);
  }

  float meanSquareError = 0;
  for (uint32_t y = 0; y < height; ++y)
    for (uint32_t x = 0; x < width; ++x){
      for (uint32_t c = 0; c < 3; ++c) {
        uint8_t ucol = frame[(y*width+x)*4+c];
        uint8_t gcol = ref.data.at((y*ref.width+x)*ref.channels+c);
        float diff = glm::abs((float)ucol - (float)gcol);
        diff *= diff;
        meanSquareError += diff;
      }
    }

  meanSquareError /= (float)(width * height * 3);
  float tol = 40.f;

  if(meanSquareError >= tol){
    std::cerr << R".(
    Finální obrázek se moc liší od reference!
    MSE je: )."<<meanSquareError<<R".(
    Akceptovatelná chyba je: )."<<tol<<R".(
    ).";
    REQUIRE(false);
  }
}
