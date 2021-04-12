/*!
 * @file
 * @brief This file contains application class.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include <memory>
#include <vector>

#include <BasicCamera/OrbitCamera.h>
#include <BasicCamera/PerspectiveCamera.h>

#include <student/gpu.hpp>
#include <framework/framebuffer.hpp>
#include <framework/window.hpp>
#include <framework/method.hpp>
#include <framework/timer.hpp>

/**
 * @brief Application class
 */
class Application: protected Window{
  public:
    Application(int32_t width,int32_t height);
    ~Application();
    template<typename CLASS>
    void registerMethod(std::string const&name,std::shared_ptr<MethodConstructionData>const&mcd = nullptr);
    void start();
    void setMethod(uint32_t m);
  private:
    void idle();
    void resize(SDL_Event const&event);
    void mouseMotionLMask(uint32_t mState,float xrel,float yrel);
    void mouseMotionRMask(uint32_t mState,float yrel);
    void mouseMotionMMask(uint32_t mState,float xrel,float yrel);
    void mouseMotion(SDL_Event const&event);
    void keyDown(SDL_Event const&event);
    void nextMethod(uint32_t key);
    void prevMethod(uint32_t key);
    void quit      (uint32_t key);
    void createMethodIfItDoesNotExist();
    void swap();

    using MethodFactory = std::function<std::shared_ptr<Method>(MethodConstructionData const*)>;

    basicCamera::OrbitCamera       orbitCamera                                  ;
    basicCamera::PerspectiveCamera perspectiveCamera                            ;
    glm::vec3                      light             = glm::vec3(100.f,100.f,100.f);

    std::vector<MethodFactory>     methodFactories                              ;
    std::vector<std::string>       methodName                                   ;
    std::vector<std::shared_ptr<MethodConstructionData>> methodConstructData    ;
    size_t                         selectedMethod    = 0                        ;
    std::shared_ptr<Method>        method                                       ;

    glm::uvec2                     windowSize                                   ;
    float                          sensitivity       = 0.01f                    ;
    float                          orbitZoomSpeed    = 0.1f                     ;

    Timer<float>                   timer                                        ;

    std::shared_ptr<Framebuffer>framebuffer;///< framebuffer
};

/**
 * @brief This function sets default scene parameters for rendering
 *
 * @param orbit output orbit camera
 * @param proj output projection camera
 * @param light output light position
 * @param width width of the window
 * @param height height of the window
 */
void defaultSceneParameters(
    basicCamera::OrbitCamera&orbit,
    basicCamera::PerspectiveCamera&proj,
    glm::vec3&light,
    uint32_t width,uint32_t height);

/**
 * @brief This function swaps color buffer with SDL_Surface
 *
 * @param surface sdl surface
 * @param color color buffer (RGBA8UI)
 * @param width width of color buffer
 * @param height height of color buffer
 */
void copyToSDLSurface(SDL_Surface*surface,uint8_t const*const color,uint32_t width,uint32_t height);

/**
 * @brief This method registers new rendering method into applicaion
 *
 * @tparam CLASS method class
 * @param name name of the method
 */
template<typename CLASS>
void Application::registerMethod(std::string const&name,std::shared_ptr<MethodConstructionData>const&mcd){
  methodFactories.push_back([&](MethodConstructionData const*mcd){return std::make_shared<CLASS>(mcd);});
  methodName.push_back(name);
  methodConstructData.push_back(mcd);
}
