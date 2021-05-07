/*!
 * @file
 * @brief This file contains implementation of gpu
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 * 
 * student: Tomáš Milostný, xmilos02
 */

#include <student/gpu.hpp>

class VertexAssembly
{
public:
    static void Run(InVertex &inVertex, VertexArray &vao, uint32_t invokeId)
    {
        ComputeVertexId(inVertex, vao, invokeId);
        ReadAttributes(inVertex, vao);
    }

private:
    static void ComputeVertexId(InVertex &inVertex, VertexArray &vao, uint32_t invokeId)
    {
        if (vao.indexBuffer == nullptr)
        {
            inVertex.gl_VertexID = invokeId;
        }
        else switch (vao.indexType)
        {
        case IndexType::UINT8:
            inVertex.gl_VertexID = ((uint8_t*)vao.indexBuffer)[invokeId];
            break;
        case IndexType::UINT16:
            inVertex.gl_VertexID = ((uint16_t*)vao.indexBuffer)[invokeId];
            break;
        case IndexType::UINT32:
            inVertex.gl_VertexID = ((uint32_t*)vao.indexBuffer)[invokeId];
            break;
        }
    }

    static void ReadAttributes(InVertex &inVertex, VertexArray &vao)
    {
        for (uint32_t i = 0; i < maxAttributes; i++)
        {
            if ((int)vao.vertexAttrib[i].type)
            {
                auto ptr = (uint8_t*)vao.vertexAttrib[i].bufferData + vao.vertexAttrib[i].stride * inVertex.gl_VertexID + vao.vertexAttrib[i].offset;

                for (int j = 0; j < (int)vao.vertexAttrib[i].type; j++)
                    inVertex.attributes[i].v4[j] = *(float*)(ptr + j * sizeof(float));
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

    Triangle() { }

    void PerspectiveDivision()
    {
        for (uint8_t v = 0; v < 3; v++)
        {
            Points[v].gl_Position.x /= Points[v].gl_Position.w;
            Points[v].gl_Position.y /= Points[v].gl_Position.w;
            Points[v].gl_Position.z /= Points[v].gl_Position.w;
        }
    }

    void ViewportTransformation(Frame &frame)
    {
        for (uint8_t v = 0; v < 3; v++)
        {
            Points[v].gl_Position.x = (Points[v].gl_Position.x * 0.5 + 0.5) * frame.width;
            Points[v].gl_Position.y = (Points[v].gl_Position.y * 0.5 + 0.5) * frame.height;
        }
    }

    //Rasterizace trojúhelníka Pinedovým algoritmem
    void Rasterize(Frame &frame, Program &prg)
    {
        //Obalový obdélník trojúhelníku
        auto minX = glm::min(Points[0].gl_Position.x, glm::min(Points[1].gl_Position.x, Points[2].gl_Position.x));
        auto minY = glm::min(Points[0].gl_Position.y, glm::min(Points[1].gl_Position.y, Points[2].gl_Position.y));
        auto maxX = glm::max(Points[0].gl_Position.x, glm::max(Points[1].gl_Position.x, Points[2].gl_Position.x));
        auto maxY = glm::max(Points[0].gl_Position.y, glm::max(Points[1].gl_Position.y, Points[2].gl_Position.y));

        minX = glm::max(minX, 0.f);
        minY = glm::max(minY, 0.f);
        maxX = glm::min(maxX, frame.width - 1.f);
        maxY = glm::min(maxY, frame.height - 1.f);

        auto deltaX1 = Points[1].gl_Position.x - Points[0].gl_Position.x;
        auto deltaX2 = Points[2].gl_Position.x - Points[1].gl_Position.x;
        auto deltaX3 = Points[0].gl_Position.x - Points[2].gl_Position.x;

        auto deltaY1 = Points[1].gl_Position.y - Points[0].gl_Position.y;
        auto deltaY2 = Points[2].gl_Position.y - Points[1].gl_Position.y;
        auto deltaY3 = Points[0].gl_Position.y - Points[2].gl_Position.y;

        //Startovací hranové funkce, pro pohyb v ose X
        auto edgeStart1 = EdgeFunction(0, minX, minY, deltaX1, deltaY1);
        auto edgeStart2 = EdgeFunction(1, minX, minY, deltaX2, deltaY2);
        auto edgeStart3 = EdgeFunction(2, minX, minY, deltaX3, deltaY3);

        //Předpočítání přepony a obsahu celého trojúhelníku
        auto hypotenuse = glm::max(PointSum(0), glm::max(PointSum(1), PointSum(2)));

        float xs[3] = { Points[0].gl_Position.x, Points[1].gl_Position.x, Points[2].gl_Position.x };
        float ys[3] = { Points[0].gl_Position.y, Points[1].gl_Position.y, Points[2].gl_Position.y };
        auto triangleArea = Area(xs, ys);

        for (int y = minY; y <= maxY; y++)
        {
            //Inicializace hranových funckí pro pohyb v ose Y
            auto e1 = edgeStart1;
            auto e2 = edgeStart2;
            auto e3 = edgeStart3;

            for (int x = minX; x <= maxX; x++)
            {
                if (e1 >= 0 && e2 >= 0 && e3 >= 0) //Fragment je v trojúhelníku
                {
                    SetPixel(x, y, frame, prg, hypotenuse, triangleArea);
                }
                e1 -= deltaY1;
                e2 -= deltaY2;
                e3 -= deltaY3;
            }
            edgeStart1 += deltaX1;
            edgeStart2 += deltaX2;
            edgeStart3 += deltaX3;
        }
        DrawLine(Points[0].gl_Position, Points[1].gl_Position, frame, prg, hypotenuse, triangleArea);
        DrawLine(Points[1].gl_Position, Points[2].gl_Position, frame, prg, hypotenuse, triangleArea);
        DrawLine(Points[2].gl_Position, Points[0].gl_Position, frame, prg, hypotenuse, triangleArea);
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

            inFragment.gl_FragCoord.z = Points[0].gl_Position.z * lambda0
                + Points[1].gl_Position.z * lambda1
                + Points[2].gl_Position.z * lambda2;

            auto h0 = Points[0].gl_Position.w;
            auto h1 = Points[1].gl_Position.w;
            auto h2 = Points[2].gl_Position.w;

            auto s = lambda0 / h0 + lambda1 / h1 + lambda2 / h2;
            lambda0 /= h0 * s;
            lambda1 /= h1 * s;
            lambda2 /= h2 * s;

            for (uint32_t i = 0; i < maxAttributes; i++)
            {
                if ((int)vs2fs[i])
                {
                    inFragment.attributes[i].v4 = Points[0].attributes[i].v4 * lambda0
                        + Points[1].attributes[i].v4 * lambda1
                        + Points[2].attributes[i].v4 * lambda2;
                }
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
            
            for (uint8_t i = 0; i < 4; i++) //Blending
                frame.color[bufferIndex + i] = glm::clamp((frame.color[bufferIndex + i] / 255.f) * (1 - alpha) + outFragment.gl_FragColor[i] * alpha, 0.f, 1.f) * 255;
        }
    }

    void DrawLine(glm::vec4 fromPoint, glm::vec4 toPoint, Frame &frame, Program &prg, float hypotenuse, float triangleArea)
    {
        bool swapXY;
        if (swapXY = glm::abs(toPoint.y - fromPoint.y) > glm::abs(toPoint.x - fromPoint.x))
        {
            Swap(&fromPoint.x, &fromPoint.y);
            Swap(&toPoint.x, &toPoint.y);
        }
        if (fromPoint.x > toPoint.x)
        {
            Swap(&fromPoint.x, &toPoint.x);
            Swap(&fromPoint.y, &toPoint.y);
        }

        auto k = (toPoint.y - fromPoint.y) / (toPoint.x - fromPoint.x);
        auto y = fromPoint.y;
        for (auto x = fromPoint.x; x <= toPoint.x; x++)
        {
            if (swapXY && y >= 0 && y < frame.width && x >= 0 && x < frame.height)
                SetPixel(y, x, frame, prg, hypotenuse, triangleArea);

            else if (!swapXY && x >= 0 && x < frame.width && y >= 0 && y < frame.height)
                SetPixel(x, y, frame, prg, hypotenuse, triangleArea);

            y += k;
        }
    }

//Pomocné Triangle privátní funkce
private:
    inline float EdgeFunction(uint8_t pointIndex, float x, float y, float deltaX, float deltaY)
    {
        return (y - Points[pointIndex].gl_Position.y) * deltaX - (x - Points[pointIndex].gl_Position.x) * deltaY;
    }

    inline float PointSum(uint8_t pointIndex)
    {
        return Points[pointIndex].gl_Position.x + Points[pointIndex].gl_Position.y;
    }

    float Lambda(uint8_t pointIndex1, uint8_t pointIndex2, InFragment &fragment, float wholeTriangleArea)
    {
        float xs[3] = { Points[pointIndex1].gl_Position.x, Points[pointIndex2].gl_Position.x, fragment.gl_FragCoord.x };
        float ys[3] = { Points[pointIndex1].gl_Position.y, Points[pointIndex2].gl_Position.y, fragment.gl_FragCoord.y };
        auto subTriangle = Area(xs, ys);

        return subTriangle / wholeTriangleArea;
    }

    static float Area(float x[3], float y[3])
    {
        //Shoelace formula
        float area = 0.f;
        for (uint8_t i = 0, j = 2; i < 3; j = i++)
            area += (x[j] + x[i]) * (y[j] - y[i]);

        return glm::abs(area / 2.f);  
    }

    static inline void Swap(float *a, float *b)
    {
        float tmp = *a;
        *a = *b;
        *b = tmp;
    }

    void SetPixel(float x, float y, Frame &frame, Program &prg, float hypotenuse, float triangleArea)
    {
        InFragment inFragment;
        if (CreateFragment(inFragment, x, y, frame, prg.vs2fs, hypotenuse, triangleArea))
        {
            OutFragment outFragment;
            prg.fragmentShader(outFragment, inFragment, prg.uniforms);
            PerFragmentOperations(frame, outFragment, x, y, inFragment.gl_FragCoord.z);
        }
    }
};

class Clipping
{
public:
    static void Perform(std::vector<Triangle*> &clippedTriangles, Triangle *triangle)
    {
        std::vector<glm::vec4*> pointsInClipSpace;
        std::vector<glm::vec4*> pointsToClip;

        for (uint8_t i = 0; i < 3; i++)
        {
            if (-triangle->Points[i].gl_Position.w <= triangle->Points[i].gl_Position.z)
                pointsInClipSpace.push_back(&triangle->Points[i].gl_Position);
            else
                pointsToClip.push_back(&triangle->Points[i].gl_Position);
        }

        glm::vec4 A, B, C, D;
        Triangle *newClipped;
        switch (pointsInClipSpace.size())
        {
        case 2: //Dva body v clip space -> rozdvojení trojúhelníku
            newClipped = new Triangle();
            newClipped->Points[0] = triangle->Points[0];
            newClipped->Points[1] = triangle->Points[1];
            newClipped->Points[2] = triangle->Points[2];

            A = *pointsInClipSpace[0];
            B = NewClippedPoint(*pointsToClip[0], *pointsInClipSpace[0]);
            C = *pointsInClipSpace[1];
            D = NewClippedPoint(*pointsInClipSpace[1], *pointsToClip[0]);
            
            //Úprava bodů v původním trojúhelníku
            *pointsInClipSpace[0] = A;
            *pointsToClip[0] = B;
            *pointsInClipSpace[1] = C;

            //Nastavení bodů v druhém trojúhelníku
            newClipped->Points[0].gl_Position = B;
            newClipped->Points[1].gl_Position = C;
            newClipped->Points[2].gl_Position = D;

            clippedTriangles.push_back(newClipped);
            clippedTriangles.push_back(triangle);
            return;

        case 1: //Jeden bod v clip space -> úprava původního trojúhelníku
            A = NewClippedPoint(*pointsToClip[0], *pointsInClipSpace[0]);
            B = *pointsInClipSpace[0];
            C = NewClippedPoint(*pointsToClip[1], *pointsInClipSpace[0]);
            *pointsToClip[0] = A;
            *pointsInClipSpace[0] = B;
            *pointsToClip[1] = C;

        case 3: //Všechny vrcholy trojúhelníku jsou v clip space -> přidá do vectoru původní trojúhelník
            clippedTriangles.push_back(triangle);
            return;

        default: //Trojúhelník je mimo clip space
            delete triangle;
        }
    }

private:
    static inline glm::vec4 NewClippedPoint(glm::vec4 fromPoint, glm::vec4 toPoint)
    {
        auto t = (-fromPoint.w - fromPoint.z) / (toPoint.w - fromPoint.w + toPoint.z - fromPoint.z);
        return fromPoint + t * (toPoint - fromPoint);
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
        std::vector<Triangle*> clippedTriangles;
        Clipping::Perform(clippedTriangles, new Triangle(ctx.vao, ctx.prg, t));

        for (auto clippedTriangle : clippedTriangles)
        {
            clippedTriangle->PerspectiveDivision();
            clippedTriangle->ViewportTransformation(ctx.frame);
            clippedTriangle->Rasterize(ctx.frame, ctx.prg);
            delete clippedTriangle;
        }
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

