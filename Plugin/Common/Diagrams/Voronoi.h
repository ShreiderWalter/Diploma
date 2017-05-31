#pragma once

#include "Delaunay.h"
#include <vector>

struct VEdge
{
	FVector4 vEdge;
	FVector4 dEdge;
};

class VSite {
public:
	VSite() = default;
	TArray<VEdge> edges;
};

class Voronoi
{
public:
	Voronoi(TArray<FVector2D> &points);
	Voronoi(Delaunay delaunay);

	TArray<VSite> sites;
private:
	Delaunay DelaunayData;

	void MakeSites(std::vector<DelaunayTriangle> triangles);

	VSite MakeSite(DelaunayTriangle tri);
};