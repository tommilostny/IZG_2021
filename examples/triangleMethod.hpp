/*!
 * @file
 * @brief This file contains 2D triangle rendering method
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include <framework/method.hpp>

namespace triangleMethod{

/**
 * @brief 2D Triangle rendering method
 */
class Method: public ::Method{
  public:
    Method(MethodConstructionData const*);
    virtual ~Method();
    virtual void onDraw(Frame&frame,glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera) override;
    GPUContext ctx;///< gpu context
};

}
