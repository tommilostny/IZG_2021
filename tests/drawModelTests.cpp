#include <tests/catch.hpp>

#include <iostream>
#include <string.h>
#include <map>
#include <set>
#include <algorithm>
#include <numeric>
#include <cstring>
#include <sstream>

#include <glm/gtc/matrix_transform.hpp>

#include <student/gpu.hpp>
#include <student/drawModel.hpp>
#include <framework/framebuffer.hpp>
#include <framework/textureData.hpp>
#include <tests/testCommon.hpp>

using namespace tests;

void drawTrianglesImpl(GPUContext&,uint32_t);

namespace dtl{

struct DrawCall{
  GPUContext ctx;
  uint32_t n;
};

std::vector<DrawCall>drawCalls;

void drawTrianglesInject(GPUContext&ctx,uint32_t n){
  drawCalls.push_back({ctx,n});
}

bool operator==(Texture const&a,Texture const&b){
  if(a.channels != b.channels)return false;
  if(a.data     != b.data    )return false;
  if(a.width    != b.width   )return false;
  if(a.height   != b.height  )return false;
  return true;
}

struct ReplaceDrawTriangle{
  ReplaceDrawTriangle(){
    drawTriangles = drawTrianglesInject;
  }
  ~ReplaceDrawTriangle(){
    drawTriangles = drawTrianglesImpl;
  }
};

}

using namespace dtl;


SCENARIO("28"){
  std::cerr << "28 - drawModels - empty model" << std::endl;
  
  auto mm = ReplaceDrawTriangle();
  drawCalls.clear();

  Model model;
  GPUContext ctx;

  drawModel(ctx,model,glm::mat4(1),glm::mat4(1),glm::vec3(1),glm::vec3(0));

  if(!drawCalls.empty()){
    std::cerr << R".(
    Tento test zkouší vykreslit prázdný model pomocí funkce drawModel.
    
    U prázdného modelu by se funkce drawTriangles neměla ani jednou zavolat.

    ).";
    printModel(model);
    REQUIRE(false);
  }

}

SCENARIO("29"){
  std::cerr << "29 - drawModels basic GPUContext settings" << std::endl;
  
  auto mm = ReplaceDrawTriangle();
  drawCalls.clear();

  Model model;

  auto cam  = glm::vec3(100.f);
  auto proj = glm::perspective(glm::radians(45.f),1.f,1.f,100.f);
  auto view = glm::lookAt(cam,glm::vec3(0.f),glm::vec3(1.f,0.f,0.f));
  auto light = glm::vec3(200.f,0.f,0.f);

  model.roots.push_back({});
  model.meshes.push_back({});
  model.roots[0].mesh = 0;
  model.meshes[0].nofIndices = 3;

  GPUContext ctx;

  drawModel(ctx,model,proj,view,light,cam);

  auto printInfo = [&](){
    std::cerr<<R".(
    Tento test zkouší vykreslit model s jednim meshem (3 vertexy) a jedním roots bez potomků.)." << std::endl;
    printModel(model);
    std::cerr<<R".(
    Ale něco se pokazilo...
    Zkouší se, jestli je správně nastaven GPUContext.

    Model obsahuje položku roots.
    To jsou kořeny stromové struktury modelu.
    Položka roots je uložena ve std::vector.

    počet roots se dá zjistit pomocí roots.size().
    přistoupit k jednotlivých kořenům lze přes [] - roots[index].

    můžete je projít třeba takto:
    for(size_t i=0;i<roots.size();++i){
      drawNode(roots[i]...);
    }

    Nápověda k implementaci drawModel:

    void drawNode(GPUcontext&ctx,Node const&node,Model const&model,...){
      if(node.mesh >= 0){ // ma tento node mesh?
        Mesh const&mesh = model.meshes[node.mesh];
        ...
        drawTriangles(...);
      }
      ...
    }

    void drawModel(ctx,model,...){
      ...
      for(size_t i=0;i<model.roots.size();++i)
        drawNode(ctx,model.roots[i],model,...);
    }

    Chyba je tato:
    ).";
  };

  bool success = true;
  success &= drawCalls.size() == 1;


  if(drawCalls.size() != 1){
    printInfo();
    std::cerr << R".(
    Funkce drawTriangles se měla zalovat právě jednou.
    ).";
    REQUIRE(false);
  }

  auto const&call = drawCalls.at(0);
  if(call.n != model.meshes[0].nofIndices){
    printInfo();
    std::cerr << R".(
    Funkce drawTriangles se měla zavolat se správným počtem vrcholů.
    Počet je uložen ve struktuře Mesh.
    Byla zavolána s: )."<<drawCalls.at(0).n<<R".( ale měla být zavolána s: )."<<model.meshes[0].nofIndices<<R".(

    ).";
    REQUIRE(false);
  }

  auto const&prg = call.ctx.prg;
  if(prg.vertexShader != drawModel_vertexShader || prg.fragmentShader != drawModel_fragmentShader){
    printInfo();
    std::cerr << R".(
    Program v GPUContext ctx měl obsahovat drawModel_vertexShader a drawModel_fragmentShader;
    ).";
    REQUIRE(false);
  }

  if(prg.vs2fs[0]!=AttributeType::VEC3 || prg.vs2fs[1]!=AttributeType::VEC3 || prg.vs2fs[2]!=AttributeType::VEC2){
    printInfo();
    std::cerr << R".(
    Program měl v poli vs2fs specifikovat, které vertex atributy se mají interpolovat.
    ctx.prg.fs2vs...
    Měly by se interpolovat 3 attributy: pozice, normála, texturovací souřadnice.
    0 pozice by měla být pozice (3 floaty)
    1 pozice by měla být normala (3 floaty)
    2 pozice by měla být tex. coordinaty (2 floaty)
    ).";
    REQUIRE(false);
  }

  auto const&u = prg.uniforms.uniform;
  if(u[0].m4 != proj*view){
    printInfo();
    std::cerr << R".(
    Program by měl v uniformech obsahovat na 0. pozici projekční matici pronásobenou view maticí.
    proj*view;
    ).";
    REQUIRE(false);
  }

  if(u[3].v3 != light){
    printInfo();
    std::cerr << R".(
    Program by měl v uniformech obsahovat na 3. pozici světlo.
    ).";
    REQUIRE(false);
  }

}

