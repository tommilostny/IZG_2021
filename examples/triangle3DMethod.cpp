/*!
 * @file
 * @brief This file contains implementation of 3D triangle rendering method.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <examples/triangle3DMethod.hpp>

namespace triangle3DMethod{

/**
 * @brief This function represents vertex shader of 3D triangle rendering method.
 *
 * @param outVertex output vertex
 * @param inVertex input vertex
 * @param uniforms uniform variables
 */
void vertexShader(OutVertex&outVertex,InVertex const&inVertex,Uniforms const&uniforms){
  outVertex.gl_Position = glm::vec4(0.f,0.f,0.f,1.f);

  glm::mat4 viewMatrix       = uniforms.uniform[3].m4;
  glm::mat4 projectionMatrix = uniforms.uniform[2].m4;
 
  glm::mat4 mvp = projectionMatrix * viewMatrix;

  if(inVertex.gl_VertexID == 0){
    outVertex.gl_Position = mvp*glm::vec4(-1.f,-1.f,0.f,1.f);
    outVertex.attributes[3].v4 = glm::vec4( 1.f, 0.f,0.f,1.f);
  }
  if(inVertex.gl_VertexID == 1){
    outVertex.gl_Position      = mvp*glm::vec4(1.f,-1.f,0.f,1.f);
    outVertex.attributes[3].v4 = glm::vec4(0.f, 1.f,0.f,1.f);
  }
  if(inVertex.gl_VertexID == 2){
    outVertex.gl_Position      = mvp*glm::vec4(-1.f,+1.f,0.f,1.f);
    outVertex.attributes[3].v4 = glm::vec4( 0.f, 0.f,1.f,1.f);
  }

}

/**
 * @brief This functionrepresents fragment shader of 3D triangle rendering method.
 *
 * @param outFragment output fragment
 * @param inFragment input fragment
 * @param uniforms uniform variables
 */
void fragmentShader(OutFragment&outFragment,InFragment const&inFragment,Uniforms const&uniforms){
  (void)uniforms;
  outFragment.gl_FragColor = inFragment.attributes[3].v4;
}

/**
 * @brief Constructor
 */
Method::Method(MethodConstructionData const*){
  ctx.prg.vertexShader = vertexShader;
  ctx.prg.fragmentShader = fragmentShader;
  ctx.prg.vs2fs[3] = AttributeType::VEC4;
}

/**
 * @brief This function is called every frame and should render 3D triangle
 *
 * @param proj projection matrix
 * @param view view matrix
 * @param light light position
 * @param camera camera position
 */
void Method::onDraw(Frame&frame,glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera){
  (void)light;
  (void)camera;
  ctx.frame = frame;
  clear(ctx,0,0,0,1);

  ctx.prg.uniforms.uniform[3].m4 = view;
  ctx.prg.uniforms.uniform[2].m4 = proj;
  drawTriangles(ctx,3);
}

/**
 * @brief Descturctor
 */
Method::~Method(){
}

}
