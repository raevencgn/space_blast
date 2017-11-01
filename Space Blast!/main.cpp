#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <string>

#include <SDL.h>
#include <SDL_image.h>

#include "texture.h"
#include "player.h"
#include "alien.h"
#include "missile.h"

#define PI 3.14159265

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 600;
const int SPAWN_RATE = 50;

enum game_state
{
	RUNNING,
	PAUSED,
	GAME_OVER
};

bool init(SDL_Renderer** renderer, SDL_Window* window)
{
	// Initialization flag
	bool success = true;

	// Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		// Create window
		window = SDL_CreateWindow("Space Blast!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (window == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			// Create renderer for window
			*renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (renderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				// Initialize renderer color
				SDL_SetRenderDrawColor(*renderer, 0xFF, 0xFF, 0xFF, 0xFF);

				// Initialize PNG loading
				int img_flags = IMG_INIT_PNG;
				if (!(IMG_Init(img_flags) & img_flags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}

				// Initialize SDL_ttf
				if (TTF_Init() == -1)
				{
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}

				// Set blend mode
				// PROBABLY NOT NEEDED
				if (SDL_SetRenderDrawBlendMode(*renderer, SDL_BLENDMODE_BLEND))
				{
					printf("Could not set blend mode! SDL Error: %s\n", SDL_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

void close(SDL_Renderer* renderer, SDL_Window* window)
{
	// Destroy window	
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	window = NULL;
	renderer = NULL;

	// DESTROY SPRITE SHEET
	// DESTROY FONT
	// DESTROY THE CHUNK OF MEMORY FOR THE OLD KEY STATE

	// Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

bool is_collision(int a_center_x, int a_center_y, int a_col_rad, int b_center_x, int b_center_y, int b_col_rad)
{
	return (a_center_x - b_center_x) * (a_center_x - b_center_x) + (a_center_y - b_center_y) * (a_center_y - b_center_y) <
		   (a_col_rad + b_col_rad) * (a_col_rad + b_col_rad);
}

// Generates a random number between min, max inclusive
int gen_rand(int min, int max)
{
	static bool is_seeded = false;
	
	if (!is_seeded)
	{
		srand(unsigned int(time(NULL)));
		is_seeded = true;
	}

	return rand() % (max - min + 1) + min;
}

bool key_released(const Uint8* old_key_state, const Uint8* new_key_state, Uint8 key)
{
	return old_key_state[key] == 1 && new_key_state[key] == 0;
}

// Check that it's the first time a key has been pressed
bool no_repeat(Uint8* old_key_state, const Uint8* new_key_state, Uint8 key)
{
	return old_key_state[key] == 0 && new_key_state[key] == 1;
}

int main(int argc, char** argv)
{
	SDL_Renderer* renderer = NULL;
	SDL_Window* window = NULL;

	// Start up SDL and create window
	if (!init(&renderer, window))
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		// Main loop flag
		bool quit = false;

		// Event handler
		SDL_Event e;

		std::vector<Missile*> missiles;
		std::vector<Alien*> aliens;
		game_state state = RUNNING;
		std::string score_string;
		int num_keys;
		const Uint8* key_state;
		Uint8* old_key_state = NULL;
		int score = 0;
		int spawn_rate = SPAWN_RATE;
		bool is_thrust = false;

		// Load assets
		// I NEED TO DO ERROR CHECKING FOR ASSETS
		Texture sprites(renderer);
		Texture text(renderer);
		sprites.load_from_file("sprites.png");

		TTF_Font* font_12 = TTF_OpenFont("font.ttf", 12);
		TTF_Font* font_24 = TTF_OpenFont("font.ttf", 24);

		std::vector<SDL_Rect> clip_player;
		// The player sprite
		clip_player.push_back({ 160, 9, 15, 16 });
		// The player sprite with thrust
		clip_player.push_back({ 184, 9, 15, 16 });
		// The player exploding
		clip_player.push_back({ 210, 3, 29, 28 });
		clip_player.push_back({ 249, 3, 29, 30 });
		clip_player.push_back({ 288, 1, 32, 32 });
		clip_player.push_back({ 329, 3, 29, 30 });
		
		std::vector<SDL_Rect> clip_alien;
		// The alien sprite
		clip_alien.push_back({ 162, 108, 13, 10 });
		// The alien exploding
		clip_alien.push_back({ 211, 156, 7, 8 });
		clip_alien.push_back({ 234, 154, 12, 13 });
		clip_alien.push_back({ 256, 153, 16, 16 });
		clip_alien.push_back({ 283, 147, 27, 28 });
		clip_alien.push_back({ 321, 145, 31, 32 });

		SDL_Rect clip_missile = { 374, 5, 3, 8 };

		Player player(&sprites, clip_player, 12, 3, SCREEN_WIDTH / 2 - clip_player[0].w / 2, SCREEN_HEIGHT / 2 - clip_player[0].h / 2, -90);

		// While application is running
		while (!quit)
		{
			if (state == RUNNING)
			{
				// Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					// User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}
				}

				// Handle keyboard input
				key_state = SDL_GetKeyboardState(&num_keys);
				
				// Rotate counter-clockwise
				if (key_state[SDL_SCANCODE_A])
				{
					player.set_vel_ang(-5);
				}

				// Rotate clockwise
				if (key_state[SDL_SCANCODE_D])
				{
					player.set_vel_ang(5);
				}

				// Remove rotation once key is released
				if (old_key_state && (key_released(old_key_state, key_state, SDL_SCANCODE_A) || key_released(old_key_state, key_state, SDL_SCANCODE_D)))
				{
					player.set_vel_ang(0);
				}

				// Apply thrust
				if (key_state[SDL_SCANCODE_W])
				{
					if (player.get_vel_mag() < 5)
					{
						player.inc_vel_x(.1 * cos(player.get_ang() * PI / 180));
						player.inc_vel_y(.1 * sin(player.get_ang() * PI / 180));
					}

					is_thrust = true;
				}

				// Remove thrust when key is released
				if (!key_state[SDL_SCANCODE_W])
				{
					player.dec_vel_x(.03);
					player.dec_vel_y(.03);

					is_thrust = false;
				}

				// Fire missiles
				if (key_state[SDL_SCANCODE_SPACE])
				{
					missiles.push_back(new Missile(&sprites, &clip_missile, 4, player.get_center_x() - clip_missile.w / 2, player.get_center_y() - clip_missile.h / 2, player.get_ang(), 5));
				}

				// Pause
				if (key_state[SDL_SCANCODE_ESCAPE] && no_repeat(old_key_state, key_state, SDL_SCANCODE_ESCAPE))
				{
					state = PAUSED;
				}
				
				// Set aside a chunk of memory for the old key states
				if (!old_key_state)
				{
					old_key_state = (Uint8*)malloc(sizeof(Uint8) * num_keys);
				}

				// Copy over the old key states
				for (int i = 0; i < num_keys; i++)
				{
					old_key_state[i] = key_state[i];
				}

				// Create new aliens at random
				if (gen_rand(0, spawn_rate) == spawn_rate)
				{
					int placement = gen_rand(0, 3);

					// Generate an alien at the left of the screen
					if (placement == 0)
					{
						aliens.push_back(new Alien(&sprites, clip_alien, 7, 0 - clip_alien[0].w, gen_rand(0, SCREEN_HEIGHT - clip_alien[0].h), 0, 1));
					}
					// Generate an alien at the right of the screen
					else if (placement == 1)
					{
						aliens.push_back(new Alien(&sprites, clip_alien, 7, SCREEN_WIDTH, gen_rand(0, SCREEN_HEIGHT - clip_alien[0].h), 0, 1));
					}
					// Generate an alien at the top of the screen
					else if (placement == 2)
					{
						aliens.push_back(new Alien(&sprites, clip_alien, 7, gen_rand(0, SCREEN_WIDTH - clip_alien[0].w), 0 - clip_alien[0].h, 0, 1));
					}
					// Generate an alien at the bottom of the screen
					else
					{
						aliens.push_back(new Alien(&sprites, clip_alien, 7, gen_rand(0, SCREEN_WIDTH - clip_alien[0].w), SCREEN_HEIGHT, 0, 1));
					}

					// Increase the spawn rate
					if (spawn_rate >= 2)
					{
						spawn_rate -= 1;
					}
				}

				// Check to see if any missiles have collided with any aliens
				if (missiles.size() > 0 && aliens.size() > 0)
				{
					for (int i = 0; i < int(missiles.size()); i++)
					{
						for (int j = 0; j < int(aliens.size()); j++)
						{
							if (is_collision(int(aliens[j]->get_center_x()), int(aliens[j]->get_center_y()), aliens[j]->get_col_rad(), int(missiles[i]->get_center_x()), int(missiles[i]->get_center_y()), missiles[i]->get_col_rad()))
							{
								aliens[j]->kill();
							}
						}
					}
				}

				// Check to see if the player has collided with any aliens
				if (aliens.size() > 0)
				{
					for (int i = 0; i < int(aliens.size()); i++)
					{
						if (is_collision(int(aliens[i]->get_center_x()), int(aliens[i]->get_center_y()), aliens[i]->get_col_rad(), int(player.get_center_x()), int(player.get_center_y()), player.get_col_rad()) && aliens[i]->is_alive())
						{
							player.kill();
							aliens[i]->kill();
						}
					}
				}

				// Move game objects
				player.move(SCREEN_WIDTH, SCREEN_HEIGHT);

				if (aliens.size() > 0)
				{
					for (int i = 0; i < int(aliens.size()); i++)
					{
						aliens[i]->move(player.get_center_x(), player.get_center_y());
					}
				}

				if (missiles.size() > 0)
				{
					for (int i = 0; i < int(missiles.size()); i++)
					{
						missiles[i]->move();
					}
				}

				// Clear screen
				SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
				SDL_RenderClear(renderer);

				// Render game objects
				score_string = "Score: " + std::to_string(score);
				SDL_Color white = { 0xFF, 0xFF, 0xFF, 0xFF };
				text.load_from_rendererd_text(font_12, score_string, white);
				text.render(10, 10, NULL, -90);

				// Render the player's lives
				for (int i = 0; i <= player.get_lives(); i++)
				{
					sprites.render(SCREEN_WIDTH - clip_player[0].w * i - 10 * i, 10, &clip_player[0], -90);
				}

				// Render missiles
				if (missiles.size() > 0)
				{
					for (int i = 0; i < int(missiles.size()); i++)
					{
						missiles[i]->render();
					}
				}

				// Render aliens
				if (aliens.size() > 0)
				{
					for (int i = 0; i < int(aliens.size()); i++)
					{
						aliens[i]->render();
					}
				}

				player.render(is_thrust);

				// Update screen
				SDL_RenderPresent(renderer);

				// Clean up any missiles that have gone off screen
				if (missiles.size() > 0)
				{
					for (int i = 0; i < int(missiles.size()); i++)
					{
						if (missiles[i]->is_off_screen(SCREEN_WIDTH, SCREEN_HEIGHT))
						{
							free(missiles[i]);
							missiles.erase(missiles.begin() + i);
						}
					}
				}

				// Clean up dead aliens
				if (aliens.size() > 0)
				{
					for (int i = 0; i < int(aliens.size()); i++)
					{
						if (!aliens[i]->is_valid())
						{
							free(aliens[i]);
							aliens.erase(aliens.begin() + i);
							score += 100;
						}
					}
				}

				// See if the player has died and has finished exploding
				if (player.get_lives() == 0 && player.is_done_exploding())
				{
					state = GAME_OVER;
				}
			}
			else if (state == PAUSED)
			{
				// Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					// User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}
				}
				
				key_state = SDL_GetKeyboardState(&num_keys);

				// Un-pause
				if (key_state[SDL_SCANCODE_ESCAPE] && no_repeat(old_key_state, key_state, SDL_SCANCODE_ESCAPE))
				{
					state = RUNNING;
				}

				// Copy over the old key states
				for (int i = 0; i < num_keys; i++)
				{
					old_key_state[i] = key_state[i];
				}

				// Pause indicator text
				SDL_Color white = {0xFF, 0xFF, 0xFF, 0xFF};
				text.load_from_rendererd_text(font_24, "PAUSED", white);
				text.render(SCREEN_WIDTH / 2 - text.get_width() / 2, SCREEN_HEIGHT / 2 - text.get_height() / 2, NULL, -90);
				SDL_RenderPresent(renderer);
			}
			else if (state == GAME_OVER)
			{
				// Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					// User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}
				}

				key_state = SDL_GetKeyboardState(&num_keys);

				// Restart
				if (key_state[SDL_SCANCODE_R])
				{
					// Reset score
					score = 0;

					// Reset spawn rate
					spawn_rate = SPAWN_RATE;

					// Restore player's lives
					player.reset(SCREEN_WIDTH, SCREEN_HEIGHT);

					// Delete missiles
					if (missiles.size() > 0)
					{
						for (int i = 0; i < int(missiles.size()); i++)
						{
							free(missiles[i]);
						}

						missiles.clear();
					}

					// Delete aliens
					if (aliens.size() > 0)
					{
						for (int i = 0; i < int(aliens.size()); i++)
						{
							free(aliens[i]);
						}

						aliens.clear();
					}

					// Go back to running state
					state = RUNNING;
				}

				// Copy over the old key states
				for (int i = 0; i < num_keys; i++)
				{
					old_key_state[i] = key_state[i];
				}

				// Game over indicator text and restart prompt
				SDL_Color white = { 0xFF, 0xFF, 0xFF, 0xFF };
				text.load_from_rendererd_text(font_24, "GAME OVER", white);
				int old_text_height = text.get_height();
				text.render(SCREEN_WIDTH / 2 - text.get_width() / 2, SCREEN_HEIGHT / 2 - text.get_height() / 2, NULL, -90);
				text.load_from_rendererd_text(font_12, "Press 'R' to Restart", white);
				text.render(SCREEN_WIDTH / 2 - text.get_width() / 2, SCREEN_HEIGHT / 2 + old_text_height / 2, NULL, -90);
				
				SDL_RenderPresent(renderer);
			}
		}
	}

	// Free resources and close SDL
	close(renderer, window);

	return 0;
}