SCENARIO("30"){
  std::cerr << "30 - drawModels basic mesh settings" << std::endl;

  auto mm = ReplaceDrawTriangle();

  drawCalls.clear();

  Model model;

  auto m = 
    glm::translate(glm::mat4(1.f),glm::vec3(10,20,30))*
    glm::rotate(glm::mat4(1.f),glm::radians(33.f),glm::vec3(0.f,1.f,0.f))*
    glm::scale(glm::mat4(1.f),glm::vec3(2.f,3.f,4.f));
  auto itm = glm::transpose(glm::inverse(m));
  auto diffColor = glm::vec4(0.f,0.2f,.3f,1.f);

  model.roots.push_back({});
  model.meshes.push_back({});
  model.roots[0].mesh = 0;
  model.meshes[0].nofIndices = 3;
  model.meshes[0].diffuseColor = diffColor;
  model.roots[0].modelMatrix = m;

  GPUContext ctx;

  drawModel(ctx,model,glm::mat4(1.f),glm::mat4(1.f),glm::vec3(1.f),glm::vec3(1.f));

  auto printInfo = [&](){
    std::cerr<<R".(
    Tento test zkouší vykreslit model s jednim meshem (3 vertexy) a jedním root a bez potomků.
    Ale něco se pokazilo...
    Zkouší se, jestli se hodnoty z meshes zpropagovaly to uniformních proměnných.)."<<std::endl;
    printModel(model);
  };

  if(drawCalls.empty()){
    std::cerr << R".(
    Toto by nemělo nastat. Pokud vám to vypsalo tuto zprávu, považujte se za génia tvorby chyb.
    ).";
    REQUIRE(false);
  }

  auto const&u = drawCalls.at(0).ctx.prg.uniforms.uniform;
  if(u[1].m4 != m){
    printInfo();
    std::cerr << R".(
    Program by měl v uniformech obsahovat na 1. pozici modelovou matici z modelu.
    ).";
    REQUIRE(false);
  }

  if(u[2].m4 != itm){
    printInfo();
    std::cerr << R".(
    Program by měl v uniformech obsahovat na 2. pozici inverzní transponovanou modelovou matici.
    inverzní transponovanou matici můžete spočítat pomocí:
    glm::mat4 itm = glm::transpose(glm::inverse(modelMatrix));
    ).";
    REQUIRE(false);

  }

  if(u[5].v4 != diffColor){
    printInfo();
    std::cerr << R".(
    Program by měl v uniformech obsahovat na 5. pozici difuzní barvu z meshe.
    ).";
    REQUIRE(false);
  }

}

SCENARIO("31"){
  std::cerr << "31 - drawModel - atrtributes" << std::endl;

  auto mm = ReplaceDrawTriangle();
  drawCalls.clear();

  Model model;

  model.roots.push_back({});
  model.meshes.push_back({});
  model.roots[0].mesh = 0;
  model.meshes[0].nofIndices = 3;
  model.meshes[0].position.bufferData = (void*)13;
  model.meshes[0].position.offset     = 1334;
  model.meshes[0].position.stride     = 32;
  model.meshes[0].position.type       = AttributeType::VEC3;
  model.meshes[0].normal.bufferData = (void*)15;
  model.meshes[0].normal.offset     = 13;
  model.meshes[0].normal.stride     = 3423;
  model.meshes[0].normal.type       = AttributeType::VEC3;
  model.meshes[0].texCoord.bufferData = (void*)17;
  model.meshes[0].texCoord.offset     = 323;
  model.meshes[0].texCoord.stride     = 33;
  model.meshes[0].texCoord.type       = AttributeType::VEC2;

  GPUContext ctx;

  drawModel(ctx,model,glm::mat4(1.f),glm::mat4(1.f),glm::vec3(1.f),glm::vec3(1.f));

  auto printInfo = [&](){
    std::cerr<<R".(
    Tento test zkouší vykreslit model s jednim meshem (3 vertexy) a jedním root a bez potomků.
    A zkoumá, jestli je GPUContext ctx právně nastaven.
    Konkrétně se jedná o nastavení VertexArray - ctx.vao.
    pozice,normaly,tex coord. by měly být nastavené na 0., 1. a 2. attributu vao.
    
    Ale něco se pokazilo...)."<<std::endl;
    printModel(model);
  };

  if(drawCalls.empty()){
    printInfo();
    std::cerr << R".(
    Toto by nemělo nastat. Pokud vám to vypsalo tuto zprávu, považujte se za génia tvorby chyb.
    ).";
    REQUIRE(false);
  }

  if(!(drawCalls.at(0).ctx.vao.vertexAttrib[0] == model.meshes[0].position)){
    printInfo();
    std::cerr << R".(
    Attribut pozice byl špatně nastaven - pozice by měla být na 0. lokaci ve vao.
    Ale vao.attributes[0] obsahuje:)."<< std::endl;
    printVertexAttrib(drawCalls.at(0).ctx.vao.vertexAttrib[0],6);
    REQUIRE(false);
  }

  if(!(drawCalls.at(0).ctx.vao.vertexAttrib[1] == model.meshes[0].normal)){
    printInfo();
    std::cerr << R".(
    Attribut normaly byl špatně nastaven - normala by měla být na 1. lokaci ve vao.
    Ale vao.attributes[1] obsahuje:)."<< std::endl;
    printVertexAttrib(drawCalls.at(0).ctx.vao.vertexAttrib[1],6);
    REQUIRE(false);
  }

  if(!(drawCalls.at(0).ctx.vao.vertexAttrib[2] == model.meshes[0].texCoord)){
    printInfo();
    std::cerr << R".(
    Attribut normaly byl špatně nastaven - normala by měla být na 2. lokaci ve vao.
    Ale vao.attributes[2] obsahuje:)."<< std::endl;
    printVertexAttrib(drawCalls.at(0).ctx.vao.vertexAttrib[2],6);
    REQUIRE(false);
  }

}

