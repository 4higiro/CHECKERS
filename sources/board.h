#ifndef BOARD
#define BOARD

#include <iostream>
using namespace std;

#include "tree.h"
#include "nueral_network.h"

#define BLACK -1
#define WHITE 1

#define MIN_RATE -24
#define MAX_RATE 24

typedef struct move_coord
{
	int i0 = 0;
	int j0 = 0;
	int i1 = 0;
	int j1 = 0;

	move_coord(int _i0, int _j0, int _i1, int _j1)
	{
		i0 = _i0;
		j0 = _j0;
		i1 = _i1;
		j1 = _j1;
	}

	bool operator==(move_coord other)
	{
		if (i0 == other.i0 && j0 == other.j0 && i1 == other.i1 && j1 == other.j1)
			return true;
		else
			return false;
	}

	move_coord() {}
};

class board
{
private:
	int matrix[8][8]{ 0 };
	bool queens[8][8]{ 0 };
	int color;
	int rate;
	move_coord last;
	tree_node<board>* tree;

	friend class gboard;

	bool takeIt(int i0, int j0)
	{
		bool take = false;
		for (int i = -1; i <= 1; i += 2)
		{
			for (int j = -1; j <= 1; j += 2)
			{
				bool border = i0 + 2 * i >= 0 && i0 + 2 * i < 8 && j0 + 2 * j >= 0 && j0 + 2 * j < 8;
				bool queen = (!queens[i0][j0] && ((color == BLACK && i > 0) || (color == WHITE && i < 0))) || queens[i0][j0];
				bool takeWhite = matrix[i0 + i][j0 + j] == WHITE && matrix[i0 + 2 * i][j0 + 2 * j] == 0; 
				bool takeBlack = matrix[i0 + i][j0 + j] == BLACK && matrix[i0 + 2 * i][j0 + 2 * j] == 0;
				if (border && takeBlack && color == WHITE && queen)
					take = true; 
				if (border && takeWhite && color == BLACK && queen)
					take = true;
			}
		}

		return take;
	}

	void fillNode(tree_node<board>*& node)
	{
		if (node->element.winner() != 0)
			return;
		for (int i0 = 0; i0 < 8; i0++)
		{
			for (int j0 = 0; j0 < 8; j0++)
			{
				for (int i1 = 0; i1 < 8; i1++)
				{
					for (int j1 = 0; j1 < 8; j1++)
					{
						if (node->element.checkMove(move_coord(i0, j0, i1, j1)))
						{
							board next = node->element;
							next.move(move_coord(i0, j0, i1, j1));
							node->add(next, 42);
						}
					}
				}
			}
		}
	}

	void rating(tree_node<board>*& node, const nueral_network& nn)
	{
		if (node->size() == 0)
		{
			int blackCheckers = 0;
			int whiteCheckers = 0;
			int blackQueens = 0;
			int whiteQueens = 0;
			for (int i = 0; i < 8; i++)
			{
				for (int j = 0; j < 8; j++)
				{
					if (node->element.matrix[i][j] == BLACK && !node->element.queens[i][j])
						blackCheckers++;
					if (node->element.matrix[i][j] == WHITE && !node->element.queens[i][j])
						whiteCheckers++;
					if (node->element.matrix[i][j] == BLACK && node->element.queens[i][j])
						blackQueens++;
					if (node->element.matrix[i][j] == WHITE && node->element.queens[i][j])
						whiteQueens++;
				}
			}
			int x = blackCheckers - whiteCheckers;
			int y = blackQueens - whiteQueens;
			node->element.rate = nn.getRate(x, y);
		}
		else
		{
			for (int i = 0; i < node->size(); i++)
			{
				tree_node<board>* current = node->getSub(i);
				rating(current, nn);
			}
		}
	}

	void buildDFS(tree_node<board>* current, int deep)
	{
		if (deep > 0)
		{
			for (int i = 0; i < current->size(); i++)
			{
				tree_node<board>* temp = current->getSub(i);
				if (temp->element.winner() != 0)
					break;
				fillNode(temp);
				buildDFS(temp, deep - 1);
			}
		}
	}

