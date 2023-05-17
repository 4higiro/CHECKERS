#ifndef GUI
#define GUI

#include <SFML/Graphics.hpp>
using namespace sf;

#include "board.h"

class gboard
{
private:
	class field
	{
	public:
		int checker = 0;
		RectangleShape rect;
		CircleShape circle;
		bool queen = false;
	};

	field** matrix;

	Vector2f size = { 8, 8 };

	class rate_field
	{
	public:
		RectangleShape shape_1;
		RectangleShape shape_2;
		Text rate;
	};

	rate_field rate;

	class radio_color
	{
	public:
		RectangleShape shape;
		Text white;
		Text black;
	};

	radio_color color;

	gboard() {}
public:
	static gboard getInstance()
	{
		static gboard instance;
		return instance;
	}

	void setPosition(const board& position)
	{
		for (int i = 0; i < size.x; i++)
		{
			for (int j = 0; j < size.y; j++)
			{
				matrix[i][j].checker = position.matrix[i][j];
				matrix[i][j].queen = position.queens[i][j];
			}
		}
	}

	void arrange()
	{
		for (int i = 0; i < size.x; i++)
		{
			for (int j = 0; j < size.y; j++)
			{
				if (matrix[i][j].checker == BLACK)
				{
					matrix[i][j].circle.setRadius(25);
					matrix[i][j].circle.setPosition(259 + j * 65, 67 + i * 65);
					matrix[i][j].circle.setFillColor(Color(255, 0, 0));
					if (matrix[i][j].queen)
					{
						matrix[i][j].circle.setOutlineThickness(5);
						matrix[i][j].circle.setOutlineColor(Color(100, 255, 100));
					}
					else
						matrix[i][j].circle.setOutlineThickness(0);
				}
				else if (matrix[i][j].checker == WHITE)
				{
					matrix[i][j].circle.setRadius(25);
					matrix[i][j].circle.setPosition(259 + j * 65, 67 + i * 65);
					matrix[i][j].circle.setFillColor(Color(255, 255, 255));
					if (matrix[i][j].queen)
					{
						matrix[i][j].circle.setOutlineThickness(5);
						matrix[i][j].circle.setOutlineColor(Color(100, 255, 100));
					}
					else
						matrix[i][j].circle.setOutlineThickness(0);
				}
				else
				{
					matrix[i][j].circle.setRadius(0);
					matrix[i][j].circle.setPosition(0, 0);
					matrix[i][j].circle.setFillColor(Color(78, 103, 204));
					matrix[i][j].circle.setOutlineThickness(0);
					matrix[i][j].circle.setOutlineColor(Color(78, 103, 204));
				}
			}
		}
	}

	void resize(const Vector2f& size)
	{
		this->size = size;
		matrix = new field * [size.x];
		for (int i = 0; i < size.x; i++)
		{
			matrix[i] = new field[size.y];
			for (int j = 0; j < size.y; j++)
			{
				if ((j % 2 == 1 && i % 2 == 0) || (j % 2 == 0 && i % 2 == 1))
					matrix[i][j].rect.setFillColor(Color(0, 0, 0));
				else
					matrix[i][j].rect.setFillColor(Color(200, 255, 255));
				matrix[i][j].rect.setSize({ 65, 65 });
				matrix[i][j].rect.setPosition(Vector2f(252 + j * 65, 60 + i * 65));
			}
		}
	}

	void rotate180()
	{
		int** chekers = new int* [size.x];
		bool** queens = new bool* [size.x];
		for (int i = 0; i < size.x; i++)
		{
			chekers[i] = new int[size.y];
			queens[i] = new bool[size.y];
			for (int j = 0; j < size.y; j++)
			{
				chekers[i][j] = matrix[i][j].checker;
				queens[i][j] = matrix[i][j].queen;
			}
		}

		for (int i = size.x - 1; i >= 0; i--)
		{
			for (int j = 0; j < size.y; j++)
			{
				matrix[(int)size.x - i - 1][(int)size.y - j - 1].checker = chekers[i][j];
				matrix[(int)size.x - i - 1][(int)size.y - j - 1].queen = queens[i][j];
			}
		}

		for (int i = 0; i < size.x; i++)
		{
			delete[] chekers[i];
			delete[] queens[i];
		}
		delete[] chekers;
		delete[] queens;
	}

