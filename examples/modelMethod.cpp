/*!
 * @file
 * @brief This file contains implementation of model visualizer
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <examples/modelMethod.hpp>
#include <student/drawModel.hpp>

namespace modelMethod{


/**
 * @brief Constructor
 */
Method::Method(ConstructionData const*mcd){
  modelData.load(mcd->modelFile);
  model = modelData.getModel();
}


/**
 * @brief This function is called every frame and should render a model
 *
 * @param proj projection matrix
 * @param view view matrix
 * @param light light position
 * @param camera camera position
 */
void Method::onDraw(Frame&frame,glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera){
  ctx.frame = frame;
  clear(ctx,.5,.5,1,0);
  drawModel(ctx,model,proj,view,light,camera);
}

/**
 * @brief Descturctor
 */
Method::~Method(){
}

}
