/*!
 * @file
 * @brief This file contains implementation of czech flag rendering method
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <examples/czFlagMethod.hpp>
#include <vector>

namespace czFlagMethod{

/**
 * @brief Czech flag vertex shader
 *
 * @param outVertex out vertex
 * @param inVertex in vertex
 * @param uniforms uniform variables
 */
void vertexShader(OutVertex&outVertex,InVertex const&inVertex,Uniforms const&uniforms){
  auto const& pos   = inVertex.attributes[0].v2;
  auto const& coord = inVertex.attributes[1].v2;
  auto const& mvp   = uniforms.uniform[0].m4;

  auto time = uniforms.uniform[1].v1;
  
  auto z = (coord.x*0.5f)*glm::sin(coord.x*10.f + time);
  outVertex.gl_Position = mvp*glm::vec4(pos,z,1.f);

  outVertex.attributes[0].v2 = coord;
}

/**
 * @brief Czech flag fragment shader
 *
 * @param outFragment output fragment
 * @param inFragment input fragment
 * @param uniforms uniform variables
 */
void fragmentShader(OutFragment&outFragment,InFragment const&inFragment,Uniforms const&uniforms){
  (void)uniforms;
  auto const& vCoord = inFragment.attributes[0].v2;
  if(vCoord.y > vCoord.x && 1.f-vCoord.y>vCoord.x){
    outFragment.gl_FragColor = glm::vec4(0.f,0.f,1.f,1.f);
  }else{
    if(vCoord.y < 0.5f){
      outFragment.gl_FragColor = glm::vec4(1.f,0.f,0.f,1.f);
    }else{
      outFragment.gl_FragColor = glm::vec4(1.f,1.f,1.f,1.f);
    }
  }
}

Method::Method(MethodConstructionData const*){

  for(size_t y=0;y<NY;++y)
    for(size_t x=0;x<NX;++x){
      glm::vec2 coord;
      coord.x = static_cast<float>(x) / static_cast<float>(NX-1);
      coord.y = static_cast<float>(y) / static_cast<float>(NY-1);
      auto const flagStart = glm::vec2(-1.5f,-1.f);
      auto const flagSize  = glm::vec2(+3.0f,+2.f);

      auto const position = flagStart + coord*flagSize;

      vertices.push_back({position,coord});
    }

  for(uint32_t y=0;y<NY-1;++y)
    for(uint32_t x=0;x<NX-1;++x){
      indices.push_back((y+0)*NX+(x+0));
      indices.push_back((y+0)*NX+(x+1));
      indices.push_back((y+1)*NX+(x+0));
      indices.push_back((y+1)*NX+(x+0));
      indices.push_back((y+0)*NX+(x+1));
      indices.push_back((y+1)*NX+(x+1));
    }


  //position
  ctx.vao.vertexAttrib[0].bufferData = vertices.data()    ;
  ctx.vao.vertexAttrib[0].type       = AttributeType::VEC2;
  ctx.vao.vertexAttrib[0].stride     = sizeof(Vertex)     ;
  ctx.vao.vertexAttrib[0].offset     = 0                  ;

  //coord
  ctx.vao.vertexAttrib[1].bufferData = vertices.data()    ;
  ctx.vao.vertexAttrib[1].type       = AttributeType::VEC2;
  ctx.vao.vertexAttrib[1].stride     = sizeof(Vertex)     ;
  ctx.vao.vertexAttrib[1].offset     = sizeof(glm::vec2)  ;

  ctx.vao.indexBuffer = indices.data()   ;
  ctx.vao.indexType   = IndexType::UINT32;

  ctx.prg.vertexShader   = vertexShader;
  ctx.prg.fragmentShader = fragmentShader;
  ctx.prg.vs2fs[0]       = AttributeType::VEC2;
}

void Method::onUpdate(float dt){
  time += dt;
}

void Method::onDraw(Frame&frame,glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera){
  (void)light;
  (void)camera;
  ctx.frame = frame;
  clear(ctx,0,0,0,1);

  auto mvp = proj*view;
  ctx.prg.uniforms.uniform[0].m4 = mvp ;
  ctx.prg.uniforms.uniform[1].v1 = time;

  drawTriangles(ctx,(NX-1)*(NY-1)*6);
}

}
