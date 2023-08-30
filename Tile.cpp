
#include "tile.h"

#define WIDTH 1920
#define HEIGHT 1080
#define FPS 60
const int DELAY_TIME = 1000.0f / FPS;

Game* game = 0;
int main(int argc, char* argv[])
{
	Uint32 frameStart, frameTime;
	game = new Game();
	game->init("Engine", 100, 100, WIDTH, HEIGHT, SDL_WINDOW_FULLSCREEN_DESKTOP);
	while (game->Running())
	{
		frameStart = SDL_GetTicks();
		game->handleEvents();
		game->update();

		frameTime = SDL_GetTicks() - frameStart;
		if (frameTime < DELAY_TIME)
		{
			SDL_Delay((int)(DELAY_TIME - frameTime));
		}
	}
	game->clean();
	return 0;
}