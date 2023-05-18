#include "SketchInteraction.h"

Coord SketchCreator::get_hovered_cell(sf::RenderWindow* w)
{
	sf::Vector2i mousePos = sf::Mouse::getPosition(*w);

	float x = (mousePos.x - topLeft.x) / meshSize;
	float y = (mousePos.y - topLeft.y) / meshSize;

	if (x < 0.f || x >= width || y < 0.f || y >= height)
		return { -1, -1 };

	return { static_cast<int>(x), static_cast<int>(y) };
}

void SketchCreator::on_right_click()
{
	Coord mouseCoord = get_hovered_cell(mainWindow);

	if (mouseCoord.i != -1)
	{
		deselect(mouseCoord.j * width + mouseCoord.i);
	}
}

void SketchCreator::on_left_click()
{
	Coord mouseCoord = get_hovered_cell(mainWindow);

	if (mouseCoord.i != -1)
	{
		select(mouseCoord.j * width + mouseCoord.i);
	}
}

void SketchCreator::select(size_t idx)
{
	curSelected.at(idx) = true;

	for (size_t i = 0; i < 4; i++)
	{
		points->operator[](4 * idx + i).color = selectedColor;
	}
}

void SketchCreator::deselect(size_t idx)
{
	curSelected.at(idx) = false;

	for (size_t i = 0; i < 4; i++)
	{
		points->operator[](4 * idx + i).color = unselectedColor;
	}
}

void SketchCreator::deselect_all()
{
	for (size_t idx = 0; idx < width * height; idx++)
		if (curSelected.at(idx) == true)
			deselect(idx);
}

void SketchCreator::save_step()
{
	pointsAtStep.push_back(new std::vector<Coord>);
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			if (curSelected.at(j * width + i))
			{
				pointsAtStep.at(currentSketchSize)->push_back({ i, j });
			}
		}
	}

	deselect_all();
	currentSketchSize++;
}

void SketchCreator::info_draw_last_grid()
{
	Coord mouseCell = get_hovered_cell(infoWindow);
	infoText.setString("(" + std::to_string(mouseCell.i) + "," + std::to_string(mouseCell.j) + ")");

	infoWindow->draw(*infoGrid);
	infoWindow->draw(*infoPoints);
	infoWindow->draw(infoText);
}

void SketchCreator::info_last_grid()
{
	if (infoWindow != nullptr)
		delete infoWindow;

	infoWindow = new sf::RenderWindow(sf::VideoMode(mainWindow->getSize().x, mainWindow->getSize().x), "Information Window");
	if (currentSketchSize >= 1)
	{
		for (size_t k = 0; k < 4 * height * width; k++) //resets the color of all points
		{
			infoPoints->operator[](k).color = unselectedColor;
		}


		std::vector<Coord> last_state = std::vector<Coord>(*pointsAtStep.at(currentSketchSize - 1));
		for (Coord c : last_state)
		{
			for (size_t k = 0; k < 4; k++)
				infoPoints->operator[]((c.j* width + c.i) * 4 + k).color = selectedColor;
		}
	}
	current_info_draw = &SketchCreator::info_draw_last_grid;
}

