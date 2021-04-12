#pragma once

#include<cstddef>
#include<string>
#include<vector>

#include<glm/glm.hpp>
#include<student/fwd.hpp>

namespace tests{

float const extern floatErr;

bool equalFloats(float const& a, float const& b,float err = floatErr);
bool equalVec2(glm::vec2 const&a,glm::vec2 const&b,float err = floatErr);
bool equalVec3(glm::vec3 const&a,glm::vec3 const&b,float err = floatErr);
bool equalVec4(glm::vec4 const&a,glm::vec4 const&b,float err = floatErr);

bool equalCounts(size_t a,size_t b,size_t err = 10);

bool greaterFloat(float a,float b,float err = floatErr);

bool lessFloat(float a,float b,float err = floatErr);

std::string str(float     const&a);
std::string str(glm::vec2 const&a);
std::string str(glm::vec3 const&a);
std::string str(glm::vec4 const&a);

std::string str(uint32_t  const&a);
std::string str(glm::uvec2 const&a);
std::string str(glm::uvec3 const&a);
std::string str(glm::uvec4 const&a);

std::string str(glm::mat4 const&m);

std::string str(IndexType const&i);
std::string str(AttributeType const&a);

void printModel(Model const&m);
void printVertexAttrib(VertexAttrib const&a,uint32_t p);

bool operator==(VertexAttrib const&a,VertexAttrib const&b);

extern std::vector<InVertex>inVertices;
extern std::vector<OutVertex>outVertices;
extern std::vector<InFragment>inFragments;
extern Uniforms unif;

void vertexShaderDump(OutVertex&,InVertex const&i,Uniforms const&u);
void vertexShaderInject(OutVertex&outVertex,InVertex const&inVertex,Uniforms const&);
void fragmentShaderDump(OutFragment&,InFragment const&inF,Uniforms const&);
void clearFrame(Frame&frame,glm::vec3 const&c = glm::vec3(0.f),float d=1.f);
void clearFrame(Frame&frame,glm::uvec3 const&c = glm::uvec3(0),float d=1.f);
glm::uvec3 readColor(Frame const&frame,glm::uvec2 const&coord);
float      readDepth(Frame const&frame,glm::uvec2 const&coord);
glm::uvec3 alphaMix(glm::uvec3 const&frameColor,glm::vec4 const&fragColor);

}
