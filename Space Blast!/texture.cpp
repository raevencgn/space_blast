#include "texture.h"
#include <SDL_image.h>

Texture::Texture(SDL_Renderer* renderer)
{
	// Initialize
	m_texture = NULL;
	m_renderer = renderer;
	m_width = 0;
	m_height = 0;
}

Texture::~Texture()
{
	// Deallocate
	free();
}

bool Texture::load_from_file(std::string path)
{
	// Remove the previous texture
	free();

	SDL_Texture* new_texture = NULL;
	SDL_Surface* loaded_surface = IMG_Load(path.c_str());

	if (loaded_surface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		// Color key image
		SDL_SetColorKey(loaded_surface, SDL_TRUE, SDL_MapRGB(loaded_surface->format, RED, GREEN, BLUE));

		// Create texture from surface pixels
		new_texture = SDL_CreateTextureFromSurface(m_renderer, loaded_surface);
		if (new_texture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			// Get image dimensions
			m_width = loaded_surface->w;
			m_height = loaded_surface->h;
		}

		// Get rid of old loaded surface
		SDL_FreeSurface(loaded_surface);
	}

	// Return success
	m_texture = new_texture;
	return m_texture != NULL;
}

bool Texture::load_from_rendererd_text(TTF_Font* font, std::string texture_text, SDL_Color text_color)
{
	// Get rid of preexisting texture
	free();

	// Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, texture_text.c_str(), text_color);
	if (textSurface == NULL)
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else
	{
		// Create texture from surface pixels
		m_texture = SDL_CreateTextureFromSurface(m_renderer, textSurface);
		if (m_texture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			// Get image dimensions
			m_width = textSurface->w;
			m_height = textSurface->h;
		}

		// Get rid of old surface
		SDL_FreeSurface(textSurface);
	}

	// Return success
	return m_texture != NULL;
}

void Texture::free()
{
	// Free texture if it exists
	if (m_texture != NULL)
	{
		SDL_DestroyTexture(m_texture);
		m_texture = NULL;
		m_width = 0;
		m_height = 0;
	}
}

void Texture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	// Set rendering space and render to screen
	SDL_Rect render_quad = { x, y, m_width, m_height };

	//Set clip rendering dimensions
	if (clip != NULL)
	{
		render_quad.w = clip->w;
		render_quad.h = clip->h;
	}

	//Render to screen
	SDL_RenderCopyEx(m_renderer, m_texture, clip, &render_quad, angle + 90, center, flip);
}

int Texture::get_width()
{
	return m_width;
}

int Texture::get_height()
{
	return m_height;
}