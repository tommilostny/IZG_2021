#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <framework/model.hpp>
#include <libs/tiny_gltf/tiny_gltf.h>

namespace tests{
void printModel(Model const&model);
}

std::string componentType2Str(int i){
  switch(i){
    case TINYGLTF_COMPONENT_TYPE_FLOAT         :return "float"   ;
    case TINYGLTF_COMPONENT_TYPE_DOUBLE        :return "double"  ;
    case TINYGLTF_COMPONENT_TYPE_BYTE          :return "int8_t"  ;
    case TINYGLTF_COMPONENT_TYPE_SHORT         :return "int16_t" ;
    case TINYGLTF_COMPONENT_TYPE_INT           :return "int32_t" ;
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT  :return "uint32_t";
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:return "uint16_t";
    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE :return "uint8_t" ;
    default:break;
  }
  return "unknow";
}

std::string accesstorType2Str(int i){
  switch(i){
    case TINYGLTF_TYPE_SCALAR:return "v1";
    case TINYGLTF_TYPE_VEC2  :return "v2";
    case TINYGLTF_TYPE_VEC3  :return "v3";
    case TINYGLTF_TYPE_VEC4  :return "v4";
    default:break;
  }
  return "unknow";
}

class ModelDataImpl{
  public:
    ModelDataImpl();
    void load(std::string const&fileName);
    ~ModelDataImpl();
    Model getModel();
    bool ret = false;
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
};

ModelDataImpl::ModelDataImpl(){
}

void ModelDataImpl::load(std::string const&fileName){
  std::string err;
  std::string warn;
  if(fileName.find(".glb")==fileName.length()-4)
    ret = loader.LoadBinaryFromFile(&model, &err, &warn, fileName.c_str());

  if(fileName.find(".gltf")==fileName.length()-5)
    ret = loader.LoadASCIIFromFile(&model, &err, &warn, fileName.c_str());

  if(!ret)
    std::cerr << "model: " << fileName << "was not be loaded" << std::endl;
}

ModelDataImpl::~ModelDataImpl(){
}

Node loadNode(tinygltf::Node const&root,tinygltf::Model const&model){
  Node res;
  res.mesh = root.mesh;
  //std::cerr << "M: " << root.matrix.size() << " T: " << root.translation.size() << " R: " << root.rotation.size() << " S: " << root.scale.size() << " W: " << root.weights.size() << std::endl;
  if(root.matrix.size() == 16){
     
    for(int i=0;i<16;++i)
      res.modelMatrix[i/4][i%4] = (float)root.matrix[i];
  }else{
    if(root.translation.size() == 3){
      auto*p = root.translation.data();
      res.modelMatrix = res.modelMatrix*glm::translate(glm::mat4(1.f),glm::vec3(p[0],p[1],p[2]));
    }
    if(root.rotation.size() == 4){
      auto*p = root.rotation.data();
      glm::quat q;
      q[0]=(float)p[0];
      q[1]=(float)p[1];
      q[2]=(float)p[2];
      q[3]=(float)p[3];
      res.modelMatrix = res.modelMatrix*glm::toMat4(q);
      //std::cerr << "r: " << p[0] << "," << p[1] << "," << p[2] << "," << p[3] << std::endl;
      //res.modelMatrix = res.modelMatrix*glm::rotate(glm::mat4(1.f),(float)p[0],glm::vec3(p[1],p[2],p[3]));
    }
    if(root.scale.size() == 3){
      auto*p = root.scale.data();
      res.modelMatrix = res.modelMatrix*glm::scale(glm::mat4(1.f),glm::vec3(p[0],p[1],p[2]));
    }
  }
  for(auto c:root.children)
    res.children.emplace_back(loadNode(model.nodes.at(c),model));
  return res;
}


