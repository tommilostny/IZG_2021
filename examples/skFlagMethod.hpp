/*!
 * @file
 * @brief This file contains procedural flag method
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include <framework/method.hpp>

/**
 * @brief This class represents procedural flag method
 */
class SKFlagMethod: public Method{
  public:
    SKFlagMethod(MethodConstructionData const*);
    virtual ~SKFlagMethod();
    virtual void onDraw(Frame&frame,glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera) override;
    GPUContext ctx;///< gpu context
};

