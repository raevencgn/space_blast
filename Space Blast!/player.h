#ifndef PLAYER_H
#define PLAYER_H

#include <vector>

#include <SDL.h>
#include "texture.h"

class Player
{
	private:
		Texture* m_texture;
		std::vector<SDL_Rect> m_clip;
		// Collision radius for the player
		int m_col_rad;
		// Used for animating the player's explosion
		int m_frame;
		int m_lives;
		bool m_invul;
		int m_invul_ticks;
		double m_pos_x;
		double m_pos_y;
		double m_ang;
		double m_vel_x;
		double m_vel_y;
		double m_vel_ang;
		
	public:
		Player(Texture* texture, std::vector<SDL_Rect> clip, int col_rad = 0, int lives = 3, double pos_x = 0, double pos_y = 0, double ang = 0, double vel_x = 0, double vel_y = 0, double vel_ang = 0);
		void move(int screen_width, int screen_height);
		void render(bool is_thrust);
		void kill();
		bool is_done_exploding();
		void reset(int screen_width, int screen_height);
		void inc_vel_x(double d_vel_x);
		void inc_vel_y(double d_vel_y);
		void inc_vel_ang(double d_vel_ang);
		void inc_invul_ticks();
		void dec_vel_x(double d_vel_x);
		void dec_vel_y(double d_vel_y);
		bool is_invul();
		double get_pos_x();
		double get_pos_y();
		double get_center_x();
		double get_center_y();
		int get_col_rad();
		double get_ang();
		int get_lives();
		double get_vel_x();
		double get_vel_y();
		double get_vel_mag();
		void set_vel_x(double vel_x);
		void set_vel_y(double vel_y);
		void set_vel_ang(double vel_ang);
		void set_ang(double ang);
		void set_invul();
};

#endif