#pragma once

#include "DelaunayTriangle.h"
#include "Vector2D.h"

#include <vector>

class Delaunay
{
public:
	const std::vector<DelaunayTriangle>& triangulate(TArray<FVector2D> &vertices);
	const std::vector<DelaunayTriangle>& getTriangles() const { return _triangles; };
	const std::vector<DelaunayEdge>& getEdges() const { return _edges; };

private:
	std::vector<DelaunayTriangle> _triangles;
	std::vector<DelaunayEdge> _edges;
};