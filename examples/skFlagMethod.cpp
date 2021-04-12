/*!
 * @file
 * @brief This file contains implementation procedural flag method
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <examples/skFlagMethod.hpp>

/**
 * @brief This function represents vertex shader of texture rendering method.
 *
 * @param outVertex output vertex
 * @param inVertex input vertex
 * @param uniforms uniform variables
 */
void skFlag_VS(OutVertex&outVertex,InVertex const&inVertex,Uniforms const&uniforms){
  (void)uniforms;
  outVertex.gl_Position = glm::vec4(0.f,0.f,0.f,1.f);

  //fullscreen quad
  glm::vec2 const verts[]={
    glm::vec2(-1.f,-1.f),
    glm::vec2(+1.f,-1.f),
    glm::vec2(-1.f,+1.f),
    glm::vec2(-1.f,+1.f),
    glm::vec2(+1.f,-1.f),
    glm::vec2(+1.f,+1.f),
  };

  outVertex.gl_Position = glm::vec4(verts[inVertex.gl_VertexID],0.f,1.f);
  outVertex.attributes[0].v2 = (glm::vec2(outVertex.gl_Position)+1.f)*.5f;
}

using namespace glm;

/**
 * @brief This function computes color of South Korean flag
 *
 * @param fragColor output color
 * @param fragCoord input coordinats [x,y]
 * @param iResolution resolution
 */
void southKoreanFlag(vec4&fragColor, vec2 fragCoord, ivec2 iResolution)
{    
  vec2 size = vec2(3.f,2.f);
  vec3 red  = vec3(0xCD,0x2E,0x3A)/vec3(255);
  vec3 blu  = vec3(0x00,0x47,0xA0)/vec3(255);

  float aspect = float(iResolution.x) / float(iResolution.y);
  vec2  uv     = fragCoord / vec2(iResolution);
  
  uv *= vec2(size.x,size.x/aspect);
  
  if(uv.x>size.x)return;
  if(uv.y>size.y)return;

  vec2 center = size/2.f;
  
  vec3 diag[4];
  diag[0] = vec3(-center.y,-center.x,+center.x*center.y+center.y*center.x)/length(center);
  diag[1] = vec3(-center.y,+center.x,+center.x*center.y-center.y*center.x)/length(center);
  diag[2] = vec3(+center.x,-center.y,-center.x*center.x+center.y*center.y)/length(center);
  diag[3] = vec3(+center.x,+center.y,-center.x*center.x-center.y*center.y)/length(center);
  
  float distDiag[4];
  
  for(int i=0;i<4;++i)
      distDiag[i] = dot(diag[i],vec3(uv,1))*48.f;
  
  float topRight = float(distDiag[0] <0.f);
  
  float centerCircle = float(length((uv-center)*48.f                   ) < 24.f);
  float smallCircle  = float(length((uv-center)*48.f-12.f*vec2(diag[2])) < 12.f);
  float smallCircle2 = float(length((uv-center)*48.f+12.f*vec2(diag[2])) < 12.f);
  float redRegion = clamp(topRight-smallCircle+smallCircle2,0.f,1.f);
  float bluRegion = (1.f-redRegion);
  
  vec3 col;
  
  col=(bluRegion*blu+redRegion*red)*centerCircle;
  col+=vec3(1.f-centerCircle);
  
  vec2 strip;
  vec2 str;
  
  for(int i=0;i<2;++i){
    strip[i] = float(
      abs(abs(distDiag[2+i])-44.f) <  8.f &&
          abs(distDiag[i  ])       < 12.f  
    );
    str[i]=float((uint(floor(abs(distDiag[2+i])/2.f))+1u)%3u>0u);
  }
  
  col*=vec3(1.f-clamp(dot(strip,str),0.f,1.f));
  
  float pieceRightBotton = float(
    abs(distDiag[0] ) >  1.f ||
        distDiag[2]   < 36.f
  );
  
  col+=vec3(1.f-pieceRightBotton);
  
  float pieceLeftBotton = float(
    abs(distDiag[1]     ) > 1.f || 
    abs(distDiag[3]+44.f) > 2.f
  );
  
  col+=vec3(1.f-pieceLeftBotton);
  
  float pieceRightTop = float(
        abs(distDiag[1]     )      >  1.f || 
    abs(abs(distDiag[3]-44.f)-6.f) >  2.f
  );
  
  col+=vec3(1.f-pieceRightTop);

  fragColor = vec4(col,1.0f);
}

/**
 * @brief This functionrepresents fragment shader of texture rendering method.
 *
 * @param outFragment output fragment
 * @param inFragment input fragment
 * @param uniforms uniform variables
 */
void skFlag_FS(OutFragment&outFragment,InFragment const&inFragment,Uniforms const&uniforms){
  auto uv = inFragment.attributes[0].v2;
  outFragment.gl_FragColor = read_texture(uniforms.textures[0],uv);
  southKoreanFlag(outFragment.gl_FragColor,inFragment.gl_FragCoord, ivec2(uniforms.uniform[0].v2));
}

/**
 * @brief Constructor
 */
SKFlagMethod::SKFlagMethod(MethodConstructionData const*){
  ctx.prg.vertexShader = skFlag_VS;
  ctx.prg.fragmentShader = skFlag_FS;
  ctx.prg.vs2fs[0] = AttributeType::VEC2;//tex coords
}

/**
 * @brief This function is called every frame and should render 3D triangle
 *
 * @param proj projection matrix
 * @param view view matrix
 * @param light light position
 * @param camera camera position
 */
void SKFlagMethod::onDraw(Frame&frame,glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera){
  (void)proj;
  (void)view;
  (void)light;
  (void)camera;
  ctx.frame = frame;
  ctx.prg.uniforms.uniform[0].v2 = vec2(frame.width,frame.height);
  clear(ctx,0,0,0,0);
  drawTriangles(ctx,6);
}

/**
 * @brief Descturctor
 */
SKFlagMethod::~SKFlagMethod(){
}
