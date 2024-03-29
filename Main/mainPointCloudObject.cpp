#include <Quasistatic/PointCloudObject.h>
#include <CommonFile/MakeMesh.h>
#include <Utils/Utils.h>

USE_PRJ_NAMESPACE

typedef double T;
typedef PointCloudObject<T>::Vec Vec;
int main(int argn,char** argc)
{
  ASSERT_MSG(argn>=4,"mainPointCloudObject: [ObjMesh path] [radius of disk] [scale] [scaleY]")
  std::string path(argc[1]);
  sizeType density=std::atoi(argc[2]);
  T scale=std::atof(argc[3]);
  T scaleY=std::atof(argn>4?argc[4]:argc[3]);
  bool graspable=true;

  ObjMesh m;
  if(beginsWith(path,"cube"))
    MakeMesh::makeBox3D(m,Vec3::Constant(scale));
  else if(beginsWith(path,"sphere"))
    MakeMesh::makeSphere3D(m,scale,16);
  else if(beginsWith(path,"cylinder"))
    MakeMesh::makeCylinder3D(m,scale,scaleY,16);
  else if(beginsWith(path,"plane")) {
    MakeMesh::makeGrid(m,Vec2i(10,10));
    m.getPos()=-Vec3(0.5f,0.5f,0);
    m.applyTrans();
    m.getScale()=scale;
    m.applyTrans();
    graspable=false;
  } else {
    std::ifstream is(path.c_str());
    m.read(is,false,false);
    scale/=m.getBB().getExtent().maxCoeff();
    m.getScale()=scale;
    m.applyTrans();
  }

  std::experimental::filesystem::v1::path pathIO(path);
  pathIO.replace_extension("");
  pathIO.replace_filename(pathIO.filename().string()+"_"+std::to_string(density));
  pathIO.replace_extension(".dat");
  PointCloudObject<T> q;
  if(exists(pathIO.string())) {
    q.SerializableBase::read(pathIO.string());
  } else {
    if(graspable)
      q.resetGraspable(m,1.0f/density);
    else q.reset(m,1.0f/density);
    q.SerializableBase::write(pathIO.string());
  }
  q.debug(10);
  pathIO.replace_extension("");
  recreate(pathIO.filename().string());
  q.writeVTK(pathIO.filename().string(),1);
  return 0;
}
