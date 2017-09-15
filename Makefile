SFML := -lsfml-graphics -lsfml-window -lsfml-system

all:
	g++ -g -c -fPIC geometry.cpp -o geometry.o $(SFML)
	g++ -g -c -fPIC voronoi.cpp -o voronoi.o $(SFML)
	g++ -g -c -fPIC cells.cpp -o cells.o $(SFML)
	g++ -g main.cpp geometry.o voronoi.o cells.o $(SFML)
