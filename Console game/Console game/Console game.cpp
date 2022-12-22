#include <iostream>
#include <string>
#include <iomanip>
#include <conio.h>
#include <fstream>
#include "consolecolors.h"
#include <ctime>
#include <signal.h>

const int virusdamage = 20;
const int bonusheal = 10;

void handler(int sig)
{
	signal(SIGINT, handler);
}

int clamp(int a, int imin, int imax)
{
	int temp;
	temp = max(a, imin);
	temp = min(temp, imax);
	return temp;
}

using namespace std;

struct object {
	char ch;
	int bg;
	int t;
};

object none = {' ', 0, 0}, player = {'P', 0, 14}, bonus = {'+', 0, 10}, virus = {'D', 0, 12}, wall = {'*', 0, 15};

int playerx = 2, playery = 2;
int player_hp = 50;
int score = 0;
bool sprint = false;

const int difficulty = 1; //отношение числа спавнящихся бонусов к числу спавнящихся вирусов

class Map {

public:
	int width;
	int height;
	string name;
	string map;
	Map(string iname, string init_map, int iwidth, int iheight)
	{
		bool success = true;
		this->name = iname;
		fstream log_file("log.txt", ios::out | ios::app);
		log_file << "======================================" << endl;
		log_file << "Map " << name << " initialize" << endl;
		
		this->width = iwidth;
		this->height = iheight;
		if (init_map.size() != (iwidth + 1) * iheight) //including \n in string
		{
			log_file << "Warning: string map size does not equal to specified width and height of map (size " << init_map.size() << ", width * height = " << (iwidth + 1) << " * " << iheight << " = " << iheight * (iwidth + 1) << ")" << endl;
			success = !success;
		}
		this->map = init_map;
		if (success)
		{
			log_file << "init successful" << endl;
			log_file << "======================================" << endl;
		}
		else
		{
			log_file << "there were some errors during init" << endl;
			log_file << "======================================" << endl;
		}
		log_file.close();
	}

	void show()
	{
		fstream log_file("log.txt", ios::out | ios::app);
		int x=0, y=0;
		for (int i = 0; i < map.size(); i++)
		{
			if (map[i] == player.ch)
			{
				x = i % (this->width+1);
				y = i / (this->width+1);
			}
		}
		for (int i = y-10; i < y+10; i++)
		{
			for (int j = x-20; j < x+20; j++)
			{
				if (((clamp(i, 0, height) != i) || (clamp(j, 0, width) != j) || (clamp(i * (width +1) + j, 0, (width + 1) * height) != i * (width+1) + j)))
				{
					cout << " ";
				}
				else
				{
					if (map[i * (width + 1) + j] == none.ch)
					{
						SetColor(none.bg, none.t);
					}
					else if (map[i * (width + 1) + j] == player.ch)
					{
						SetColor(player.bg, player.t);
					}
					else if (map[i * (width + 1) + j] == bonus.ch)
					{
						SetColor(bonus.bg, bonus.t);
					}
					else if (map[i * (width + 1) + j] == virus.ch)
					{
						SetColor(virus.bg, virus.t);
					}
					else if (map[i * (width + 1) + j] == wall.ch)
					{
						SetColor(wall.bg, wall.t);
					}
					else
					{
						log_file << "Error while showing map: unidentified character " << this->map[i * (width + 1) + j] << " on position " << i * (width + 1) + j << endl;
						SetColor(0, 15);
					}
					if (map[i * (width + 1) + j] == '\n')
					{

					}
					else
						cout << map[i * (width + 1) + j];
				}
			}
			SetColor(0, 15);
			cout << endl;
		}
		log_file.close();
	}
};

void generate(int n, char ch, Map& map)
{
	for (int i = 0; i < n; i++)
	{
		int attempt = 0;
		for (attempt = 0; attempt < map.map.size(); attempt++)
		{
			int temp = rand() % ((map.width + 1) * map.height);
			if (map.map[temp] == none.ch)
			{
				map.map[temp] = ch;
				break;
			}
		}
	}
}

void update_map(int dx, int dy, Map& map)
{
	int pos = playery * (map.width + 1) + playerx;
	map.map[pos] = none.ch;
	int newy = clamp(playery + dy, 0, map.height);
	int newx = clamp(playerx + dx, 0, map.width);
	int new_pos = newy * (map.width + 1) + newx;
	int map_multiplier = (map.map.size() / 500) + 1;
	bool passable = true;
	fstream log_file("log.txt", ios::out | ios::app);
	if (map.map[new_pos] == none.ch)
	{
	}
	else if (map.map[new_pos] == player.ch)
	{
		passable = false;
	}
	else if (map.map[new_pos] == bonus.ch)
	{
		player_hp += bonusheal;
		score += bonusheal;
		generate(map_multiplier, '+', map);
		generate(clamp(difficulty/2, 1, difficulty) * map_multiplier, 'D', map);
	}
	else if (map.map[new_pos] == virus.ch)
	{
		player_hp -= virusdamage;
		generate(difficulty * map_multiplier, 'D', map);
	}
	else if (map.map[new_pos] == wall.ch)
	{
		passable = false;
	}
	else
	{
		log_file << "Error while updating map: player tried to pass a field with unidentified character " << map.map[new_pos] << "; place of that symbol is x=" << (playerx + dx) << " y=" << (playery + dy) << endl;
		passable = false;
	}
	log_file.close();
	if (passable)
	{
		map.map[new_pos] = player.ch;
		playerx = newx;
		playery = newy;
	}
	else
	{
		map.map[pos] = player.ch;
	}
}