SCENARIO("32"){
  std::cerr << "32 - drawModels mesh textures" << std::endl;

  auto mm = ReplaceDrawTriangle();
  drawCalls.clear();

  Model model;

  Texture tex[2];

  tex[0].channels = 3;
  tex[0].data     = (uint8_t*)1001;
  tex[0].height   = 100;
  tex[0].width    = 100;

  tex[1].channels = 4;
  tex[1].data     = (uint8_t*)1002;
  tex[1].height   = 200;
  tex[1].width    = 200;

  model.roots.push_back({});
  model.roots.push_back({});
  model.meshes.push_back({});
  model.meshes.push_back({});
  model.textures.push_back(tex[0]);
  model.textures.push_back(tex[1]);
  model.roots[0].mesh = 0;
  model.roots[1].mesh = 1;
  model.meshes[0].diffuseTexture = 1;
  model.meshes[0].nofIndices = 3;
  model.meshes[1].diffuseTexture = -1;
  model.meshes[1].nofIndices = 3;

  GPUContext ctx;

  drawModel(ctx,model,glm::mat4(1.f),glm::mat4(1.f),glm::vec3(1.f),glm::vec3(1.f));

  auto printInfo = [&](){
    std::cerr<<R".(
    Model obsahuje textury a měla by se vybrat správná textura nebo žádná podle mesh.diffuseTexture.
    Diffusni textura by měla být nastavena v programu na pozici 0.
    ctx.prg.uniforms.textures[0] = ...;
    Pokud textura neexistuje, tato skutečnost by měla být signalizována programu.
    Prosignalizaci použijte uniformní proměnout typu float na lokaci: 6.
    Pokud textura existuje nastavte tuto proměnnou na 1.f
    Pokud textura neexistuje nastavte tuto proměnnou na 0.f

    Něco se pokazilo...)."<<std::endl;
    printModel(model);
  };

  if(drawCalls.size() != 2){
    printInfo();
    std::cerr << R".(
    drawTriangles se měl zavolat 2x protože máme 2 roots...
    ).";
    REQUIRE(false);
  }

  if(!equalFloats(drawCalls.at(0).ctx.prg.uniforms.uniform[6].v1,1.f)){
    printInfo();
    std::cerr << R".(
    mesh 0. neměl nastavenou 6. uniformní proměnnou na 1.f, i když jeho textura existuje...
    byla nastavena na: )."<<drawCalls.at(0).ctx.prg.uniforms.uniform[6].v1<<R".(
    ).";
    REQUIRE(false);
  }

  if(!(drawCalls.at(0).ctx.prg.uniforms.textures[0] == tex[1])){
    printInfo();
    std::cerr << R".(
    mesh 0. měl obdržet 1. texturu
    ).";
    REQUIRE(false);
  }

  if(!equalFloats(drawCalls.at(1).ctx.prg.uniforms.uniform[6].v1,0.f)){
    printInfo();
    std::cerr << R".(
    mesh 1. neměl nastavenou 6. uniformní proměnnou na 0.f, i když jeho textura neexistuje...
    byla nastavena na: )."<<drawCalls.at(1).ctx.prg.uniforms.uniform[6].v1<<R".(
    ).";
    REQUIRE(false);
  }

  if(!(drawCalls.at(1).ctx.prg.uniforms.textures[0] == Texture{})){
    printInfo();
    std::cerr << R".(
    mesh 1. by neměl obdržet texturu, protože jeho číslo diffuseTexture == -1
    můžete to udělat takto
    ).";
    REQUIRE(false);
  }



}


