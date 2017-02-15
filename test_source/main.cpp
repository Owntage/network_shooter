//#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include "scenes.h"
#include "render_window.h"
#include "gui.h"
#include <iostream>
#include <memory>
#include <algorithm>
#include <string>
#include <chrono>
#include <sstream>
#include <render_system.h>
#include <network.h>
#include <components/weapon_component.h>
#include <math.h>
using namespace std;


#define CONSOLE_WIDTH 400
#define CONSOLE_HEIGHT 100

#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 300

#define PI 3.141593

#define POINTS_COUNT 10

float dotProduct(const sf::Vector2f& a, const sf::Vector2f& b)
{
	return a.x * b.x + a.y * b.y;
}

float crossProduct(const sf::Vector2f& a, const sf::Vector2f& b)
{
	return a.x * b.y - a.y * b.x;
}

float distanceSquared(const sf::Vector2f& a, const sf::Vector2f& b)
{
	float x = a.x - b.x;
	float y = a.y - b.y;
	return x * x + y * y;
}

float vectorDistance(const sf::Vector2f& a, const sf::Vector2f& b)
{
	return sqrt(distanceSquared(a, b));
}

float vectorLength(const sf::Vector2f& a)
{
	sf::Vector2f zeroVec(0, 0);
	return vectorDistance(a, zeroVec);
}

float pointToSegmentDistance(const sf::Vector2f& s1, const sf::Vector2f& s2, const sf::Vector2f& p)
{
	sf::Vector2f s1p = p - s1;
	sf::Vector2f s1s2 = s2 - s1;
	float firstDotProduct = dotProduct(s1p, s1s2);
	if(firstDotProduct < 0)
	{
		return vectorDistance(p, s1);
	}
	sf::Vector2f s2p = p - s2;
	sf::Vector2f s2s1 = -s1s2;
	float secondDotProduct = dotProduct(s2p, s2s1);
	if(secondDotProduct < 0)
	{
		return vectorDistance(p, s2);
	}
	float parallelogramSquare = abs(crossProduct(s1p, s2p));
	return parallelogramSquare / vectorLength(s1s2);
}

float sphereToSegmentDistance(const sf::Vector2f& s1, const sf::Vector2f& s2, const sf::Vector2f& p, float radius)
{
	return pointToSegmentDistance(s1, s2, p) - radius;
}

bool sphereSegmentIntersection(const sf::Vector2f& s1, const sf::Vector2f& s2, const sf::Vector2f& p, float radius)
{
	return sphereToSegmentDistance(s1, s2, p, radius) < 0;
}

float getAngle(const sf::Vector2f& base, const sf::Vector2f& p)
{
	sf::Vector2f delta = p - base;
	if(delta.x < 0) 
	{
		return atan(delta.y / delta.x) + PI;
	}
	else
	{
		if(delta.y < 0)
		{
			return atan(delta.y / delta.x) + PI * 2;
		}
		else
		{
			return atan(delta.y / delta.x);
		}
	}
}

float correctAngle(float angle)
{
	while(angle >= 2 * PI)
	{
		angle -= 2 * PI;
	}
	while(angle < 0)
	{
		angle += 2 * PI;
	}
	return angle;
}

float getSphereDeltaAngle(const sf::Vector2f& p, const sf::Vector2f& sphere, float radius)
{
	float angleSinus = radius / vectorDistance(p, sphere);
	return asin(angleSinus);
}

float getAngularIntersection(float startAngleA, float endAngleA, float startAngleB, float endAngleB)
{
	startAngleA = correctAngle(startAngleA);
	endAngleA = correctAngle(endAngleA);
	startAngleB = correctAngle(startAngleB);
	endAngleB = correctAngle(endAngleB);
	if(startAngleA > endAngleA)
	{
		return getAngularIntersection(startAngleA, 2 * PI, startAngleB, endAngleB) +
			getAngularIntersection(0, endAngleA, startAngleB, endAngleB);
	}
	if(startAngleB > endAngleB)
	{
		return getAngularIntersection(startAngleA, endAngleA, startAngleB, 2 * PI) +
			getAngularIntersection(startAngleA, endAngleA, 0, endAngleB);
	}
	float startAngle = max(startAngleA, startAngleB);
	float endAngle = min(endAngleA, endAngleB);
	return max(endAngle - startAngle, 0.0f);
}




