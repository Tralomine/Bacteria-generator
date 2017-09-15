#pragma once

#include <cfloat>
#include <cmath>
#include <SFML/System.hpp>

void mitchellBestCandidate(std::vector<sf::Vector2f> * points, int count, sf::Rect<float> rect)
{
	if(points->empty())
		points->push_back(sf::Vector2f(rect.left + rand()%(int)rect.width, rect.top + rand()%(int)rect.height));
	for(size_t i(0); i < (unsigned)count; i++){
		sf::Vector2f farthest;
		bool next(true);
		do{
			next = true;
			float distMax(0);
			for (size_t j(0); j < 64; j++) {
				sf::Vector2f temp(rect.left + rand()%(int)rect.width, rect.top + rand()%(int)rect.height);
				float sqDistMin(FLT_MAX);
				for (size_t k(0); k < points->size(); k++) {
					float sqDist(sqdist(temp, (*points)[k]));
					if(sqDist < sqDistMin){
						sqDistMin = sqDist;
					}
				}
				if(sqDistMin > distMax){
					distMax = sqDistMin;
					farthest = temp;
				}
			}
			for (size_t j = 0; j < i; j++) {
				if((*points)[j] == farthest) next = false;
			}
		}while(!next);
		points->push_back(farthest);
	}
}

//TODO
void poissonDisc(std::vector<sf::Vector2f> * points, int count, sf::Rect<float> rect)
{
  //trouver la densité pour connaitre le rayon
}

void randomPoints(std::vector<sf::Vector2f> * points, int count, sf::Rect<float> rect)
{
  for (size_t i(0); i < (unsigned)count; i++) {
    points->push_back(sf::Vector2f(rect.left + rand()%(int)rect.width, rect.top + rand()%(int)rect.height));
  }
}
