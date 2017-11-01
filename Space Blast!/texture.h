#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <SDL.h>
#include <SDL_ttf.h>

// Values for color key (pink)
const Uint8 RED = 0xCD;
const Uint8 GREEN = 0x69;
const Uint8 BLUE = 0xD6;


class Texture
{
	private:
		SDL_Texture* m_texture;
		SDL_Renderer* m_renderer;
		int m_width;
		int m_height;

	public:
		Texture(SDL_Renderer* renderer);
		~Texture();
		bool load_from_file(std::string path);
		bool load_from_rendererd_text(TTF_Font* font, std::string texture_text, SDL_Color text_color);
		void free();
		void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
		int get_width();
		int get_height();
};

#endif