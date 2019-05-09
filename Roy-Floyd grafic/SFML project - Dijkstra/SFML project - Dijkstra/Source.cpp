#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

std::ifstream in("resources/muchii.txt");

//create window
sf::RenderWindow window(sf::VideoMode(700, 500), "Roy-Floyd");


std::vector<sf::CircleShape> nodes;		//points
std::vector<sf::Text> indexes;				//indexes of points
std::vector<sf::VertexArray> edges(sf::Lines);		//edges
std::vector<sf::VertexArray> temp_edges(sf::Lines); //temporary edges while running the algortihmm
std::vector<sf::Vector2f> pos; // positions of points
std::vector<sf::Text> costs;	//edge costs
std::vector<sf::CircleShape> directions; //circles that show direction
std::vector<sf::CircleShape> temp_directions; //temporary circles that show direction while running the algorithm
sf::Font font;					//font for indexes and costs
sf::Time waitTime;  //time for function to wait, in order to see changing edges
sf::Time solutionWaitTime; //stop time, in order to see solutions

int m; // number of edges
int a[101][101]; //matrix where I store edges
int b[101][101]; //matrix copy
bool initialize = 0; //bool that decides when to start algorithm, changes to 1 when you press enter
bool ok = 0; //bool that knows if the algorithm has run;

void read()
{
	int i, j, p;
	in >> m;
	for (int k = 0; k < m; k++)
	{
		in >> i >> j >> p;
		a[i][j] = p;
		b[i][j] = p;
	}
}

void draw_node_store_in_vector()
{
	sf::CircleShape circle(20.0f);
	float radius = circle.getRadius();
	circle.setPointCount(100);
	circle.setFillColor(sf::Color(251, 234, 235));
	circle.setOutlineThickness(1.5f);
	circle.setOutlineColor(sf::Color(0, 0, 0));
	circle.setPosition(sf::Vector2f(sf::Mouse::getPosition(window)));
	circle.setOrigin(radius, radius);
	nodes.push_back(circle);
}

void draw_index_store_in_vector()
{
	sf::Text index;
	index.setFont(font);
	index.setString(std::to_string(indexes.size()));
	index.setCharacterSize(30);
	index.setFillColor(sf::Color::Black);
	index.setOrigin(7.f, 19.f);
	index.setPosition(sf::Vector2f(sf::Mouse::getPosition(window)));
	indexes.push_back(index);
}

void draw_edge_store_in_vector(int i, int j, sf::Color color, std::vector<sf::VertexArray> &vector)
{
	sf::VertexArray line(sf::Lines, 2);
	line[0].position = pos[i];
	line[0].color = color;
	line[1].position = pos[j];
	line[1].color = color;
	vector.push_back(line);
}

void draw_line(int i, int j, sf::Color color)
{
	sf::VertexArray line(sf::Lines, 2);
	line[0].position = pos[i];
	line[0].color = color;
	line[1].position = pos[j];
	line[1].color = color;
	window.draw(line);
}

void draw_costs_store_in_vector(int i, int j, int mat[101][101], sf::Color color)
{
	sf::Text cost;
	cost.setFont(font);
	cost.setString(std::to_string(mat[i][j]));
	cost.setCharacterSize(30);
	cost.setFillColor(color);
	cost.setOrigin(7.f, 19.f);
	cost.setPosition(pos[i].x + (4 * (pos[j].x - pos[i].x) / 5), pos[i].y + (4 * (pos[j].y - pos[i].y) / 5));
	costs.push_back(cost);
}

void draw_direction_store_in_vector(int i, int j, sf::Color color, std::vector<sf::CircleShape> &vector)
{
	sf::CircleShape circle(10.0f);
	float radius = circle.getRadius();
	circle.setPointCount(100);
	circle.setFillColor(color);
	float slope2 = atan2(pos[j].y - pos[i].y, pos[j].x - pos[i].x);
	int offset = 22;
	circle.setPosition(pos[j].x - offset * cos(slope2), pos[j].y - offset * sin(slope2));
	circle.setOrigin(radius, radius);
	vector.push_back(circle);
}



void draw_graph()
{
	for (const auto& l : edges)
	{
		window.draw(l);
	}

	for (const auto& x : temp_edges)
	{
		window.draw(x);
	}

	for (const auto& d : directions)
	{
		window.draw(d);
	}

	for (const auto& d : temp_directions)
	{
		window.draw(d);
	}

	for (const auto& c : nodes)
	{
		window.draw(c);
	}

	for (const auto& i : indexes)
	{
		window.draw(i);
	}

	for (const auto& n : costs)
	{
		window.draw(n);
	}
}

