#pragma once
#include "SDL.h"
#include "SDL_ttf.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <math.h>
using std::vector;
using std::string;

#define TILES 2
#define HORIZONTAL 192
#define VERTICAL 108
#define UISIZE 50
#define OPTIONS 3
#define DISTANCE 2
#define MIN 2
#define MAX 40
#define SPACE 1


//TODO: la prima fila/colonna in map va aggiustata in base all'altezza delle texture, dato che la lunghezza dovrebbe essere costante di 2 cifre

int min(int a, int b) {
	if (b < a)
		return b;
	return a;
};

int max(int a, int b) {
	if (b > a)
		return b;
	return a;
};

SDL_Texture* textToTexture(SDL_Renderer* Renderer, string &text, TTF_Font* gFont, int fontSize, SDL_Color color) {
	char tempcha[100];
	SDL_Surface* textSurface;
	for (int i = 0; i < text.size(); i++) {
		tempcha[i] = text[i];
	}
	tempcha[text.size()] = '\0';
	textSurface = TTF_RenderText_Solid(gFont, tempcha, color);
	return SDL_CreateTextureFromSurface(Renderer, textSurface);
}

class Slider
{
public:
	SDL_Rect sl, bar, number, tag;
	float vMin, vMax;

	bool selected;
	float s, value;

	SDL_Texture* tagT;
	SDL_Texture* numberT;

	void start() {
		sl.h = UISIZE;
		sl.w = UISIZE / 2;
		bar.h = UISIZE / 4;
		bar.w = UISIZE * 4.9;
		number.h = UISIZE / 2;
		number.w = UISIZE / 2;
	};

	void start2(int vmin, int vmax, int barx, int bary) {
		vMin = vmin;
		vMax = vmax;
		value = vmin;
		bar.y = bary;
		bar.x = barx;
		sl.x = bar.x - sl.w / 2;
		sl.y = bar.y;
		number.y = sl.y + sl.h;
		number.x = sl.x;
		tag.x = bar.x + UISIZE / 8;
		tag.y = bar.y - tag.h - UISIZE / 8;
		selected = false;
	};

	bool check(int mousex, int mousey) {
		if (mousex > sl.x && mousex < sl.x + sl.w && mousey > sl.y && mousey < sl.y + sl.h) {
			selected = true;
			s = sl.x - mousex;
		}
		return selected;
	};

	void update(int mousex, int mousey) {
		if (selected) {
			if (mousex + s >= bar.x - sl.w / 2 && mousex + s <= bar.x + bar.w - sl.w / 2) {
				sl.x = mousex + s;
			}
			else if (mousex + s < bar.x)
				sl.x = bar.x - sl.w / 2;
			else
				sl.x = bar.x + bar.w - sl.w / 2;

			value = (sl.x - (bar.x - sl.w / 2)) * ((vMax - vMin) / bar.w) + vMin;
			number.x = sl.x + sl.w / 2;
		}
	};

	int getValue() {
		return int(value);
	};
};

class Tile
{
public:
	SDL_FRect big, small;
	SDL_Texture* texture;
	int state;

	void start(float x, float y, int w, int h) {
		position(x, y, w, h);
		state = 0;
	}
	
	void position(float x, float y, int w, int h) {
		big.x = x;
		big.y = y;
		big.w = w;
		big.h = h;
		small.x = x + SPACE;
		small.y = y + SPACE;
		small.w = w - 2 * SPACE;
		small.h = h - 2 * SPACE;
	};

	void update(int a) {
		state = a;
	};
};

class Button 
{
public:
	SDL_Rect area;
	SDL_Texture* label;
	bool clicked;

	void start(int x, int y, SDL_Renderer* Renderer, string& text, TTF_Font* gFont, int fontSize, SDL_Color color) {
		area.x = x;
		area.y = y;
		label = textToTexture(Renderer, text, gFont, fontSize, color);
		SDL_QueryTexture(label, NULL, NULL, &area.w, &area.h);
		clicked = false;
	};

	bool check(int mousex, int mousey) {
		if (mousex > area.x && mousex<area.x + area.w && mousey>area.y && mousey < area.y + area.h)
			return true;
		return false;
	};
};


class Game
{
public:

	//funzionamento
	bool running;
	int windowx, windowy;
	int textwidthX, fontSize;
	bool start;

	//rendering
	SDL_Window* Window;
	SDL_Renderer* Renderer;

	vector <Slider> sliders;

	SDL_Surface* tempSurface;
	SDL_Texture* slidertriangle[2];

	TTF_Font* gFont;
	SDL_Surface* textSurface;
	string tempstring;
	char tempcha[10];

	//input
	bool lclick, rclick;
	int mousex, mousey, mousexlast, mouseylast;

	Tile map[MAX+1][MAX+1];
	SDL_Texture* numbers[MAX];
	SDL_Rect grid;
	Button save;
	bool changerect, changeslider;
	int current_value;
	int tileX, tileY;
	int lastTileX, lastTileY;
	long long frame;
	SDL_Color tc;


