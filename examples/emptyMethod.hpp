/*!
 * @file
 * @brief This file contains empty rendering method
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include <framework/method.hpp>

namespace emptyMethod{

/**
 * @brief Empty rendering method
 */
class Method: public ::Method{
  public:
    Method(MethodConstructionData const*){}
    virtual void onDraw(Frame&frame,glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera) override;
};

void Method::onDraw(Frame&frame,glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera){
  (void)proj;
  (void)view;
  (void)light;
  (void)camera;
  GPUContext ctx;
  ctx.frame = frame;
  clear(ctx,0,0,0,1);
}

}
