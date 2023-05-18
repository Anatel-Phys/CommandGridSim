#include <SFML/Graphics.hpp>

struct TimePoint
{
	int val;
	float t;

	TimePoint(int i_val, float i_t)
	{
		val = i_val;
		t = i_t;
	}
};

#pragma once
class PatternSimulation
{
private:
	int m_width, m_height;

	float m_timeBeforeReset;
	float m_refreshTime;
	sf::Clock m_patternTimer;
	sf::Clock m_patternResetTimer;

	std::vector<TimePoint> m_vertSensorData;
	std::vector<TimePoint> m_horizSensorData;

	std::vector<float> m_sensorXs;
	std::vector<float> m_sensorYs;
	float m_sensorWidth;

	//visual
	sf::VertexArray* m_p_vertLines;
	sf::VertexArray* m_p_horizLines;
	sf::RenderWindow* m_p_window;
	float m_windowWidth, m_windowHeight;
	sf::Event m_eventHandler;

	//helpers
	bool check_collision();
	bool readingPattern;

public:
	PatternSimulation(int i_nSensorX, int i_nSensorY, float i_timeBeforeReset, float i_refreshTime, int i_windowWidth, int i_windowHeight, float i_sensorWidth = 20.f);
	bool is_running();
	void run();
	std::vector<TimePoint> get_patternX();
	std::vector<TimePoint> get_patternY();

};