	void setRateField(int rating, const Font& font)
	{
		rate.rate.setFont(font);
		rate.rate.setString(to_string(rating));
		rate.rate.setCharacterSize(50);
		rate.rate.setFillColor(Color(0,0,0));
		rate.rate.setPosition(850, 500);
		rate.shape_1.setFillColor(Color(255, 255, 255));
		rate.shape_2.setFillColor(Color(255, 0, 0));
		int h1 = 0, h2 = 0;
		if (rating > 0)
		{
			int rel = 260 / 24 * rating;
			h1 = 260 - rel;
			h2 = 260 + rel;
		}
		else
		{
			int rel = 260 / 24 * abs(rating);
			h1 = 260 + rel;
			h2 = 260 - rel;
		}
		rate.shape_1.setPosition(Vector2f(850, 60));
		rate.shape_1.setSize(Vector2f(100, h1));
		rate.shape_2.setPosition(Vector2f(850, 60 + h1));
		rate.shape_2.setSize(Vector2f(100, h2));
	}

	void setRadioButton(Font& font)
	{
		color.black.setFont(font);
		color.black.setString("BLACK");
		color.black.setCharacterSize(50);
		color.black.setFillColor(Color(255, 0, 0));
		color.black.setPosition(60, 250);

		color.white.setFont(font);
		color.white.setString("WHITE");
		color.white.setCharacterSize(50);
		color.white.setFillColor(Color(255, 255, 255));
		color.white.setPosition(60, 350);

		color.shape.setFillColor(Color(0, 0, 0));
		color.shape.setSize(Vector2f(170, 80));
		color.shape.setPosition(Vector2f(50, 240));
	}

	bool checkClickRadioButton(const Vector2i& pos)
	{
		return (pos.x >= 50 && pos.x <= 220 && ((pos.y >= 240 && pos.y <= 320) || (pos.y >= 340 && pos.y <= 420)));
	}

	void clickRadioButton(const Vector2i& pos, int& choice_color)
	{
		if (pos.y >= 240 && pos.y <= 320)
		{
			if (choice_color == BLACK)
				return;
			else
			{
				choice_color = BLACK;
				color.shape.setPosition(Vector2f(50, 240));
			}
		}
		else
		{
			if (choice_color == WHITE)
				return;
			else
			{
				choice_color = WHITE;
				color.shape.setPosition(Vector2f(50, 340));
			}
		}
	}

	bool checkClickOnBoard(const Vector2i& pos)
	{
		return pos.x >= 252 && pos.x <= 772 && pos.y >= 60 && pos.y <= 580;
	}

	void clickOnBoard(const Vector2i& pos, move_coord& m, bool f)
	{
		Vector2i current = Vector2i((pos.x - 252) / 65, (pos.y - 60) / 65);
		if (color.shape.getPosition().y != 240)
		{
			if (!f)
			{
				m.i0 = current.y;
				m.j0 = current.x;
			}
			else
			{
				m.i1 = current.y;
				m.j1 = current.x;
			}
		}
		else
		{
			if (!f)
			{
				m.i0 = size.y - current.y - 1;
				m.j0 = size.x - current.x - 1;
			}
			else
			{
				m.i1 = size.y - current.y - 1;
				m.j1 = size.x - current.x - 1;
			}
		}
	}

	void draw(RenderWindow* w)
	{
		for (int i = 0; i < size.x; i++)
		{
			for (int j = 0; j < size.y; j++)
			{
				w->draw(matrix[i][j].rect);
				w->draw(matrix[i][j].circle);
			}
		}
		w->draw(rate.shape_1);
		w->draw(rate.shape_2);
		w->draw(rate.rate);
		w->draw(color.shape);
		w->draw(color.black);
		w->draw(color.white);
	}
};

#endif 