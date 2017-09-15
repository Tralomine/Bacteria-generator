#include "voronoi.hpp"

Delaunay::Delaunay(std::vector<sf::Vector2f> const& vertices):
_vertices(vertices)
{
  //super triangle
	sf::Vector2f min(_vertices[0]);
	sf::Vector2f max(min);

	for(auto& p : _vertices){
		if(p.x < min.x) min.x = p.x;
		if(p.y < min.y) min.y = p.y;
		if(p.x > max.x) max.x = p.x;
		if(p.y > max.y) max.y = p.y;
	}

	sf::Vector2f delta = max - min;
	float deltaMax = std::max(delta.x, delta.y);
	sf::Vector2f mid = (min + max) / 2.f;

	sf::Vector2f p1(mid.x - 20 * deltaMax, mid.y - 5 * deltaMax);
	sf::Vector2f p2(mid.x, mid.y + 20 * deltaMax);
	sf::Vector2f p3(mid.x + 20 * deltaMax, mid.y - 5 * deltaMax);

	//on rajoute le superTriangle
	_triangles.push_back(Triangle(p1, p2, p3));

	for(size_t i(0); i < _vertices.size(); i++){
		std::vector<Edge> polygon;	//polygon contenant les bords du trou qui sera formé lorsqu'on aura enlevé les mauvais triangles
		for(size_t t(0); t < _triangles.size(); t++){
			if(_triangles[t].circumCircleContains(_vertices[i])){	//on supprime les triangles qui contiennent le point dans leur cercle, mais on rajoute leurs cotés au polygon
				polygon.push_back({_triangles[t].p1, _triangles[t].p2});
				polygon.push_back({_triangles[t].p2, _triangles[t].p3});
				polygon.push_back({_triangles[t].p3, _triangles[t].p1});
				_triangles.erase(_triangles.begin()+t);
				t--;
			}
		}
		//on supprime les cotés en doubles, qui sont donc au milieu du polygon
		for(size_t e1(0); e1 < polygon.size(); e1++){
			for(size_t e2(e1+1); e2 < polygon.size(); e2++){
				if(polygon[e1] == polygon[e2]){
					polygon.erase(polygon.begin()+e2);	//e2 > e1 donc on le supprime en premier !!!
					polygon.erase(polygon.begin()+e1);
					e1--; break;	//on supprime le coté aux deux endroits de la liste
				}
			}
		}
		//on rajoute tous les triangles formés par les bords du polygon et le point
		for(auto& edge : polygon){
			_triangles.push_back(Triangle(edge.p1, edge.p2, _vertices[i]));
		}
	}

  //====================================VORONOI=====================================//

  for (size_t i = 0; i < _vertices.size(); i++) {
    std::vector<Triangle> trianglesDisorder;
    Polygon poly;
    //trouve les triangles autour du point p
    for (auto& tr : _triangles) {
      if(tr.p1 == _vertices[i] || tr.p2 == _vertices[i] || tr.p3 == _vertices[i]){
        trianglesDisorder.push_back(tr);
      }
    }
    size_t count(0);
    while(!poly.isClosed() && count < 10){	// TODO A CHANGER !!!! (fais crasher quand deux vertices sont superposés / ne termine pas quand c'est au bord / que y a trop de triangles)
      count++;
      for (auto& tr1 : trianglesDisorder){
        for (auto& tr2 : trianglesDisorder){
          if(tr1 == tr2) continue;	//on se relie pas a soi même, c'est stupide
          if(tr1.e1 == tr2.e1 || tr1.e1 == tr2.e2 || tr1.e1 == tr2.e3 ||
             tr1.e2 == tr2.e2 || tr1.e2 == tr2.e3 ||
             tr1.e3 == tr2.e3){
            poly.addEdge(Edge(tr2.circumCenter, tr1.circumCenter));
          }
        }
      }
    }
    if(!poly.isClosed() && !poly.edges.empty()){
      poly.addEdge(Edge(poly.edges.back().p2, poly.edges.front().p1));
    }
    poly.m_center = _vertices[i];
    _polygons.push_back(poly);
  }

	//on supprime tous les triangles contenant un des points du méga triangle
	for (size_t i = 0; i < _triangles.size(); i++) {
		if(_triangles[i].hasVertex(p1) || _triangles[i].hasVertex(p2) ||_triangles[i].hasVertex(p3)){
			_triangles.erase(_triangles.begin()+i);
			i--;
		}
	}
}

std::vector<Edge> Delaunay::getEdges() const {
	std::vector<Edge> _edges;
	for(auto& tr : _triangles){
		_edges.push_back({tr.p1, tr.p2});
		_edges.push_back({tr.p2, tr.p3});
		_edges.push_back({tr.p3, tr.p1});
	}
	return _edges;
}
