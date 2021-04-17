#include <SFML/Graphics.hpp>
#include "algorithm"
#include <cstdlib>
#include <iostream>
#include "SFML/Audio/Music.hpp"
#include<Windows.h>

using namespace sf;

const int width = 18;
const int height = 18;
const int total_colors = 8; // 0...7

const int offset_x = 2;
const int offset_y = 3;

// Кол-во рядов (вертикаль)
const int rows = 20;
// Кол-во строк (горизонталь)
const int columns = 10;

int field[columns][rows]; // игровое поле
int new_field[columns][rows]; // игровое поле после удаления строк
bool rows_were_deleted = false;
using namespace std;

int additionalVAR;
int additionalVAR_1;
int additionalVAR_2;
int additionalVAR_3;
int score;
int menu()// меню
{
	int lines;
	cout << "choose line\n" << endl;
	cout << "1) start infinite game\n"
		<< "2) start game on time\n"
		<< "3) exit\n" << endl;
	cout << "number of line:";
	cin >> lines;
	return lines;
}
using namespace sf;
// Текущий тип нашей фигуры (индекс для figures)
int ftype;

/*
	моно - 1
	ди либо до - 2
	три - 3
	тетра - 4
*/

// Массив фигурок-тетрамино
int figures[7][4] =
{
	1,3,5,7, // I
	2,4,5,7, // Z
	3,5,4,6, // S
	3,5,4,7, // T
	2,3,5,7, // L
	3,5,7,6, // J
	2,3,4,5, // O
};

// Дельта Х (Куда двигаться относительно горизонтали)
int dx = 0;

// Задержка перед сдвигом фигуры вниз.
float delay = 0.5f;
// Создаем таймер
sf::Clock timer;

struct Point {
	int x;
	int y;
};

Point tetramino[4];
 bool collision_check() {
	for (int i = 0; i < 4; i++) {
		if (tetramino[i].y >= (rows - 1)) {
			return true;
		}
	}
	for (int i = 0; i < 4; i++) {
		int x = tetramino[i].x;
		int y = tetramino[i].y;
		if (field[x][y + 1] != -1) {
			return true;
		}
	}
	return false;
}

bool out_of_bounds(Point t[4]) {
	bool out_of_bound = false;
	for (int i = 0; i <= 3; i++) {
		if (t[i].x >= columns || t[i].x < 0)
		{
			out_of_bound = true;
		}
		if (t[i].y >= rows)
		{
			out_of_bound = true;
		}
	}
	return out_of_bound;
};

bool is_trying_to_replace(Point t[4]) {
	for (int i = 0; i < 4; i++) {
		int x = t[i].x;
		int y = t[i].y;
		if (field[x][y] != -1) {
			return true;
		}
	}
	return false;
}

void delete_rows() {
	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < columns; x++) {
			new_field[x][y] = field[x][y];
		}
	}

	for (int y = 0; y < rows; y++) {
		bool have_empty_slots = false;
		for (int x = 0; x < columns; x++) {
			if (new_field[x][y] == -1) {
				have_empty_slots = true;
			}
		}
		if (!have_empty_slots) {
			rows_were_deleted = true;
			for (; y >= 0; y--) {
				for (int x = 0; x < columns; x++) {
					if (y == 0) {
						new_field[x][y] = -1;
					} else {
						new_field[x][y] = new_field[x][y - 1];
					}
				}
			}
		}
	}
}

void rotate() {
	Point XO = tetramino[1];
	Point new_tetramino[4];
	for (int i = 0; i <= 3; i++) {
		int new_x = XO.x - tetramino[i].y + XO.y;
		int new_y = XO.y + tetramino[i].x - XO.x;
		new_tetramino[i].x = new_x;
		new_tetramino[i].y = new_y;
	}
	if (!out_of_bounds(new_tetramino) 
		&& !is_trying_to_replace(new_tetramino)) {
		for (int i = 0; i <= 3; i++) {
			tetramino[i] = new_tetramino[i];
		}
	}
}

void move() {
	int dy = 0;
	if (timer.getElapsedTime().asSeconds() > delay) {
		dy = 1;
		timer.restart();
	}
	Point new_tetramino[4];
	for (int i = 0; i <= 3; i++) {
		new_tetramino[i].x = tetramino[i].x + dx;
		new_tetramino[i].y = tetramino[i].y + dy;
	}
	if (!out_of_bounds(new_tetramino) 
		&& !is_trying_to_replace(new_tetramino)) {
		for (int i = 0; i <= 3; i++) {
			tetramino[i].y = new_tetramino[i].y;
			tetramino[i].x = new_tetramino[i].x;
		}
	}
	dx = 0;
}

void generate_new_tetramino()
{
	ftype = (rand() % 7);

	for (int i = 0; i < 4; i++) {
		tetramino[i].x = figures[ftype][i] % 2;
		tetramino[i].y = (figures[ftype][i] / 2) - 1;
	}
}

void draw_field(sf::Sprite sprite, sf::RenderWindow& window) {
	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < columns; x++) {
			if (field[x][y] == -1)
			{
				continue;
			}
			sprite.setTextureRect(IntRect(field[x][y] * 18, 0, 18, 18));
			sprite.setPosition(x * width, y * height);
			// Смещение относительно подложки.
			sprite.move(70, 59);
			window.draw(sprite);
		}
	}
}

