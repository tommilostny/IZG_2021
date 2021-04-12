/*!
 * @file
 * @brief This file contains model visualizer
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include <framework/method.hpp>
#include <framework/model.hpp>

namespace modelMethod{

class ConstructionData: public MethodConstructionData{
  public:
    ConstructionData(std::string const&modelFile):modelFile(modelFile){}
    std::string modelFile;
};

/**
 * @brief This class represents model visualizer
 */
class Method: public ::Method{
  public:
    Method(ConstructionData const*mcd);
    Method(MethodConstructionData const*mcd):Method((ConstructionData const*)mcd){}
    virtual ~Method();
    virtual void onDraw(Frame&frame,glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera) override;
    ModelData modelData;
    Model     model;
    GPUContext ctx;///< gpu context
};

}
