#include "geometry.hpp"

inline float sqdist(sf::Vector2f const& p1, sf::Vector2f const& p2)
{
	return ((p1.x - p2.x) * (p1.x - p2.x)) + ((p1.y - p2.y) * (p1.y - p2.y));
}

Edge::Edge(const sf::Vector2f &p1, const sf::Vector2f &p2):
p1(p1),
p2(p2)
{}

Edge Edge::flip(){
	sf::Vector2f temp(p2);
	p2 = p1;
	p1 = temp;
	return *this;
}

Triangle::Triangle(const sf::Vector2f &_p1, const sf::Vector2f &_p2, const sf::Vector2f &_p3):
p1(_p1),
p2(_p2),
p3(_p3),
e1(_p1, _p2),
e2(_p2, _p3),
e3(_p3, _p1)
{
	float ab = (p1.x * p1.x) + (p1.y * p1.y);
	float cd = (p2.x * p2.x) + (p2.y * p2.y);
	float ef = (p3.x * p3.x) + (p3.y * p3.y);

	circumCenter.x = (ab * (p3.y - p2.y) + cd * (p1.y - p3.y) + ef * (p2.y - p1.y)) / (p1.x * (p3.y - p2.y) + p2.x * (p1.y - p3.y) + p3.x * (p2.y - p1.y)) / 2.f;
	circumCenter.y = (ab * (p3.x - p2.x) + cd * (p1.x - p3.x) + ef * (p2.x - p1.x)) / (p1.y * (p3.x - p2.x) + p2.y * (p1.x - p3.x) + p3.y * (p2.x - p1.x)) / 2.f;
	sqCircumRadius = sqdist(p1, circumCenter);
}

bool Triangle::hasVertex(const sf::Vector2f &v) const
{return p1 == v || p2 == v || p3 == v;}

bool Triangle::circumCircleContains(const sf::Vector2f &v) const
{
	float sqdist = ((v.x - circumCenter.x) * (v.x - circumCenter.x)) + ((v.y - circumCenter.y) * (v.y - circumCenter.y));
	return sqdist <= sqCircumRadius;
}

bool Triangle::containsVertex(const sf::Vector2f &pos) const
{
  // Compute vectors
  sf::Vector2f v0 = p2 - p1;
  sf::Vector2f v1 = p3 - p1;
  // Compute dot products
  float dot00 = dotProduct(v0, v0);
  float dot01 = dotProduct(v0, v1);
  float dot0p = dotProduct(v0, pos - p1);
  float dot11 = dotProduct(v1, v1);
  float dot1p = dotProduct(v1, pos - p1);
  // Compute barycentric coordinates
  float denom = (dot00 * dot11 - dot01 * dot01);
  float u = (dot11 * dot0p - dot01 * dot1p) / denom;
  float v = (dot00 * dot1p - dot01 * dot0p) / denom;
  // Check if point is in triangle
  return (u >= 0) && (v >= 0) && (u + v < 1);
}

Polygon::Polygon(Edge p, sf::Color color):
edges({p}),
m_color(color)
{}

Polygon::Polygon(sf::Color color):
m_color(color)
{}

void Polygon::addEdge(Edge e){
	if(edges.empty()){
		edges.push_front(e);
		return;
	}else{
		for (auto& ed : edges) {
			if(ed == e) return;
		}
    if(e.p1 == e.p2) return;

		if(e.p1 == edges.back().p2){
			edges.push_back(e);
			return;
		}
		if(e.p2 == edges.back().p2){
			edges.push_back(e.flip());
			return;
		}
		if(e.p2 == edges.front().p1){
			edges.push_front(e);
			return;
		}
		if(e.p1 == edges.front().p1){
			edges.push_front(e.flip());
			return;
		}
	}
}

bool Polygon::isClosed() const
{
	if(!edges.empty())
		return edges.front().p1 == edges.back().p2;
	return false;
}

void Polygon::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	std::vector<sf::Vertex> poly;
	for (auto ed : edges) {
		poly.push_back(sf::Vertex(ed.p1, m_color, m_center));
	}
	poly.push_back(sf::Vertex(edges[0].p1, m_color, m_center));
	target.draw(poly.data(), poly.size(), sf::TrianglesFan, states);
}

bool operator == (const Edge & e1, const Edge & e2)
{
	return 	(e1.p1 == e2.p1 && e1.p2 == e2.p2) ||
	        (e1.p1 == e2.p2 && e1.p2 == e2.p1);
}

bool operator == (const Triangle &t1, const Triangle &t2)
{
	return
				(t1.p1 == t2.p1 || t1.p1 == t2.p2 || t1.p1 == t2.p3) &&
				(t1.p2 == t2.p1 || t1.p2 == t2.p2 || t1.p2 == t2.p3) &&
				(t1.p3 == t2.p1 || t1.p3 == t2.p2 || t1.p3 == t2.p3);
				/* prend en compte les points confondus :
				(t1.p1 == t2.p1 && ((t1.p2 == t2.p2 && t1.p3 == t2.p3) || (t1.p2 == t2.p3 && t1.p3 == t2.p2))) ||
				(t1.p2 == t2.p1 && ((t1.p1 == t2.p2 && t1.p3 == t2.p3) || (t1.p1 == t2.p3 && t1.p3 == t2.p2))) ||
				(t1.p3 == t2.p1 && ((t1.p2 == t2.p2 && t1.p1 == t2.p3) || (t1.p2 == t2.p3 && t1.p1 == t2.p2)));
				*/
}

inline float dotProduct(sf::Vector2f a, sf::Vector2f b)
{
  return a.x*b.x + a.y*b.y;
}

bool withinTriangles(sf::Vector2f const& pos, std::vector<Triangle> const& triangles)
{
  for(auto& tr : triangles){
    if(tr.containsVertex(pos)) return true;
  }
  return false;
}

bool withinTrianglesFan(sf::Vector2f const& pos, std::vector<sf::Vertex> const& triangles)
{
  for (size_t i = 2; i < triangles.size(); i++) {
    if(Triangle(triangles[0].position, triangles[i-1].position, triangles[i].position).containsVertex(pos)) return true;
  }
  return false;
}

bool withinTrianglesStrip(sf::Vector2f const& pos, std::vector<sf::Vertex> const& triangles)
{
  for (size_t i = 2; i < triangles.size(); i++) {
    if(Triangle(triangles[i-2].position, triangles[i-1].position, triangles[i].position).containsVertex(pos)) return true;
  }
  return false;
}
