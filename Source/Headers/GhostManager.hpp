#pragma once

class GhostManager
{
	// Theo dõi đợt (wave) scatter/chase hiện tại.
	unsigned char current_wave;

	// Bộ đếm thời gian (frame logic) để kiểm soát thời lượng của mỗi đợt.
	unsigned short wave_timer;

	std::array<Ghost, 4> ghosts;

public:
	GhostManager();

	void draw(bool i_flash, sf::RenderWindow &i_window);
	void reset(unsigned char i_level, const std::array<Position, 4> &i_ghost_positions);
	void update(unsigned char i_level, std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH> &i_map, Pacman &i_pacman, unsigned int &i_score, sf::Sound& eatGhostSound, sf::Sound& gameOverSound, sf::Sound& eatPelletSound);
};