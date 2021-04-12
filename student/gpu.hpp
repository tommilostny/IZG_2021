/*!
 * @file
 * @brief This file contains class that represents graphic card.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */
#pragma once

#include <student/fwd.hpp>

void clear(GPUContext&ctx,float r,float g,float b,float a);

/**
 * @brief Function that renders triangles
 *
 * @param ctx GPUContext
 * @param n number of vertices
 */
extern void(*drawTriangles)(GPUContext&ctx,uint32_t n);

glm::vec4 read_texture(Texture const&texture,glm::vec2 uv);
