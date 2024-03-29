#ifndef CYLINDER_GEOM_CELL_H
#define CYLINDER_GEOM_CELL_H

#include "StaticGeom.h"

PRJ_BEGIN

struct ALIGN_16 CylinderGeomCell : public StaticGeomCell {
  using Serializable::read;
  using Serializable::write;
  CylinderGeomCell();
  CylinderGeomCell(const Mat4& T,sizeType dim,scalar rad,scalar y);
  virtual bool read(std::istream& is,IOData* dat);
  virtual bool write(std::ostream& os,IOData* dat) const;
  std::shared_ptr<SerializableBase> copy() const;
  scalar getRad() const;
  scalar getY() const;
protected:
  CylinderGeomCell(const Mat4& T,sizeType dim,const std::string& type);
  virtual void getMeshInner(ObjMesh& mesh) const;
  virtual BBox<scalar> getBBInner() const;
  virtual bool distInner(const Vec3& pt,Vec3& n) const;
  virtual bool closestInner(const Vec3& pt,Vec3& n,Vec3* normal) const;
  virtual scalar rayQueryInner(const Vec3& x0,const Vec3& dir) const;
  ALIGN_16 scalar _rad,_y;
};

PRJ_END

#endif
