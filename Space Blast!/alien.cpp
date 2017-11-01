#include "alien.h"
#include <math.h>

#define PI 3.14159265

Alien::Alien(Texture* texture, std::vector<SDL_Rect> clip, int col_rad, double pos_x, double pos_y, double ang, double vel)
{
	m_texture = texture;
	m_clip = clip;
	m_col_rad = col_rad;
	m_frame = 1;
	m_pos_x = pos_x;
	m_pos_y = pos_y;
	m_ang = ang;
	m_vel = vel;
	m_alive = true;
	m_valid = true;
}

void Alien::move(double player_center_x, double player_center_y)
{
	double d_center_x;
	double d_center_y;

	// Alien is to the left of the player
	if (get_center_x() + m_vel < player_center_x)
	{
		m_pos_x += m_vel;
	}
	// Alien is to the right of the palyer
	else if (get_center_x() - m_vel > player_center_x)
	{
		m_pos_x -= m_vel;
	}
	// Alien is aligned with the player along the x-axis
	else
	{
		m_pos_x = player_center_x - m_clip[0].w / 2;
	}

	// Alien is below the player
	if (get_center_y() + m_vel < player_center_y)
	{
		m_pos_y += m_vel;
	}
	// Alien is above the player
	else if (get_center_y() - m_vel > player_center_y)
	{
		m_pos_y -= m_vel;
	}
	// Alien is aligned with the player along the y-axis
	else
	{
		m_pos_y = player_center_y - m_clip[0].h / 2;
	}

	// Get the delta's
	d_center_x = player_center_x - get_center_x();
	d_center_y = player_center_y - get_center_y();

	// First quadrant
	if (d_center_x > 0 && d_center_y > 0)
	{
		m_ang = atan(d_center_y / d_center_x) * 180 / PI;
	}
	// Second quadrant
	else if (d_center_x < 0 && d_center_y > 0)
	{
		m_ang = 180 - atan(d_center_y / abs(d_center_x)) * 180 / PI;
	}
	// Third quadrant
	else if (d_center_x < 0 && d_center_y < 0)
	{
		m_ang = 270 - atan(abs(d_center_y) / abs(d_center_x)) * 180 / PI;
	}
	// Fourth quadrant
	else if (d_center_x > 0 && d_center_y < 0)
	{
		m_ang = 360 - atan(abs(d_center_y) / d_center_x) * 180 / PI;
	}
	// HANDLE ANGLE IF ON EDGES OF QUADRANTS
}

void Alien::render()
{
	// Alien is alive
	if (m_alive)
	{
		m_texture->render(int(m_pos_x), int(m_pos_y), &m_clip[0], m_ang);
	}
	// Alien is exploding
	else
	{
		if (m_frame < int(m_clip.size()))
		{
			m_texture->render(int(m_pos_x), int(m_pos_y), &m_clip[m_frame], m_ang);
			m_frame++;
		}
		// Alien has finished exploding, clean up
		else
		{
			m_valid = false;
		}
	}
}

void Alien::kill()
{
	m_alive = false;
}

bool Alien::is_alive()
{
	return m_alive;
}

bool Alien::is_valid()
{
	return m_valid;
}

double Alien::get_center_x()
{
	return m_pos_x + m_clip[0].w / 2;
}

double Alien::get_center_y()
{
	return m_pos_y + m_clip[0].h / 2;
}

int Alien::get_col_rad()
{
	return m_col_rad;
}