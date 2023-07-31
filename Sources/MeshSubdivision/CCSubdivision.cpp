//
// Created by fangl on 2023/7/31.
//

#include "Headers/CCSubdivision.h"


namespace SubDivision {

bool CCSubdivision::run(const Mesh& mesh, Mesh& updated_mesh) {
    QVector<QVector3D> edge_points = getEdgesMidPoints(mesh);
    QVector<QVector3D> face_points = getFacesMidPoints(mesh);
    QVector<int> updated_face_points;
    QVector<int> updated_edge_points;
    QVector<int> updated_vertices;
    QVector<QVector<int>> polygons;

    // 1. Face （get updated_face_points）
    updateFacePoints(face_points, updated_face_points);

    // 2. Edge （get updated_edge_points）
    updateEdgePoints(mesh, face_points, updated_edge_points);

    // 3. original points （get updated_vertices）
    if(!updateVertices(mesh, face_points, edge_points, updated_vertices)) {
        return false;
    }

    // 4. process subdivision （get polygons）
    subdivision(mesh, updated_face_points, updated_edge_points, updated_vertices, polygons);

    // 5. update Mesh
    updateMesh(polygons, updated_mesh);

    return true;
}

void CCSubdivision::updateFacePoints(const QVector<QVector3D> &face_points, QVector<int> &updated_face_points)
{
    updated_face_points.resize(face_points.size());
    for(int i = 0; i < face_points.size() ; i++) {
        updated_face_points[i] = addPoint(face_points[i]);
    }
}

void CCSubdivision::updateEdgePoints(const SubDivision::Mesh &mesh,
                                     const QVector<QVector3D> &face_points,
                                     QVector<int> &updated_edge_points)
{
    const auto& edges = mesh.getEdges();
    updated_edge_points.resize(edges.size());
    // for every edges
    for(int i = 0 ; i < edges.size() ; i++) {
        const std::shared_ptr<Edge>& edge = edges[i];
        const QVector3D& p1 = mesh.getVertexPos(edge->vId1);
        const QVector3D& p2 = mesh.getVertexPos(edge->vId2);
        const QVector<int>& associate_polygons = edge->adjacentPolygons;

        // whether is on the boundary.
        bool is_boundary = associate_polygons.size() <= 1;

        if(is_boundary) {
            updated_edge_points[i] = addPoint((p1 + p2) / 2.0);
        } else {
            QVector3D sum_face_pt = QVector3D(0,0,0);
            for(const auto& id : associate_polygons) {
                sum_face_pt += face_points[id];
            }
            updated_edge_points[i] = addPoint((p1 + p2 + sum_face_pt) / 4.0);
        }
    }
}

// calculate new vertices
bool CCSubdivision::updateVertices(const Mesh& mesh, const QVector<QVector3D>& face_points,
                                        const QVector<QVector3D>& edge_points,
                                        QVector<int>& updated_vertices) {
    const auto& vertices = mesh.getVertices();

    updated_vertices.resize(vertices.size());
    for(int i = 0 ;i < vertices.size() ; i++) {
        const std::shared_ptr<Vertex>& vertex = vertices[i];

        const QVector3D& origin_point = vertex->pos;
        int num_adjacent_edges = vertex->adjacentEdges.size();
        int num_adjacent_polygons = vertex->adjacentPolygons.size();
        bool is_boundary = num_adjacent_edges != num_adjacent_polygons;

        QVector3D edge_pt = QVector3D(0,0,0);
        for(const auto& id : vertex->adjacentEdges) {
            edge_pt += edge_points[id];
        }
        edge_pt = edge_pt / (float)num_adjacent_edges;

        // 对于边界点来说，就是平均边点1/4，自己3/4
        if(is_boundary) {
            QVector3D updated_vertex = edge_pt * 0.25 + origin_point * 0.75;
            updated_vertices[i] = addPoint(updated_vertex);

        } else {
            QVector3D face_pt = QVector3D(0,0,0);
            for(const auto& id : vertex->adjacentPolygons) {
                face_pt += face_points[id];
            }
            face_pt = face_pt / (float)num_adjacent_polygons;

            if(num_adjacent_polygons < 3) {
                return false;
            }

            // alpha and beta can be adjusted
            QVector3D updated_vertex = (face_pt + 2 * edge_pt + (float)(num_adjacent_polygons - 3) * origin_point) / (float)num_adjacent_polygons;

            updated_vertices[i] = addPoint(updated_vertex);
        }
    }

    return true;
}

void CCSubdivision::subdivision(const Mesh& mesh, const QVector<int>& updated_face_points,
                                const QVector<int>& updated_edge_points,
                                const QVector<int>& updated_vertices,
                                QVector<QVector<int>>& updated_polygons) {
    const auto& polygons = mesh.getPolygons();

    for(int i = 0 ; i < polygons.size() ;i++) {
        const std::shared_ptr<Polygon>& polygon = polygons[i];

        const QVector<int>& edges = polygon->edgeIds;
        for(int j = 0 , sz = edges.size() ; j < sz; j++) {
            int edge_id1 = edges[j];
            int edge_id2 = edges[(j + 1) % sz];
            int vertex_id;
            //
            if(Mesh::getCommonVertex(mesh.getEdgePtr(edge_id1), mesh.getEdgePtr(edge_id2), vertex_id)) {
                updated_polygons.push_back(QVector<int>{updated_face_points[i],
                                                           updated_edge_points[edge_id1],
                                                           updated_vertices[vertex_id],
                                                           updated_edge_points[edge_id2] });
            } else {
                qDebug() << "WARNING: neighbour edges are not continuous.\n";
            }
        }
    }
}



}
