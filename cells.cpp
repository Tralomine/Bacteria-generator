#include <cmath>

#include "cells.hpp"
#include "geometry.hpp"

void randomPoints(std::vector<sf::Vector2f> * points, int count, sf::Rect<float> rect);

inline sf::Color interpolateCubic(sf::Color const& c1, sf::Color const& c2, float f)
{
  f = 3*f*f - 2*f*f*f;
  return sf::Color(c1.r + (c2.r-c1.r)*f, c1.g + (c2.g-c1.g)*f, c1.b + (c2.b-c1.b)*f);
}

Cell::Cell(Polygon poly):
m_poly(poly),
m_colorNoy(75 - rand()%45, 50 - rand()%30, 0 + rand()%10),  //nuance de brun foncé
m_noySize(8+rand()%8)
{
  m_colorMbr.push_back(sf::Color(130 + rand()%50, 180 + rand()%60, 230 + rand()%25));  //nuance de bleu
  for (size_t i(0); i < poly.size(); i++) {
    m_colorMbr.push_back(sf::Color(m_colorMbr[0].r +rand()%20-10, m_colorMbr[0].g + rand()%20-10, std::min(m_colorMbr[0].b + rand()%20-10, 255)));
  }
  m_colorMbr.erase(m_colorMbr.begin());

  if(rand()%2) m_colorCyt = sf::Color(240 - rand()%20, 200 - rand()%30, 160 - rand()%90);  //nuance de beige
  else m_colorCyt = sf::Color(160 - rand()%40, 240 - rand()%20, 140 - rand()%40);  //nuance de vert

  m_vertices.push_back(sf::Vertex(poly.m_center, m_colorCyt));
  for (size_t j(0); j < poly.size(); j++){
    std::vector<sf::Vector2f> bezier;
    bezier.push_back(getBezier(getBezier(poly.edges[(j==0) ? poly.size()-1 : j-1].p1, poly.edges[(j==0) ? poly.size()-1 : j-1].p2, 0.6666),
                               getBezier(poly.edges[j].p1, poly.edges[j].p2, 0.33333),
                               0.5));
    bezier.push_back(getBezier(poly.edges[j].p1, poly.edges[j].p2, 0.33333));
    bezier.push_back(getBezier(poly.edges[j].p1, poly.edges[j].p2, 0.6666));
    bezier.push_back(getBezier(getBezier(poly.edges[j].p1, poly.edges[j].p2, 0.6666),
                               getBezier(poly.edges[(j+1)%poly.size()].p1, poly.edges[(j+1)%poly.size()].p2, 0.33333),
                               0.5));
    for(float adv(0); adv <= 1; adv += 0.025){
      m_vertices.push_back(sf::Vertex(getBezier(bezier, adv), m_colorCyt));
      m_colorsMbr.push_back(interpolateCubic(m_colorMbr[j], m_colorMbr[(j+1)%poly.size()], adv));
    }
    m_vertices.push_back(sf::Vertex(getBezier(bezier, 1), m_colorCyt));
    m_colorsMbr.push_back(interpolateCubic(m_colorMbr[j], m_colorMbr[(j+1)%poly.size()], 1));

  }
  //genere des organites autour du noyaux
  int nbrOrga = rand()%5+3;
  for (size_t i = 0; i < nbrOrga; i++) {
    sf::Vector2f opos;
    do{
      opos = m_poly.m_center + sf::Vector2f(rand()%200-100,rand()%200-100);
    }while(!withinTrianglesFan(opos, m_vertices));
    m_organites.push_back(Organite(opos));
  }

}

void Cell::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
  target.draw(m_vertices.data(), m_vertices.size(), sf::TrianglesFan, states);

  for(auto& o : m_organites){
    target.draw(o);
  }

  for (size_t i(1); i < m_vertices.size(); i++) {
    sf::CircleShape c(6);
    c.setPosition(m_vertices[i].position - sf::Vector2f(6, 6));
    c.setFillColor(m_colorsMbr[i-1]);
    target.draw(c);
  }

  //triyng to add virusish things on membranes
  // for(size_t i(2); i < m_vertices.size(); i+=20){
  //   sf::CircleShape c(5);
  //   c.setOrigin(5, 5);
  //   c.setPosition(m_vertices[i].position);
  //   c.scale(1, 2);
  //   c.setRotation(std::atan((m_vertices[i-1].position.y - m_vertices[i].position.y) / (m_vertices[i-1].position.x - m_vertices[i].position.x))*180.0 / 3.1415926535897932384626);
  //   c.setFillColor(sf::Color(255, 255, 0));
  //   target.draw(c);
  // }

  sf::CircleShape c(m_noySize);
  c.setFillColor(m_colorNoy);
  c.setPosition(m_poly.m_center - sf::Vector2f(m_noySize, m_noySize));
  target.draw(c);
}

Organite::Organite(sf::Vector2f pos):
m_pos(pos)
{
  int type = rand()%3;
  switch (type) {
    case 0:{
      m_type = POINTS;
      int nbr(rand()%10+5);
      randomPoints(&m_points, nbr, sf::Rect<float>(-15, -15, 30, 30));
      break;}
    case 1:{
      m_type = FILAMENT;
      int nbr(rand()%20+10);
      randomPoints(&m_points, nbr, sf::Rect<float>(-15, -15, 30, 30));
      break;}
    case 2:{
      m_type = CIRCLE;
      break;}
    default:
      break;
  }
}

void Organite::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
  switch (m_type) {
    case POINTS:{
      for (auto& p : m_points) {
        sf::CircleShape c(2);
        c.setFillColor(sf::Color(50, 20, 10));
        c.setPosition(p+m_pos);
        target.draw(c, states);
      }
      break;}
    case FILAMENT:{
      sf::VertexArray a(sf::LineStrip);
      for(auto& p : m_points){
        a.append(sf::Vertex(p+m_pos, sf::Color::Black));
      }
      target.draw(a, states);
      break;}
    case CIRCLE:{
      sf::CircleShape c(12);
      c.setFillColor(sf::Color(128, 128, 128));
      c.setPosition(m_pos - sf::Vector2f(12, 12));
      target.draw(c, states);
      break;}
    }
}
