/*!
 * @file
 * @brief This file contains implementation of 3D triangle rendering method.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <examples/texturedQuadMethod.hpp>

namespace texturedQuad{

/**
 * @brief This function represents vertex shader of texture rendering method.
 *
 * @param outVertex output vertex
 * @param inVertex input vertex
 * @param uniforms uniform variables
 */
void vertexShader(OutVertex&outVertex,InVertex const&inVertex,Uniforms const&uniforms){
  (void)uniforms;
  outVertex.gl_Position = glm::vec4(0.f,0.f,0.f,1.f);

  //fullscreen quad
  glm::vec2 const verts[]={
    glm::vec2(-1.f,-1.f),
    glm::vec2(+1.f,-1.f),
    glm::vec2(-1.f,+1.f),
    glm::vec2(-1.f,+1.f),
    glm::vec2(+1.f,-1.f),
    glm::vec2(+1.f,+1.f),
  };

  outVertex.gl_Position = glm::vec4(verts[inVertex.gl_VertexID],0.f,1.f);
  outVertex.attributes[0].v2 = (glm::vec2(outVertex.gl_Position)+1.f)*.5f;
}

/**
 * @brief This functionrepresents fragment shader of texture rendering method.
 *
 * @param outFragment output fragment
 * @param inFragment input fragment
 * @param uniforms uniform variables
 */
void fragmentShader(OutFragment&outFragment,InFragment const&inFragment,Uniforms const&uniforms){
  auto uv = inFragment.attributes[0].v2;
  outFragment.gl_FragColor = read_texture(uniforms.textures[0],uv);
}

/**
 * @brief Constructor
 */
Method::Method(ConstructionData const*cd){
  ctx.prg.vertexShader   = vertexShader  ; 
  ctx.prg.fragmentShader = fragmentShader;
  ctx.prg.vs2fs[0]       = AttributeType::VEC2;//tex coords
  tex = loadTexture(cd->imageFile);
  ctx.prg.uniforms.textures[0] = tex.getTexture();
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
  (void)proj;
  (void)view;
  (void)light;
  (void)camera;
  ctx.frame = frame;
  clear(ctx,0,0,0,0);
  drawTriangles(ctx,6);
}

/**
 * @brief Descturctor
 */
Method::~Method(){
}

}
