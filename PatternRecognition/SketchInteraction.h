#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <iostream>
#include "DataModule.h"

//main example
/*
int main()
{
	std::vector<Sketch> sketches;

	for (int i = 0; i < 3; i++)
	{
		SketchCreator sc(16, 16);

		sc.run();

		auto sketch = sc.get_sketch();
		Sketch s = sc.get_processed_sketch();
		sketches.push_back(s);
	}

	save_sketches(sketches, "test.txt");
}
*/

//keybindings : C : clear_all
//				backspace : delete last grid added
//				return(enter) : add current grid
//				D : display last grid
class SketchCreator
{
private:
	sf::RenderWindow* infoWindow;
	sf::RenderWindow* mainWindow;
	sf::VertexArray* grid;
	sf::VertexArray* points;
	sf::VertexArray* infoGrid; 
	sf::VertexArray* infoPoints; //vertex arrays dedicated to the display on the information window (e.g the last validated grid state)
	void(SketchCreator::* current_info_draw)();
	std::vector<std::vector<Coord>*> pointsAtStep;
	sf::Event ev;
	sf::Text cellHoveredText;
	sf::Text infoText;
	sf::Font font;
	bool informationClosing;
	sf::Color selectedColor;
	sf::Color unselectedColor;
	
	size_t currentSketchSize;
	int width, height;
	std::vector<bool> curSelected;

	Point topLeft;
	Point botRight;
	float meshSize;


	Coord get_hovered_cell(sf::RenderWindow* w);
	void on_right_click();
	void on_left_click();

	void select(size_t idx);
	void deselect(size_t idx);
	void deselect_all();
	void save_step();

	void info_last_grid();
	void info_draw_last_grid();

	//utilities
	std::vector<Sketch>* targetDataset;

public:
	SketchCreator(int _width, int _height, int _windowWidth = 1000, int _windowHeight = 1000);
	void run();
	std::vector<std::vector<Coord>*> get_sketch();
	int get_width();
	int get_height();

	//returns the drawn sketch, with deletion of the empty parts and normalization between 0 and 1
	Sketch get_processed_sketch();
};

