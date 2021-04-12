/*!
 * @file
 * @brief This file contains 3D triangle rendering method.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include <framework/method.hpp>

namespace triangle3DMethod{

/**
 * @brief This class represents 3D texture rendering method
 */
class Method: public ::Method{
  public:
    Method(MethodConstructionData const*mcd);
    virtual ~Method();
    virtual void onDraw(Frame&frame,glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera) override;
    GPUContext ctx;///< gpu context
};

}
