/*!
 * @file
 * @brief This file contains class for command line parsing.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include <ArgumentViewer/ArgumentViewer.h>
#include <iostream>
#include <string>

#ifndef CMAKE_ROOT_DIR
/**
 * location of project
 */
#define CMAKE_ROOT_DIR ".."
#endif


/**
 * @brief This class parses command line arguments
 */
class Arguments{
  public:
    /**
     * @brief Constructor
     *
     * @param argc number of arguments
     * @param argv[] arguments
     */
    Arguments(int argc,char*argv[]){
      args = std::make_shared<argumentViewer::ArgumentViewer>(argc,argv);
      windowSize = args->geti32v("--window-size",{500,500},"size of the window");
      runPerformanceTests = args->isPresent("-p"          ,"runs performance tests");
      runConformanceTests = args->isPresent("-c"          ,"runs conformance tests");
      selectedTest        = args->geti32   ("--test"      ,-1,"run only this selected test");
      takeScreenShot      = args->isPresent("-s"          ,"takes screenshot of app");
      upToTest            = args->isPresent("--up-to-test","run all tests up to selected test by --test argument");
      method              = args->getu32   ("--method"    ,0,"selects a rendering method");
      groundTruthFile     = args->gets     ("-g"          ,std::string(CMAKE_ROOT_DIR)+"/resources/images/output.png"                      ,"specify groundTruth image"    );
      modelFile           = args->gets     ("--model"     ,std::string(CMAKE_ROOT_DIR)+"/resources/models/china.glb"                       ,"model file in gltf/glb format");
      imageFile           = args->gets     ("--img"       ,std::string(CMAKE_ROOT_DIR)+"/resources/images/you_will_not_find_this_image.png","texture file for texturedQuadMethod"                 );
      perfTests           = args->getu32   ("-f"          ,10,"number of frames that are tests during performance tests");

      auto printHelp  = args->isPresent("-h"    ,"prints help");
      printHelp |= args->isPresent("--help","prints help");

      if(printHelp || !args->validate()){
        std::cerr << args->toStr() << std::endl;
        stop = true;
      }

    }
  std::shared_ptr<argumentViewer::ArgumentViewer>args;///< argument viewer
  std::vector<int32_t>windowSize;///< window size
  std::string groundTruthFile = "../tests/output.bmp";///< ground truth file
  std::string modelFile       = "../tests/model.glb";///< models file
  std::string imageFile       = "../test/image.jpg";///< image file
  uint32_t method = 0;///< start with this method
  bool runPerformanceTests;///< should we run performance tests
  bool runConformanceTests;///< sould we run conformance tests
  bool takeScreenShot;///< should we take a screnshot
  bool stop = false; ///< should we immediately stop
  uint32_t perfTests; ///< number of frames in performance tests
  int      selectedTest; ///< selected conformance test
  bool     upToTest; ///< run tests up to selected test
};

