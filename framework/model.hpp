#pragma once

#include<vector>
#include<string>
#include<iostream>

#include<student/fwd.hpp>

class ModelDataImpl;
class ModelData{
  public:
    ModelData();
    void load(std::string const&fileName);
    ~ModelData();
    Model getModel();
  private:
    friend class ModelDataImpl;
    ModelDataImpl*impl = nullptr;
};