SketchCreator::SketchCreator(int _width, int _height, int _windowWidth, int _windowHeight)
{
	width = _width;
	height = _height;
	currentSketchSize = 0;
	curSelected.resize(_height * _width);
	infoWindow = nullptr;
	informationClosing = false;

	selectedColor = sf::Color::Red;
	unselectedColor = sf::Color::Blue;

	float leftMargin = _windowWidth / 10.f;
	float topMargin = _windowHeight / 10.f;

	mainWindow = new sf::RenderWindow(sf::VideoMode(_windowWidth, _windowHeight), "Sketch Creator");

	meshSize = fminf((_windowWidth - 2.f * leftMargin) / _width, (_windowHeight - 2.f * topMargin) / _height);
	topLeft = Point(leftMargin, topMargin);
	botRight = Point(_windowWidth - leftMargin, _windowHeight - topMargin);

	grid = new sf::VertexArray(sf::Lines, 2 * (_width + _height + 2));
	points = new sf::VertexArray(sf::Quads, _width * _height * 4);
	float pointSize = meshSize / 4.f;

	float textSize = 100.f;
	font.loadFromFile("Fonts/MinecraftRegular-Bmg3.otf");
	cellHoveredText.setFont(font);
	cellHoveredText.setPosition((botRight.x + topLeft.x) / 2.f - textSize / 2.f, botRight.y + topMargin / 4.f);
	cellHoveredText.setString("(xx,xx)");

	infoText.setFont(font);
	infoText.setPosition((botRight.x + topLeft.x) / 2.f - textSize / 2.f, botRight.y + topMargin / 4.f);

	Point pointCenter;
	for (size_t i = 0; i < _width; i++)
	{
		for (size_t j = 0; j < _height; j++)
		{
			pointCenter = { topLeft.x + (i + 0.5f) * meshSize, topLeft.y + (j + 0.5f) * meshSize };

			points->operator[]((j* _width + i) * 4).position = sf::Vector2f(pointCenter.x - 0.5f * pointSize, pointCenter.y - 0.5f * pointSize);
			points->operator[]((j* _width + i) * 4 + 1).position = sf::Vector2f(pointCenter.x + 0.5f * pointSize, pointCenter.y - 0.5f * pointSize);
			points->operator[]((j* _width + i) * 4 + 2).position = sf::Vector2f(pointCenter.x + 0.5f * pointSize, pointCenter.y + 0.5f * pointSize);
			points->operator[]((j* _width + i) * 4 + 3).position = sf::Vector2f(pointCenter.x - 0.5f * pointSize, pointCenter.y + 0.5f * pointSize);

			points->operator[]((j* _width + i) * 4).color = unselectedColor;
			points->operator[]((j* _width + i) * 4 + 1).color = unselectedColor;
			points->operator[]((j* _width + i) * 4 + 2).color = unselectedColor;
			points->operator[]((j* _width + i) * 4 + 3).color = unselectedColor;

		}
	}

	//for (size_t i = 0; i < _width + 1; i++)
	//{
	//	grid->operator[](i * 2).position = sf::Vector2f(topLeft.x + i * meshSize, topLeft.y);
	//	grid->operator[](i * 2 + 1).position = sf::Vector2f(topLeft.x + i * meshSize, topLeft.y + _height * meshSize);

	//	grid->operator[](i * 2).color = sf::Color::White;
	//	grid->operator[](i * 2 + 1).color = sf::Color::White;
	//}

	//for (size_t j = 0; j < _height + 1; j++)
	//{
	//	grid->operator[](2 * (width + 1) + 2 * j).position = sf::Vector2f(topLeft.x, topLeft.y + j * meshSize);
	//	grid->operator[](2 * (width + 1) + 2 * j + 1).position = sf::Vector2f(topLeft.x + _width * meshSize, topLeft.y + j * meshSize);
	//	
	//	grid->operator[](2 * (width + 1) + 2 * j).color = sf::Color::White;
	//	grid->operator[](2 * (width + 1) + 2 * j + 1).color = sf::Color::White;
	//}

	//alternative grid rendering
	for (size_t i = 0; i < _width; i++)
	{
		grid->operator[](i * 2).position = sf::Vector2f(topLeft.x + i * meshSize + 0.5 * meshSize, topLeft.y + 0.5 * meshSize);
		grid->operator[](i * 2 + 1).position = sf::Vector2f(topLeft.x + i * meshSize + 0.5 * meshSize, topLeft.y + _height * meshSize - 0.5 * meshSize);

		grid->operator[](i * 2).color = sf::Color::White;
		grid->operator[](i * 2 + 1).color = sf::Color::White;
	}

	for (size_t j = 0; j < _height; j++)
	{
		grid->operator[](2 * (width + 1) + 2 * j).position = sf::Vector2f(topLeft.x + 0.5 * meshSize, topLeft.y + j * meshSize + 0.5 * meshSize);
		grid->operator[](2 * (width + 1) + 2 * j + 1).position = sf::Vector2f(topLeft.x + _width * meshSize - 0.5 * meshSize, topLeft.y + j * meshSize + 0.5 * meshSize);
		
		grid->operator[](2 * (width + 1) + 2 * j).color = sf::Color::White;
		grid->operator[](2 * (width + 1) + 2 * j + 1).color = sf::Color::White;
	}

	infoGrid = new sf::VertexArray(sf::Lines, 2 * (_width + _height + 2));
	infoPoints = new sf::VertexArray(sf::Quads, _width * _height * 4);
	*infoGrid = *grid;
	*infoPoints = *points;

}

void SketchCreator::run()
{
	while (mainWindow->isOpen())
	{
		if (informationClosing)
		{
			infoWindow->close();
			delete infoWindow;
			infoWindow = nullptr;
			informationClosing = false;
		}

		Coord mouseCell = get_hovered_cell(mainWindow);
		cellHoveredText.setString("(" + std::to_string(mouseCell.i) + "," + std::to_string(mouseCell.j) + ")");

		while (mainWindow->pollEvent(ev))
		{
			switch (ev.type)
			{
			default:
				break;
			case sf::Event::Closed:
				mainWindow->close();
				break;
			case sf::Event::MouseButtonPressed:
				switch (ev.key.code)
				{
				default:
					break;
				case sf::Mouse::Right:
					on_right_click();
					break;
				case sf::Mouse::Left:
					on_left_click();
					break;
				}
			case sf::Event::KeyPressed:
				switch (ev.key.code)
				{
				default:
					break;
				case sf::Keyboard::C:
					deselect_all();
					break;
				case sf::Keyboard::D:
					info_last_grid();
					break;
				case sf::Keyboard::Return:
					save_step();
					break;
				}
			}
		}	

		if (infoWindow != nullptr)
		{
			while (infoWindow->pollEvent(ev))
			{
				switch (ev.type)
				{
				default:
					break;
				case sf::Event::Closed:
					informationClosing = true;
					break;
				}
			}
		}

		mainWindow->clear(sf::Color::Black);
		mainWindow->draw(*grid);
		mainWindow->draw(*points);
		mainWindow->draw(cellHoveredText);
		mainWindow->display();

		if (infoWindow != nullptr)
		{
			infoWindow->clear(sf::Color::Black);
			(this->*current_info_draw)();
			infoWindow->display();
		}
	}
}

std::vector<std::vector<Coord>*> SketchCreator::get_sketch()
{
	return pointsAtStep;
}

int SketchCreator::get_width()
{
	return width;
}

int SketchCreator::get_height()
{
	return height;
}

Sketch SketchCreator::get_processed_sketch()
{
	return process_sketch(pointsAtStep, width, height);
}
