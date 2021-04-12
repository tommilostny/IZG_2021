#include <tests/renderMethodFrame.hpp>

#include <memory>

#include <framework/application.hpp>
#include <examples/modelMethod.hpp>
#include <framework/framebuffer.hpp>

#include <BasicCamera/OrbitCamera.h>
#include <BasicCamera/PerspectiveCamera.h>

#include <student/gpu.hpp>

void drawTrianglesImpl(GPUContext&,uint32_t);

std::vector<uint8_t>renderMethodFrame(uint32_t width,uint32_t height,std::string const&modelFile){
  auto cd = std::make_shared<modelMethod::ConstructionData>(modelFile);
  auto method = modelMethod::Method{&*cd};

  auto framebuffer = std::make_shared<Framebuffer>(width,height);

  auto orbitCamera       = basicCamera::OrbitCamera();
  auto perspectiveCamera = basicCamera::PerspectiveCamera();
  glm::vec3 light;

  defaultSceneParameters(orbitCamera,perspectiveCamera,light,width,height);

  auto const proj   = perspectiveCamera.getProjection();
  auto const view   = orbitCamera      .getView      ();
  auto const camera = glm::vec3(glm::inverse(view)*glm::vec4(0.f,0.f,0.f,1.f));

  auto frame = framebuffer->getFrame();

  drawTriangles = drawTrianglesImpl;
  method.onDraw(frame,proj,view,light,camera);

  auto       f     = frame.color;
  auto const w     = frame.width;
  auto const h     = frame.height;

  std::vector<uint8_t>res;

  if(f == nullptr)
    return std::vector<uint8_t>(4*width*height);

  for(uint32_t i=0;i<w*h*4;++i)
    res.push_back(f[i]);

  return res;
}
