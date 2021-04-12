#include <tests/testCommon.hpp>

#include <glm/glm.hpp>
#include <sstream>
#include <iostream>

namespace tests{

float const floatErr = 0.001f;

bool equalFloats(float const& a, float const& b,float err) {
  return glm::abs(a - b) <= err;
}

bool equalVec4(glm::vec4 const&a,glm::vec4 const&b,float err){
  return
    equalFloats(a.x,b.x,err) &&
    equalFloats(a.y,b.y,err) &&
    equalFloats(a.z,b.z,err) &&
    equalFloats(a.w,b.w,err) ;
}

bool equalVec3(glm::vec3 const&a,glm::vec3 const&b,float err){
  return
    equalFloats(a.x,b.x,err) &&
    equalFloats(a.y,b.y,err) &&
    equalFloats(a.z,b.z,err) ;
}

bool equalVec2(glm::vec2 const&a,glm::vec2 const&b,float err){
  return
    equalFloats(a.x,b.x,err) &&
    equalFloats(a.y,b.y,err);
}

bool equalCounts(size_t a,size_t b,size_t err){
  if(a<b)return (b-a)<err;
  return (a-b)<err;
}

bool greaterFloat(float a,float b,float err){
  return a>b-err;
}

bool lessFloat(float a,float b,float err){
  return a<b+err;
}

std::string str(float     const&a){
  std::stringstream ss;
  ss << a;
  return ss.str();
}

std::string str(glm::vec2 const&a){
  std::stringstream ss;
  ss << "(" << a.x << "," << a.y << ")";
  return ss.str();
}

std::string str(glm::vec3 const&a){
  std::stringstream ss;
  ss << "(" << a.x << "," << a.y << "," << a.z << ")";
  return ss.str();
}

std::string str(glm::vec4 const&a){
  std::stringstream ss;
  ss << "(" << a.x << "," << a.y << "," << a.z << "," << a.w << ")";
  return ss.str();
}

std::string str(uint32_t     const&a){
  std::stringstream ss;
  ss << a;
  return ss.str();
}

std::string str(glm::uvec2 const&a){
  std::stringstream ss;
  ss << "(" << a.x << "," << a.y << ")";
  return ss.str();
}

std::string str(glm::uvec3 const&a){
  std::stringstream ss;
  ss << "(" << a.x << "," << a.y << "," << a.z << ")";
  return ss.str();
}

std::string str(glm::uvec4 const&a){
  std::stringstream ss;
  ss << "(" << a.x << "," << a.y << "," << a.z << "," << a.w << ")";
  return ss.str();
}

std::string str(glm::mat4 const&m){
  std::stringstream ss;
  ss << "(";
  for(int i=0;i<16;++i){
    if(i>0)ss<<",";
    ss << m[i/4][i%4];
  }
  ss << ")";
  return ss.str();
}

std::string str(IndexType const&i){
  if(i==IndexType::UINT8 )return "IndexType::UINT8" ;
  if(i==IndexType::UINT16)return "IndexType::UINT16";
  if(i==IndexType::UINT32)return "IndexType::UINT32";
  return "unknown";
}

std::string str(AttributeType const&a){
  if(a==AttributeType::FLOAT)return "AttributeType::FLOAT";
  if(a==AttributeType::VEC2 )return "AttributeType::VEC2" ;
  if(a==AttributeType::VEC3 )return "AttributeType::VEC3" ;
  if(a==AttributeType::VEC4 )return "AttributeType::VEC4" ;
  if(a==AttributeType::EMPTY)return "AttributeType::EMPTY";
  return "unknown";
}

std::vector<InVertex>inVertices;
std::vector<OutVertex>outVertices;
std::vector<InFragment>inFragments;
Uniforms unif;

void vertexShaderDump(OutVertex&,InVertex const&i,Uniforms const&u){
  unif = u;
  inVertices.push_back(i);
}

void vertexShaderInject(OutVertex&outVertex,InVertex const&inVertex,Uniforms const&){
  if(inVertex.gl_VertexID<outVertices.size())
    outVertex = outVertices.at(inVertex.gl_VertexID);
}

void fragmentShaderDump(OutFragment&,InFragment const&inF,Uniforms const&u){
  unif = u;
  inFragments.push_back(inF);
}

void clearFrame(Frame&frame,glm::uvec3 const&color,float d){
  for(uint32_t y=0;y<frame.height;++y)
    for(uint32_t x=0;x<frame.width;++x){
      auto pix = y*frame.width+x;
      for(uint32_t c=0;c<3;++c)
        frame.color[pix*4+c] = color[c];
      frame.color[pix*4+3] = 0;
      frame.depth[pix] = d;
    }
}

void clearFrame(Frame&frame,glm:: vec3 const&c,float d){
  clearFrame(frame,glm::uvec3(glm::clamp(c*255.f,glm::vec3(0.f),glm::vec3(255.))),d);
}

glm::uvec3 readColor(Frame const&frame,glm::uvec2 const&coord){
  auto pix = coord.y*frame.width + coord.x;
  return glm::uvec3(frame.color[pix*4+0],frame.color[pix*4+1],frame.color[pix*4+2]);
}

float      readDepth(Frame const&frame,glm::uvec2 const&coord){
  auto pix = coord.y*frame.width + coord.x;
  return frame.depth[pix];
}

glm::uvec3  alphaMix(glm::uvec3 const&frameColor,glm::vec4 const&fragColor){
  return glm::clamp(glm::mix(glm::vec3(frameColor)/255.f,glm::vec3(fragColor),fragColor[3]),0.f,1.f)*255.f;
}


std::string padding(size_t n=2){
  return std::string(n,' ');
}

void printChild(Node const&node,size_t child,size_t p);

void printNodeContent(Node const&node,size_t p){
  std::cerr << padding(p) << "  mesh = " << node.mesh << std::endl;
  std::cerr << padding(p) << "  modelMatrix = " << str(node.modelMatrix) << std::endl;
  std::cerr << padding(p) << "  children["<<node.children.size()<<"] = {";
  if(!node.children.empty()){
    std::cerr << std::endl;
    for(size_t i=0;i<node.children.size();++i)
      printChild(node.children.at(i),i,p+2);
    std::cerr << padding(p) << "  ";
  }
  std::cerr << "}" << std::endl;
}

void printChild(Node const&node,size_t child,size_t p){
  std::cerr << padding(p+2) << "child" << child << "{" << std::endl;
  printNodeContent(node,p+2);
  std::cerr << padding(p+2) << "}" << std::endl;
}

void printVertexAttrib(VertexAttrib const&a,uint32_t p){
  std::cerr << padding(p) << "bufferData = " << a.bufferData<< std::endl;
  std::cerr << padding(p) << "type       = " << str(a.type) << std::endl;
  std::cerr << padding(p) << "stride     = " << a.stride    << std::endl;
  std::cerr << padding(p) << "offset     = " << a.offset    << std::endl;
}

void printModel(Model const&model){
  std::cerr << padding(4)<<"Model vypadá takto:" << std::endl;
  std::cerr << padding(4)<<"model{" << std::endl;
  std::cerr << padding(4)<<"  meshes  [" << model.meshes  .size() << "]"<< std::endl;
  std::cerr << padding(4)<<"  roots   [" << model.roots   .size() << "]"<< std::endl;
  std::cerr << padding(4)<<"  textures[" << model.textures.size() << "]"<< std::endl;
  std::cerr << padding(4)<<"}" << std::endl;
  for(size_t i=0;i<model.meshes.size();++i){
    auto const&mesh = model.meshes.at(i);
    std::cerr << padding(4) << "mesh" << i << "{" << std::endl;
    std::cerr << padding(4) << "  nofIndices     = " << mesh.nofIndices        << std::endl;
    std::cerr << padding(4) << "  diffuseColor   = " << str(mesh.diffuseColor) << std::endl;
    std::cerr << padding(4) << "  diffuseTexture = " << mesh.diffuseTexture    << std::endl;
    std::cerr << padding(4) << "  indices        = " << mesh.indices           << std::endl;
    std::cerr << padding(4) << "  indexType      = " << str(mesh.indexType)    << std::endl;
    std::cerr << padding(4) << "  position{" << std::endl;
    printVertexAttrib(mesh.position,8);
    std::cerr << padding(4) << "  }" << std::endl;
    std::cerr << padding(4) << "  normal{" << std::endl;
    printVertexAttrib(mesh.normal,8);
    std::cerr << padding(4) << "  }" << std::endl;
    std::cerr << padding(4) << "  texCoord{" << std::endl;
    printVertexAttrib(mesh.texCoord,8);
    std::cerr << padding(4) << "  }" << std::endl;
    std::cerr << padding(4) << "}" << std::endl;
  }
  for(size_t i=0;i<model.textures.size();++i){
    auto const&texture = model.textures.at(i);
    std::cerr << padding(4) << "texture" << i << "{" << std::endl;
    std::cerr << padding(4) << "  data     = " << (void*)texture.data << std::endl;
    std::cerr << padding(4) << "  channels = " << texture.channels    << std::endl;
    std::cerr << padding(4) << "  width    = " << texture.width       << std::endl;
    std::cerr << padding(4) << "  height   = " << texture.height      << std::endl;
    std::cerr << padding(4) << "}" << std::endl;
  }
  for(size_t i=0;i<model.roots.size();++i){
    auto const&node = model.roots.at(i);
    std::cerr << padding(4) << "root" << i << "{" << std::endl;
    printNodeContent(node,4);
    std::cerr << padding(4) << "}" << std::endl;
  }
}


bool operator==(VertexAttrib const&a,VertexAttrib const&b){
  if(a.bufferData != b.bufferData)return false;
  if(a.offset != b.offset)return false;
  if(a.stride != b.stride)return false;
  if(a.type != b.type)return false;
  return true;
}


}
