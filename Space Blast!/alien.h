#ifndef ALIEN_H
#define ALIEN_H

#include <vector>

#include <SDL.h>
#include "texture.h"

class Alien
{
	private:
		Texture* m_texture;
		std::vector<SDL_Rect> m_clip;
		// Collision radius for the alien
		int m_col_rad;
		// Used for animating the alien's explosion
		int m_frame;
		double m_pos_x;
		double m_pos_y;
		double m_ang;
		double m_vel;
		bool m_alive;
		// Flag to determing if the alien needs cleaned up
		bool m_valid;

	public:
		Alien(Texture* texture, std::vector<SDL_Rect> clip, int col_rad, double pos_x, double pos_y, double ang, double vel);
		void move(double player_center_x, double player_center_y);
		void render();
		void kill();
		bool is_alive();
		bool is_valid();
		double get_center_x();
		double get_center_y();
		int get_col_rad();
};

#endif