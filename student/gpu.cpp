/*!
 * @file
 * @brief This file contains implementation of gpu
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <student/gpu.hpp>
#include <iostream> //remove on release

class VertexAssembly
{
public:
	static void run(InVertex &in_vertex, VertexArray &vao, uint32_t invoke_num)
	{
		compute_vertex_id(in_vertex, vao, invoke_num);
		read_attributes(in_vertex, vao);
	}

private:
	static void compute_vertex_id(InVertex &in_vertex, VertexArray &vao, uint32_t invoke_num)
	{
		if (vao.indexBuffer == nullptr)
		{
			in_vertex.gl_VertexID = invoke_num;
		}
		else switch (vao.indexType)
		{
		case IndexType::UINT8:
			in_vertex.gl_VertexID = ((uint8_t*)vao.indexBuffer)[invoke_num];
			break;
		case IndexType::UINT16:
			in_vertex.gl_VertexID = ((uint16_t*)vao.indexBuffer)[invoke_num];
			break;
		case IndexType::UINT32:
			in_vertex.gl_VertexID = ((uint32_t*)vao.indexBuffer)[invoke_num];
			break;
		}
	}

	static void read_attributes(InVertex &in_vertex, VertexArray &vao)
	{
		for (uint32_t i = 0; i < maxAttributes && vao.vertexAttrib[i].type != AttributeType::EMPTY; i++)
		{
			auto step = (vao.vertexAttrib[i].stride * in_vertex.gl_VertexID) / sizeof(float);
			auto ptr = (float*)vao.vertexAttrib[i].bufferData + vao.vertexAttrib[i].offset / sizeof(float) + step;

			switch (vao.vertexAttrib[i].type)
			{
			case AttributeType::FLOAT:
				in_vertex.attributes[i].v1 = *ptr;
				break;
			case AttributeType::VEC2:
				in_vertex.attributes[i].v2.x = *ptr;
				in_vertex.attributes[i].v2.y = *(ptr + step);
				break;
			case AttributeType::VEC3:
				in_vertex.attributes[i].v3.x = *ptr;
				in_vertex.attributes[i].v3.y = *(ptr + step);
				in_vertex.attributes[i].v3.z = *(ptr + 2 * step);
				break;
			case AttributeType::VEC4:
				in_vertex.attributes[i].v4.x = *ptr;
				in_vertex.attributes[i].v4.y = *(ptr + step);
				in_vertex.attributes[i].v4.z = *(ptr + 2 * step);
				in_vertex.attributes[i].v4.w = *(ptr + 3 * step);
				break;
			}
		}
	}
};

//! [drawTrianglesImpl]
void drawTrianglesImpl(GPUContext &ctx, uint32_t nofVertices){
	(void)ctx;
	(void)nofVertices;
	/// \todo Tato funkce vykreslí trojúhelníky podle daného nastavení.<br>
	/// ctx obsahuje aktuální stav grafické karty.
	/// Parametr "nofVertices" obsahuje počet vrcholů, který by se měl vykreslit (3 pro jeden trojúhelník).<br>
	/// Bližší informace jsou uvedeny na hlavní stránce dokumentace.

	for (uint32_t v = 0; v < nofVertices; v++)
	{
		InVertex in_vertex;
		OutVertex out_vertex;

		VertexAssembly::run(in_vertex, ctx.vao, v);

		ctx.prg.vertexShader(out_vertex, in_vertex, ctx.prg.uniforms);
	}
}
//! [drawTrianglesImpl]

/**
 * @brief This function reads color from texture.
 *
 * @param texture texture
 * @param uv uv coordinates
 *
 * @return color 4 floats
 */
glm::vec4 read_texture(Texture const&texture,glm::vec2 uv){
	if(!texture.data)return glm::vec4(0.f);
	auto uv1 = glm::fract(uv);
	auto uv2 = uv1*glm::vec2(texture.width-1,texture.height-1)+0.5f;
	auto pix = glm::uvec2(uv2);
	//auto t   = glm::fract(uv2);
	glm::vec4 color = glm::vec4(0.f,0.f,0.f,1.f);
	for(uint32_t c=0;c<texture.channels;++c)
		color[c] = texture.data[(pix.y*texture.width+pix.x)*texture.channels+c]/255.f;
	return color;
}

/**
 * @brief This function clears framebuffer.
 *
 * @param ctx GPUContext
 * @param r red channel
 * @param g green channel
 * @param b blue channel
 * @param a alpha channel
 */
void clear(GPUContext&ctx,float r,float g,float b,float a){
	auto&frame = ctx.frame;
	auto const nofPixels = frame.width * frame.height;
	for(size_t i=0;i<nofPixels;++i){
		frame.depth[i] = 10e10f;
		frame.color[i*4+0] = static_cast<uint8_t>(glm::min(r*255.f,255.f));
		frame.color[i*4+1] = static_cast<uint8_t>(glm::min(g*255.f,255.f));
		frame.color[i*4+2] = static_cast<uint8_t>(glm::min(b*255.f,255.f));
		frame.color[i*4+3] = static_cast<uint8_t>(glm::min(a*255.f,255.f));
	}
}