void pmove(int dir, int x, Map& map)
{
	switch (dir)
	{
	case 0:
		for (int i = x - 1; i >= 0; i--)
		{
			update_map(0, -1, map);
		}
		break;
	case 1:
		for (int i = 0; i < x; i++)
		{
			update_map(1, 0, map);
		}
		break;
	case 2:
		for (int i = 0; i < x; i++)
		{
			update_map(0, 1, map);
		}
		break;
	case 3:
		for (int i = x - 1; i >= 0; i--)
		{
			update_map(-1, 0, map);
		}
		break;
	}
}

int main()
{
	signal(SIGINT, handler);
	srand(time(NULL));
	fstream l("log.txt", ios::out | ios::trunc);
	l.close();
	string game_map_map = 
"****************************************\n\
* *   +             *   +              *\n\
* P                **       *    *     *\n\
*       *    *       *       *    *    *\n\
*               D  * *              D  *\n\
*                  ***    D + *****    *\n\
*  *          +               *   *    *\n\
*                  ***        *** *    *\n\
*     +                +               *\n\
*            *    +         D D        *\n\
*     *           *****           +D   *\n\
* *   +             *   +              *\n\
* D                **       *    *     *\n\
*       *    *       *       *    *    *\n\
*               D  * *              D  *\n\
*                  ***    D + *****    *\n\
*  *          +               *   *    *\n\
*                  ***        *** *    *\n\
*     +                +               *\n\
*            *    +         D D        *\n\
*     *           *****           +D   *\n\
* *   +             *   +              *\n\
* +                **       *   *      *\n\
*       *    *       *      *    *     *\n\
*               D  * *             D   *\n\
*                  ***    D + *****    *\n\
*  *          +               *   *    *\n\
*                  ***        *** *    *\n\
*     +                +               *\n\
*            *    +         D D        *\n\
*     *           *****            +D  *\n\
****************************************\n";
	Map game_map("game map", game_map_map, 40, 32);

	bool exit = false;
	while (!exit)
	{
		system("cls");
		game_map.show();
		cout << "Player x: " << setw(2) << playerx << " Player y: " << setw(2) << playery;
		if (player_hp > 200)
			SetColor(0, 11);
		else
		{
			if (player_hp <= 200)
				SetColor(0, 10);
			if (player_hp <= 100)
				SetColor(0, 15);
			if (player_hp <= 50)
				SetColor(0, 14);
			if (player_hp <= 20)
				SetColor(0, 12);
		}
		cout << " Player hp: " << setw(5) << player_hp;
		if (score <= 200 / difficulty)
			SetColor(0, 12);
		else
		{
			if (score > 200 / difficulty)
				SetColor(0, 14);
			if (score > 400 / difficulty)
				SetColor(0, 15);
			if (score > 800 / difficulty)
				SetColor(0, 10);
			if (score > 1600 / difficulty)
				SetColor(0, 11);
		}
		cout << " Score: " << setw(6) << score << endl;
		SetColor(0, 15);
		cout << "Sprint: ";
		SetColor(0, (sprint) ? 10 : 12);
		cout << ((sprint) ? "enabled" : "disabled");
		SetColor(0, 15);
		if (player_hp <= 0)
		{
			exit = true;
			continue;
		}
		switch (_getch())
		{
		case 'w':
			pmove(0, 1 + sprint * 2, game_map);
			//update_map(0, -1 - sprint * 2, game_map);
			break;
		case 'a':
			pmove(3, 1 + sprint * 2, game_map);
			//update_map(-1 - sprint * 2, 0, game_map);
			break;
		case 's':
			pmove(2, 1 + sprint * 2, game_map);
			//update_map(0, 1 + sprint * 2, game_map);
			break;
		case 'd':
			pmove(1, 1 + sprint * 2, game_map);
			//update_map(1 + sprint * 2, 0, game_map);
			break;
		case 'p':
			sprint = !sprint;
			break;
		case 'q':
			exit = true;
			break;
		default:
			break;
		}
	}
	system("cls");
	cout << "Game over! Score = " << score << endl;
}
