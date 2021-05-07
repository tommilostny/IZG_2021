/*!
 * @file
 * @brief This file contains functions for model rendering
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */
#include <student/drawModel.hpp>
#include <student/gpu.hpp>

void drawNode(GPUContext &ctx, Node const&node, Model const&model, glm::mat4 &matrix, glm::mat4 const&proj, glm::mat4 const&view, glm::vec3 const&light)
{
    if (node.mesh >= 0)
    {
        auto mesh = model.meshes[node.mesh];

        auto combinedMatrix = matrix * node.modelMatrix;
        ctx.prg.uniforms.uniform[1].m4 = combinedMatrix;
        ctx.prg.uniforms.uniform[2].m4 = glm::inverse(glm::transpose(combinedMatrix));

        ctx.prg.vertexShader = drawModel_vertexShader;
        ctx.prg.fragmentShader = drawModel_fragmentShader;

        ctx.prg.vs2fs[0] = AttributeType::VEC3;
        ctx.prg.vs2fs[1] = AttributeType::VEC3;
        ctx.prg.vs2fs[2] = AttributeType::VEC2;

        ctx.prg.uniforms.uniform[0].m4 = proj * view;
        ctx.prg.uniforms.uniform[3].v3 = light;

        drawTriangles(ctx, mesh.nofIndices);
    }

    for (auto child : node.children)
        drawNode(ctx, child, model, matrix, proj, view, light);
}

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

    auto identityMatrix = glm::mat4(1.f);

    for (auto root : model.roots)
        drawNode(ctx, root, model, identityMatrix, proj, view, light);
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