Model ModelDataImpl::getModel(){
  Model res;
  if(!ret)return res;

  //std::cerr << "nofMeshes   : " << model.meshes   .size() << std::endl;
  //std::cerr << "nofNodes    : " << model.nodes    .size() << std::endl;
  //std::cerr << "nofMatrials : " << model.materials.size() << std::endl;
  //std::cerr << "nofImages   : " << model.images   .size() << std::endl;
  //std::cerr << "nofTextures : " << model.textures .size() << std::endl;

  //std::cerr << "before loaded nodes" << std::endl;
  auto const&scene = model.scenes.at(0);
  for(auto const&node_id:scene.nodes){
    //std::cerr << __LINE__ << std::endl;
    auto const&root = model.nodes.at(node_id);
    //std::cerr << __LINE__ << std::endl;
    res.roots.push_back(loadNode(root,model));
    //std::cerr << __LINE__ << std::endl;
  }
  //std::cerr << "loaded nodes" << std::endl;

  for(auto const&img:model.images){
    res.textures.push_back({});
    auto&tex = res.textures.back();
    //std::cerr << "w: " << img.width << " h: " << img.height << " c: " << img.component << " " << img.name << std::endl;
    //std::cerr << "size: " << img.image.size() << std::endl;
    tex.width    = img.width;
    tex.height   = img.height;
    tex.channels = img.component;
    tex.data     = img.image.data();
  }

  for(auto const&mesh:model.meshes){
    
    //std::cerr <<__FILE__ << "/" << __LINE__ << std::endl;

    for(auto const&primitive:mesh.primitives){
      if(primitive.mode != TINYGLTF_MODE_TRIANGLES)continue;

      res.meshes.push_back({});
      auto&m_mesh = res.meshes.back();

      if (primitive.material >= 0) {
          //std::cerr << "material: " << primitive.material << std::endl;
          auto const& mat = model.materials.at(primitive.material);
          auto baseColorTextureIndex = mat.pbrMetallicRoughness.baseColorTexture.index;
          //std::cerr << "baseColorTexture.Index: " << baseColorTextureIndex << std::endl;
          for (size_t i = 0; i < mat.pbrMetallicRoughness.baseColorFactor.size(); ++i)
              m_mesh.diffuseColor[(uint32_t)i] = (float)mat.pbrMetallicRoughness.baseColorFactor.at(i);
        if(baseColorTextureIndex<0){
          m_mesh.diffuseTexture = -1;
        }else{
          //std::cerr << "texIndex: " << model.materials.at(primitive.material).pbrMetallicRoughness.baseColorTexture.index << std::endl;
          m_mesh.diffuseTexture = model.textures.at(mat.pbrMetallicRoughness.baseColorTexture.index).source;
        }
      }else
        m_mesh.diffuseTexture = -1;

      //std::cerr << "diffuseTex: " << m_mesh.diffuseTexture << std::endl;

      //m_mesh.texture.width    = model.images.at(0).width;
      //m_mesh.texture.height   = model.images.at(0).height;
      //m_mesh.texture.channels = model.images.at(0).component;
      //m_mesh.texture.data     = model.images.at(0).image.data();

      if(primitive.indices >= 0){
          auto const&ia  = model.accessors.at(primitive.indices);
          auto const&ibv = model.bufferViews.at(ia.bufferView);
          m_mesh.nofIndices = (uint32_t)ia.count;
          m_mesh.indices = model.buffers.at(ibv.buffer).data.data() + ibv.byteOffset + ia.byteOffset;
          //std::cerr << "  ibv : " << ia.bufferView ;
          //std::cerr << "  iNof: " << ia.count      ;
          //std::cerr << "  ibuf: " << ibv.buffer    ;
          //std::cerr << "  ioff: " << ibv.byteOffset;
          //std::cerr << std::endl;
          if(ia.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT  )m_mesh.indexType = IndexType::UINT32;
          if(ia.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)m_mesh.indexType = IndexType::UINT16;
          if(ia.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE )m_mesh.indexType = IndexType::UINT8 ;
      }else{
        //std::cerr << "dont have indices" << std::endl;
      }
    //std::cerr <<__FILE__ << "/" << __LINE__ << std::endl;

      //uint32_t nofVertices = 0;
      for(auto const&attrib:primitive.attributes){

        VertexAttrib*att = nullptr;
        auto const&accessor = model.accessors.at(attrib.second);

        //std::cerr << attrib.first << std::endl;
        //std::cerr << " sparse    : " << accessor.sparse.isSparse << std::endl;
        //std::cerr << " count     : " << accessor.count << std::endl;
        //std::cerr << " comp_type : " << componentType2Str(accessor.componentType) << std::endl;
        //std::cerr << " offset    : " << accessor.byteOffset << std::endl;
        //std::cerr << " bufferView: " << accessor.bufferView << std::endl;
        //std::cerr << " components: " << accesstorType2Str(accessor.type) << std::endl;
        if(std::string(attrib.first) == "POSITION"){
          att = &m_mesh.position;


          //m_mesh.nofIndices = accessor.count;


        }
        if(std::string(attrib.first) == "NORMAL"){
          att = &m_mesh.normal;
        }

        if(std::string(attrib.first) == "TEXCOORD_0"){
          att = &m_mesh.texCoord;
        }
    //std::cerr << __LINE__ << std::endl;

        if(att){
          auto const&bufferView = model.bufferViews.at(accessor.bufferView);
          auto bufId  = bufferView.buffer;
          auto stride = bufferView.byteStride;
          auto offset = bufferView.byteOffset;
          //auto size   = bufferView.byteLength;
          auto bptr   = model.buffers.at(bufId).data.data() + accessor.byteOffset;

          att->bufferData = bptr;
          att->offset     = offset;
          att->stride     = stride;

          if(accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT){
            if(accessor.type == TINYGLTF_TYPE_SCALAR)att->type = AttributeType::FLOAT;
            if(accessor.type == TINYGLTF_TYPE_VEC2  )att->type = AttributeType::VEC2 ;
            if(accessor.type == TINYGLTF_TYPE_VEC3  )att->type = AttributeType::VEC3 ;
            if(accessor.type == TINYGLTF_TYPE_VEC4  )att->type = AttributeType::VEC4 ;
            if(att->stride == 0)att->stride = sizeof(float)*(uint32_t)att->type;
          }
          //std::cerr << "  bufId : " << bufId       << std::endl;
          //std::cerr << "  stride: " << att->stride << std::endl;
          //std::cerr << "  offset: " << att->offset << std::endl;
          //std::cerr << "  type  : " << (uint32_t)att->type  << std::endl;
          //float*p = (float*)(((uint8_t*)att->bufferData)+att->offset);
          //for(int i=0;i<4*(uint32_t)att->type;++i)
          //  std::cerr << p[i] << std::endl;
        }
      }
    //std::cerr << __LINE__ << std::endl;

      

    }
    //std::cerr << __LINE__ << std::endl;
    
  }
    //std::cerr << __LINE__ << std::endl;

  //tests::printModel(res);
  return res;
}

void ModelData::load(std::string const&fileName){
  impl->load(fileName);
}

ModelData::ModelData(){
  impl = new ModelDataImpl();
}

ModelData::~ModelData(){
  delete impl;
}

Model ModelData::getModel(){
  return impl->getModel();
}
