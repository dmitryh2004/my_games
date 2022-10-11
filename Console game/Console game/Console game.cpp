#include <iostream>
#include <string>
#include <iomanip>
#include <conio.h>
#include <fstream>
#include "consolecolors.h"
#include <ctime>

#define virusdamage 20
#define bonusheal 10

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
		for (int i = 0; i < map.size(); i++)
		{
			if (map[i] == none.ch)
			{
				SetColor(none.bg, none.t);
			}
			else if (map[i] == player.ch)
			{
				SetColor(player.bg, player.t);
			}
			else if (map[i] == bonus.ch)
			{
				SetColor(bonus.bg, bonus.t);
			}
			else if (map[i] == virus.ch)
			{
				SetColor(virus.bg, virus.t);
			}
			else if (map[i] == wall.ch)
			{
				SetColor(wall.bg, wall.t);
			}
			else if (map[i] == '\n')
			{
				SetColor(0, 15);
			}
			else
			{
				log_file << "Error while showing map: unidentified character " << this->map[i] << " on position " << i << endl;
				SetColor(0, 15);
			}
			cout << map[i];
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
		generate(1, '+', map);
		generate(clamp(difficulty/2, 1, difficulty), 'D', map);
	}
	else if (map.map[new_pos] == virus.ch)
	{
		player_hp -= virusdamage;
		generate(difficulty, 'D', map);
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

int main()
{
	srand(time(NULL));
	fstream l("log.txt", ios::out | ios::trunc);
	l.close();
	string game_map_map = 
"********************\n\
* *   +            *\n\
* P                *\n\
*       *    *     *\n\
*               D  *\n\
*                  *\n\
*  *          +    *\n\
*                  *\n\
*     +            *\n\
*            *     *\n\
*     *            *\n\
********************\n";
	Map game_map("game map", game_map_map, 20, 12);

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
		if (player_hp <= 0)
		{
			exit = true;
			continue;
		}
		switch (_getch())
		{
		case 'w':
			update_map(0, -1, game_map);
			break;
		case 'a':
			update_map(-1, 0, game_map);
			break;
		case 's':
			update_map(0, 1, game_map);
			break;
		case 'd':
			update_map(1, 0, game_map);
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
