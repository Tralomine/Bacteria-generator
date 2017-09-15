#pragma once

#include <vector>
#include <SFML/System/Vector2.hpp>
#include "geometry.hpp"

class Delaunay
{
	public:
		Delaunay(std::vector<sf::Vector2f> const& vertices);

    const std::vector<Polygon> getVoronoi(){return _polygons;}
		const std::vector<sf::Vector2f> getVertices(){return _vertices;}
		const std::vector<Triangle> getTriangles() const {return _triangles; };

		std::vector<Edge> getEdges() const;

	private:
		std::vector<sf::Vector2f> _vertices;
		std::vector<Triangle> _triangles;
		std::vector<Polygon> _polygons;
};
