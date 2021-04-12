/*!
 * @file
 * @brief This file contains implementation of rendering method that renders triangle stored in buffer.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <examples/triangleBufferMethod.hpp>

namespace triangleBufferMethod{

/**
 * @brief This function is vertex shader of triangle buffer method
 *
 * @param outVertex output vertex
 * @param inVertex input vertex
 * @param uniforms uniform variables
 */
void vertexShader(OutVertex&outVertex,InVertex const&inVertex,Uniforms const&uniforms){
  (void)uniforms;
  outVertex.gl_Position = glm::vec4(inVertex.attributes[0].v2,0.f,1.f);
}

/**
 * @brief This function is fragment shader of triangle buffer method
 *
 * @param outFragment output fragment
 * @param inFragment input fragment 
 * @param uniforms uniform variables
 */
void fragmentShader(OutFragment&outFragment,InFragment const&inFragment,Uniforms const&uniforms){
  (void)inFragment;
  (void)uniforms;
  outFragment.gl_FragColor = glm::vec4(0.f,1.f,0.f,1.f);
}


Method::Method(MethodConstructionData const*){
  buffer = {
    -.5f,-.5f,
    -.1f,-.5f,
    -.5f,-.1f,

    +.1f,-.5f,
    +.5f,-.5f,
    +.5f,-.1f,

    -.5f,+.1f,
    -.1f,+.5f,
    -.5f,+.5f,

    +.1f,+.5f,
    +.5f,+.1f,
    +.5f,+.5f,
  };

  ctx.vao.vertexAttrib[0].bufferData = buffer.data()      ;
  ctx.vao.vertexAttrib[0].type       = AttributeType::VEC2;
  ctx.vao.vertexAttrib[0].stride     = sizeof(float)*2    ;
  ctx.vao.vertexAttrib[0].offset     = 0                  ;

  ctx.prg.vertexShader   = vertexShader;
  ctx.prg.fragmentShader = fragmentShader;

}


void Method::onDraw(Frame&frame,glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera){
  (void)proj;
  (void)view;
  (void)light;
  (void)camera;
  ctx.frame = frame;
  clear(ctx,0,0,0,0);
  drawTriangles(ctx,3*4);
}

}