int main()
{
	srand(time(nullptr));
	//GameGuiManager gameGuiManager(800, 600);

	RenderWindow::getInstance()->window.setView(sf::View(sf::Vector2f(0.0, 0.0), sf::Vector2f(800, 600)));
	RenderWindow::getInstance()->window.setFramerateLimit(60);

	//glPointSize(5);

	vector<sf::Vector2f> points;
	for(int i = 0; i < POINTS_COUNT; i++) {
		int x = rand() % 100;
		int y = rand() % 100;
		points.push_back(sf::Vector2f(x, y));
	}

	sort(points.begin(), points.end(), [](sf::Vector2f p1, sf::Vector2f p2){
		if(p1.x != p2.x) return p1.x < p2.x;
		return p1.x < p2.y;
	});

	vector<bool> pointsInHull;
	pointsInHull.resize(POINTS_COUNT);
	for(int i = 0; i < POINTS_COUNT; i++)
	{
		pointsInHull[i] = true;
	}

	
	bool first = true;

	//render
	while(RenderWindow::getInstance()->window.isOpen())
	{
		RenderWindow::getInstance()->window.clear();
		sf::RectangleShape rect;
		//vertex.color = sf::Color::White;
		rect.setSize(sf::Vector2f(5, 5));
		rect.setOrigin(2.5, 2.5);
		int prevVertex = -1;
		int prevPrevVertex = -1;
		for(int i = 0; i < 10; i++)
		{
			rect.setPosition(points[i]);
			RenderWindow::getInstance()->window.draw(rect);
			if(points[0].y < points[i].y) continue;
			if(prevVertex == -1)
			{
				prevVertex = i;
				continue;
			}
			
			sf::Vertex line[2];
			line[0] = points[prevVertex];
			line[1] = points[i];
			
			float cp = crossProduct(points[prevVertex] - points[prevPrevVertex], points[i] - points[prevVertex]);

			if(cp < 0 && prevPrevVertex != -1)
			{
				line[0] = points[prevPrevVertex];
			}

			if(prevPrevVertex != -1 && first)
			{
				cout << "crossProduct: " << cp << endl;
			}
			prevPrevVertex = prevVertex;
			prevVertex = i;
			RenderWindow::getInstance()->window.draw(line, 2, sf::PrimitiveType::Lines);

		}
		first = false;
		RenderWindow::getInstance()->window.display();

		sf::Event event;
		while(RenderWindow::getInstance()->window.pollEvent(event))
		{
			if(event.type == sf::Event::Closed)
			{
				RenderWindow::getInstance()->window.close();
			}
		}
	}

	/*
	sf::Vertex verticle;
	verticle.color = sf::Color::White;
	verticle.position = sf::Vector2f(0.0, 0.0);

	sf::Vector2f lightSource(0, 100);
	sf::Vector2f sphere(0, 0);
	float sphereRadius = 20;
	float lightRadius = 40;
	float lightSourceAngle = 0;
	float lightSourceRadius = 100;

	while(RenderWindow::getInstance()->window.isOpen())
	{
		lightSourceAngle += 2.0 / 60.0;
		lightSource.x = cos(lightSourceAngle) * lightSourceRadius;
		lightSource.y = sin(lightSourceAngle) * lightSourceRadius;
		lightSourceRadius = sin(lightSourceAngle * PI) * 66 + 100;
		RenderWindow::getInstance()->window.clear();

		for(int i = 0; i < WINDOW_WIDTH; i++)
		{
			for(int j = 0; j < WINDOW_HEIGHT; j++)
			{
				verticle.position = sf::Vector2f(i - WINDOW_WIDTH / 2, j - WINDOW_HEIGHT / 2);
				
				float distance = vectorDistance(verticle.position, lightSource);
				//float distance = pointToSegmentDistance(s1, s2, sf::Vector2f(i - WINDOW_WIDTH / 2, j - WINDOW_HEIGHT / 2));
				
				distance /= 100;
				distance += 0.7;
				float intensity = 1;
				if(distance > 1) {
					intensity /= pow(distance, 1.5);
				}
				
				bool inside = false;
				if(vectorDistance(sphere, verticle.position) < sphereRadius)
				{
					inside = true;
					intensity = 1;
				}

				if(sphereSegmentIntersection(verticle.position, lightSource, sphere, sphereRadius) && !inside) {
					intensity = 1;
					float sphereDeltaAngle = getSphereDeltaAngle(verticle.position, sphere, sphereRadius);
					float lightDeltaAngle = getSphereDeltaAngle(verticle.position, lightSource, lightRadius);
					float sphereAngle = getAngle(verticle.position, sphere);
					float lightAngle = getAngle(verticle.position, lightSource);
					float angularIntersection = getAngularIntersection(
						sphereAngle - sphereDeltaAngle, 
						sphereAngle + sphereDeltaAngle,
						lightAngle - lightDeltaAngle,
						lightAngle + lightDeltaAngle);
					intensity = lightDeltaAngle * 2 - angularIntersection;
					intensity /= lightDeltaAngle;
					//intensity = 1;
					//intensity /= pow(distance, 1.5);
					if(distance > 1) {
						intensity /= pow(distance, 1.5);
					}
				}

				verticle.color = sf::Color(255 * intensity, 255 * intensity, 255 * intensity, 255);
				RenderWindow::getInstance()->window.draw(&verticle, 1, sf::PrimitiveType::Points);
			}
		}

		RenderWindow::getInstance()->window.display();
		sf::Event event;
		while(RenderWindow::getInstance()->window.pollEvent(event))
		{
			if(event.type == sf::Event::Closed)
			{
				RenderWindow::getInstance()->window.close();
			}
		}
	}
	
	*/
	return 0;
}