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
	static void Run(InVertex &inVertex, VertexArray &vao, uint32_t invokeNum)
	{
		ComputeVertexId(inVertex, vao, invokeNum);
		ReadAttributes(inVertex, vao);
	}

private:
	static void ComputeVertexId(InVertex &inVertex, VertexArray &vao, uint32_t invokeNum)
	{
		if (vao.indexBuffer == nullptr)
		{
			inVertex.gl_VertexID = invokeNum;
		}
		else switch (vao.indexType)
		{
		case IndexType::UINT8:
			inVertex.gl_VertexID = ((uint8_t*)vao.indexBuffer)[invokeNum];
			break;
		case IndexType::UINT16:
			inVertex.gl_VertexID = ((uint16_t*)vao.indexBuffer)[invokeNum];
			break;
		case IndexType::UINT32:
			inVertex.gl_VertexID = ((uint32_t*)vao.indexBuffer)[invokeNum];
			break;
		}
	}

	static void ReadAttributes(InVertex &inVertex, VertexArray &vao)
	{
		for (uint32_t i = 0; i < maxAttributes; i++)
		{
			if (vao.vertexAttrib[i].type == AttributeType::EMPTY)
				continue;

			auto ptr = (uint8_t*)vao.vertexAttrib[i].bufferData + vao.vertexAttrib[i].stride * inVertex.gl_VertexID + vao.vertexAttrib[i].offset;

			switch (vao.vertexAttrib[i].type)
			{
			case AttributeType::FLOAT:
				inVertex.attributes[i].v1 = *(float*)ptr;
				break;
			case AttributeType::VEC2:
				inVertex.attributes[i].v2.x = *(float*)ptr;
				inVertex.attributes[i].v2.y = *(float*)(ptr + sizeof(float));
				break;
			case AttributeType::VEC3:
				inVertex.attributes[i].v3.x = *(float*)ptr;
				inVertex.attributes[i].v3.y = *(float*)(ptr + sizeof(float));
				inVertex.attributes[i].v3.z = *(float*)(ptr + 2 * sizeof(float));
				break;
			case AttributeType::VEC4:
				inVertex.attributes[i].v4.x = *(float*)ptr;
				inVertex.attributes[i].v4.y = *(float*)(ptr + sizeof(float));
				inVertex.attributes[i].v4.z = *(float*)(ptr + 2 * sizeof(float));
				inVertex.attributes[i].v4.w = *(float*)(ptr + 3 * sizeof(float));
				break;
			}
		}
	}
};

class PrimitiveAssembly
{
public:
	static void Run(std::vector<OutVertex> &primitive, VertexArray &vao, Program &prg, uint32_t primitiveIndex, uint8_t vecticesCount)
	{
		for (uint32_t v = 0; v < vecticesCount; v++)
		{
			InVertex inVertex;
			OutVertex outVertex;

			VertexAssembly::Run(inVertex, vao, v + primitiveIndex);

			prg.vertexShader(outVertex, inVertex, prg.uniforms);
			primitive.push_back(outVertex);
		}
	}
};

class PerspectiveDivision
{
public:
	static void Run(std::vector<OutVertex> &primitive)
	{
		for (int v = 0; v < 3; v++)
		{
			for (size_t i = 0; i < maxAttributes; i++)
			{
				auto xyzw = primitive[v].attributes[i].v4;
				primitive[v].attributes[i].v3 = glm::vec3(xyzw.x / xyzw.w, xyzw.y / xyzw.w, xyzw.z / xyzw.w);
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

	for (uint32_t t = 0; t < nofVertices; t += 3)
	{
		std::vector<OutVertex> primitive;
		PrimitiveAssembly::Run(primitive, ctx.vao, ctx.prg, t, 3);
		PerspectiveDivision::Run(primitive);
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