int main(int argc, char* argv[])
{
	Music music;//созданиен звука и его запуск
	music.openFromFile("music1.ogg");
	music.play();
    checkpoint:
	int lines = menu();
	switch (lines) {
	case 1:
		cout << "game was started" << endl;
		music.openFromFile("music2.ogg");
		music.setLoop(true);
		music.play();
		additionalVAR = 0;
		additionalVAR_1 = 0;
		additionalVAR_2 = 0;
		score = 0;
		break;
	case 2:
		cout << "game was started, hold on for five minutes!" << endl;
		additionalVAR = 1;
		music.openFromFile("music2.ogg");
		music.setLoop(true);
		music.play();
		additionalVAR_1 = 0;
		additionalVAR_2 = 0;
		score = 0;
		break;
	case 3:
		cout << "exit process..." << endl;
		Sleep(3000);
		exit(EXIT_SUCCESS);
		break;
	default:
		cout << "error wrong number" << endl;
		goto checkpoint;

	}
	srand(time(0));
	for(int i = 0; i < columns; i++)
	{
		std::fill(std::begin(field[i]), std::end(field[i]), -1);
	}
	RenderWindow window(VideoMode(320, 480), "Tetris", Style::Close);

	// Создание и загрузка текстур/спрайтов
	Texture texture;
	Texture background_texture;
	texture.loadFromFile("tiles.png");
	background_texture.loadFromFile("background.png");
	Sprite sprite(texture);
	Sprite background(background_texture);

	generate_new_tetramino();
	// Главный цикл 
	while (window.isOpen())
	{
		// очередь событий
		if (collision_check() == true) {
			background_texture.loadFromFile("GAMEover.png");
			window.draw(background);
			draw_field(sprite, window);
			additionalVAR_1 = 1;
			additionalVAR_2++;
		}
		if (score == 100000 && additionalVAR == 1) {
			additionalVAR = 0;
			additionalVAR_1 = 1;
			background_texture.loadFromFile("YOUwin.png");
			window.draw(background);
		}
		score++;
		Event event;
		while (window.pollEvent(event))
		{
			// закрытие окна
			if (event.type == Event::Closed)
				
				window.close();

			if (event.type == Event::KeyPressed)
			{
				if (event.key.code == Keyboard::Down) {
					delay = 0.1f;
				}
			}
			
			if (event.type == Event::KeyReleased) {
				if (event.key.code == Keyboard::Up) {
					rotate();
				}
				if (event.key.code == Keyboard::Down) {
					delay = 0.5f;
				}
			}

			if (Keyboard::isKeyPressed(Keyboard::Left))
			{
				dx = -1;
			}
			else if (Keyboard::isKeyPressed(Keyboard::Right))
			{
				dx = 1;
			}
		}

		// окрашивание фона
		window.clear(Color::White);
		window.draw(background);
		/*
		// Заполнение поля целиком, для проверки
		for (int y = 0; y < rows; y++) {
			sprite.setTextureRect(IntRect((y % total_colors) * width, 0, 18, 18));
			for (int x = 0; x < columns; x++) {
				sprite.setPosition(width * x, height * y);
				window.draw(sprite);
			}
		}
		*/
		

		/*
		// Вывод всех типов фигур, для проверки
		for (int type = 0; type < 7; type++) {
			for (int i = 0; i < 4; i++) {
				tetramino[i].x = figures[type][i] % 2;
				tetramino[i].y = figures[type][i] / 2;
			}

			sprite.setTextureRect(IntRect((type % total_colors) * width, 0, 18, 18));
			int offset = type * 3 * width;
			for (int i = 0; i < 4; i++) {
				sprite.setPosition((tetramino[i].x * width) + offset, tetramino[i].y * height);
				window.draw(sprite);
			}
		}
		*/

		move();
		if (collision_check()) {
			for (int i = 0; i < 4; i++) {
				int x = tetramino[i].x;
				int y = tetramino[i].y;
				field[x][y] = ftype;
			}
			if (additionalVAR_1 == 0) {
				generate_new_tetramino();
			}
			// Удаляем уже полностью заполненные строчки.
			delete_rows();
		}

		// Отрисовка поля
		draw_field(sprite, window);

		if (rows_were_deleted) {
			window.display();
			rows_were_deleted = false;
			for (int y = 0; y < rows; y++) {
				for (int x = 0; x < columns; x++) {
					field[x][y] = new_field[x][y];
				}
			}

			timer.restart();
			while (timer.getElapsedTime().asSeconds() < 0.5f) {
				continue;
			}
			timer.restart();

			window.clear(Color::White);
			window.draw(background);

			draw_field(sprite, window);
		}



		// Отрисовка управляемой фигуры
		if(additionalVAR_1 ==0)
		{
			sprite.setTextureRect(IntRect(ftype * 18, 0, 18, 18));
			for (int i = 0; i < 4; i++) {
				int x = tetramino[i].x;
				int y = tetramino[i].y;
				sprite.setPosition(x * width, y * height);
				// Смещение относительно подложки.
				sprite.move(70, 59);
				window.draw(sprite);

			}
		}
		if (additionalVAR_2==5){
			cout << "You will return in menu after three second" << endl;
			Sleep(3000);
			goto checkpoint;
		}
		
		//	cout << score<< endl; // для проверки
		window.display();
	}
	return 0;
}