SCENARIO("33"){
  std::cerr << "33 - drawModels node in node" << std::endl;

  auto mm = ReplaceDrawTriangle();
  drawCalls.clear();

  Model model;

  std::vector<glm::mat4>m;
  m.push_back(glm::translate(glm::mat4(1),glm::vec3(1,2,3)));
  m.push_back(glm::translate(glm::mat4(1),glm::vec3(2,3,3)));

  auto it = [&](glm::mat4 const&m){return glm::transpose(glm::inverse(m));};

  model.roots.push_back({});
  model.roots[0].mesh = -1;
  model.roots[0].modelMatrix = m[0];
  model.roots[0].children.push_back({});
  model.roots[0].children[0].mesh = 0;
  model.roots[0].children[0].modelMatrix = m[1];
  model.meshes.push_back({});
  model.meshes[0].nofIndices = 3;

  GPUContext ctx;

  drawModel(ctx,model,glm::mat4(1.f),glm::mat4(1.f),glm::vec3(1.f),glm::vec3(1.f));

  auto printInfo = [&](){
    std::cerr<<R".(
    Root modelu obsahuje 1 potomka a ten má odkaz na mesh.
    Pouze potomek by se měl vykreslit s korektní transformační maticí.
    Ale něco se pokazilo...)."<<std::endl;
    printModel(model);
  };

  if(drawCalls.size() != 1){
    printInfo();
    std::cerr << R".(
    drawTriangles se měl zavolat 1x protože pouze jeden Node má odkaz na mesh...
    ).";
    REQUIRE(false);
  }


  if(drawCalls.at(0).ctx.prg.uniforms.uniform[1].m4 != m[0]*m[1]){
    printInfo();
    std::cerr << R".(
    Modelová matice měla být pronásobení matic v cestě node to kořene stromu...
    ).";
    REQUIRE(false);
  }

  if(drawCalls.at(0).ctx.prg.uniforms.uniform[2].m4 != it(m[0]*m[1])){
    printInfo();
    std::cerr << R".(
    Inverzní modelová metice měla být inverzí transpozicí pronásobených matic v cestě node do kořene stromu...
    ).";
    REQUIRE(false);
  }
}

SCENARIO("34"){
  std::cerr << "34 - drawModels node in node in node in node" << std::endl;

  auto mm = ReplaceDrawTriangle();
  drawCalls.clear();

  Model model;

  std::vector<glm::mat4>m;
  m.push_back(glm::translate(glm::mat4(1),glm::vec3(1,2,3)));
  m.push_back(glm::translate(glm::mat4(1),glm::vec3(2,3,3)));
  m.push_back(glm::translate(glm::mat4(1),glm::vec3(20,3,3)));
  m.push_back(glm::translate(glm::mat4(1),glm::vec3(2,300,3)));

  auto it = [&](glm::mat4 const&m){return glm::transpose(glm::inverse(m));};

  model.roots.push_back({});
  model.roots[0].mesh = 0;
  model.roots[0].modelMatrix = m[0];

  model.roots[0].children.push_back({});
  model.roots[0].children[0].mesh = 0;
  model.roots[0].children[0].modelMatrix = m[1];

  model.roots[0].children[0].children.push_back({});
  model.roots[0].children[0].children[0].mesh = 0;
  model.roots[0].children[0].children[0].modelMatrix = m[2];

  model.roots[0].children[0].children[0].children.push_back({});
  model.roots[0].children[0].children[0].children[0].mesh = 0;
  model.roots[0].children[0].children[0].children[0].modelMatrix = m[3];

  model.meshes.push_back({});
  model.meshes[0].nofIndices = 3;

  GPUContext ctx;

  drawModel(ctx,model,glm::mat4(1.f),glm::mat4(1.f),glm::vec3(1.f),glm::vec3(1.f));

  auto printInfo = [&](){
    std::cerr<<R".(
    4 Node jsou zřetězeny za sebe. Každý obsahuje referenci na mesh.
    Ten mesh by se měl vykreslit 4x pokaždé se správnout trasformační maticí.
    Strom Node se měl projít pomocí pre order přístupu.
    https://en.wikipedia.org/wiki/Tree_traversal


    Nápověda:

    void drawNode(GPUContext&ctx,Node const&node,Model const&model,glm::mat4 const&prubeznaMatice){
      if(node.mesh){//pokud je mesh vykresli

        ctx.prg.uniforms.uniform[1].m4 = ZKOBINUJ(prubeznaMatice,node.modelMatrix);
        ctx.prg.uniforms.uniform[2].m4 = ...
        ...
        drawTriangles(...);
      }
      //pak zpracuj potomky
      for(size_t i=0;i<node.children.size();++i)
        drawNode(ctx,node.children[i],model,...);//rekurze
    }

    void drawModel(){
      ...

      glm::mat4 jednotkovaMatrice = glm::mat4(1.f);
      for(size_t i=0;i<model.roots.size();++i)
        drawNode(...,model.roots[i],... , jednotkovaMatrice);
    }

    Ale něco se pokazilo...)."<<std::endl;
    printModel(model);
  };

  if(drawCalls.size() != 4){
    printInfo();
    std::cerr << R".(
    drawTriangles se měl zavolat 4x, protože každý node obsahuje referenci na mesh...
    ).";
    REQUIRE(false);
  }


  if(
      drawCalls.at(0).ctx.prg.uniforms.uniform[1].m4 !=    m[0] || 
      drawCalls.at(0).ctx.prg.uniforms.uniform[2].m4 != it(m[0])
      ){
    printInfo();
    std::cerr << R".(
    Když se kreslil roots[0], byly matice spatně...
    ).";
    REQUIRE(false);
  }

  if(
      drawCalls.at(1).ctx.prg.uniforms.uniform[1].m4 !=    m[0]*m[1] || 
      drawCalls.at(1).ctx.prg.uniforms.uniform[2].m4 != it(m[0]*m[1])
      ){
    printInfo();
    std::cerr << R".(
    Když se roots[0].children[0], byly matice spatně...
    ).";
    REQUIRE(false);
  }

  if(
      drawCalls.at(2).ctx.prg.uniforms.uniform[1].m4 !=    m[0]*m[1]*m[2] || 
      drawCalls.at(2).ctx.prg.uniforms.uniform[2].m4 != it(m[0]*m[1]*m[2])
      ){
    printInfo();
    std::cerr << R".(
    Když se roots[0].children[0].children[0], byly matice spatně...
    ).";
    REQUIRE(false);
  }

  if(
      drawCalls.at(3).ctx.prg.uniforms.uniform[1].m4 !=    m[0]*m[1]*m[2]*m[3] || 
      drawCalls.at(3).ctx.prg.uniforms.uniform[2].m4 != it(m[0]*m[1]*m[2]*m[3])
      ){
    printInfo();
    std::cerr << R".(
    Když se roots[0].children[0].children[0].children[0], byly matice spatně...
    ).";
    REQUIRE(false);
  }

}

