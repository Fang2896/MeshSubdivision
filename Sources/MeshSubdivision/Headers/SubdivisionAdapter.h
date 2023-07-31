//
// Created by fangl on 2023/7/31.
//

#ifndef MY_MESHSUBDIVISION_SUBDIVISIONADAPTER_H_
#define MY_MESHSUBDIVISION_SUBDIVISIONADAPTER_H_

#include <QVector>
#include <QVector3D>

#include "Mesh.h"


namespace SubDivision {

class SubdivisionAdapter {
public:
    static QVector<QVector3D> getEdgesMidPoints(const Mesh& mesh);
    static QVector<QVector3D> getFacesMidPoints(const Mesh& mesh);
    void updateMesh(QVector<QVector<int>>& polygons, Mesh& updated_mesh);

    virtual bool run(const Mesh& mesh, Mesh& updated_mesh) = 0;

    inline const QVector3D& getPoint(int i) {return  pointsData_.at(i);}
    inline int addPoint(const QVector3D& p) {
        pointsData_.push_back(p);
        return pointsData_.size() - 1;
    }

protected:
    QVector<QVector3D> pointsData_;

};


}



#endif // MY_MESHSUBDIVISION_SUBDIVISIONADAPTER_H_
