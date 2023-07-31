//
// Created by fangl on 2023/7/31.
//

#include "Headers/SubdivisionAdapter.h"

namespace SubDivision {

QVector<QVector3D> SubdivisionAdapter::getEdgesMidPoints(const SubDivision::Mesh &mesh)
{
    const QVector<std::shared_ptr<Edge>>& edges = mesh.getEdges();
    QVector<QVector3D> edge_points(edges.size());

    for(int i = 0 ; i < edges.size() ; i++) {
        const QVector3D& p1 = mesh.getVertexPos(edges[i]->vId1);
        const QVector3D& p2 = mesh.getVertexPos(edges[i]->vId2);
        QVector3D mid_point = (p1 + p2) / 2.0;
        edge_points[i] = mid_point;
    }

    return edge_points;
}

QVector<QVector3D> SubdivisionAdapter::getFacesMidPoints(const Mesh& mesh) {

    const QVector<std::shared_ptr<Polygon>>& polygons = mesh.getPolygons();
    QVector<QVector3D> face_points(polygons.size());

    for(int i = 0 ; i < polygons.size() ; i++) {
        const auto& points = polygons[i]->vertexIds;
        const int num_points = points.size();
        QVector3D sum_points = QVector3D(0,0,0);
        for(int j = 0 ; j < num_points ; j++) {
            sum_points += mesh.getVertexPos(points[j]);
        }
        face_points[i] = sum_points / static_cast<float>(num_points);
    }

    return face_points;
}

void SubdivisionAdapter::updateMesh(QVector<QVector<int>>& polygons, Mesh& updated_mesh) {
    updated_mesh.init(pointsData_, polygons);
}


}