SCENARIO("35"){
  std::cerr << "35 - drawModels - tree" << std::endl;

  auto mm = ReplaceDrawTriangle();
  drawCalls.clear();

  Model model;

  std::vector<glm::mat4>m;
  m.push_back(glm::translate(glm::mat4(1),glm::vec3(1,2,3)));
  m.push_back(glm::translate(glm::mat4(1),glm::vec3(2,3,3)));
  m.push_back(glm::translate(glm::mat4(1),glm::vec3(20,3,3)));
  m.push_back(glm::translate(glm::mat4(1),glm::vec3(2,300,3)));
  m.push_back(glm::translate(glm::mat4(1),glm::vec3(23,31,-303)));
  m.push_back(glm::translate(glm::mat4(1),glm::vec3(-33,3,-213)));
  m.push_back(glm::translate(glm::mat4(1),glm::vec3(645,-32,173)));
  m.push_back(glm::translate(glm::mat4(1),glm::vec3(43,-43,9)));
  m.push_back(glm::translate(glm::mat4(1),glm::vec3(-423,56,6)));
  m.push_back(glm::translate(glm::mat4(1),glm::vec3(2423,532,-5223)));
  m.push_back(glm::translate(glm::mat4(1),glm::vec3(32,-200,-33)));
  m.push_back(glm::translate(glm::mat4(1),glm::vec3(64,323,-11)));
  m.push_back(glm::translate(glm::mat4(1),glm::vec3(22,212,13)));
  m.push_back(glm::translate(glm::mat4(1),glm::vec3(0,12,-33)));
  m.push_back(glm::translate(glm::mat4(1),glm::vec3(1000,102,1337)));

  auto it = [&](glm::mat4 const&m){return glm::transpose(glm::inverse(m));};

  model.roots.push_back({});
  model.roots[0].mesh = 0;
  model.roots[0].modelMatrix = m[0];
  model.roots[0].children.push_back({});
  model.roots[0].children[0].mesh = 0;
  model.roots[0].children[0].modelMatrix = m[1];
  model.roots[0].children.push_back({});
  model.roots[0].children[1].mesh = 0;
  model.roots[0].children[1].modelMatrix = m[2];

  model.roots[0].children[1].children.push_back({});
  model.roots[0].children[1].children[0].mesh = 0;
  model.roots[0].children[1].children[0].modelMatrix = m[3];
  model.roots[0].children[1].children.push_back({});
  model.roots[0].children[1].children[1].mesh = 0;
  model.roots[0].children[1].children[1].modelMatrix = m[4];

  model.roots.push_back({});
  model.roots[1].mesh = 0;
  model.roots[1].modelMatrix = m[5];
  model.roots[1].children.push_back({});
  model.roots[1].children[0].mesh = 0;
  model.roots[1].children[0].modelMatrix = m[6];
  model.roots[1].children.push_back({});
  model.roots[1].children[1].mesh = 0;
  model.roots[1].children[1].modelMatrix = m[7];
  model.roots[1].children.push_back({});
  model.roots[1].children[2].mesh = 0;
  model.roots[1].children[2].modelMatrix = m[8];


  model.roots[1].children[1].children.push_back({});
  model.roots[1].children[1].children[0].mesh = 0;
  model.roots[1].children[1].children[0].modelMatrix = m[9];
  model.roots[1].children[1].children.push_back({});
  model.roots[1].children[1].children[1].mesh = 0;
  model.roots[1].children[1].children[1].modelMatrix = m[10];

  model.roots[1].children[2].children.push_back({});
  model.roots[1].children[2].children[0].mesh = 0;
  model.roots[1].children[2].children[0].modelMatrix = m[11];
  model.roots[1].children[2].children.push_back({});
  model.roots[1].children[2].children[1].mesh = 0;
  model.roots[1].children[2].children[1].modelMatrix = m[12];

  model.roots[1].children[1].children[1].children.push_back({});
  model.roots[1].children[1].children[1].children[0].mesh = 0;
  model.roots[1].children[1].children[1].children[0].modelMatrix = m[13];
  model.roots[1].children[1].children[1].children.push_back({});
  model.roots[1].children[1].children[1].children[1].mesh = 0;
  model.roots[1].children[1].children[1].children[1].modelMatrix = m[14];


  model.meshes.push_back({});
  model.meshes[0].nofIndices = 3;

  GPUContext ctx;

  drawModel(ctx,model,glm::mat4(1.f),glm::mat4(1.f),glm::vec3(1.f),glm::vec3(1.f));

  auto printInfo = [&](){
    std::cerr<<R".(
    Kreslí se komplexní strom uzlů, v každném uzlu je reference na jeden mesh.
    Ten mesh by se měl vykreslit: )."<<m.size()<<R".( pokaždé se správnout trasformační maticí.
    Strom modelu by se měly projít pre order přístupem:
    https://en.wikipedia.org/wiki/Tree_traversal

    Nápověda:

    void drawNode(GPUContext&ctx,Node const&node,Model const&model,glm::mat4 const&prubeznaMatice){
      if(node.mesh){//pokud je mesh vykresli

        ctx.prg.uniforms.uniform[1].m4 = ZKOBINUJ(prubeznaMatice,node.modelMatrix);
        ctx.prg.uniforms.uniform[2].m4 = ...
        ...
        drawTriangles(...);
      }
      //pak zpracuj potomky
      for(size_t i=0;i<node.children.size();++i)
        drawNode(ctx,node.children[i],model,...);//rekurze
    }

    void drawModel(){
      ...

      glm::mat4 jednotkovaMatrice = glm::mat4(1.f);
      for(size_t i=0;i<model.roots.size();++i)
        drawNode(...,model.roots[i],... , jednotkovaMatrice);
    }

    Ale něco se pokazilo...)."<<std::endl;
    printModel(model);
  };

  if(drawCalls.size() != m.size()){
    printInfo();
    std::cerr << R".(
    drawTriangles se měl zavolat: )."<<m.size()<<R".(, protože každý node obsahuje referenci na mesh...
    ale byl zavolán: )."<<drawCalls.size()<<R".("
    ).";
    REQUIRE(false);
  }

  bool success = true;

  success &= drawCalls.at( 0).ctx.prg.uniforms.uniform[1].m4 == m[0];
  success &= drawCalls.at( 1).ctx.prg.uniforms.uniform[1].m4 == m[0]*m[1];
  success &= drawCalls.at( 2).ctx.prg.uniforms.uniform[1].m4 == m[0]*m[2];
  success &= drawCalls.at( 3).ctx.prg.uniforms.uniform[1].m4 == m[0]*m[2]*m[3];
  success &= drawCalls.at( 4).ctx.prg.uniforms.uniform[1].m4 == m[0]*m[2]*m[4];
  success &= drawCalls.at( 5).ctx.prg.uniforms.uniform[1].m4 == m[5];
  success &= drawCalls.at( 6).ctx.prg.uniforms.uniform[1].m4 == m[5]*m[6];
  success &= drawCalls.at( 7).ctx.prg.uniforms.uniform[1].m4 == m[5]*m[7];
  success &= drawCalls.at( 8).ctx.prg.uniforms.uniform[1].m4 == m[5]*m[9];
  success &= drawCalls.at( 9).ctx.prg.uniforms.uniform[1].m4 == m[5]*m[10];
  success &= drawCalls.at(10).ctx.prg.uniforms.uniform[1].m4 == m[5]*m[10]*m[13];
  success &= drawCalls.at(11).ctx.prg.uniforms.uniform[1].m4 == m[5]*m[10]*m[14];
  success &= drawCalls.at(12).ctx.prg.uniforms.uniform[1].m4 == m[5]*m[8];
  success &= drawCalls.at(13).ctx.prg.uniforms.uniform[1].m4 == m[5]*m[8]*m[11];
  success &= drawCalls.at(14).ctx.prg.uniforms.uniform[1].m4 == m[5]*m[8]*m[12];

  if(success){
    printInfo();
    std::cerr << R".(
    nějaká modelová matice byla špatně vypočítaná...
    ).";
    REQUIRE(false);
  }

  success &= drawCalls.at( 0).ctx.prg.uniforms.uniform[2].m4 == it(m[0]            );
  success &= drawCalls.at( 1).ctx.prg.uniforms.uniform[2].m4 == it(m[0]*m[1]       );
  success &= drawCalls.at( 2).ctx.prg.uniforms.uniform[2].m4 == it(m[0]*m[2]       );
  success &= drawCalls.at( 3).ctx.prg.uniforms.uniform[2].m4 == it(m[0]*m[2]*m[3]  );
  success &= drawCalls.at( 4).ctx.prg.uniforms.uniform[2].m4 == it(m[0]*m[2]*m[4]  );
  success &= drawCalls.at( 5).ctx.prg.uniforms.uniform[2].m4 == it(m[5]            );
  success &= drawCalls.at( 6).ctx.prg.uniforms.uniform[2].m4 == it(m[5]*m[6]       );
  success &= drawCalls.at( 7).ctx.prg.uniforms.uniform[2].m4 == it(m[5]*m[7]       );
  success &= drawCalls.at( 8).ctx.prg.uniforms.uniform[2].m4 == it(m[5]*m[9]       );
  success &= drawCalls.at( 9).ctx.prg.uniforms.uniform[2].m4 == it(m[5]*m[10]      );
  success &= drawCalls.at(10).ctx.prg.uniforms.uniform[2].m4 == it(m[5]*m[10]*m[13]);
  success &= drawCalls.at(11).ctx.prg.uniforms.uniform[2].m4 == it(m[5]*m[10]*m[14]);
  success &= drawCalls.at(12).ctx.prg.uniforms.uniform[2].m4 == it(m[5]*m[8]       );
  success &= drawCalls.at(13).ctx.prg.uniforms.uniform[2].m4 == it(m[5]*m[8]*m[11] );
  success &= drawCalls.at(14).ctx.prg.uniforms.uniform[2].m4 == it(m[5]*m[8]*m[12] );

  if(success){
    printInfo();
    std::cerr << R".(
    nějaká inverzní transponovaná modelová matice byla špatně vypočítaná...
    ).";
    REQUIRE(false);
  }
}


