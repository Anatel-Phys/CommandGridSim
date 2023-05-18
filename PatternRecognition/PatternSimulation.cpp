#include "PatternSimulation.h"
#include <iostream>

bool PatternSimulation::check_collision()
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(*m_p_window);
    bool collisionDetected = false;

    for (int i = 0; i < m_width; i++)
    {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && mousePos.x > m_sensorXs.at(i) - m_sensorWidth / 2. && mousePos.x < m_sensorXs.at(i) + m_sensorWidth / 2.)
        {
            m_p_vertLines->operator[](2 * i).color = sf::Color::Red;
            m_p_vertLines->operator[](2 * i + 1).color = sf::Color::Red;
            collisionDetected = true;
            if (!readingPattern)
                m_patternTimer.restart();

            m_vertSensorData.push_back(TimePoint(i, m_patternTimer.getElapsedTime().asSeconds()));
        }
        else
        {
            m_p_vertLines->operator[](2 * i).color = sf::Color::Blue;
            m_p_vertLines->operator[](2 * i + 1).color = sf::Color::Blue;
        }
    }

    for (int i = 0; i < m_height; i++)
    {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && mousePos.y > m_sensorYs.at(i) - m_sensorWidth / 2. && mousePos.y < m_sensorYs.at(i) + m_sensorWidth / 2.)
        {
            m_p_horizLines->operator[](2 * i).color = sf::Color::Red;
            m_p_horizLines->operator[](2 * i + 1).color = sf::Color::Red;
            if (!readingPattern)
                m_patternTimer.restart();
            collisionDetected = true;

            m_horizSensorData.push_back(TimePoint(i, m_patternTimer.getElapsedTime().asSeconds()));
        }
        else
        {
            m_p_horizLines->operator[](2 * i).color = sf::Color::Blue;
            m_p_horizLines->operator[](2 * i + 1).color = sf::Color::Blue;
        }
    }   

    return collisionDetected;
}

PatternSimulation::PatternSimulation(int i_nSensorX, int i_nSensorY, float i_timeBeforeReset, float i_refreshTime, int i_windowWidth, int i_windowHeight, float i_sensorWidth)
{
    readingPattern = false;

    m_timeBeforeReset = i_timeBeforeReset;
    m_refreshTime = i_refreshTime;
    m_sensorWidth = i_sensorWidth;

    m_p_window = new sf::RenderWindow(sf::VideoMode(i_windowWidth, i_windowHeight), "Pattern Simulation");
    m_p_window->setFramerateLimit(static_cast<int>(1./i_refreshTime));

    float marginX = i_windowWidth / 10.;
    float marginY = i_windowHeight / 10.;

    m_windowWidth = i_windowWidth;
    m_windowHeight = i_windowHeight;
    
    m_sensorXs.resize(i_nSensorX);
    m_sensorYs.resize(i_nSensorY);
    m_width = i_nSensorX;
    m_height = i_nSensorY;

    if (i_nSensorX > 1)
    {
        for (int i = 0; i < i_nSensorX; i++)
        {
            m_sensorXs.at(i) = marginX + i * static_cast<float>((i_windowWidth - 2 * marginX)) / (i_nSensorX - 1);
        }
    }
    else if (i_nSensorX == 1)
    {
        m_sensorXs.at(0) = static_cast<float>(i_windowWidth) / 2.;
    }

    if (i_nSensorY > 1)
    {
        for (int i = 0; i < i_nSensorY; i++)
        {
            m_sensorYs.at(i) = marginY + i * static_cast<float>((i_windowHeight - 2 * marginY)) / (i_nSensorY - 1);
        }
    }
    else if (i_nSensorY == 1)
    {
        m_sensorYs.at(0) = static_cast<float>(i_windowHeight) / 2.;
    }

    m_p_vertLines = new sf::VertexArray(sf::Lines, 2 * i_nSensorX);
    m_p_horizLines = new sf::VertexArray(sf::Lines, 2 * i_nSensorY);

    for (int i = 0; i < i_nSensorX; i++)
    {
        m_p_vertLines->operator[](2 * i).color = sf::Color::Blue;
        m_p_vertLines->operator[](2 * i + 1).color = sf::Color::Blue;

        m_p_vertLines->operator[](2 * i).position = sf::Vector2f(m_sensorXs.at(i), marginY);
        m_p_vertLines->operator[](2 * i + 1).position = sf::Vector2f(m_sensorXs.at(i), i_windowHeight - marginY);
    }

    for (int i = 0; i < i_nSensorY; i++)
    {
        m_p_horizLines->operator[](2 * i).color = sf::Color::Blue;
        m_p_horizLines->operator[](2 * i + 1).color = sf::Color::Blue;

        m_p_horizLines->operator[](2 * i).position = sf::Vector2f(marginX, m_sensorYs.at(i));
        m_p_horizLines->operator[](2 * i + 1).position = sf::Vector2f(i_windowWidth - marginX, m_sensorYs.at(i));
    }

    m_patternTimer.restart();
    m_patternResetTimer.restart();
}

bool PatternSimulation::is_running()
{
    return m_p_window->isOpen();
}

void PatternSimulation::run()
{
    while (m_p_window->pollEvent(m_eventHandler))
    {
        switch (m_eventHandler.type)
        {
        default:
            break;
        case sf::Event::Closed:
            m_p_window->close();
            break;
        }
    }

    if (check_collision())
    {
        if (!readingPattern)
        {
            readingPattern = true;
            m_patternResetTimer.restart();
            m_patternTimer.restart();
        }
        else
        {
            m_patternResetTimer.restart();
        }
    }
    else
    {
        if (readingPattern)
        {
            if (m_patternResetTimer.getElapsedTime().asSeconds() > m_timeBeforeReset)
            {
                readingPattern = false;
                m_horizSensorData.push_back(TimePoint(-1, -1.f));
                m_vertSensorData.push_back(TimePoint(-1, -1.f));
            }
        }
    }
    m_p_window->clear();
    m_p_window->draw(*m_p_vertLines);
    m_p_window->draw(*m_p_horizLines);
    m_p_window->display();

    if (!this->is_running())
    {
        delete m_p_horizLines;
        delete m_p_vertLines;

        if (readingPattern)
        {
            m_horizSensorData.push_back(TimePoint(-1, -1.f));
            m_vertSensorData.push_back(TimePoint(-1, -1.f));
        }
    }
}

std::vector<TimePoint> PatternSimulation::get_patternX()
{
    return m_vertSensorData;
}

std::vector<TimePoint> PatternSimulation::get_patternY()
{
    return m_horizSensorData;
}
