//
// Created by fangl on 2023/7/31.
//

#include "Headers/Mesh.h"

namespace SubDivision {

void Mesh::init(const QVector<QVector3D> &vertices, const QVector<QVector<int>> &polygons)
{
    // fill data
    vertices_.reserve(vertices.size());
    for(int i = 0 ; i < vertices.size() ; i++) {
        auto vertex = std::make_shared<Vertex>();
        vertex->vId = i;
        vertex->pos = vertices[i];
        vertices_.push_back(vertex);
    }

    polygons_.reserve(polygons.size());
    std::map<std::pair<int, int>, int> edge_pairs;
    for(const auto & polygon_i : polygons) {
        auto res_polygon = std::make_shared<Polygon>();

        // vertices
        res_polygon->vertexIds.resize(polygon_i.size());
        for(int j = 0 ,sz = polygon_i.size(); j < sz ; j++) {
            res_polygon->vertexIds[j] = polygon_i[j];
        }

        // edges
        res_polygon->edgeIds.reserve(polygon_i.size());
        for(int j = 0 ,sz = polygon_i.size(); j < sz ; j++) {
            int nxt = (j + 1) % sz;

            std::pair<int, int> edge_pair(polygon_i[j], polygon_i[nxt]);
            if(edge_pair.first > edge_pair.second) {
                std::swap(edge_pair.first, edge_pair.second);
            }

            if(!edge_pairs.count(edge_pair)) {
                edge_pairs[edge_pair] = edges_.size();
                auto edge = std::make_shared<Edge>();
                edge->vId1 = edge_pair.first;
                edge->vId2 = edge_pair.second;
                edges_.push_back(edge);
            }
            int edge_id = edge_pairs.at(edge_pair);
            res_polygon->edgeIds.push_back(edge_id);
        }

        polygons_.push_back(res_polygon);
    }

    // create relationship among vertex, edge and polygon
    for(int i = 0; i < polygons_.size(); i++) {
        const auto& polygon = polygons_[i];
        for(auto& vertex: polygon->vertexIds) {
            vertices_[vertex]->adjacentPolygons.push_back(i);
        }

        for(auto& edge: polygon->edgeIds) {
            edges_[edge]->adjacentPolygons.push_back(i);
        }
    }

    for(int i = 0 ; i < edges_.size() ; i++) {
        const auto& edge = edges_[i];
        vertices_[edge->vId1]->adjacentEdges.push_back(i); // alternative
        vertices_[edge->vId2]->adjacentEdges.push_back(i); // alternative
    }

    reCalculateNormals();
}

void Mesh::reCalculateNormals() {
    for(auto & polygon : polygons_) {
        const size_t num_points = polygon->NumVertex();
        polygon->normal = QVector3D(0,0,0);
        if(num_points <= 2) {
            qDebug() <<"Warning: line exists in mesh.\n";
            continue;
        }
        const QVector3D& p1 = getVertexPos(polygon->vertexIds[0]);
        const QVector3D& p2 = getVertexPos(polygon->vertexIds[1]);
        const QVector3D& p3 = getVertexPos(polygon->vertexIds[2]);
        polygon->normal = (QVector3D::crossProduct(p2-p1, p3-p1)).normalized();
    }
}

// for rendering
QVector<float> Mesh::convertToTriangleMeshData() {
    QVector<float> triangular_mesh;

    int num_mesh = 0;
    for(auto & polygon : polygons_) {
        const int num_points = polygon->NumVertex();
        if(num_points <= 2) {
            qDebug() <<"Warning: line exists in mesh.\n";
            continue;
        }
        num_mesh += num_points - 2;
    }

    triangular_mesh.reserve(num_mesh * 9);
    //push vertex and color data by order `p(0) p(1) p(1)`
    auto func = [&](const QVector3D& p){
        for(int k = 0 ; k < 3 ; k++) {
            triangular_mesh.push_back(p[k]);
        }
    };

    size_t num_vertex = 0;
    for(auto & polygon : polygons_) {
        if(polygon->NumVertex() <= 2) {
            qDebug() <<"Warning: line exists in mesh.\n";
            continue;
        }
        const QVector<int>& points = polygon->vertexIds;

        const QVector3D& p1 = getVertexPos(points[0]);
        for(int j = 1 ,sz = points.size() - 1; j < sz ;j++) {
            const QVector3D& p2 = getVertexPos(points[j]);
            const QVector3D& p3 = getVertexPos(points[j + 1]);
            //push vertex
            func(p1);
            func(p2);
            func(p3);
            num_vertex += 3;
        }
    }

    return triangular_mesh;
}


}


