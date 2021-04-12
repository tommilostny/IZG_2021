/*!
 * @file
 * @brief This file contains czech flag rendering method
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include <framework/method.hpp>

#include <vector>

namespace czFlagMethod{

/**
 * @brief Czech flag rendering method
 */
class Method: public ::Method{
  public:
    Method(MethodConstructionData const*);
    virtual ~Method(){}
    virtual void onDraw(Frame&frame,glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera) override;
    virtual void onUpdate(float dt) override;
    GPUContext           ctx       ;///< gpu context

    struct Vertex{
      glm::vec2 position;
      glm::vec2 texCoord;
    };

    std::vector<Vertex  >vertices  ;///< vertex buffer   
    std::vector<uint32_t>indices   ;///< index buffer
    float                time = 0.f;///< elapsed time
    uint32_t const       NX   = 100;///< nof vertices in x direction
    uint32_t const       NY   = 10 ;///< nof vertices in y direction
};

}
