/*!
 * @file
 * @brief This file contains implementation of 2D triangle rendering method
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <examples/triangleMethod.hpp>

namespace triangleMethod{

/**
 * @brief Vertex shader
 *
 * @param outVertex output vertex
 * @param inVertex input vertex
 * @param uniforms uniform variables
 */
void vertexShader(OutVertex&outVertex,InVertex const&inVertex,Uniforms const&uniforms){
  (void)uniforms;
  if(inVertex.gl_VertexID == 0)
    outVertex.gl_Position = glm::vec4(-1,-1,0,1);
  if(inVertex.gl_VertexID == 1)
    outVertex.gl_Position = glm::vec4(+1,-1,0,1);
  if(inVertex.gl_VertexID == 2)
    outVertex.gl_Position = glm::vec4(-1,+1,0,1);
}

/**
 * @brief Fragment shader
 *
 * @param outFragment output fragment
 * @param inFragment input fragment
 * @param uniforms uniform variables
 */
void fragmentShader(OutFragment&outFragment,InFragment const&inFragment,Uniforms const&uniforms){
  (void)inFragment;
  (void)uniforms;
  outFragment.gl_FragColor = glm::vec4(1.f);
}


Method::Method(MethodConstructionData const*){
  ctx.prg.vertexShader   = vertexShader;
  ctx.prg.fragmentShader = fragmentShader;
}

Method::~Method(){
}


void Method::onDraw(Frame&frame,glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera){
  (void)proj;
  (void)view;
  (void)light;
  (void)camera;
  ctx.frame = frame;
  clear(ctx,0,0,0,1);
  drawTriangles(ctx,3);
}

}
