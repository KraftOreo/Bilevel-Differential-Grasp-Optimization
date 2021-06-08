#include <Quasistatic/GraspPlanner.h>
#include <Quasistatic/CentroidClosednessEnergy.h>
#include <Quasistatic/ObjectClosednessEnergy.h>
#include <Quasistatic/LogBarrierObjEnergy.h>
#include <Quasistatic/ConvexLogBarrierSelfEnergy.h>
#include <Utils/Utils.h>
#include <string>
#include <fstream>
#include <chrono>

USE_PRJ_NAMESPACE

typedef double T;
typedef PointCloudObject<T>::Vec Vec;
typedef PointCloudObject<T>::Vec3T Vec3T;
Vec initializeParams(std::string path, Vec &x)
{
  std::string line;
  int i = 0;
  std::ifstream fin;
  fin.open((path));
  float a;
  std::cout << "Initial parameters are: ";
  while (fin >> a) {
    x[i] = a;
    std::cout << x[i] << " ";
    i++;
  }
  std::cout << std::endl;
  return x;
}

int main(int argn,char** argc)
{
  mpfr_set_default_prec(1024U);
  RandEngine::useDeterministic();
  RandEngine::seed(0);

  ASSERT_MSG(argn>=4,"mainGraspPlan: [urdf path] [density] [obj path] [initial parameters]")
  std::string path(argc[1]);
  sizeType density=std::atoi(argc[2]);
  std::string pathObj(argc[3]);
//   std::string objName(argc[4]);
//   std::string objScale(argc[5]);
//   bool useFGT(argc[6]);
  std::string initParamsPath;
  std::cout << "argn = " << argn << std::endl;

    std::string initParamPath(argc[4]);
    initParamsPath=initParamPath;


  std::cout << initParamsPath << std::endl;

  //load hand
  std::experimental::filesystem::v1::path pathIO(path);
  pathIO.replace_extension("");
  pathIO.replace_filename(pathIO.filename().string()+"_"+std::to_string(density));
  pathIO.replace_extension(".dat");
  GraspPlanner<T> planner;
  ASSERT_MSG(exists(pathIO.string()),"Use mainGripper to create gripper first")
  planner.SerializableBase::read(pathIO.string());

  //test objective
  PointCloudObject<T> obj;
  obj.SerializableBase::read(pathObj);
  Vec x0=Vec::Zero(planner.body().nrDOF());
  std::string handName=" ";

    x0=initializeParams(initParamsPath, x0);


  pathIO=path;
  pathIO.replace_extension("");
  recreate(pathIO.filename().string());
   planner.writeVTK(x0,pathIO.filename().string(),1);
   planner.writeLocalVTK(pathIO.filename().string(),1);
   planner.writeLimitsVTK("limits");
//   std::string beforeOptimizeFileName="beforeOptimize_"+handName+ "_"+ objName+"_"+objScale;
//   std::cout << beforeOptimizeFileName<< std::endl;
//   planner.writeVTK(x0, beforeOptimizeFileName,1);
//   std::ofstream initialParameters(beforeOptimizeFileName+"/initialParameters.txt");
//   for(const auto &e:x0)
//     initialParameters << e << " ";

  Options ops;
  GraspPlannerParameter param(ops);

    param._normalExtrude=2;
    param._metric=Q_INF;
    param._coefOC = 0;
    param._coefCC = 0;
    param._coefO = 0;
    param._coefS = 0;
    planner.evaluateQInf(x0, obj, param);
//   param._maxIter=1000;
//   x0=planner.optimize(false,x0,obj,param);
//   auto stop=std::chrono::high_resolution_clock::now();
//   auto duration=std::chrono::duration_cast<std::chrono::seconds>(stop - start);
//   std::cout << "Optimization time using " << (useFGT?"FGT": "No FGT") << " is: " << duration.count() << std::endl;

//   std::string afterOptimizeFileName="afterOptimize_"+handName+ "_" + objName+"_"+objScale;
//   planner.writeVTK(x0,afterOptimizeFileName, 1);
//   obj.writeVTK("object",1,planner.rad()*param._normalExtrude);
//   std::ofstream afterOptimizeFile(afterOptimizeFileName + "/parameters.txt");
//   for(const auto &e:x0)
//     afterOptimizeFile << e << " ";
  return 0;
}