/*!
 * @file
 * @brief This file contains example structure of drawTriangle function
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <student/fwd.hpp>

///\cond HIDDEN_SYMBOLS

//![drawTrianglesExample_before_vs]
void runVertexAssembly(){
  computeVertexID();
  readAttributes();
}

void drawTriangles(GPUContext&ctx,uint32_t nofVertices){
  for(every vertex v){
    InVertex inVertex;
    OutVertex outVertex;
    runVertexAssembly(inVertex,ctx.vao,v);
    prg.vertexShader(outVertex,inVertex,ctx.prg.uniforms);
  }
}
//![drawTrianglesExample_before_vs]



//![drawTrianglesExample_after_vs]
void runVertexAssembly(){
  computeVertexID();
  readAttributes();
}

void runPrimitiveAssembly(primitive,VertexArray vao,t,Program prg){
  for(every vertex v in triangle){
    InVertex inVertex;
    runVertexAssembly(inVertex,vao,t+v);
    prg.vertexShader(primitive.vertex,inVertex,prg.uniforms);
  }
}

void rasterizeTriangle(frame,primitive,prg){
  for(pixels in frame){
    if(pixels in primitive){
      InFragment inFragment;
      createFragment(inFragment,primitive,barycentrics,pixelCoord,prg);
      OutFragment outFragment;
      prg.fragmentShader(outFragment,inFragment,uniforms);
    }
  }
}

void drawTriangles(GPUContext&ctx,uint32_t nofVertices){
  for(every triangle t){
    Primitive primitive;
    runPrimitiveAssembly(primitive,ctx.vao,t,ctx.prg)

    runPerspectiveDivision(primitive)
    runViewportTransformation(primitive,ctx.frame)
    rasterizeTriangle(ctx.frame,primitive,ctx.prg);
  }
}
//![drawTrianglesExample_after_vs]




//![drawTrianglesExample_pfo]
void runVertexAssembly(){
  computeVertexID()
  readVertexAttributes();
}

void runPrimitiveAssembly(primitive,VertexArray vao,t,Program prg){
  for(every vertex v in triangle){
    InVertex inVertex;
    runVertexAssembly(inVertex,vao,t+v);
    prg.vertexShader(primitive.vertex,inVertex,prg.uniforms);
  }
}

void rasterizeTriangle(frame,primitive,prg){
  for(pixels in frame){
    if(pixels in primitive){
      InFragment inFragment;
      createFragment(inFragment,primitive,barycentrics,pixelCoord,prg);
      OutFragment outFragment;
      prg.fragmentShader(outFragment,inFragment,uniforms);
      clampColor(outFragment,0,1);
      perFragmentOperations(frame,outFragment,inFragment.gl_FragCoord.z)
    }
  }

}

void drawTriangles(GPUContext&ctx,uint32_t nofVertices){
  for(every triangle t){
    Primitive primitive;
    runPrimitiveAssembly(primitive,ctx.vao,t,ctx.prg)

    runPerspectiveDivision(primitive)
    runViewportTransformation(primitive,ctx.frame)
    rasterizeTriangle(ctx.frame,primitive,ctx.prg);
  }
}
//![drawTrianglesExample_pfo]



//![drawTrianglesExample]
void runVertexAssembly(){
  computeVertexID()
  readVertexAttributes();
}

void runPrimitiveAssembly(primitive,VertexArray vao,t,Program prg){
  for(every vertex v in triangle){
    InVertex inVertex;
    runVertexAssembly(inVertex,vao,t+v);
    prg.vertexShader(primitive.vertex,inVertex,prg.uniforms);
  }
}

void rasterizeTriangle(frame,primitive,prg){
  for(pixels in frame){
    if(pixels in primitive){
      InFragment inFragment;
      createFragment(inFragment,primitive,barycentrics,pixelCoord,prg);
      OutFragment outFragment;
      prg.fragmentShader(outFragment,inFragment,uniforms);
      clampColor(outFragment,0,1);
      perFragmentOperations(frame,outFragment,inFragment.gl_FragCoord.z)
    }
  }

}

void drawTriangles(GPUContext&ctx,uint32_t nofVertices){
  for(every triangle t){
    Primitive primitive;
    runPrimitiveAssembly(primitive,ctx.vao,t,ctx.prg)

    ClippedPrimitive clipped;
    performeClipping(clipped,primitive);

    for(all clipped triangle c in clipped){
      runPerspectiveDivision(c)
      runViewportTransformation(c,ctx.frame)
      rasterizeTriangle(ctx.frame,c,ctx.prg);
    }
  }
}
//![drawTrianglesExample]


//![drawModel]
void drawNode(GPUContext&ctx,Node const&node,Model const&model,glm::mat4 const&prubeznaMatice){
  if(node.mesh>=0){
    mesh = model.meshes[node.mesh];

    ctx.prg.uniforms.uniform[1].m4 = ZKOBINUJ(prubeznaMatice,node.modelMatrix);
    ctx.prg.uniforms.uniform[2].m4 = inverzni tranponovana...
    ctx.vao.attribute[0] = mesh.position ...
    ...
    ctx.vao.indexBuffer = ...
    if(mesh.diffuseTexture>=0)
      ctx.prg.uniforms.textures[0] = model.textures[mesh.diffuseTexture];
    ...
    drawTriangles(ctx,mesh.nofIndices);
  }

  for(size_t i=0;i<node.children.size();++i)
    drawNode(ctx,node.children[i],model,...); rekurze
}

void drawModel(){
  ...

  glm::mat4 jednotkovaMatrice = glm::mat4(1.f);
  for(size_t i=0;i<model.roots.size();++i)
    drawNode(...,model.roots[i],... , jednotkovaMatrice);
}
//![drawModel]

///\endcond
