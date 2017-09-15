#pragma once

#include <deque>

#include <SFML/Graphics.hpp>

float sqdist(sf::Vector2f const& p1, sf::Vector2f const& p2);

class Edge
{
	public:
		Edge(const sf::Vector2f &p1, const sf::Vector2f &p2);

		Edge flip();

		sf::Vector2f p1, p2;
};

class Triangle
{
	public:
		Triangle(const sf::Vector2f &_p1, const sf::Vector2f &_p2, const sf::Vector2f &_p3);

		bool hasVertex(const sf::Vector2f &v) const;
		bool circumCircleContains(const sf::Vector2f &v) const;
		bool containsVertex(const sf::Vector2f &pos) const;

		sf::Vector2f p1, p2, p3;
		Edge e1, e2, e3;
		sf::Vector2f circumCenter;
		float sqCircumRadius;
};

class Polygon : public sf::Drawable
{
	public:
		Polygon(Edge p, sf::Color color = sf::Color::White);
		Polygon(sf::Color color = sf::Color::White);

		void addEdge(Edge e);
		bool isClosed() const;

		const size_t size(){return edges.size();}

		std::deque<Edge> edges;
		sf::Color m_color;
		sf::Vector2f m_center;

	private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

bool operator == (const Edge &e1, const Edge &e2);
bool operator == (const Triangle &t1, const Triangle &t2);

float dotProduct(sf::Vector2f a, sf::Vector2f b);

bool withinTriangles(sf::Vector2f const& pos, std::vector<Triangle> const& triangles);
bool withinTrianglesFan(sf::Vector2f const& pos, std::vector<sf::Vertex> const& triangles);
bool withinTrianglesStrip(sf::Vector2f const& pos, std::vector<sf::Vertex> const& triangles);
