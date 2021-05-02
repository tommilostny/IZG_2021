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

class Triangle
{
private:
	OutVertex _points[3];

	inline float Minimum(float a, float b) { return a > b ? b : a; }
	inline float Maximum(float a, float b) { return a > b ? a : b; }

	inline float EdgeFunction(glm::vec4 &point, float x, float y, float deltaX, float deltaY)
	{
		return (y - point.y) * deltaX - (x - point.x) * deltaY;
	}

public:
	//Primitive Assembly jako konstruktor trojúhelníka
	Triangle(VertexArray &vao, Program &prg, uint32_t triangleId)
	{
		for (uint32_t v = triangleId; v < triangleId + 3; v++)
		{
			InVertex inVertex;
			VertexAssembly::Run(inVertex, vao, v);
			prg.vertexShader(_points[v - triangleId], inVertex, prg.uniforms);
		}
	}

	void PerspectiveDivision()
	{
		for (uint8_t v = 0; v < 3; v++)
		{
			auto point = _points[v].gl_Position;
			_points[v].gl_Position = glm::vec4(point.x / point.w, point.y / point.w, point.z / point.w, 1.0);
		}
	}

	void ViewportTransformation(Frame &frame)
	{
		for (uint8_t v = 0; v < 3; v++)
		{
			auto point = _points[v].gl_Position;
			_points[v].gl_Position = glm::vec4((point.x * 0.5 + 0.5) * frame.width, (point.y * 0.5 + 0.5) * frame.height, point.z, 1.0);
		}
	}

	//Rasterize triangle with Pineda algorithm
	void Rasterize(Frame &frame, Program &prg)
	{
		auto minX = Minimum(_points[0].gl_Position.x, Minimum(_points[1].gl_Position.x, _points[2].gl_Position.x));
		auto minY = Minimum(_points[0].gl_Position.y, Minimum(_points[1].gl_Position.y, _points[2].gl_Position.y));
		auto maxX = Maximum(_points[0].gl_Position.x, Maximum(_points[1].gl_Position.x, _points[2].gl_Position.x));
		auto maxY = Maximum(_points[0].gl_Position.y, Maximum(_points[1].gl_Position.y, _points[2].gl_Position.y));

		minX = Maximum(minX, 0);
		minY = Maximum(minY, 0);
		maxX = Minimum(maxX, frame.width - 1);
		maxY = Minimum(maxY, frame.height - 1);

		auto deltaX1 = _points[1].gl_Position.x - _points[0].gl_Position.x;
		auto deltaX2 = _points[2].gl_Position.x - _points[1].gl_Position.x;
		auto deltaX3 = _points[0].gl_Position.x - _points[2].gl_Position.x;

		auto deltaY1 = _points[1].gl_Position.y - _points[0].gl_Position.y;
		auto deltaY2 = _points[2].gl_Position.y - _points[1].gl_Position.y;
		auto deltaY3 = _points[0].gl_Position.y - _points[2].gl_Position.y;

		auto edgeF1 = EdgeFunction(_points[0].gl_Position, minX, minY, deltaX1, deltaY1);
		auto edgeF2 = EdgeFunction(_points[1].gl_Position, minX, minY, deltaX2, deltaY2);
		auto edgeF3 = EdgeFunction(_points[2].gl_Position, minX, minY, deltaX3, deltaY3);

		for (auto x = minX; x <= maxX; x++)
		{
			auto e1 = edgeF1;
			auto e2 = edgeF2;
			auto e3 = edgeF3;

			for (auto y = minY; y <= maxY; y++)
			{
				if (e1 >= 0 && e2 >= 0 && e3 >= 0)
				{
					InFragment inFragment;
					inFragment.gl_FragCoord = glm::vec4(x + 0.5, y + 0.5, 0, 1);

					if (inFragment.gl_FragCoord.x > 0 && inFragment.gl_FragCoord.x < frame.width
						&& inFragment.gl_FragCoord.y > 0 && inFragment.gl_FragCoord.y < frame.height
						)//&& inFragment.gl_FragCoord.x + inFragment.gl_FragCoord.y <= sqrt((frame.width - minX) * (frame.height - minY)))
					{
						OutFragment outFragment;
						prg.fragmentShader(outFragment, inFragment, prg.uniforms);
					}
				}
				e1 += deltaX1; e2 += deltaX2; e3 += deltaX3;
			}
			edgeF1 -= deltaY1; edgeF2 -= deltaY2; edgeF3 -= deltaY3;
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

