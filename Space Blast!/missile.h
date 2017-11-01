#ifndef MISSILE_H
#define MISSILE_H

#include <SDL.h>
#include "texture.h"

class Missile
{
	private:
		Texture* m_texture;
		SDL_Rect* m_clip;
		// Collision radius for the missile
		int m_col_rad;
		double m_pos_x;
		double m_pos_y;
		double m_ang;
		double m_vel;

	public:
		Missile(Texture* texture = NULL, SDL_Rect* clip = NULL, int col_rad = 0, double pos_x = 0, double pos_y = 0, double ang = 0, double vel = 0);
		void move();
		bool is_off_screen(int screen_width, int screen_height);
		void render();
		double get_center_x();
		double get_center_y();
		int get_col_rad();
};

#endif