/*!
 * @file
 * @brief This file contains implementation of gpu
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <student/gpu.hpp>

#include <glm/gtc/matrix_transform.hpp> //idk..
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

class Triangle
{
public:
	OutVertex Points[3];

	//Primitive Assembly jako konstruktor trojúhelníka
	Triangle(VertexArray &vao, Program &prg, uint32_t triangleId)
	{
		for (uint32_t v = triangleId; v < triangleId + 3; v++)
		{
			InVertex inVertex;
			VertexAssembly::Run(inVertex, vao, v);
			prg.vertexShader(Points[v - triangleId], inVertex, prg.uniforms);
		}
	}

	void PerspectiveDivision()
	{
		for (int v = 0; v < 3; v++)
		{
			auto point = Points[v].gl_Position;
			Points[v].gl_Position = glm::vec4(point.x / point.w, point.y / point.w, point.z / point.w, 1.0);
		}
	}

	void ViewportTransformation(Frame &frame)
	{
		auto translate = glm::translate(glm::mat4(1.0), glm::vec3(1.0, 1.0, 0.0));
		auto scale = glm::scale(glm::mat4(1.0), glm::vec3(frame.width, frame.height, 1.0));

		for (int v = 0; v < 3; v++)
		{
			Points[v].gl_Position = scale * translate * Points[v].gl_Position;
		}
	}

	void Rasterize(Frame &frame, Program &prg)
	{
		auto v1 = Points[0].gl_Position;
		auto v2 = Points[1].gl_Position;
		auto v3 = Points[2].gl_Position;

		auto minX = Minimum(v1.x, Minimum(v2.x, v3.x));
		auto minY = Minimum(v1.y, Minimum(v2.y, v3.y));
		auto maxX = Maximum(v1.x, Maximum(v2.x, v3.x));
		auto maxY = Maximum(v1.y, Maximum(v2.y, v3.y));

		minX = Maximum(minX, 0);
		minY = Maximum(minY, 0);
		maxX = Minimum(maxX, frame.width - 1);
		maxY = Minimum(maxY, frame.height - 1);

		auto deltaX1 = v2.x - v1.x;
		auto deltaX2 = v3.x - v2.x;
		auto deltaX3 = v1.x - v3.x;

		auto deltaY1 = v2.y - v1.y;
		auto deltaY2 = v3.y - v2.y;
		auto deltaY3 = v1.y - v3.y;

		auto edgeF1 = (minY - v1.y) * deltaX1 - (minX - v1.x) * deltaY1;
		auto edgeF2 = (minY - v2.y) * deltaX2 - (minX - v2.x) * deltaY2;
		auto edgeF3 = (minY - v3.y) * deltaX3 - (minX - v3.x) * deltaY3;

		for (float y = minY; y <= maxY; y++)
		{
			bool even = (int)(y - minY) % 2 == 0;
			int startX = even ? minX : maxX;
			int endX = even ? maxX + 1 : minX - 1;
			int stepX = even ? 1 : -1;

			for (float x = startX; x != endX; x += stepX)
			{
				if (edgeF1 >= 0 && edgeF2 >= 0 && edgeF3 >= 0)
				{
					InFragment inFragment;
					OutFragment outFragment;
					prg.fragmentShader(outFragment, inFragment, prg.uniforms);
				}
				if (x != endX - stepX)
				{
					edgeF1 += even ? -deltaY1 : deltaY1;
					edgeF2 += even ? -deltaY2 : deltaY2;
					edgeF3 += even ? -deltaY3 : deltaY3;
				}
			}
			edgeF1 += deltaX1;
			edgeF2 += deltaX2;
			edgeF3 += deltaX3;
		}
	}

private:
	inline float Minimum(float a, float b) { return a > b ? b : a; }
	inline float Maximum(float a, float b) { return a > b ? a : b; }
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
		auto triangle = new Triangle(ctx.vao, ctx.prg, t);

		triangle->PerspectiveDivision();
		triangle->ViewportTransformation(ctx.frame);
		triangle->Rasterize(ctx.frame, ctx.prg);

		delete triangle;
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