SCENARIO("36"){
  std::cerr << "36 - drawModel_VertexShader" << std::endl;


  InVertex inV;
  OutVertex outV;
  Uniforms u;



  auto proj  = glm::perspective(glm::radians(60.f),1.f,1.f,100.f);
  auto view  = glm::lookAt(glm::vec3(100.f,120.f,-30.f),glm::vec3(0.f,0.f,10.f),glm::vec3(0.f,1.f,0.f));
  auto model = glm::translate(glm::rotate(glm::scale(glm::mat4(1.f),glm::vec3(3.f,5.f,-3.f)),glm::radians(5.f),glm::vec3(1.f,2.f,1.f)),glm::vec3(10.f,30.f,-4.f));
  auto itm   = glm::transpose(glm::inverse(model));
  auto pos   = glm::vec3(1.f,2.f,-3.f);
  auto nor   = glm::vec3(glm::cos(1)*glm::cos(2),glm::sin(1),glm::sin(1)*glm::cos(2));
  auto coord = glm::vec2(0.3,0.8);

  inV.attributes[0].v3 = pos;
  inV.attributes[1].v3 = nor;
  inV.attributes[2].v2 = coord;

  u.uniform[0].m4 = proj*view;
  u.uniform[1].m4 = model;
  u.uniform[2].m4 = itm;


  drawModel_vertexShader(outV,inV,u);

  glm::vec4 egl_Position = proj*view*model*glm::vec4(pos,1.f);
  glm::vec3 epos         = glm::vec3(model*glm::vec4(pos,1.f));
  glm::vec3 enor         = glm::vec3(itm*glm::vec4(nor,0.f));
  glm::vec2 ecoord       = coord;

  auto printInfo = [&](){
    std::cerr<<R".(

    Tento test zkouší, zda je funkce drawModel_vertexShader správně naimplementována.
    Tato funkce reprezentuje vertex shader.
    Jeho funkcí je ztransformovat vstupní vrchol pomocí matic na výstupní vrchol.
    Vstupní vrchol je tvořen třemi atributy: pozice, normála, texrovací koordináty.

    Pozice  je v 0 atributu vstupního vrcholu
    Normála je v 1 atributu vstupního vrcholu
    Tex.kor. jsou v 2 atributu vstupního vrcholu

    Shader ma přístup k třem maticím:

    0.) matice vp = projection * view je maticí, kde je přednásobená projekční matice a view matice.
    tato matice je v 0. uniformní proměnné.
    1.) modelová matice. Ta je v 1. uniformní proměnné.
    2.) inverzní transponovaná modelová matice. Ta je v 2. uniformní proměnné.

    Výstup shaderu je pozice výstupního vrcholu ve clip space gl_Position a 
    tři atributy: pozice ve world-space, normála ve world space a texturovací souřadnice.

    Pamatujte, pro transformaci pozice do clip-space je potřeba pronásobit pozici projekční, view i modelovou maticí.

    Normály by se měly násobit inverzní transponovanou maticí.
    Normály by měly mít homogenní složku = 0 protože to jsou vektory.

    Něco se pokazilo...)."<<std::endl;
  };

  if(!equalVec4(egl_Position,outV.gl_Position)){
    printInfo();
    std::cerr << R".(
    Pozice gl_Position je špatně.
    Měla být: )."<<str(egl_Position)<<R".(
    Ale  je : )."<<str(outV.gl_Position)<<R".(
    ).";
    REQUIRE(false);
  }

  if(!equalVec3(epos,outV.attributes[0].v3)){
    printInfo();
    std::cerr << R".(
    Pozice ve world space je špatně.
    Měla být: )."<<str(epos)<<R".(
    Ale  je : )."<<str(outV.attributes[0].v3)<<R".(
    ).";
    REQUIRE(false);
  }

  if(!equalVec3(enor,outV.attributes[1].v3)){
    printInfo();
    std::cerr << R".(
    Normála ve world space je špatně.
    Měla být: )."<<str(enor)<<R".(
    Ale  je : )."<<str(outV.attributes[1].v3)<<R".(
    ).";
    REQUIRE(false);
  }

  if(!equalVec2(ecoord,outV.attributes[2].v2)){
    printInfo();
    std::cerr << R".(
    Texturovací koordináty jsou špatně.
    Měla být: )."<<str(ecoord)<<R".(
    Ale  je : )."<<str(outV.attributes[2].v2)<<R".(
    ).";
    REQUIRE(false);
  }


}

