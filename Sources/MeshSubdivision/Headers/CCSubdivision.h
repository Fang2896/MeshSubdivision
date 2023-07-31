//
// Created by fangl on 2023/7/31.
//

#ifndef MY_MESHSUBDIVISION_CCSUBDIVISION_H_
#define MY_MESHSUBDIVISION_CCSUBDIVISION_H_

#include "Mesh.h"
#include "SubdivisionAdapter.h"


namespace SubDivision {

class CCSubdivision : public SubdivisionAdapter {
public:
    bool run(const Mesh& mesh, Mesh& updated_mesh) override;
    void updateFacePoints(const QVector<QVector3D>& face_points,
                          QVector<int>& updated_face_points);
    void updateEdgePoints(const Mesh& mesh,
                          const QVector<QVector3D>& face_points,
                          QVector<int>& updated_edge_points);
    bool updateVertices(const Mesh& mesh, const QVector<QVector3D>& face_points,
                        const QVector<QVector3D>& edge_points,
                        QVector<int>& updated_vertices);

    static void subdivision(const Mesh& mesh, const QVector<int>& updated_face_points,
                const QVector<int>& updated_edge_points,
                const QVector<int>& updated_vertices,
                QVector<QVector<int>>& updated_polygons);

};


}



#endif // MY_MESHSUBDIVISION_CCSUBDIVISION_H_
