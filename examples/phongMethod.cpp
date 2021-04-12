/*!
 * @file
 * @brief This file contains implementation of phong rendering method
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <examples/phongMethod.hpp>
#include <framework/bunny.hpp>

namespace phongMethod{

//! [PhongMethod]
/**
 * @brief This function represents vertex shader of phong method.
 *
 * @param outVertex output vertex
 * @param inVertex input vertex
 * @param uniforms uniform variables
 */
void vertexShader(OutVertex&outVertex,InVertex const&inVertex,Uniforms const&uniforms){
  auto const pos = glm::vec4(inVertex.attributes[0].v3,1.f);
  auto const&nor = inVertex.attributes[1].v3;
  auto const&viewMatrix       = uniforms.uniform[0].m4;
  auto const&projectionMatrix = uniforms.uniform[1].m4;

  auto mvp = projectionMatrix*viewMatrix;

  outVertex.gl_Position = mvp * pos;
  outVertex.attributes[0].v3 = pos;
  outVertex.attributes[1].v3 = nor;
}

/**
 * @brief This function represents fragment shader of phong method.
 *
 * @param outFragment output fragment
 * @param inFragment input fragment
 * @param uniforms uniform variables
 */
void fragmentShader(OutFragment&outFragment,InFragment const&inFragment,Uniforms const&uniforms){
  auto const& light          = uniforms.uniform[2].v3;
  auto const& cameraPosition = uniforms.uniform[3].v3;
  auto const& vpos           = inFragment.attributes[0].v3;
  auto const& vnor           = inFragment.attributes[1].v3;
  auto vvnor = glm::normalize(vnor);

  auto l = glm::normalize(light-vpos);
  float diffuseFactor                    = glm::dot(l, vvnor);
  if (diffuseFactor < 0.f) diffuseFactor = 0.f;

  auto v = glm::normalize(cameraPosition-vpos);
  auto r = -glm::reflect(v,vvnor);
  float specularFactor                     = glm::dot(r, l);
  if (specularFactor < 0.f) specularFactor = 0.f;
  float const shininess                    = 40.f;

  if (diffuseFactor < 0)
    specularFactor = 0;
  else
    specularFactor = powf(specularFactor, shininess);

  float t = vvnor[1];
  if(t<0.f)t=0.f;
  t*=t;
  auto materialDiffuseColor = glm::mix(glm::vec3(0.f,1.f,0.f),glm::vec3(1.f,1.f,1.f),t);
  
  float const nofStripes = 10;
  float factor = 1.f / nofStripes * 2.f;

  auto xs = static_cast<float>(glm::mod(vpos.x+glm::sin(vpos.y*10.f)*.1f,factor)/factor > 0.5);

  materialDiffuseColor = glm::mix(glm::mix(glm::vec3(0.f,.5f,0.f),glm::vec3(1.f,1.f,0.f),xs),glm::vec3(1.f),t);

  auto materialSpecularColor = glm::vec3(1.f);

  auto diffuseColor  = materialDiffuseColor  * diffuseFactor;
  auto specularColor = materialSpecularColor * specularFactor;

  auto const color = glm::min(diffuseColor + specularColor,glm::vec3(1.f));
  outFragment.gl_FragColor = glm::vec4(color,1.f);
}

/**
 * @brief Constructoro f phong method
 */
Method::Method(MethodConstructionData const*){
  //position
  ctx.vao.vertexAttrib[0].bufferData = bunnyVertices      ;
  ctx.vao.vertexAttrib[0].type       = AttributeType::VEC3;
  ctx.vao.vertexAttrib[0].stride     = sizeof(BunnyVertex);
  ctx.vao.vertexAttrib[0].offset     = 0                  ;

  //normal
  ctx.vao.vertexAttrib[1].bufferData = bunnyVertices      ;
  ctx.vao.vertexAttrib[1].type       = AttributeType::VEC3;
  ctx.vao.vertexAttrib[1].stride     = sizeof(BunnyVertex);
  ctx.vao.vertexAttrib[1].offset     = sizeof(glm::vec3)  ;

  ctx.vao.indexBuffer = bunnyIndices     ;
  ctx.vao.indexType   = IndexType::UINT32;

  ctx.prg.vertexShader   = vertexShader;
  ctx.prg.fragmentShader = fragmentShader;
  ctx.prg.vs2fs[0]       = AttributeType::VEC3;
  ctx.prg.vs2fs[1]       = AttributeType::VEC3;
}


/**
 * @brief This function draws phong method.
 *
 * @param proj projection matrix
 * @param view view matrix
 * @param light light position
 * @param camera camera position
 */
void Method::onDraw(Frame&frame,glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera){
  ctx.frame = frame;
  clear(ctx,.5f,.5f,.5f,1.f);
  ctx.prg.uniforms.uniform[0].m4 = view  ;
  ctx.prg.uniforms.uniform[1].m4 = proj  ;
  ctx.prg.uniforms.uniform[2].v3 = light ;
  ctx.prg.uniforms.uniform[3].v3 = camera;
  drawTriangles(ctx,sizeof(bunnyIndices)/sizeof(VertexIndex));
}

//! [PhongMethod]

/**
 * @brief Destructor of phong method.
 */
Method::~Method(){
}

}
