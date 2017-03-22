#include <cassert>
#include <iostream>

#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <SFML/Graphics.hpp>

#include "ExtColor.hpp"

std::vector<sf::Vertex> img2vec(sf::Image img)
{
	std::vector<sf::Vertex> result;

	sf::Vector2u size = img.getSize();
	for (std::size_t x = 0; x < size.x; x++)
	{
		for (std::size_t y = 0; y < size.y; y++)
		{
			sf::Vertex point({ static_cast<float>(x), static_cast<float>(y) });
			point.color = img.getPixel(x, y);
			result.push_back(point);
		}
	}

	return result;
}

using MoveLogType = std::vector<std::pair<sf::Vertex, sf::Vertex>>;
void rearrangePixel(sf::Image source, sf::Image dest, sf::Image& output, MoveLogType& move_log)
{
	//assert(source.getSize() == dest.getSize());

	auto source_vec = img2vec(source);
	auto dest_vec = img2vec(dest);

	auto sortByLum = [](sf::Vertex lhs, sf::Vertex rhs)
	{
		return RGB2LAB(RGB{ lhs.color.r, lhs.color.g, lhs.color.b }).l > RGB2LAB(RGB{ rhs.color.r, rhs.color.g, rhs.color.b }).l;
	};

	std::sort(source_vec.begin(), source_vec.end(), sortByLum);
	std::sort(dest_vec.begin(), dest_vec.end(), sortByLum);

	output = source;
	for (std::size_t i = 0; i < source_vec.size(); i++)
	{
		move_log.push_back(std::make_pair(source_vec[i], dest_vec[i]));

		output.setPixel(static_cast<std::size_t>(dest_vec[i].position.x), 
						static_cast<std::size_t>(dest_vec[i].position.y), source_vec[i].color);
	}
}

void drawMoveLog(MoveLogType move_log, float t, sf::RenderWindow& window)
{
	if (t > 1.0f) t = 1.0f;

	sf::VertexArray points(sf::Points);
	for (auto point_pair : move_log)
	{
		sf::Vertex point({ (1 - t) * point_pair.first.position.x + (t * point_pair.second.position.x), 
						   (1 - t) * point_pair.first.position.y + (t * point_pair.second.position.y) });

		point.color = point_pair.first.color;
		points.append(point);
	}

	window.draw(points);
}
int main()
{
	sf::Image source_img;
	source_img.loadFromFile("input.png");

	sf::Image dest_img;
	dest_img.loadFromFile("input2.png");

	if (source_img.getSize() != dest_img.getSize())
	{
		std::cout << "Image size mismatch: resizing...\n";
		cv::Mat source_mat = cv::imread("input.png");
		cv::Mat dest_mat = cv::imread("input2.png");

		cv::resize(source_mat, source_mat, dest_mat.size());
		cv::imwrite("input.png", source_mat);

		source_img.loadFromFile("input.png");
	}
	
	sf::Image output_img;
	MoveLogType move_log;
	rearrangePixel(source_img, dest_img, output_img, move_log);
	output_img.saveToFile("output.png");

	////////////////
	sf::RenderWindow window(sf::VideoMode(output_img.getSize().x, output_img.getSize().y), "Pixel rearrangement");
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);

	sf::Clock clock;
	float t = 0.0f;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
		if(clock.getElapsedTime().asMilliseconds() >= 25)
		{
			t += 0.01f;
			clock.restart();
		}
		drawMoveLog(move_log, t, window);
		window.display();
	}
	return 0;
}