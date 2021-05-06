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
			_points[v].gl_Position.x /= _points[v].gl_Position.w;
			_points[v].gl_Position.y /= _points[v].gl_Position.w;
			_points[v].gl_Position.z /= _points[v].gl_Position.w;
		}
	}

	void ViewportTransformation(Frame &frame)
	{
		for (uint8_t v = 0; v < 3; v++)
		{
			_points[v].gl_Position.x = (_points[v].gl_Position.x * 0.5 + 0.5) * frame.width;
			_points[v].gl_Position.y = (_points[v].gl_Position.y * 0.5 + 0.5) * frame.height;
		}
	}

	//Rasterizace trojúhelníka Pinedovým algoritmem
	void Rasterize(Frame &frame, Program &prg)
	{
		//Obalový obdélník trojúhelníku
		auto minX = Min(_points[0].gl_Position.x, Min(_points[1].gl_Position.x, _points[2].gl_Position.x));
		auto minY = Min(_points[0].gl_Position.y, Min(_points[1].gl_Position.y, _points[2].gl_Position.y));
		auto maxX = Max(_points[0].gl_Position.x, Max(_points[1].gl_Position.x, _points[2].gl_Position.x));
		auto maxY = Max(_points[0].gl_Position.y, Max(_points[1].gl_Position.y, _points[2].gl_Position.y));

		minX = Max(minX, 0);
		minY = Max(minY, 0);
		maxX = Min(maxX, frame.width - 1);
		maxY = Min(maxY, frame.height - 1);

		auto deltaX1 = _points[1].gl_Position.x - _points[0].gl_Position.x;
		auto deltaX2 = _points[2].gl_Position.x - _points[1].gl_Position.x;
		auto deltaX3 = _points[0].gl_Position.x - _points[2].gl_Position.x;

		auto deltaY1 = _points[1].gl_Position.y - _points[0].gl_Position.y;
		auto deltaY2 = _points[2].gl_Position.y - _points[1].gl_Position.y;
		auto deltaY3 = _points[0].gl_Position.y - _points[2].gl_Position.y;

		//Startovací hranové funkce, pro pohyb v ose X
		auto edgeStart1 = EdgeFunction(0, minX, minY, deltaX1, deltaY1);
		auto edgeStart2 = EdgeFunction(1, minX, minY, deltaX2, deltaY2);
		auto edgeStart3 = EdgeFunction(2, minX, minY, deltaX3, deltaY3);

		//Předpočítání přepony a obsahu celého trojúhelníku
		auto hypotenuse = Max(PointSum(0), Max(PointSum(1), PointSum(2)));
		auto triangleArea = Area(_points[0].gl_Position.x, _points[0].gl_Position.y,
							_points[1].gl_Position.x, _points[1].gl_Position.y,
							_points[2].gl_Position.x, _points[2].gl_Position.y);

		for (uint32_t x = minX; x <= maxX; x++)
		{
			//Inicializace hranových funckí pro pohyb v ose Y
			auto e1 = edgeStart1;
			auto e2 = edgeStart2;
			auto e3 = edgeStart3;

			for (uint32_t y = minY; y <= maxY; y++)
			{
				if (e1 >= 0 && e2 >= 0 && e3 >= 0) //Fragment je v trojúhelníku
				{
					InFragment inFragment;
					if (CreateFragment(inFragment, x, y, frame, prg.vs2fs, hypotenuse, triangleArea))
					{
						OutFragment outFragment;
						prg.fragmentShader(outFragment, inFragment, prg.uniforms);
						PerFragmentOperations(frame, outFragment, x, y, inFragment.gl_FragCoord.z);
					}
				}
				e1 += deltaX1;
				e2 += deltaX2;
				e3 += deltaX3;
			}
			edgeStart1 -= deltaY1;
			edgeStart2 -= deltaY2;
			edgeStart3 -= deltaY3;
		}
	}

