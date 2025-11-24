// Headers/Pacman.hpp
#pragma once

// Pacman "KẾ THỪA" từ Entity
class Pacman : public Entity
{
	// Cờ (flag) báo hiệu hoạt ảnh (ví dụ: chết) đã kết thúc.
	bool animation_over;
	// Cờ (flag) cho biết Pacman có đang trong trạng thái chết (đã bị bắt) hay không.
	bool dead;
	unsigned short energizer_timer;

public:
	Pacman();

	bool get_animation_over();
	bool get_dead();
	unsigned short get_energizer_timer();
	void draw(bool i_victory, sf::RenderWindow &i_window);

	// 'virtual' và 'override' báo cho C++ biết rằng chúng ta đang định nghĩa lại hàm 'reset' từ lớp cha 'Entity'
	virtual void reset() override;

	void set_dead(bool i_dead);
	void update(unsigned char i_level, std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH> &i_map, unsigned int &i_score, sf::Sound& eatPelletSound);
};