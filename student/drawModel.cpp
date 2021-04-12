/*!
 * @file
 * @brief This file contains functions for model rendering
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */
#include <student/drawModel.hpp>
#include <student/gpu.hpp>


/**
 * @brief This function renders a model
 *
 * @param ctx GPUContext
 * @param model model structure
 * @param proj projection matrix
 * @param view view matrix
 * @param light light position
 * @param camera camera position (unused)
 */
//! [drawModel]
void drawModel(GPUContext&ctx,Model const&model,glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera){
  (void)ctx;
  (void)model;
  (void)proj;
  (void)view;
  (void)light;
  (void)camera;
  /// \todo Tato funkce vykreslí model.<br>
  /// Vaším úkolem je správně projít model a vykreslit ho pomocí funkce drawTriangles (nevolejte drawTrianglesImpl, je to z důvodu testování).
  /// Bližší informace jsou uvedeny na hlavní stránce dokumentace.
}
//! [drawModel]

/**
 * @brief This function represents vertex shader of texture rendering method.
 *
 * @param outVertex output vertex
 * @param inVertex input vertex
 * @param uniforms uniform variables
 */
//! [drawModel_vs]
void drawModel_vertexShader(OutVertex&outVertex,InVertex const&inVertex,Uniforms const&uniforms){
  (void)outVertex;
  (void)inVertex;
  (void)uniforms;
  /// \todo Tato funkce reprezentujte vertex shader.<br>
  /// Vaším úkolem je správně trasnformovat vrcholy modelu.
  /// Bližší informace jsou uvedeny na hlavní stránce dokumentace.
}
//! [drawModel_vs]

/**
 * @brief This functionrepresents fragment shader of texture rendering method.
 *
 * @param outFragment output fragment
 * @param inFragment input fragment
 * @param uniforms uniform variables
 */
//! [drawModel_fs]
void drawModel_fragmentShader(OutFragment&outFragment,InFragment const&inFragment,Uniforms const&uniforms){
  (void)outFragment;
  (void)inFragment;
  (void)uniforms;
  /// \todo Tato funkce reprezentujte fragment shader.<br>
  /// Vaším úkolem je správně obarvit fragmenty a osvětlit je pomocí lambertova osvětlovacího modelu.
  /// Bližší informace jsou uvedeny na hlavní stránce dokumentace.
}
//! [drawModel_fs]

