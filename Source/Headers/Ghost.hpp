// Headers/Ghost.hpp
#pragma once


// Ghost "KẾ THỪA" từ Entity
class Ghost : public Entity
{
	// Chế độ di chuyển hiện tại: 0 = Scatter (phân tán), 1 = Chase (săn đuổi).
	bool movement_mode;
	// Cờ (flag) cho biết ma có thể đi xuyên qua cửa nhà ma hay không.
	bool use_door;
	// Trạng thái sợ hãi: 0 = Bình thường, 1 = Sợ hãi (màu xanh), 2 = Bị ăn (chỉ còn mắt).
	unsigned char frightened_mode;
	// Bộ đếm thời gian để điều chỉnh tốc độ khi ma đang ở trạng thái sợ hãi.
	unsigned char frightened_speed_timer;
	// ID của ma (0: Blinky, 1: Pinky, 2: Inky, 3: Clyde).
	unsigned char id;

	// Vị trí "nhà" của ma (bên trong lồng).
	Position home;
	// Vị trí ngay bên ngoài cửa nhà ma.
	Position home_exit;
	// Vị trí mục tiêu (pixel) mà ma đang hướng tới.
	Position target;

public:
	Ghost(unsigned char i_id);

	bool pacman_collision(const Position &i_pacman_position);

	float get_target_distance(unsigned char i_direction);

	void draw(bool i_flash, sf::RenderWindow &i_window);

	// Hàm reset() CỤ THỂ của Ghost
	void reset(const Position &i_home, const Position &i_home_exit);

	
	virtual void reset() override;

	void switch_mode();
	void update(unsigned char i_level, std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH> &i_map, Ghost &i_ghost_0, Pacman &i_pacman, unsigned int &i_score, sf::Sound& eatGhostSound, sf::Sound& gameOverSound, sf::Sound& eatPelletSound);
	void update_target(unsigned char i_pacman_direction, const Position &i_ghost_0_position, const Position &i_pacman_position);
};