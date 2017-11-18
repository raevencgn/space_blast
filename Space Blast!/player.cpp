#include "player.h"
#include <math.h>

#define PI 3.14159265

// PASS IN POSITION AS SDL_Point
Player::Player(Texture* texture, std::vector<SDL_Rect> clip, int col_rad, int lives, double pos_x, double pos_y, double ang, double vel_x, double vel_y, double ang_vel)
{
	m_texture = texture;
	m_clip = clip;
	m_col_rad = col_rad;
	m_frame = 2;
	m_lives = lives;
	m_invul = false;
	m_invul_ticks = 0;
	m_pos_x = pos_x;
	m_pos_y = pos_y;
	m_ang = ang;
	m_vel_x = vel_x;
	m_vel_y = vel_y;
	m_vel_ang = ang_vel;
}

void Player::move(int screen_width, int screen_height)
{
	// Calculate the center of the player
	int center_x = int(m_pos_x) + m_clip[0].w / 2;
	int center_y = int(m_pos_y) + m_clip[0].h / 2;

	// NOTE: Simultaneous collisions will be handled across two calls to 'move()'.
	//       This isn't the most accurate way of doing things, but it's good enough.

	// Check to see if the player has gone over the left edge of the screen
	if (center_x + m_vel_x - m_col_rad < 0)
	{
		center_x = m_col_rad;
		m_pos_x = center_x - m_clip[0].w / 2;
		m_vel_x *= -1;
	}
	// Check to see if the player has gone over the right edge of the screen
	else if (center_x + m_vel_x + m_col_rad >= screen_width)
	{
		center_x = screen_width - m_col_rad;
		m_pos_x = center_x - m_clip[0].w / 2;
		m_vel_x *= -1;
	}
	// Check to see if the player has gone over the top edge of the screen
	else if (center_y + m_vel_y - m_col_rad < 0)
	{
		center_y = m_col_rad;
		m_pos_y = center_y - m_clip[0].h / 2;
		m_vel_y *= -1;
	}
	// Check to see if the player has gone over the bottom edge of the screen
	else if (center_y + m_vel_y + m_col_rad >= screen_height)
	{
		center_y = screen_width - m_col_rad;
		m_pos_y = center_y - m_clip[0].h / 2;
		m_vel_y *= -1;
	}
	// Player remains on the screen
	else
	{
		m_pos_x += m_vel_x;
		m_pos_y += m_vel_y;
	}

	// Adjust angle
	m_ang += m_vel_ang;
}

void Player::render(bool is_thrust)
{
	if (m_lives > 0)
	{
		if (is_thrust)
		{
			m_texture->render(int(m_pos_x), int(m_pos_y), &m_clip[1], m_ang);
		}
		else
		{
			m_texture->render(int(m_pos_x), int(m_pos_y), &m_clip[0], m_ang);
		}
	}
	else
	{
		if (m_frame < int(m_clip.size()))
		{
			m_texture->render(int(m_pos_x), int(m_pos_y), &m_clip[m_frame], m_ang);
			m_frame++;
		}
	}
}

void Player::kill()
{
	if (m_lives > 0)
	{
		m_lives -= 1;
	}
}

bool Player::is_done_exploding()
{
	return m_frame == m_clip.size();
}

void Player::reset(int screen_width, int screen_height)
{
	// Re-init the player's attributes
	m_pos_x = screen_width / 2 - m_clip[0].w / 2;
	m_pos_y = screen_height / 2 - m_clip[0].h / 2;
	m_frame = 1;
	m_lives = 3;
	m_ang = -90;
	m_vel_x = 0;
	m_vel_y = 0;
	m_vel_ang = 0;
	m_invul = false;
	m_invul_ticks = 0;
}

void Player::inc_vel_x(double d_vel_x)
{
	m_vel_x += d_vel_x;
}

void Player::inc_vel_y(double d_vel_y)
{
	m_vel_y += d_vel_y;
}

void Player::inc_vel_ang(double d_vel_ang)
{
	m_vel_ang += d_vel_ang;
}

void Player::inc_invul_ticks()
{
	if (m_invul_ticks < 50)
	{
		m_invul_ticks++;
	}
	else
	{
		m_invul_ticks = 0;
		m_invul = false;
	}
}

void Player::dec_vel_x(double d_vel_x)
{
	if (m_vel_x != 0)
	{
		int sign = m_vel_x > 0 ? 1 : -1;

		m_vel_x = abs(m_vel_x) - abs(d_vel_x) >= 0 ? m_vel_x - sign * abs(d_vel_x) : 0;
	}
}

void Player::dec_vel_y(double d_vel_y)
{
	if (m_vel_y != 0)
	{
		int sign = m_vel_y > 0 ? 1 : -1;

		m_vel_y = abs(m_vel_y) - abs(d_vel_y) >= 0 ? m_vel_y - sign * abs(d_vel_y) : 0;
	}
}

bool Player::is_invul()
{
	return m_invul;
}

double Player::get_pos_x()
{
	return m_pos_x;
}

double Player::get_pos_y()
{
	return m_pos_y;
}

double Player::get_center_x()
{
	return m_pos_x + m_clip[0].w / 2;
}

double Player::get_center_y()
{
	return m_pos_y + m_clip[0].h / 2;
}

int Player::get_col_rad()
{
	return m_col_rad;
}

double Player::get_ang()
{
	return m_ang;
}

double Player::get_vel_x()
{
	return m_vel_x;
}

double Player::get_vel_y()
{
	return m_vel_y;
}

double Player::get_vel_mag()
{
	return sqrt(pow(m_vel_x, 2) + pow(m_vel_y, 2));
}

void Player::set_ang(double ang)
{
	m_ang = ang;
}

void Player::set_vel_x(double vel_x)
{
	m_vel_x = vel_x;
}

void Player::set_vel_y(double vel_y)
{
	m_vel_y = vel_y;
}

void Player::set_vel_ang(double vel_ang)
{
	m_vel_ang = vel_ang;
}

int Player::get_lives()
{
	return m_lives;
}

void Player::set_invul()
{
	m_invul = true;
}