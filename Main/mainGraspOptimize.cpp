#include <Grasp/GraspPlanner.h>
#include <Grasp/CentroidClosednessEnergy.h>
#include <Grasp/ObjectClosednessEnergy.h>
#include <Grasp/LogBarrierObjEnergy.h>
#include <Grasp/LogBarrierSelfEnergy.h>
#include <Utils/Utils.h>

USE_PRJ_NAMESPACE

typedef double T;
typedef GraspQualityMetric<T>::Vec Vec;
typedef GraspQualityMetric<T>::Vec3T Vec3T;
int main(int argn,char** argc)
{
  mpfr_set_default_prec(1024U);
  RandEngine::useDeterministic();
  RandEngine::seed(0);

  ASSERT_MSG(argn==4,"mainGraspPlan: [urdf path] [sample density] [obj path]")
  std::string path(argc[1]);
  sizeType density=std::atoi(argc[2]);
  std::string pathObj(argc[3]);

  //load hand
  std::experimental::filesystem::v1::path pathIO(path);
  pathIO.replace_extension("");
  pathIO.replace_filename(pathIO.filename().string()+"_"+std::to_string(density));
  pathIO.replace_extension(".dat");
  GraspPlanner<T> planner;
  if(exists(pathIO.string())) {
    planner.SerializableBase::read(pathIO.string());
  } else {
    planner.reset(path,1.0f/density);
    planner.fliterSample([&](sizeType lid,const Vec3T&,const Vec3T& n)->bool{
      if(lid==1)
        return n.dot(Vec3T(0,0,1))>0.9f;
      else if(lid==2)
        return n.dot(Vec3T(0,0,-1))>0.9f;
      else if(lid==5)
        return n.dot(Vec3T(0,0,1))>0.9f;
      else return n.dot(Vec3T(0,1,0))>0.9f;
    });
    planner.SerializableBase::write(pathIO.string());
  }

  //test objective
  GraspQualityMetric<T> obj;
  obj.SerializableBase::read(pathObj);
  Vec x0=Vec::Zero(planner.body().nrDOF());
  x0.template segment<3>(0)=Vec3T(0.0f, -0.2f, -0.2f);
  x0[5]=M_PI/2;
  x0[6]=0.5f;
  x0[9]=0.5f;

  pathIO=path;
  pathIO.replace_extension("");
  recreate(pathIO.filename().string());
  planner.writeVTK(x0,pathIO.filename().string(),1);
  planner.writeLocalVTK(pathIO.filename().string(),1);
  planner.writeLimitsVTK("limits");
  planner.writeVTK(x0,"beforeOptimize",1);
  Options ops;
  GraspPlannerParameter param(ops);
  param._normalExtrude=10;
  param._maxIter=15000;
  x0=planner.optimize(false,x0,obj,param);
  param._normalExtrude=2;
  param._maxIter=15000;
  x0=planner.optimize(false,x0,obj,param);
  planner.writeVTK(x0,"afterOptimize",1);
  obj.writeVTK("object",1,planner.rad()*param._normalExtrude);
  return 0;
}
