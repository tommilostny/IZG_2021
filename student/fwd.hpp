/*!
 * @file
 * @brief This file contains forward declarations and constants.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include <glm/glm.hpp>
#include <cstdint>
#include <vector>

//#define MAKE_STUDENT_RELEASE

uint32_t const maxAttributes = 16;///< maximum number of vertex/fragment attributes
uint32_t const maxUniforms   = 16;///< maximum number of uniform variables
uint32_t const maxTextures   = 8 ;///< maximum number of textures

/**
 * @brief This struct represent a texture
 */
//! [Texture]
struct Texture{
  uint8_t const* data     = nullptr;///< pointer to data
  uint32_t       width    = 0      ;///< width of the texture
  uint32_t       height   = 0      ;///< height of the texture
  uint32_t       channels = 3      ;///< number of channels of the texture
};
//! [Texture]

/**
 * @brief This enum represents vertex/fragment attribute type.
 */
//! [AttributeType]
enum class AttributeType{
  EMPTY = 0, ///< disabled attribute
  FLOAT = 1, ///< 1x 32-bit float
  VEC2  = 2, ///< 2x 32-bit floats
  VEC3  = 3, ///< 3x 32-bit floats
  VEC4  = 4, ///< 4x 32-bit floats
};
//! [AttributeType]

/**
 * @brief This union represents one vertex/fragment attribute
 */
//! [Attribute]
union Attribute{
  Attribute(){}
  float     v1; ///< single float
  glm::vec2 v2; ///< vector of two floats
  glm::vec3 v3; ///< vector of three floats
  glm::vec4 v4 = glm::vec4(1.f); ///< vector of four floats
};
//! [Attribute]

/**
 * @brief This struct represents input vertex of vertex shader.
 */
//! [InVertex]
struct InVertex{
  Attribute attributes[maxAttributes]    ; ///< vertex attributes
  uint32_t  gl_VertexID               = 0; ///< vertex id
};
//! [InVertex]

/**
 * @brief This struct represents output vertex of vertex shader.
 */
//! [OutVertex]
struct OutVertex{
  Attribute attributes[maxAttributes]                     ; ///< vertex attributes
  glm::vec4 gl_Position               = glm::vec4(0,0,0,1); ///< clip space position
};
//! [OutVertex]

/**
 * @brief This struct represents input fragment.
 */
//! [InFragment]
struct InFragment{
  Attribute attributes[maxAttributes]               ; ///< fragment attributes
  glm::vec4 gl_FragCoord              = glm::vec4(1); ///< fragment coordinates
};
//! [InFragment]

/**
 * @brief This struct represents output fragment.
 */
//! [OutFragment]
struct OutFragment{
  glm::vec4 gl_FragColor = glm::vec4(0.f); ///< fragment color
};
//! [OutFragment]

/**
 * @brief This union represents one uniform variable.
 */
//! [Uniform]
union Uniform{
  Uniform(){}
  float     v1; ///< single float
  glm::vec2 v2; ///< two floats
  glm::vec3 v3; ///< three floats
  glm::vec4 v4; ///< four floats
  glm::mat4 m4 = glm::mat4(1.f); ///< 4x4 float matrix
};
//! [Uniform]

/**
 * @brief This struct represents shader program uniform variables
 */
//! [Uniforms]
struct Uniforms{
  Uniform uniform [maxUniforms];///< uniform variables
  Texture textures[maxTextures];///< textures
};
//! [Uniforms]

/**
 * @brief This enum represents index type
 */
//! [IndexType]
enum class IndexType{
  UINT8  = 1, ///< uin8_t type
  UINT16 = 2, ///< uin16_t type
  UINT32 = 4, ///< uint32_t type
};
//! [IndexType]

/**
 * @brief Function type for vertex shader
 *
 * @param outVertex output vertex
 * @param inVertex input vertex
 * @param uniforms uniform variables
 */
//! [VertexShader]
using VertexShader   = void(*)(
    OutVertex      &outVertex,
    InVertex  const&inVertex ,
    Uniforms  const&uniforms );
//! [VertexShader]

