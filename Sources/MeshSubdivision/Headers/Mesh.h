//
// Created by fangl on 2023/7/31.
//

#ifndef MY_MESHSUBDIVISION_MESH_H_
#define MY_MESHSUBDIVISION_MESH_H_

#include <QDebug>
#include <QVector>
#include <QVector3D>
#include <memory>


namespace SubDivision {

struct Vertex {
    int vId;
    QVector3D pos;
    // saved the index of adjacent edges and polygons
    QVector<int> adjacentEdges;
    QVector<int> adjacentPolygons;
};

struct Edge {
    int vId1;
    int vId2;
    QVector<int> adjacentPolygons;

    bool getNeighbourPolygon(const int& inPolygonId1, int& outPolygonId2) {
        for(const auto& i : adjacentPolygons) {
            if(inPolygonId1 != i) {
                outPolygonId2 = i;
                return true;
            }
        }
        return false;
    }
};

struct Polygon {
    QVector<int> vertexIds;
    QVector<int> edgeIds;
    QVector3D normal;

    [[nodiscard]] int NumVertex() const { return vertexIds.size(); }
};

class Mesh {
public:
    void init(const QVector<QVector3D>& vertices, const QVector<QVector<int>>& polygon);
    QVector<float> convertToTriangleMeshData();

    [[nodiscard]] inline std::shared_ptr<Vertex> getVertexPtr(const int i) const {return vertices_.value(i);}
    [[nodiscard]] inline QVector3D& getVertexPos(const int i) const {return vertices_.value(i)->pos;}
    [[nodiscard]] inline std::shared_ptr<Edge> getEdgePtr(const int i) const {return edges_.value(i);}
    [[nodiscard]] inline std::shared_ptr<Polygon> getPolygonPtr(const int i) const {return polygons_.value(i);}

    [[nodiscard]] inline const QVector<std::shared_ptr<Vertex>>& getVertices() const {return vertices_;}
    [[nodiscard]] inline const QVector<std::shared_ptr<Edge>>& getEdges() const {return edges_;}
    [[nodiscard]] inline const QVector<std::shared_ptr<Polygon>>& getPolygons() const {return polygons_;}

    static bool getCommonVertex(const std::shared_ptr<Edge>& e1, const std::shared_ptr<Edge>& e2, int& commonVertexId) {
        if(e1->vId1 == e2->vId1 || e1->vId1 == e2->vId2) {
            commonVertexId = e1->vId1;
            return true;
        } else if(e1->vId2 == e2->vId1 || e1->vId2 == e2->vId2) {
            commonVertexId = e1->vId2;
            return true;
        }
        return false;
    }

    inline bool findEdgeInPolygon(const int& polygon_id, const int& start_index,
                         const int& vertex_id1,const int& vertex_id2,
                         int& edgeId) const { // find the edge whose start points is vertex1 and end point is vertex 2 in the polygon
        const std::shared_ptr<Polygon>& polygon = polygons_[polygon_id];
        const auto& polygon_edges = polygon->edgeIds;
        for(int j = 0 ,sz = polygon_edges.size(); j < sz ; j++) {
            int index = (j + start_index) % sz;
            const std::shared_ptr<Edge>& edge  = getEdgePtr(polygon_edges[index]);
            if((edge->vId1 == vertex_id1 && edge->vId2 == vertex_id2) ||
                (edge->vId1 == vertex_id2 && edge->vId2 == vertex_id1)) {
                edgeId = polygon_edges[index];
                return true;
            }
        }

        return false;
    }

private:
    void reCalculateNormals();

    QVector<std::shared_ptr<Vertex>> vertices_;
    QVector<std::shared_ptr<Edge>> edges_;
    QVector<std::shared_ptr<Polygon>> polygons_;
};

}


#endif // MY_MESHSUBDIVISION_MESH_H_
