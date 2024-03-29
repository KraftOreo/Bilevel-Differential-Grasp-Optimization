#ifndef COMPOSITE_GEOM_CELL_H
#define COMPOSITE_GEOM_CELL_H

#include "StaticGeom.h"

PRJ_BEGIN

struct ALIGN_16 CompositeGeomCell : public StaticGeomCell {
  using Serializable::read;
  using Serializable::write;
  CompositeGeomCell();
  CompositeGeomCell(const CompositeGeomCell& other);
  CompositeGeomCell(const Mat4& T,std::vector<std::shared_ptr<StaticGeomCell> > children);
  virtual bool read(std::istream& is,IOData* dat);
  virtual bool write(std::ostream& os,IOData* dat) const;
  std::shared_ptr<SerializableBase> copy() const;
  virtual void setRes(sizeType res);
  std::shared_ptr<StaticGeomCell> getChild(sizeType i) const;
  sizeType nrChildren() const;
protected:
  virtual void getMeshInner(ObjMesh& mesh) const;
  virtual BBox<scalar> getBBInner() const;
  virtual bool distInner(const Vec3& pt,Vec3& n) const;
  virtual bool closestInner(const Vec3& pt,Vec3& n,Vec3* normal=NULL) const;
  virtual scalar rayQueryInner(const Vec3& x0,const Vec3& dir) const;
  ALIGN_16 std::vector<std::shared_ptr<StaticGeomCell> > _children;

};

PRJ_END

#endif