	void increasing(tree_node<board>* current)
	{
		if (current->size() == 0)
		{
			fillNode(current);
			return; 
		}
		for (int i = 0; i < current->size(); i++)
			increasing(current->getSub(i)); 
	}

	int alphaBeta(tree_node<board>* current, int alpha, int beta)
	{
		if (current->size() == 0)
			return current->element.rate;
		if (current->element.winner() == BLACK)
			return MAX_RATE;
		if (current->element.winner() == WHITE)
			return MIN_RATE;
		if (current->element.color == BLACK)
		{
			current->element.rate = MIN_RATE;
			for (int i = 0; i < current->size(); i++)
			{
				if (alpha < beta)
				{
					current->element.rate = max(current->element.rate, alphaBeta(current->getSub(i), alpha, beta));
					alpha = max(alpha, current->element.rate); 
				}
			}
			return current->element.rate;
		}
		if (current->element.color == WHITE)
		{
			current->element.rate = MAX_RATE;
			for (int i = 0; i < current->size(); i++)
			{
				if (alpha < beta)
				{
					current->element.rate = min(current->element.rate, alphaBeta(current->getSub(i), alpha, beta));
					beta = min(beta, current->element.rate); 
				}
			}
			return current->element.rate;
		}
	}
public:
	board()
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				if ((j % 2 == 1 && i % 2 == 0) || (j % 2 == 0 && i % 2 == 1))
					matrix[i][j] = BLACK;
			}
		}

		for (int i = 5; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				if ((j % 2 == 1 && i % 2 == 0) || (j % 2 == 0 && i % 2 == 1))
					matrix[i][j] = WHITE;
			}
		}

		color = BLACK;
		rate = 0;
	}

	board(const board& other)
	{
		for (int i = 0; i < 8; i++)
		{
			for (int j = 0; j < 8; j++)
			{
				this->matrix[i][j] = other.matrix[i][j];
				this->queens[i][j] = other.queens[i][j];
			}
		}
		this->color = other.color;
		this->rate = other.rate;
		this->last = other.last;
	}

	void print()
	{
		cout << "===========================" << endl;
		cout << "   ";
		for (int i = 0; i < 8; i++)
			cout << " " << i << " ";
		cout << endl;
		for (int i = 0; i < 8; i++)
		{
			cout << " " << i << " ";
			for (int j = 0; j < 8; j++)
			{
				if (matrix[i][j] == BLACK && queens[i][j])
					cout << "(B)";
				else if (matrix[i][j] == WHITE && queens[i][j])
					cout << "(W)";
				else if (matrix[i][j] == BLACK && !queens[i][j])
					cout << "(b)";
				else if (matrix[i][j] == WHITE && !queens[i][j])
					cout << "(w)";
				else
					cout << " o ";
			}
			cout << endl;
		}
		cout << "===========================" << endl;
	}

	bool checkMove(move_coord m)
	{
		if (color != BLACK && color != WHITE)
			return false; // Данного цвета не существует
		if (m.i0 >= 8 || m.i0 < 0)
			return false; // Номер горизонтали не существует
		if (m.j0 >= 8 || m.j0 < 0)
			return false; // Номер вертикали не существует
		if (m.i1 >= 8 || m.i1 < 0)
			return false; // Номер горизонтали не существует
		if (m.j1 >= 8 || m.j1 < 0)
			return false; // Номер вертикали не существует
		if (!(m.j0 % 2 == 1 && m.i0 % 2 == 0) && !(m.j0 % 2 == 0 && m.i0 % 2 == 1))
			return false; // Клетка не черного цвета
		if (!(m.j1 % 2 == 1 && m.i1 % 2 == 0) && !(m.j1 % 2 == 0 && m.i1 % 2 == 1))
			return false; // Клетка не черного цвета
		if (matrix[m.i0][m.j0] != color)
			return false; // Шашка не того цвета
		if (matrix[m.i1][m.j1] != 0)
			return false; // Клетка занята
		if (color == BLACK && m.i1 <= m.i0 && !queens[m.i0][m.j0])
			return false; // Ход назад невозможен (не для дамки)
		if (color == WHITE && m.i1 >= m.i0 && !queens[m.i0][m.j0])
			return false; // Ход назад невозможен (не для дамки)
		bool take = takeIt(m.i0, m.j0); // Есть ли взятие
		if ((abs(m.i1 - m.i0) != 1 || abs(m.j1 - m.j0) != 1) && !take)
			return false; // Ход вперед больше чем на 1 клетку невозможен (кроме взятия)
		if (take && !queens[m.i0][m.j0] && color == BLACK && !((m.i1 == m.i0 + 2 && m.j1 == m.j0 + 2 && matrix[m.i0 + 1][m.j0 + 1] == WHITE) || (m.i1 == m.i0 + 2 && m.j1 == m.j0 - 2 && matrix[m.i0 + 1][m.j0 - 1] == WHITE)))
			return false; // Взятие совершено некорректно (не для дамки)
		if (take && !queens[m.i0][m.j0] && color == WHITE && !((m.i1 == m.i0 - 2 && m.j1 == m.j0 + 2 && matrix[m.i0 - 1][m.j0 + 1] == BLACK) || (m.i1 == m.i0 - 2 && m.j1 == m.j0 - 2 && matrix[m.i0 - 1][m.j0 - 1] == BLACK)))
			return false; // Взятие совершено некорректно (не для дамки)
		if (take && queens[m.i0][m.j0] && color == BLACK && !((m.i1 == m.i0 + 2 && m.j1 == m.j0 + 2 && matrix[m.i0 + 1][m.j0 + 1] == WHITE) || (m.i1 == m.i0 + 2 && m.j1 == m.j0 - 2 && matrix[m.i0 + 1][m.j0 - 1] == WHITE) || (m.i1 == m.i0 - 2 && m.j1 == m.j0 + 2 && matrix[m.i0 - 1][m.j0 + 1] == WHITE) || (m.i1 == m.i0 - 2 && m.j1 == m.j0 - 2 && matrix[m.i0 - 1][m.j0 - 1] == WHITE)))
			return false; // Взятие совершено некорректно (для дамки)
		if (take && queens[m.i0][m.j0] && color == WHITE && !((m.i1 == m.i0 - 2 && m.j1 == m.j0 + 2 && matrix[m.i0 - 1][m.j0 + 1] == BLACK) || (m.i1 == m.i0 - 2 && m.j1 == m.j0 - 2 && matrix[m.i0 - 1][m.j0 - 1] == BLACK) || (m.i1 == m.i0 + 2 && m.j1 == m.j0 + 2 && matrix[m.i0 + 1][m.j0 + 1] == BLACK) || (m.i1 == m.i0 + 2 && m.j1 == m.j0 - 2 && matrix[m.i0 + 1][m.j0 - 1] == BLACK)))
			return false; // Взятие совершено некорректно (для дамки)
		if (!take)
		{
			for (int i = 0; i < 8; i++)
			{
				for (int j = 0; j < 8; j++)
				{
					if (matrix[i][j] == color && (i != m.i0 || j != m.j0) && takeIt(i, j))
						return false; // Найдено пропущенное взятие - игрок обязан совершить взятие
				}
			}
		}
		return true; // Не найдено ошибок - ход возможен
	}

	void move(move_coord m)
	{
		bool take = takeIt(m.i0, m.j0);
		if (!checkMove(m))
			return; 
		if ((color == BLACK && m.i1 == 7) || (color == WHITE && m.i1 == 0))
			queens[m.i1][m.j1] = 1; 
		if (abs(m.i1 - m.i0) != 1)
		{
			int x = 0;
			int y = 0;
			if (m.i1 < m.i0)
				x = m.i0 - 1;
			else
				x = m.i0 + 1;
			if (m.j1 < m.j0)
				y = m.j0 - 1;
			else
				y = m.j0 + 1;
			matrix[x][y] = 0;
			queens[x][y] = 0;
		}
		matrix[m.i1][m.j1] = matrix[m.i0][m.j0]; 
		matrix[m.i0][m.j0] = 0;
		if (queens[m.i0][m.j0])
		{
			queens[m.i0][m.j0] = 0;
			queens[m.i1][m.j1] = 1;
		}
		if (take && takeIt(m.i1, m.j1) && m.i1 != 0 && m.i1 != 7)
			color = -color;
		color = -color;
		last = m;
	}

	void build(int deep, const nueral_network& nn)
	{
		tree = new tree_node<board>(*this, 42);
		fillNode(tree); 
		buildDFS(tree, deep);   
		rating(tree, nn); 
	}

	void descent(move_coord m, const nueral_network& nn)
	{
		tree_node<board>* head = tree;
		for (int i = 0; i < tree->size(); i++)
		{
			if (tree->getSub(i)->element.last == m)
				head = tree->getSub(i);
			else
				tree_node<board>::clear(tree->getSub(i));
		}
		tree = head; 
		increasing(tree); 
		rating(tree, nn); 
	}

	int winner()
	{
		bool moving = false;
		for (int i0 = 0; i0 < 8; i0++)
		{
			for (int j0 = 0; j0 < 8; j0++)
			{
				for (int i1 = 0; i1 < 8; i1++)
				{
					for (int j1 = 0; j1 < 8; j1++)
					{
						if (checkMove(move_coord(i0, j0, i1, j1)))
						{
							moving = true;
							break;
						}
					}
					if (moving)
						break;
				}
				if (moving)
					break;
			}
			if (moving)
				break;
		}
		if (moving)
			return 0;
		else
			return -color;
	}

	move_coord getMove()
	{
		int r = alphaBeta(tree, MIN_RATE, MAX_RATE);
		for (int i = 0; i < tree->size(); i++)
		{
			if (r == tree->getSub(i)->element.rate)
				return tree->getSub(i)->element.last;
		}
	}

	int getColor()
	{
		return color;
	}

	tree_node<board>* getTree()
	{
		return tree; 
	}

	int getRate()
	{
		return rate;
	}

	int getTreeRate()
	{
		return alphaBeta(tree, MIN_RATE, MAX_RATE);
	}

	void clear()
	{
		tree_node<board>::clear(tree);
	}
};

