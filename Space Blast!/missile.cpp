#include "missile.h"

#define PI 3.14159265

Missile::Missile(Texture* texture, SDL_Rect* clip, int col_rad, double pos_x, double pos_y, double ang, double vel)
{
	m_texture = texture;
	m_clip = clip;
	m_col_rad = col_rad;
	m_pos_x = pos_x;
	m_pos_y = pos_y;
	m_ang = ang;
	m_vel = vel;
}

void Missile::move()
{
	m_pos_x += m_vel * cos(m_ang * PI / 180);
	m_pos_y += m_vel * sin(m_ang * PI / 180); 
}

bool Missile::is_off_screen(int screen_width, int screen_height)
{
           // Check if the missile has gone off the left of the screen
	return get_center_x() + m_col_rad < 0 ||
		   // Check if the missile has gone off the right of the screen
		   get_center_x() - m_col_rad >= screen_width ||
		   // Check if the missile has gone off the top of the screen
		   get_center_y() + m_col_rad < 0 ||
		   // Check if the missile has cone off the bottom of the screen
		   get_center_y() - m_col_rad >= screen_height;
}

void Missile::render()
{
	m_texture->render(int(m_pos_x), int(m_pos_y), m_clip, m_ang);
}

double Missile::get_center_x()
{
	return m_pos_x + m_clip->w / 2;
}

double Missile::get_center_y()
{
	return m_pos_y + m_clip->h / 2;
}

int Missile::get_col_rad()
{
	return m_col_rad;
}