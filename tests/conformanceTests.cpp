#include <iostream>
#include <sstream>
#include <iomanip>

#include <tests/conformanceTests.hpp>

#define CATCH_CONFIG_RUNNER
#include <tests/catch.hpp>

std::string groundTruthFile;
std::string modelFile      ;

void runConformanceTests(std::string const&groundTruth,std::string const&model,int test,bool upTo) {
  groundTruthFile = groundTruth;
  modelFile       = model      ;
  //int         argc   = 1;
  //char const* argv[1] = {"test"};

  Catch::Config cfg;
  auto tests = Catch::getAllTestCasesSorted(cfg);
  auto nofTests = tests.size();

  std::vector<char const*>argv;
  std::vector<std::string>argvs;
  argvs.push_back("test");

  auto insertTest = [&](size_t i){
    std::stringstream ss;
    ss << "\"Scenario: " << std::setfill('0') << std::setw(2) << i << "\",";
    argvs.push_back(ss.str().c_str());
  };

  if(test>=0&&(size_t)test<nofTests){
    if(upTo){
      for(size_t i=0;i<=(size_t)test;++i)
        insertTest(i);
    }else{
      insertTest(test);
    }
  }else{
    for(size_t i=0;i<nofTests;++i)
      insertTest(i);
  }

  for(auto const&s:argvs)argv.push_back(s.c_str());
  int result = Catch::Session().run((int)argv.size(), argv.data());

  size_t maxPoints = 18;
  std::cout << std::fixed << std::setprecision(1) << maxPoints * (float)(nofTests-result)/(float)nofTests << std::endl;

  //if(test>=0 && test < (int)nofTests){
  //  if(upTo){
  //    for(int i=0;i<=test;++i){
  //      //try{
  //        tests.at(i).invoke();
  //      //}catch(std::exception&){}
  //    }
  //  }else{


  //    

  //    //argv.push_back("--list-test-names-only");
  //    //argv.push_back("\"Scenario: 00\"");
  //    //
  //    //std::stringstream ss;
  //    //ss << "\"Scenario: 00\"";
  //    //argv.push_back(ss.str().c_str());
  //    for(auto const&a:argv)std::cerr << "#" << a << "#" << std::endl;

  //    //tests.at(test).invoke();
  //  }
  //  return;
  //}


  //int  result = Catch::Session().run(argc, argv);
 
}