namespace dtl{
glm::vec4 computeExpectedColor(
    glm::vec3 const&pos  ,
    glm::vec3 const&nor  ,
    glm::vec2 const&coord,
    glm::vec3 const&light,
    glm::vec4 const&diffC,
    Texture   const&tex  ,
    float           isT){

  glm::vec4 dC;
  if(isT>0)
    dC = read_texture(tex,coord);
  else
    dC = diffC;
  
  auto L = glm::normalize(light-pos);
  float dF = glm::max(glm::dot(L,glm::normalize(nor)),0.f);
  float aF = 0.2f;

  glm::vec3 dL = glm::vec3(dC)*dF;
  glm::vec3 aL = glm::vec3(dC)*aF;

  return glm::vec4(aL+dL,dC.a);
}

glm::vec4 computeColor(
    glm::vec3 const&pos  ,
    glm::vec3 const&nor  ,
    glm::vec2 const&coord,
    glm::vec3 const&light,
    glm::vec4 const&diffC,
    Texture   const&tex  ,
    float           isT){

  InFragment inF;
  OutFragment outF;
  Uniforms u;

  inF.attributes[0].v3 = pos;
  inF.attributes[1].v3 = nor;
  inF.attributes[2].v2 = coord;
  
  u.uniform[3].v3 = light;
  u.uniform[5].v4 = diffC;
  u.uniform[6].v1 = isT;
  u.textures[0] = tex;

  drawModel_fragmentShader(outF,inF,u);

  return outF.gl_FragColor;
}

void testLambert(
    glm::vec3 const&pos  ,
    glm::vec3 const&nor  ,
    glm::vec2 const&coord,
    glm::vec3 const&light,
    glm::vec4 const&diffC,
    Texture   const&tex  ,
    float           isT){
  auto expected = computeExpectedColor(pos,nor,coord,light,diffC,tex,isT);
  auto color    = computeColor(pos,nor,coord,light,diffC,tex,isT);

  auto printInfo = [&](){
    std::cerr<<R".(

    Tento test zkouší, zda je funkce drawModel_fragmentShader správně naimplementována.
    Tato funkce reprezentuje fragment shader.
    Fragment shader by měl spočítat barvu pomocí lambertova osvětlovacího modelu.
    https://en.wikipedia.org/wiki/Lambertian_reflectance
    Vstupem FS je fragment a výstupem je fragment s vypočtenou barvout.

    Vstupní fragment je tvořen třemi atributy: pozice, normála, texrovací koordináty.

    Pozice  je v 0 atributu vstupního vrcholu
    Normála je v 1 atributu vstupního vrcholu
    Tex.kor. jsou v 2 atributu vstupního vrcholu

    Shader ma přístup k uniformním proměnným:
    uniforms.uniform[3].v3 - pozice světla
    uniforms.uniform[5].v4 - difuzní barva materialu, - pokud není textura
    uniforms.uninfor[6].v1 - 0.f znamená, že není textura, 1.f znamená, že je textura

    výstupní barva by měla být zapsána do proměnné outFragment.gl_FragColor

    Lambertův osvětlovcí model počítá barvu ze dlou složek ambientní a diffusní.

    ambietní složka
    aL = dC*aF;
    difuzní složka
    dL = dC*dF;

    výsledná barva je:
    vec4(aL+dL,dC.a);

    dC je difuzní barva matriálu, je to buď hodnota z textury nebo z uniformní proměnné, když není textura k dispozici.
    Pro čtení z textur použijte funkci read_texture(Texture const&tex,glm::vec2 const&coord);

    aF je ambietní faktor == 0.2f v našem případě.
    dF je difuzní faktor
    difuzní faktor je spočítán jako 
    skalární součin vektoru z fragmentu do světla a normály fragmentu ořezaný do nezáporných hodnot.
    dF = glm::clamp(glm::dot(L,N),0.f,1.f)

    Pozor oba vektory musí být normalizované!
    pro normalizaci můžete využít funkci: glm::normalize()

    dC.a je alpha - neprůhlednost

    Něco se pokazilo...)."<<std::endl;
  };

