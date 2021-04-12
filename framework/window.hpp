/*!
 * @file
 * @brief This file contains window class
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include<functional>
#include<map>

#include<SDL.h>

/**
 * @brief This class represents SDL window
 */
class Window{
  public:
    /**
     * @brief Type of event callback function
     */
    using EventCallback = std::function<void(SDL_Event const&e)>;
    /**
     * @brief Type of idle callback function
     */
    using IdleCallback  = std::function<void()>;
    Window(){}
    Window(int32_t width,int32_t height,char const*name);
    virtual ~Window();
    void mainLoop();
    void setCallback(uint32_t event,EventCallback const&clb);
    void setWindowCallback(uint32_t event,EventCallback const&clb);
    void setIdleCallback(IdleCallback const&clb);
    void reInitRenderer();
    SDL_Window*getWindow();
  protected:
    void initSDL();
    void initWindow(int32_t width,int32_t height,char const*name);
    void initSurface();
    void initRenderer();
    void initEvents();
    void processEvents();
    void processEvent(SDL_Event const&event);
    void processWindowEvent(SDL_Event const&event);
    void callIdleCallback();
    SDL_Window*                   window         ;///< window handle
    SDL_Surface*                  surface        ;///< surface
    SDL_Renderer*                 renderer       ;///< SDL2 renderer
    bool                          running        ;///< is main loop running
    std::map<Uint32,EventCallback>eventCallbacks ;///< map of event callback function
    std::map<Uint8 ,EventCallback>windowCallbacks;///< map of event callback function for window event
    IdleCallback                  idleCallback   ;///< function that is called in mainloop when there are no events
};

