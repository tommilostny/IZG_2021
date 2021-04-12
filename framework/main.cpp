#include <assert.h>

#include<framework/window.hpp>
#include<framework/application.hpp>
#include<examples/emptyMethod.hpp>
#include<examples/triangleMethod.hpp>
#include<examples/triangleClip1Method.hpp>
#include<examples/triangleClip2Method.hpp>
#include<examples/triangle3DMethod.hpp>
#include<examples/triangleBufferMethod.hpp>
#include<examples/czFlagMethod.hpp>
#include<examples/phongMethod.hpp>
#include<examples/texturedQuadMethod.hpp>
#include<examples/skFlagMethod.hpp>
#include<examples/modelMethod.hpp>
#include<tests/conformanceTests.hpp>
#include<tests/performanceTest.hpp>
#include<tests/takeScreenShot.hpp>

#include<framework/arguments.hpp>

#ifdef _MSC_VER
#include "windows.h"
#endif

int main(int argc,char*argv[]){

#ifdef _MSC_VER
  // windows specific 
  SetConsoleOutputCP(CP_UTF8);
  setvbuf(stdout, nullptr, _IOFBF, 1000);
#endif

  try{
    auto args = Arguments(argc,argv);
    if(args.stop)
      return 0;

    if(args.runConformanceTests){
      runConformanceTests(args.groundTruthFile,args.modelFile,args.selectedTest,args.upToTest);
      return 0;
    }

    if(args.runPerformanceTests){
      runPerformanceTest(args.modelFile,args.perfTests);
      return 0;
    }

    if(args.takeScreenShot){
      takeScreenShot(args.groundTruthFile,args.modelFile);
      return 0;
    }

    auto app = Application(args.windowSize[0],args.windowSize[1]);
    app.registerMethod<emptyMethod         ::Method>("empty window"                                            );
    app.registerMethod<triangleMethod      ::Method>("triangle 2D"                                             );
    app.registerMethod<triangleClip1Method ::Method>("triangle clipping (one point is clipped by near plane)"  );
    app.registerMethod<triangleClip2Method ::Method>("triangle clipping (two points are clipped by near plane)");
    app.registerMethod<triangle3DMethod    ::Method>("triangle 3D"                                             );
    app.registerMethod<triangleBufferMethod::Method>("triangle stored in buffer"                               );
    app.registerMethod<czFlagMethod        ::Method>("czech flag"                                              );
    app.registerMethod<phongMethod         ::Method>("phong bunny"                                             );
    app.registerMethod<texturedQuad        ::Method>("textured quad"                                           ,std::make_shared<texturedQuad::ConstructionData>(args.imageFile));
    app.registerMethod<SKFlagMethod                >("South Korean flag"                                       );
    app.registerMethod<modelMethod         ::Method>("model loader"                                            ,std::make_shared<modelMethod ::ConstructionData>(args.modelFile));
    app.setMethod(args.method);
    app.start();

  }catch(std::exception&e){
    std::cerr << e.what() << std::endl;
  }

  return EXIT_SUCCESS;
}

