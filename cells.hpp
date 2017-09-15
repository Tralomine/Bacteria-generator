#include <SFML/Graphics.hpp>

#include "geometry.hpp"

enum organiteType{
  POINTS,
  FILAMENT,
  CIRCLE,
};

class Organite : public sf::Drawable
{
  public:
    Organite(sf::Vector2f pos);

  private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    organiteType m_type;
    sf::Vector2f m_pos;
    std::vector<sf::Vector2f> m_points;
    sf::Color color;
};

class Cell : public sf::Drawable
{
  public:
    Cell(Polygon poly);
  private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    Polygon m_poly;
    std::vector<sf::Vertex> m_vertices;

    sf::Color m_colorCyt;
    std::vector<sf::Color> m_colorMbr, m_colorsMbr;
    sf::Color m_colorNoy;
    int m_noySize;

    std::vector<Organite> m_organites;
};


sf::Vector2f getBezier(std::vector<sf::Vector2f> points, float adv);
sf::Vector2f getBezier(sf::Vector2f p1, sf::Vector2f p2, float adv);