int main()
{
	float timp;
	float timp2;
	//std::cout << "Interpretare grafica a algoritmului Roy-Floyd\n";
	//std::cout << "Instructiuni:\n" << "1. In fisierul numit muchii.txt din folderul resurse, specificati numarul de muchii, urmat de acele muchii care se vor afla in graful cu costuri orientat. \n";
	//std::cout << "2.  Desenati pe ecran nodurile grafului folosind click stanga.\n";
	//std::cout << "3. Apasati tasta m pentru a desena muchiile pe ecran.\n";
	//std::cout << "4. Apasati tasta Enter de doua ori pentru a rula algoritmul.\n\n";
	std::cout << "Care va fi timpul de rulare al algoritmului? ";
	std::cin >> timp;
	std::cout << "Care va fi timpul de asteptare la gasirea unei solutii? ";
	std::cin >> timp2;
	waitTime = sf::seconds(timp);
	solutionWaitTime = sf::seconds(timp2);
	read();

	if (!font.loadFromFile("resources/TimesNewerRoman-Regular.otf"))
	{
		//error
	}

	//run the program as long as the window is open
	while (window.isOpen())
	{
		//check all the window's events that were triggered since the last iteration
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::MouseButtonPressed:
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					draw_node_store_in_vector();
					draw_index_store_in_vector();
				}

			case sf::Event::KeyPressed:
			{
				if (event.key.code == sf::Keyboard::Escape)
				{
					window.close();
					break;
				}

				if (event.key.code == sf::Keyboard::M)
				{
					for (int i = 0; i < nodes.size(); i++)
					{
						pos.push_back(nodes[i].getPosition());
					}
					for (int i = 0; i < pos.size(); i++)
					{
						for (int j = 0; j < pos.size(); j++)
						{
							if (a[i][j])
							{
								draw_edge_store_in_vector(i, j, sf::Color::Black, edges);
								draw_costs_store_in_vector(i, j, a, sf::Color::Red);
								draw_direction_store_in_vector(i, j, sf::Color::Black, directions);
							}
						}
					}
				}
			}

			if (event.key.code == sf::Keyboard::T)
			{

				for (int i = 0; i < pos.size(); i++)
				{
					std::cout << pos[i].x << ' ' << pos[i].y << '\n';
				}
			}

			if (event.key.code == sf::Keyboard::Enter)
			{


				initialize = !initialize;
				ok = 1;
			}
			}
		}

		if (initialize) //Roy-Floyd
		{
			int i, j, k;
			for (k = 0; k < nodes.size(); k++)
			{
				for (i = 0; i < nodes.size(); i++)
				{

					for (j = 0; j < nodes.size(); j++)
					{
						window.clear(sf::Color::White);

						//set temporary values for the current frame

						draw_edge_store_in_vector(i, k, sf::Color::Blue, temp_edges);
						draw_edge_store_in_vector(k, j, sf::Color::Blue, temp_edges);
						draw_edge_store_in_vector(i, j, sf::Color::Red, temp_edges);
						if (i != j && i != k && k != j)
						{
							draw_direction_store_in_vector(i, k, sf::Color::Blue, temp_directions);
							draw_direction_store_in_vector(k, j, sf::Color::Blue, temp_directions);
							draw_direction_store_in_vector(i, j, sf::Color::Red, temp_directions);
						}
						nodes[i].setOutlineColor(sf::Color::Red);
						nodes[j].setOutlineColor(sf::Color::Red);
						nodes[k].setOutlineColor(sf::Color::Blue);
						nodes[i].setOutlineThickness(3.0f);
						nodes[j].setOutlineThickness(3.0f);
						nodes[k].setOutlineThickness(3.0f);

						draw_graph();

						sf::sleep(waitTime); //wait time for current frame
						window.display();

						if (a[i][k] && a[k][j] && (a[i][j] > a[i][k] + a[k][j] || !a[i][j]) && i != j)

						{
							a[i][j] = a[i][k] + a[k][j];
							sf::sleep(solutionWaitTime);
						}

						//reset values back to normal and clear temporary vector
						temp_edges.clear();
						temp_edges.resize(0);
						temp_directions.clear();
						temp_directions.resize(0);

						nodes[i].setOutlineColor(sf::Color::Black);
						nodes[j].setOutlineColor(sf::Color::Black);
						nodes[k].setOutlineColor(sf::Color::Black);
						nodes[i].setOutlineThickness(1.5f);
						nodes[j].setOutlineThickness(1.5f);
						nodes[k].setOutlineThickness(1.5f);
					}
				}
			}
		}


		else
		{

			if (ok)
			{
				//need to compare matrix a and b and show differences
				costs.empty();
				costs.resize(0);
				edges.empty();
				edges.resize(0);
				directions.empty();
				directions.resize(0);
				for (int i = 0; i <= nodes.size(); i++)
					for (int j = 0; j <= nodes.size(); j++)
					{
						if (a[i][j])
						{

							if (a[i][j] != b[i][j])
							{
								draw_costs_store_in_vector(i, j, a, sf::Color::Blue);
								draw_edge_store_in_vector(i, j, sf::Color::Red, edges);
								draw_direction_store_in_vector(i, j, sf::Color::Blue, directions);

							}
							else
							{
								draw_costs_store_in_vector(i, j, a, sf::Color::Red);
								draw_edge_store_in_vector(i, j, sf::Color::Black, edges);
								draw_direction_store_in_vector(i, j, sf::Color::Black, directions);
							}

						}
					}

			}
			window.clear(sf::Color::White);
			draw_graph();
			window.display();
		}
	}
	return 0;
}