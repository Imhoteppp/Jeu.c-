#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#define SDL_MAIN_HANDLED
#endif
#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>

#define window_width 800
#define window_height 800
#define tile 50
#define grid_width 16
#define grid_height 16

const char * title = "ESGI Snake";

const int action_delay = 50;
const int timer = 6;

SDL_Surface * screen = NULL;

int action_timer = 0;
int game_active = 1;

int player_x, player_y;
int player_dx, player_dy;

int apple_x, apple_y;

int snake_x[grid_width * grid_height];
int snake_y[grid_width * grid_height];

int length = 0;

void draw_head(int x, int y, int r, int g, int b) {
	roundedBoxRGBA(screen, x * tile + 2, y * tile + 2, (x + 1) * tile - 2, (y + 1) * tile - 2, tile / 4, r, g, b, 255);
}

void draw_body(int x, int y, int r, int g, int b) {
	roundedBoxRGBA(screen, x * tile + 7, y * tile + 7, (x + 1) * tile - 7, (y + 1) * tile - 7, tile / 4, r, g, b, 255);
}

void draw_apple(int x, int y, int r, int g, int b) {
	roundedBoxRGBA(screen, x * tile + 4, y * tile + 4, (x + 1) * tile - 4, (y + 1) * tile - 4, tile / 4, r, g, b, 255);
}

void place_apple() {
	int ok = 0;
	int i;
	while (ok == 0) {
		ok = 1;
		apple_x = rand() % grid_width;
		apple_y = rand() % grid_height;
		if (apple_x == player_x && apple_y == player_y) {
			ok = 0;
		}
		for (i = 0; i < length; i++) {
			if (snake_x[i] == apple_x) {
				if (snake_y[i] == apple_y) {
					ok = 0;
				}
			}
		}
	}
}

void display() {
	int i;
	int j;
	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 51, 51, 51));

	for (i = 0; i < grid_width; ++i) {
		for (j = 0; j < grid_height; ++j) {
			SDL_Rect pos;
			pos.x = i * tile + 5;
			pos.y = j * tile + 5;
			pos.w = tile - 10;
			pos.h = tile - 10;
			SDL_FillRect(screen, &pos, SDL_MapRGB(screen->format, 20, 20, 20));
		}
	}

	draw_apple(apple_x, apple_y, 255, 51, 51);

	for (i = 0; i < length; i++) {
		draw_body(snake_x[i], snake_y[i], 0, 200, 80);
	}

	draw_head(player_x, player_y, 51, 255, 102);

	char buffer[64];
	sprintf(buffer, "Score : %d", length);
	stringRGBA(screen, 5, 5, buffer, 255, 255, 255, 255);
}

void actions() {
	int i;
	int k;
	int perdu;

	if (action_timer > 0) {
		action_timer = action_timer - 1;
		return;
	}

	action_timer = timer;

	int new_x = player_x + player_dx;
	int new_y = player_y + player_dy;

	if (new_x < 0) {
		new_x = grid_width - 1;
	}
	if (new_x >= grid_width) {
		new_x = 0;
	}
	if (new_y < 0) {
		new_y = grid_height - 1;
	}
	if (new_y >= grid_height) {
		new_y = 0;
	}

	perdu = 0;
	for (i = 0; i < length; i++) {
		if (snake_x[i] == new_x) {
			if (snake_y[i] == new_y) {
				perdu = 1;
			}
		}
	}
	if (perdu == 1) {
		game_active = 0;
		return;
	}

	for (k = length; k > 0; k--) {
		snake_x[k] = snake_x[k - 1];
		snake_y[k] = snake_y[k - 1];
	}
	if (length > 0) {
		snake_x[0] = player_x;
		snake_y[0] = player_y;
	}

	player_x = new_x;
	player_y = new_y;

	if (player_x == apple_x) {
		if (player_y == apple_y) {
			length = length + 1;
			place_apple();
		}
	}
}

int main(int argc, char * argv[]) {

#ifdef _WIN32
	AllocConsole();
	freopen("CON", "w", stdout);
	freopen("CON", "w", stderr);
	freopen("CON", "r", stdin);
#endif

	srand(time(NULL));

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		fprintf(stderr, "Error in SDL_Init : %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	if ((screen = SDL_SetVideoMode(window_width, window_height, 32, SDL_HWSURFACE | SDL_DOUBLEBUF)) == NULL) {
		fprintf(stderr, "Error in SDL_SetVideoMode : %s\n", SDL_GetError());
		SDL_Quit();
		exit(EXIT_FAILURE);
	}

	SDL_WM_SetCaption(title, NULL);

	player_x = grid_width / 2;
	player_y = grid_height / 2;
	player_dx = 0;
	player_dy = 1;

	place_apple();

	SDL_Event event;
	unsigned long last_action = SDL_GetTicks();

	while (game_active) {

		SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
		display();
		SDL_Flip(screen);

		while (SDL_PollEvent(&event)) {

			switch (event.type) {

				case SDL_QUIT : {
					game_active = 0;
				} break;

				case SDL_KEYDOWN : {

					switch (event.key.keysym.sym) {

						case SDLK_ESCAPE : {
							game_active = 0;
						} break;

						case SDLK_UP : {
							if (player_dy != 1) {
								player_dx = 0;
								player_dy = -1;
							}
						} break;

						case SDLK_DOWN : {
							if (player_dy != -1) {
								player_dx = 0;
								player_dy = 1;
							}
						} break;

						case SDLK_RIGHT : {
							if (player_dx != -1) {
								player_dx = 1;
								player_dy = 0;
							}
						} break;

						case SDLK_LEFT : {
							if (player_dx != 1) {
								player_dx = -1;
								player_dy = 0;
							}
						} break;

						default : break;
					}
				} break;

				default : break;
			}
		}

		if (SDL_GetTicks() - last_action > action_delay) {
			last_action = SDL_GetTicks();
		}

		SDL_Delay(1000 / 60);
		actions();
	}

	SDL_FreeSurface(screen);
	SDL_Quit();

	exit(EXIT_SUCCESS);
}