  if(!equalVec4(expected,color)){
    printInfo();
    std::cerr << R".(
    Barva měla být: )."<<str(expected)<<R".(
    Ale byla: )."<<str(color)<<R".(

    Pozice                 fragmentu byla: )."<<str(pos  )<<R".(
    Normála                fragmentu byla: )."<<str(nor  )<<R".(
    Texturovací souřadnice fragmentu byly: )."<<str(coord)<<R".(
    Pozice světla byla                   : )."<<str(light)<<R".(
    Diffuzní barva byla                  : )."<<str(diffC)<<R".(
    Příznak textury byl                  : )."<<str(isT  )<<R".(
    Barva v textuře byla                 : )."<<str(read_texture(tex,coord))<<R".(

    ).";
    REQUIRE(false);
  }
}

}


SCENARIO("37"){
  std::cerr << "37 - drawModel_fragmentShader" << std::endl;

  auto tex0 = TextureData(100,100,3);
  auto tex1 = TextureData(100,100,3);

  auto genTex0 = [&](TextureData&d){
    for(uint32_t y=0;y<d.height;++y)
      for(uint32_t x=0;x<d.width;++x)
        for(uint32_t c=0;c<d.channels;++c){
          auto uv = glm::vec2(x,y)/glm::vec2(d.width,d.height);
          d.data[(y*d.width+x)*d.channels+c]=(uint8_t)(glm::clamp(glm::sin(uv.x+uv.y*c),0.f,1.f)*255.f);
        }
  };

  genTex0(tex1);

  Texture textures[]={
    tex0.getTexture(),
    tex1.getTexture(),
  };

  for(uint32_t i=0;i<2;++i){
    testLambert(glm::vec3(0.f),glm::vec3(0,1,0),glm::vec2(0.3,0.6),glm::vec3(0,1,0),
        glm::vec4(.5),textures[i],(float)i);

    testLambert(glm::vec3(0.f),glm::vec3(0,1000,0),glm::vec2(0.2,0.8),glm::vec3(0,1,0),
        glm::vec4(.5),textures[i],(float)i);

    testLambert(glm::vec3(0.f),glm::vec3(0,1,0),glm::vec2(0.5,0.5),glm::vec3(0,1000,0),
        glm::vec4(.5),textures[i],(float)i);

    testLambert(glm::vec3(0.f),glm::vec3(0,1000,0),glm::vec2(0.2,0.6),glm::vec3(0,1000,0),
        glm::vec4(.5),textures[i],(float)i);

    testLambert(glm::vec3(0,-1.f,0),glm::vec3(0,1,0),glm::vec2(0.9,0.1),glm::vec3(0,0,0),
        glm::vec4(.5),textures[i],(float)i);

    testLambert(glm::vec3(0,0.f,0),glm::vec3(1,0,0),glm::vec2(0.55,0.45),glm::vec3(0,1,0),
        glm::vec4(.5),textures[i],(float)i);

    testLambert(glm::vec3(0,0.f,0),glm::vec3(0,-1,0),glm::vec2(0.45,0.55),glm::vec3(0,1,0),
        glm::vec4(.5),textures[i],(float)i);

  }

}