void printRules()
{
	cout << "АНГЛИЙСКИЕ ШАШКИ (Игрок vs Компьютер)" << endl;
	cout << "Правила игры:" << endl;
	cout << "1) Все шашки располагаются в шахматном порядке по черным полям. По 12 шашек" << endl;
	cout << "   у черных и белых." << endl;
	cout << "2) Шашки бывают двух цветов: черные(b) и белые(w). Первым ходом партию" << endl;
	cout << "   открывают черные" << endl;
	cout << "3) Ходить можно только вперед по диаганали по черным полям. Для этого" << endl;
	cout << "   необходимо сначала ввести координаты расположения шашки, затем ввести" << endl;
	cout << "   координаты клетки доски, на которую вы хотите поставить выбранную шашку" << endl;
	cout << "4) Взятие шашки соперника может происходить в том случае, если рядом с вашей" << endl;
	cout << "   шашкой находится шашка соперника. Для того чтобы произвести взятие" << endl;
	cout << "   необходимо совершить ход, как бы перескакивая через шашку соперника," << endl;
	cout << "   также переместив ее по диаганали, а также поставив свою шашку за шашку" << endl;
	cout << "   соперника. Поле, находящиеся за шашкой соперника при этом должно быть свободно." << endl;
	cout << "   Взятая шашка соперника при этом исчезает с доски." << endl;
	cout << "5) Если после произведенного взятия его можно продолжить, то игрок обязан" << endl;
	cout << "   продолжить взятие. В случае нескольких вариантов взятия игрок имеет право выбрать" << endl;
	cout << "   любое из возможных продолжений." << endl;
	cout << "6) Если ваша шашка доходит до последней горизонтали (для черных это горизонталь 7," << endl;
	cout << "   а для белых это горизонталь 0), то она превращается в дамку (на доске помечается" << endl;
	cout << "   заглавной буквой). Дамка имеет право ходить и производить взятие по диаганали назад" << endl;
	cout << "7) Цель игры - убрать с доски все шашки соперника. Выигрывает соответственно тот, кто" << endl;
	cout << "   произвел взятие всех шашек соперника." << endl;
	system("pause");
}

#endif