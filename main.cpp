#include <SFML/Graphics.hpp>

#include "geometry.hpp"
#include "voronoi.hpp"
#include "randompoints.hpp"
#include "cells.hpp"

sf::Vector2f getBezier(std::vector<sf::Vector2f> points, float adv)
{
  while(points.size() > 2){
    std::vector<sf::Vector2f> v;
    for (size_t i = 0; i < points.size()-1; i++) {
      v.push_back(points[i] + (points[i+1] - points[i]) * adv);
    }
    points = v;
  }
  return points[0] + (points[1] - points[0]) * adv;
}

inline sf::Vector2f getBezier(sf::Vector2f p1, sf::Vector2f p2, float adv)
{return p1 + (p2 - p1) * adv;}

int main(int argc, char const *argv[])
{
  size_t width(1920), height(1020);
  srand(time(NULL));

  std::vector<sf::Vector2f> vertices;
  mitchellBestCandidate(&vertices, 50, sf::Rect<float>(0, 0, width, height));

  Delaunay delaunay(vertices);
  std::vector<Polygon> voronoi(delaunay.getVoronoi());

  sf::VertexArray vertexes(sf::Lines);
  sf::VertexArray triangles(sf::Triangles);

  std::vector<Cell> cells;

  //deleting cells that are outside of the windows
  for (size_t i = 0; i < voronoi.size(); i++) {
    for (size_t j = 0; j < voronoi[i].size(); j++) {
      if(!sf::Rect<float>(0, 0, width, height).contains(voronoi[i].edges[j].p1)){
        voronoi.erase(voronoi.begin()+i);
        i--;
        break;
      }
    }
  }

  //deleting random cell to have spaced cells
  for (size_t i(0); i < voronoi.size(); i++) {
    if(rand()%5 > 1){
      voronoi.erase(voronoi.begin()+i);
      i--;
    }
  }

  //B-spline transformation and calcul
  for (auto poly : voronoi) {
    Polygon b_spline;
    b_spline.m_center = poly.m_center;
    for (size_t i = 0; i < poly.size(); i++) {
      b_spline.addEdge(Edge(
                            getBezier(getBezier(poly.edges[(i==0) ? poly.size()-1 : i-1].p1, poly.edges[(i==0) ? poly.size()-1 : i-1].p2, 0.5),
                                      getBezier(poly.edges[i].p1, poly.edges[i].p2, 0.5),
                                      0.5),
                            getBezier(getBezier(poly.edges[i].p1, poly.edges[i].p2, 0.5),
                                      getBezier(poly.edges[(i+1)%poly.size()].p1, poly.edges[(i+1)%poly.size()].p2, 0.5),
                                      0.5)
                            ));
      // vertexes.append(sf::Vertex(poly.edges[i].p1, sf::Color(128, 128, 128)));
      // vertexes.append(sf::Vertex(poly.edges[i].p2, sf::Color(128, 128, 128)));
    }
    cells.push_back(Cell(b_spline));
  }

  sf::RenderWindow app(sf::VideoMode(width, height), "bateria generator");

  while (app.isOpen()) {
    sf::Event event;
    while (app.pollEvent(event)) {
      if(event.type == sf::Event::Closed){
        app.close();
      }
    }

    app.clear(sf::Color::White);
    app.draw(vertexes);
    app.draw(triangles);
    for(auto& c : cells){
      app.draw(c);
    }
    app.display();
  }

  return 0;
}
