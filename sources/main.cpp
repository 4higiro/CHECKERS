#include <iostream> 
#include <vector> 
using namespace std;

#include "gui.h"
#include "board.h" 

#define GUI_VERSION

int main()
{
#ifdef CONSOLE_VERSION
	setlocale(LC_ALL, "Ru"); // ������� ����
	printRules(); // ������ ������� ������
	int choiceColor; // ����� �����
	nueral_network nn(1, 2, 0); // ��������� ��������� ����
	while (true)
	{
		cout << "�������� ����: ������ (-1) ��� ����� (1) --> ";
		cin >> choiceColor; // ���� �����
		if (choiceColor != BLACK && choiceColor != WHITE)
			cout << "������������ ����" << endl;
		else
			break;
	}
	board game; // �������� ������� �����
	game.build(3, nn); // ���������� ������ ��� �������� 3-� ��������� �����
	game.print(); // ������ ������� ������� �� �����
	while (game.winner() == 0) // ���� ���� �� ����������
	{
		// ����� ����������� ����
		if (game.getColor() == BLACK)
			cout << "��� ������" << endl;
		else
			cout << "��� �����" << endl;
		move_coord m = game.getMove(); // ���� ����������
		// ���� ������ � ����
		if (choiceColor == game.getColor())
		{
			// ������ ������������ ����������
			cout << "������������ ����������: �� " << m.i0 << "-" << m.j0 << " � " << m.i1 << "-" << m.j1 << endl;
			// ���������������� ����
			cout << "������� ���������� �����:" << endl;
			int i0, j0;
			cin >> i0 >> j0;
			cout << "������� ���������� ����:" << endl;
			int i1, j1;
			cin >> i1 >> j1;
			m = move_coord(i0, j0, i1, j1);
		}
		else
			system("pause"); // ��������� ��� ������ ������� ������� �������������
		// �������� ����������� ����
		if (!game.checkMove(m))
		{
			// ���� �� �������� - ����������
			cout << "������������ ���" << endl;
			continue;
		}
		game.move(m); // ���������� ����
		game.descent(m, nn); // ����� �� ������
		game.print(); // ������ ������� �������
	}
	// ������ ��������� � ���������� ������
	if (game.winner() == BLACK)
		cout << "������ �������� ������� ������" << endl;
	else if (game.winner() == WHITE)
		cout << "������ �������� ������� �����" << endl;
	else
		cout << "������ �������� ������" << endl;
	system("pause");

#endif
	
#ifdef GUI_VERSION
	setlocale(LC_ALL, "Ru"); 

	Vector2f resolution = Vector2f(1024, 640);
	
	RenderWindow window(VideoMode(resolution.x, resolution.y), L"���������� �����");
	VideoMode vm = VideoMode::getDesktopMode();
	window.setFramerateLimit(60);

	Font font;
	font.loadFromFile("resources/font.otf");

	nueral_network nn(1, 2, 0);

	board game;
	game.build(5, nn);

	gboard game_board = gboard::getInstance();
	game_board.resize({ 8, 8 });
	game_board.setRateField(0, font);
	game_board.setRadioButton(font);

	Text message;
	message.setFont(font);
	message.setCharacterSize(30);
	message.setFillColor(Color(0, 0, 0));
	message.setPosition(60, 500);

	int choice_color = BLACK;
	bool f = false;
	move_coord m;
	int color = BLACK;

	while (window.isOpen())
	{
		Event event;
		while (window.pollEvent(event))
		{
			Vector2i mouse_pos = Mouse::getPosition(window);
			switch (event.type)
			{
			case Event::Closed:
				window.close();
				break;
			case Event::MouseButtonPressed:
				switch (event.mouseButton.button)
				{
				case Mouse::Left:
					if (game_board.checkClickRadioButton(mouse_pos))
						game_board.clickRadioButton(mouse_pos, choice_color);
					if (game_board.checkClickOnBoard(mouse_pos))
					{
						game_board.clickOnBoard(mouse_pos, m, f);
						if (f && !game.checkMove(m))
						{
							f = false;
							continue;
						}
						if (f)
						{
							game.move(m);
							game.descent(m, nn);
							color = game.getColor();
						}
						f = !f;
					}
					break;
				}
				break;
			case Event::KeyPressed:
				switch (event.key.code)
				{
				case Keyboard::Escape:
					window.close();
					break;
				}
				break;
			}
		}
		game_board.setPosition(game);
		if (choice_color == BLACK)
			game_board.rotate180();
		game_board.arrange();
		game_board.setRateField(game.getTreeRate(), font);
		if (game.winner() == BLACK)
		{
			message.setString("BLACK WIN!");
			window.draw(message);
			game_board.setRateField(MAX_RATE, font);
		}
		else if (game.winner() == WHITE)
		{
			message.setString("WHITE WIN!");
			window.draw(message);
			game_board.setRateField(MIN_RATE, font);
		}
		game_board.draw(&window);
		window.display();
		window.clear(Color(78, 103, 204));
		if (color != choice_color)
		{
			m = game.getMove();
			game.move(m);
			game.descent(m, nn);
			color = game.getColor();
		}
	}

#endif

	return 0;
}