	bool init(const char* title, int xpos, int ypos, int width, int height, int flags)//prepara SDL
	{
		if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
		{
			Window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
			if (Window != 0)
			{
				Renderer = SDL_CreateRenderer(Window, -1, 0);
				if (Renderer != 0)
				{
					SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
					SDL_GetRendererOutputSize(Renderer, &windowx, &windowy);
				}
				else
				{
					return false;
				}
			}
			else
			{
				std::cout << "window init fail\n";
				return false;
			}
		}
		else
		{
			std::cout << "SDL init fail\n";
			return false;
		}
		if (TTF_Init() < 0) {
			std::cout << "TTF init fail\n";
			return false;
		}
		std::cout << "init success\n";
		running = true;
		init2();
		return true;
	};

	void init2() {
		fontSize = 64;
		gFont = TTF_OpenFont("assets/FreeSans.ttf", fontSize);
		changeslider = true;
		changerect = true;
		current_value = 1;
		tileX = 10;
		tileY = 10;
		tc.r = tc.g = tc.b = tc.a = 255;
		startsliders();
		coordinates();
		string tempString = "Save";
		save.start(sliders[1].bar.x, sliders[1].bar.y + windowy / 8, Renderer, tempString, gFont, 64, tc);
		grid.x = grid.y = UISIZE;
		grid.w = grid.h = windowy - 1.2 * grid.y;
		
		lclick = false;
		frame = 0;



		start = true;
	};

	void startsliders() {
		Slider t;

		for (int i = 0; i < 2; i++) {
			sliders.push_back(t);
			sliders[i].start();
			sliders[i].start2(MIN, MAX, windowx - (sliders[i].bar.w + UISIZE), 3 * UISIZE * (i + 1));
		}
		for (int i = 0; i < sliders.size(); i++) {

			if (i == 0)
				tempstring = "X";
			else
				tempstring = "Y";
			
			sliders[i].tagT = textToTexture(Renderer, tempstring, gFont, 64, tc);
			SDL_QueryTexture(sliders[i].tagT, NULL, NULL, &sliders[i].tag.w, &sliders[i].tag.h);
			sliders[i].tag.w /= 2;
			sliders[i].tag.h /= 2;
			sliders[i].tag.y = sliders[i].bar.y - sliders[i].tag.h - UISIZE / 8;
		}
		slidersUpdate();
		tempSurface = SDL_LoadBMP("assets/tri1.bmp");
		slidertriangle[0] = SDL_CreateTextureFromSurface(Renderer, tempSurface);
		tempSurface = SDL_LoadBMP("assets/tri2.bmp");
		slidertriangle[1] = SDL_CreateTextureFromSurface(Renderer, tempSurface);
	};

	void coordinates() {
		char t[3];
		t[2] = '\0';
		for (int i = 1; i <= MAX; i++) {
			t[0] = (i / 10)+48;
			t[1] = (i % 10)+48;
			textSurface = TTF_RenderText_Solid(gFont, t, tc);
			numbers[i-1] = SDL_CreateTextureFromSurface(Renderer, textSurface);
		}
	}

