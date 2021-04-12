/*!
 * @file
 * @brief This file contains triangle clipping 2 rendering method
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include <framework/method.hpp>

namespace triangleClip2Method{

/**
 * @brief Triangle clipping 2 rendering method
 */
class Method: public ::Method{
  public:
    Method(MethodConstructionData const*);
    virtual ~Method(){}
    virtual void onDraw(Frame&frame,glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera) override;
    GPUContext ctx;
};

}