protected:
	bool CreateFragment(InFragment &inFragment, float x, float y, Frame &frame, AttributeType *vs2fs, float hypotenuse, float triangleArea)
	{
		inFragment.gl_FragCoord.x = x + 0.5;
		inFragment.gl_FragCoord.y = y + 0.5;

		if (inFragment.gl_FragCoord.x + inFragment.gl_FragCoord.y <= hypotenuse) //kontrola přepony
		{
			auto lambda0 = Lambda(2, 1, inFragment, triangleArea);
			auto lambda1 = Lambda(0, 2, inFragment, triangleArea);
			auto lambda2 = Lambda(1, 0, inFragment, triangleArea);

			inFragment.gl_FragCoord.z = _points[0].gl_Position.z * lambda0
				+ _points[1].gl_Position.z * lambda1
				+ _points[2].gl_Position.z * lambda2;

			auto h0 = _points[0].gl_Position.w;
			auto h1 = _points[1].gl_Position.w;
			auto h2 = _points[2].gl_Position.w;

			auto s = lambda0 / h0 + lambda1 / h1 + lambda2 / h2;
			lambda0 /= h0 * s;
			lambda1 /= h1 * s;
			lambda2 /= h2 * s;

			for (uint32_t i = 0; i < maxAttributes; i++)
			{
				if (vs2fs[i] == AttributeType::EMPTY)
					continue;

				inFragment.attributes[i].v4 = _points[0].attributes[i].v4 * lambda0
					+ _points[1].attributes[i].v4 * lambda1
					+ _points[2].attributes[i].v4 * lambda2;
			}
			return true;
		}
		return false;
	}

	void PerFragmentOperations(Frame &frame, OutFragment &outFragment, uint32_t x, uint32_t y, float fragmentDepth)
	{
		auto bufferIndex = x + y * frame.width;
		if (fragmentDepth < frame.depth[bufferIndex]) //Depth test
		{
			auto alpha = outFragment.gl_FragColor.a;
			
			if (alpha > 0.5) frame.depth[bufferIndex] = fragmentDepth;
			bufferIndex <<= 2;
			
			//Blending
			frame.color[bufferIndex] = ClampColor((frame.color[bufferIndex] / 255.0) * (1 - alpha) + outFragment.gl_FragColor.r * alpha, 0, 1) * 255;
			frame.color[bufferIndex + 1] = ClampColor((frame.color[bufferIndex + 1] / 255.0) * (1 - alpha) + outFragment.gl_FragColor.g * alpha, 0, 1) * 255;
			frame.color[bufferIndex + 2] = ClampColor((frame.color[bufferIndex + 2] / 255.0) * (1 - alpha) + outFragment.gl_FragColor.b * alpha, 0, 1) * 255;
			frame.color[bufferIndex + 3] = ClampColor((frame.color[bufferIndex + 3] / 255.0) * (1 - alpha) + outFragment.gl_FragColor.a * alpha, 0, 1) * 255;
		}
	}

//Pomocné Triangle privátní funkce
private:
	static inline float Min(float a, float b) { return a > b ? b : a; }
	static inline float Max(float a, float b) { return a > b ? a : b; }

	inline float EdgeFunction(uint8_t pointIndex, float x, float y, float deltaX, float deltaY)
	{
		return (y - _points[pointIndex].gl_Position.y) * deltaX - (x - _points[pointIndex].gl_Position.x) * deltaY;
	}

	inline float PointSum(uint8_t pointIndex)
	{
		return _points[pointIndex].gl_Position.x + _points[pointIndex].gl_Position.y;
	}

	float Lambda(uint8_t pointIndex1, uint8_t pointIndex2, InFragment &fragment, float wholeTriangleArea)
	{
		auto subTriangle = Area(_points[pointIndex1].gl_Position.x, _points[pointIndex1].gl_Position.y,
								_points[pointIndex2].gl_Position.x, _points[pointIndex2].gl_Position.y,
								fragment.gl_FragCoord.x, fragment.gl_FragCoord.y);

		return subTriangle / wholeTriangleArea;
	}

	static float Area(float ax, float ay, float bx, float by, float cx, float cy)
	{
		auto a = sqrt(pow(cx - bx, 2) + pow(cy - by, 2));
		auto b = sqrt(pow(ax - cx, 2) + pow(ay - cy, 2));
		auto c = sqrt(pow(bx - ax, 2) + pow(by - ay, 2));

		auto s = (a + b + c) / 2.0;
		return sqrt(s * (s - a) * (s - b) * (s - c));
	}

	static float ClampColor(float color, float from, float to)
	{
		if (color < from) return from;
		if (color > to) return to;
		return color;
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