	void handleEvents() {//registra i tasti cliccati
		SDL_Event event;
		if (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					running = false;
					break;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
					lclick = true;
					sliderCheck();
					buttonCheck();
					break;
				case SDL_BUTTON_RIGHT:
					rclick = true;
					break;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
					lclick = false;
					if (save.clicked) {
						save.clicked = false;
						render();
					}
					sliderUnselect();
					resetHold();
					break;
				case SDL_BUTTON_RIGHT:
					rclick = false;
					break;
				}
				break;
			case SDL_QUIT:
				running = false;
				break;
			default:
				break;
			}
		}
	};

	void sliderCheck() {
		for (int i = 0; i < sliders.size(); i++) {
			if(sliders[i].check(mousex,mousey))
				changeslider = true;
		}
	};

	void buttonCheck() {
		if (!save.check(mousex, mousey))
			return;
		save.clicked = true;
		std::ofstream fout;
		fout.open("output.txt");
		for (int i = 1; i <= tileY; i++) {
			for (int j = 1; j <= tileX; j++) {
				fout << map[i][j].state;
			}
			fout << "\n";
		}
		fout.close();
	}

	void sliderUnselect() {
		for (int i = 0; i < sliders.size(); i++) {
			sliders[i].selected = false;
		}
	};

	void resetHold() {
		lastTileX = 0;
		lastTileY = 0;
	};

	void update() {
		frame++;
		updateMouse();
		if (!lclick && !start)
			return;
		slidersUpdate();
		changeMap();
		changeRect();
		render();
		resetbools();
	};

	void updateMouse() {
		mousexlast = mousex;
		mouseylast = mousey;
		SDL_GetMouseState(&mousex, &mousey);
	};


	void slidersUpdate() {
		for (int i = 0; i < sliders.size(); i++) {
			if (sliders[i].selected) {

				sliders[i].update(mousex, mousey);
				tempstring = std::to_string(int(sliders[i].value));

				sliders[i].numberT = textToTexture(Renderer, tempstring, gFont, 64, tc);
				SDL_QueryTexture(sliders[i].numberT, NULL, NULL, &sliders[i].number.w, &sliders[i].number.h);
				sliders[i].number.w /= 2;
				sliders[i].number.h /= 2;

				updateSize();
			}
		}
	};

	void updateSize() {
		tileX = sliders[0].value;
		tileY = sliders[1].value;
	};

	void changeMap() {
		float x, y, w, h;
		int t = max(tileX, tileY);

		w = h = float(grid.w) / t;
		grid.x = grid.y = w;
		grid.w = grid.h = windowy - 1.2 * grid.y;

		for (int i = 0; i <= MAX; i++) {
			for (int j = 0; j <= MAX; j++) {
				map[i][j].position(j*float(grid.w)/t,i*float(grid.h)/t,w,h);
			}
		}

	};

	void changeRect() {
		if (mousey >= grid.y + grid.h || mousey < grid.y || mousex>grid.x + grid.w || mousex < grid.x)
			return;

		int x, y;
		x = y = 0;

		for (int i = 1; i <= MAX; i++) {
			if (map[0][i].big.x<mousex && map[0][i].big.x + map[0][i].big.w > mousex){
				x = i;
				break;
			}				
		}
		for (int i = 1; i <= MAX; i++) {

			if (map[i][0].big.y<mousey && map[i][0].big.y + map[0][i].big.h > mousey) {
				y = i;
				break;
			}
		}
		if (x > tileX || y > tileY)
			return;
		if (map[y][x].state == 0 && !(lastTileX == x && lastTileY == y)) {
			lastTileX = x;
			lastTileY = y;
			map[y][x].update(1);
		}
		else if (lastTileX != x || lastTileY != y) {
			lastTileX = x;
			lastTileY = y;
			map[y][x].update(0);
		}

	};

	void render() {
		SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
		SDL_RenderClear(Renderer);

		SDL_SetRenderDrawColor(Renderer, 128, 128, 128, 255);

		if(!save.clicked)
			SDL_RenderFillRect(Renderer, &save.area);

		for (int i = 1; i <= tileX; i++) {
			SDL_RenderCopyF(Renderer, numbers[i - 1], NULL, &map[0][i].big);
			SDL_RenderFillRectF(Renderer, &map[0][i].big);
		}
		for (int i = 1; i <= tileY; i++) {
			SDL_RenderCopyF(Renderer, numbers[i - 1], NULL, &map[i][0].big);
			SDL_RenderFillRectF(Renderer, &map[i][0].big);
		}

		SDL_SetRenderDrawColor(Renderer, 50, 50, 50, 255);
		for (int i = 1; i <= tileY; i++) {
			for (int j = 1; j <= tileX; j++) {
				map[i][j].big.h++;
				map[i][j].big.w++;
				SDL_RenderFillRectF(Renderer, &map[i][j].big);
				map[i][j].big.h--;
				map[i][j].big.w--;
			}
		}
		if(save.clicked)
			SDL_RenderFillRect(Renderer, &save.area);
		SDL_SetRenderDrawColor(Renderer, 255, 255, 255, 255);
		for (int i = 1; i <= tileY; i++) {
			for (int j = 1; j <= tileX; j++) {
				if(map[i][j].state)
					SDL_RenderFillRectF(Renderer, &map[i][j].small);
			}
		}

		renderSliders();
		renderTextures();
		SDL_RenderPresent(Renderer);
	};

	void renderSliders() {
		for (int i = 0; i < sliders.size(); i++) {
			SDL_RenderFillRect(Renderer, &sliders[i].bar);
			if (sliders[i].selected)
				SDL_RenderCopy(Renderer, slidertriangle[1], NULL, &sliders[i].sl);
			else
				SDL_RenderCopy(Renderer, slidertriangle[0], NULL, &sliders[i].sl);
			SDL_RenderCopy(Renderer, sliders[i].numberT, NULL, &sliders[i].number);
			SDL_RenderCopy(Renderer, sliders[i].tagT, NULL, &sliders[i].tag);
		}
	};

	void renderTextures() {
		int h, w;

		for (int i = 1; i <= tileX; i++) {
			SDL_RenderCopyF(Renderer, numbers[i-1], NULL, &map[0][i].small);
		}
		for (int i = 1; i <= tileY; i++) {
			SDL_RenderCopyF(Renderer, numbers[i-1], NULL, &map[i][0].small);
		}
		SDL_RenderCopy(Renderer, save.label, NULL, &save.area);
	}

	void resetbools() {
		changerect = false;
		changeslider = false;
		start = false;
	};

	void clean() {//chiude tutto
		std::cout << "cleaning\n";
		SDL_DestroyWindow(Window);
		SDL_DestroyRenderer(Renderer);
		TTF_Quit();
		SDL_Quit();
	};

	bool Running() { return running; }
};