/**
 * @brief Function type for fragment shader
 *
 * @param outFragment output fragment
 * @param inFragment input fragment 
 * @param uniforms uniform variables
 */
//! [FragmentShader]
using FragmentShader = void(*)(
    OutFragment      &outFragment,
    InFragment  const&inFragment ,
    Uniforms    const&uniforms   );
//! [FragmentShader]

/**
 * @brief This struct describes location of one vertex attribute.
 */
//! [VertexAttrib]
struct VertexAttrib{
  void const*   bufferData = nullptr             ;///< pointer to buffer
  uint64_t      stride     = 0                   ;///< stride in bytes
  uint64_t      offset     = 0                   ;///< offset in bytes
  AttributeType type       = AttributeType::EMPTY;///< type of attribute
};
//! [VertexAttrib]

/**
 * @brief This structure represents setting for vertex pulller (vertex assembly) unit.
 * VertexArrays holds setting for reading vertices from buffers.
 */
//! [VertexArray]
struct VertexArray{
  VertexAttrib vertexAttrib[maxAttributes];     ///< settings for vertex attributes
  void const*  indexBuffer = nullptr;           ///< pointer to index buffer of NULL
  IndexType    indexType   = IndexType::UINT32; ///< type of indices
};
//! [VertexArray]

/**
 * @brief This structu represents a program.
 * Vertex Shader is executed on every InVertex.
 * Fragment Shader is executed on every rasterized InFragment.
 */
//! [Program]
struct Program{
  VertexShader   vertexShader   = nullptr; ///< vertex shader
  FragmentShader fragmentShader = nullptr; ///< fragment shader
  Uniforms       uniforms                ; ///< uniform variables 
  AttributeType  vs2fs[maxAttributes] = {AttributeType::EMPTY}; ///< which attributes are interpolated from vertex shader to fragment shader
};
//! [Program]

/**
 * @brief This structure represents a frame.
 * Frame (or framebuffer) is used as output of rendering.
 */
//! [Frame]
struct Frame{
  uint8_t* color  = nullptr; ///< color buffer
  float  * depth  = nullptr; ///< depth buffer
  uint32_t width  = 0      ; ///< width of frame
  uint32_t height = 0      ; ///< height of frame
};
//! [Frame]



/**
 * @brief This structure represents a GPU state (context).
 * GPUContext holds all data required for rendering.
 *
 */
//! [GPUContext]
struct GPUContext{
  VertexArray vao                    ; ///< active vertex array (input/ triangles)
  Program     prg                    ; ///< active program (shaders, uniforms, textures)
  Frame       frame                  ; ///< active frame (output of rendering)
};
//! [GPUContext]


/**
 * @brief This struct represents a mesh
 */
//! [Mesh]
struct Mesh{
  void const*  indices     = nullptr          ;///< indices to vertices or nullptr
  IndexType    indexType   = IndexType::UINT32;///< type of indices
  VertexAttrib position                       ;///< position vertex attribute
  VertexAttrib normal                         ;///< normal vertex attribute
  VertexAttrib texCoord                       ;///< tex. coord vertex attribute
  uint32_t     nofIndices  = 0                ;///< nofIndices or nofVertices (if there is no indexing)
  glm::vec4    diffuseColor = glm::vec4(1.f)  ;///< default diffuseColor (if there is no texture)
  int          diffuseTexture = -1            ;///< diffuse texture or -1 (no texture)
};
//! [Mesh]

/**
 * @brief This structure represents node in tree structure of model
 */
//! [Node]
struct Node{
  glm::mat4        modelMatrix = glm::mat4(1.f);///< model transformation matrix
  int32_t          mesh = -1;                   ///< id of mesh or -1 if no mesh
  std::vector<Node>children;                    ///< list of children nodes
};
//! [Node]

/**
 * @brief This struct represent model
 */
//! [Model]
struct Model{
  std::vector<Mesh   >meshes  ;///< list of all meshes in model
  std::vector<Node   >roots   ;///< list of roots of node trees
  std::vector<Texture>textures;///< list of all textures in model
};
//! [Model]
