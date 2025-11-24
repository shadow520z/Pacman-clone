#include <array>
#include <chrono>
#include <ctime>
#include <SFML/Graphics.hpp>
#include <optional>
#include <string>
#include <cmath>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <SFML/Audio.hpp>
#include "Headers/Global.hpp"
#include "Headers/Entity.hpp"
#include "Headers/Pacman.hpp"
#include "Headers/Ghost.hpp"
#include "Headers/GhostManager.hpp"
#include "Headers/ConvertSketch.hpp"
#include "Headers/DrawMap.hpp"
#include "Headers/MapCollision.hpp"

using namespace sf;
using namespace std;

Pacman::Pacman() : animation_over(0),
				   dead(0),
				   energizer_timer(0),
				   Entity()
{
	// Hàm khởi tạo (constructor) cho lớp Pacman, thiết lập các giá trị mặc định.
}

bool Pacman::get_animation_over()
{
	return animation_over;
}

bool Pacman::get_dead()
{
	return dead;
}

unsigned short Pacman::get_energizer_timer()
{
	return energizer_timer;
}

void Pacman::draw(bool i_victory, RenderWindow &i_window)
{
	unsigned char frame = static_cast<unsigned char>(floor(animation_timer / static_cast<float>(PACMAN_ANIMATION_SPEED)));
	Texture texture;
	Sprite sprite(texture);

	sprite.setPosition({static_cast<float>(position.x), static_cast<float>(position.y)});
	// Nếu Pacman chết HOẶC chiến thắng, chạy hoạt ảnh chết/biến mất.
	if (1 == dead || 1 == i_victory)
	{
		if (animation_timer < PACMAN_DEATH_FRAMES * PACMAN_ANIMATION_SPEED)
		{
			animation_timer++;
			if (!texture.loadFromFile("Resources/Images/PacmanDeath16.png"))
			{
				cerr << "Error!" << endl;
			}

			sprite.setTexture(texture);
			sprite.setTextureRect(IntRect({CELL_SIZE * frame, 0}, {CELL_SIZE, CELL_SIZE}));

			i_window.draw(sprite);
		}
		else
		{
			animation_over = 1;
		}
	}
	// Nếu Pacman còn sống và game chưa thắng.
	else
	{
		if (!texture.loadFromFile("Resources/Images/Pacman16.png"))
		{
			cerr << "Error!" << endl;
		}
		sprite.setTexture(texture);
		sprite.setTextureRect(IntRect({CELL_SIZE * frame, CELL_SIZE * direction}, {CELL_SIZE, CELL_SIZE}));
		i_window.draw(sprite);
		animation_timer = (1 + animation_timer) % (PACMAN_ANIMATION_FRAMES * PACMAN_ANIMATION_SPEED);
	}
}

void Pacman::reset()
{
	animation_over = 0;
	dead = 0;
	direction = 0;
	animation_timer = 0;
	energizer_timer = 0;
}

void Pacman::set_dead(bool i_dead)
{
	dead = i_dead;
	if (1 == dead)
	{
		animation_timer = 0;
	}
}

Ghost::Ghost(unsigned char i_id) : id(i_id),
								   Entity()
{
	// Hàm khởi tạo cho lớp Ghost, gán ID cho con ma (0: Blinky, 1: Pinky, 2: Inky, 3: Clyde).
}

void Ghost::draw(bool i_flash, RenderWindow &i_window)
{
	unsigned char body_frame = static_cast<unsigned char>(floor(animation_timer / static_cast<float>(GHOST_ANIMATION_SPEED)));
	Texture texture;
	Sprite body(texture);
	Sprite face(texture);
	if (!texture.loadFromFile("Resources/Images/Ghost16.png"))
	{
		cerr << "Error!" << endl;
	}

	body.setTexture(texture);
	body.setPosition({static_cast<float>(position.x), static_cast<float>(position.y)});
	body.setTextureRect(IntRect({CELL_SIZE * body_frame, 0}, {CELL_SIZE, CELL_SIZE}));

	face.setTexture(texture);
	face.setPosition({static_cast<float>(position.x), static_cast<float>(position.y)});
	// Chế độ 0: Ma ở trạng thái bình thường (Chase/Scatter).
	if (0 == frightened_mode)
	{
		// Tô màu cho từng con ma dựa trên ID của nó.
		switch (id)
		{
		case 0:
		{
			// Màu đỏ
			body.setColor(Color(255, 0, 0));

			break;
		}
		case 1:
		{
			// Màu hồng
			body.setColor(Color(255, 182, 255));

			break;
		}
		case 2:
		{
			// Màu xanh
			body.setColor(Color(0, 255, 255));

			break;
		}
		case 3:
		{
			// Màu cam
			body.setColor(Color(255, 182, 85));
		}
		}

		face.setTextureRect(IntRect({CELL_SIZE * direction, CELL_SIZE}, {CELL_SIZE, CELL_SIZE}));

		i_window.draw(body);
	}
	// Chế độ 1: Ma ở trạng thái sợ hãi (Frightened).
	else if (1 == frightened_mode)
	{
		body.setColor(Color(36, 36, 255));
		// Khuôn mặt "sợ hãi" luôn giống nhau bất kể hướng đi.
		face.setTextureRect(IntRect({4 * CELL_SIZE, CELL_SIZE}, {CELL_SIZE, CELL_SIZE}));

		// Xử lý nhấp nháy khi sắp hết thời gian sợ hãi.
		if (1 == i_flash && 0 == body_frame % 2)
		{
			body.setColor(Color(255, 255, 255));
			face.setColor(Color(255, 0, 0));
		}
		else
		{
			body.setColor(Color(36, 36, 255));
			face.setColor(Color(255, 255, 255));
		}

		i_window.draw(body);
	}
	// Chế độ 2: Ma đã bị ăn (chỉ còn mắt).
	else
	{
		// Chỉ vẽ mắt, ma đang quay về nhà.
		face.setTextureRect(IntRect({CELL_SIZE * direction, 2 * CELL_SIZE}, {CELL_SIZE, CELL_SIZE}));
	}

	i_window.draw(face);

	// Quay vòng bộ đếm thời gian hoạt ảnh để lặp lại (loop) animation.
	animation_timer = (1 + animation_timer) % (GHOST_ANIMATION_FRAMES * GHOST_ANIMATION_SPEED);
}

void Ghost::reset(const Position &i_home, const Position &i_home_exit)
{
	movement_mode = 0; // Bắt đầu ở chế độ Scatter.

	use_door = 0 < id;

	direction = 0;
	frightened_mode = 0;
	frightened_speed_timer = 0;

	animation_timer = 0;

	home = i_home;
	home_exit = i_home_exit;
	target = i_home_exit;
}
void Ghost::reset()
{
	reset({0, 0}, {0, 0});
}

GhostManager::GhostManager() : current_wave(0),
							   wave_timer(LONG_SCATTER_DURATION),
							   ghosts({Ghost(0), Ghost(1), Ghost(2), Ghost(3)})
{
	// Khởi tạo 4 con ma với ID tương ứng.
}

void GhostManager::draw(bool i_flash, RenderWindow &i_window)
{
	for (Ghost &ghost : ghosts)
	{
		ghost.draw(i_flash, i_window);
	}
}

void GhostManager::reset(unsigned char i_level, const array<Position, 4> &i_ghost_positions)
{
	current_wave = 0;

	// Tăng độ khó: Thời gian scatter ngắn lại khi cấp độ tăng.
	wave_timer = static_cast<unsigned short>(LONG_SCATTER_DURATION / pow(2, i_level));

	for (unsigned char a = 0; a < 4; a++)
	{
		ghosts[a].set_position(i_ghost_positions[a].x, i_ghost_positions[a].y);
	}

	for (Ghost &ghost : ghosts)
	{

		ghost.reset(ghosts[2].get_position(), ghosts[0].get_position());
	}
}

// Chuyển đổi bản đồ từ ký tự sang mảng 2D các ô (cells).
array<array<Cell, MAP_HEIGHT>, MAP_WIDTH> convert_sketch(const array<string, MAP_HEIGHT> &i_map_sketch, array<Position, 4> &i_ghost_positions, Pacman &i_pacman)
{
	array<array<Cell, MAP_HEIGHT>, MAP_WIDTH> output_map{};

	for (unsigned char a = 0; a < MAP_HEIGHT; a++)
	{
		for (unsigned char b = 0; b < MAP_WIDTH; b++)
		{
			output_map[b][a] = Cell::Empty;
			switch (i_map_sketch[a][b])
			{
			case '#': // bức tường
			{
				output_map[b][a] = Cell::Wall;

				break;
			}
			case '=': // cửa nhà ma
			{
				output_map[b][a] = Cell::Door;

				break;
			}
			case '.': // pellets
			{
				output_map[b][a] = Cell::Pellet;

				break;
			}

			case '0': // Ma Đỏ
			{
				i_ghost_positions[0].x = CELL_SIZE * b;
				i_ghost_positions[0].y = CELL_SIZE * a;

				break;
			}
			case '1': // Ma Hồng
			{
				i_ghost_positions[1].x = CELL_SIZE * b;
				i_ghost_positions[1].y = CELL_SIZE * a;
				break;
			}

			case '2': // Ma Xanh
			{
				i_ghost_positions[2].x = CELL_SIZE * b;
				i_ghost_positions[2].y = CELL_SIZE * a;

				break;
			}

			case '3': // Ma Cam
			{
				i_ghost_positions[3].x = CELL_SIZE * b;
				i_ghost_positions[3].y = CELL_SIZE * a;

				break;
			}
			case 'P': // Pacman
			{
				i_pacman.set_position(CELL_SIZE * b, CELL_SIZE * a);

				break;
			}
			case 'o': // hạt năng lượng
			{
				output_map[b][a] = Cell::Energizer;
			}
			}
		}
	}

	return output_map;
}

// Hàm vẽ bản đồ dựa trên mảng 2D các ô (cells).
void draw_map(const array<array<Cell, MAP_HEIGHT>, MAP_WIDTH> &i_map, RenderWindow &i_window)
{

	Texture texture;

	if (!texture.loadFromFile("Resources/Images/Map16.png"))
	{
		cerr << "Error!" << endl;
	}
	Sprite sprite(texture);

	for (unsigned char a = 0; a < MAP_WIDTH; a++)
	{
		for (unsigned char b = 0; b < MAP_HEIGHT; b++)
		{
			sprite.setPosition({static_cast<float>(CELL_SIZE * a), static_cast<float>(CELL_SIZE * b)});

			// Cắt (crop) phần texture tương ứng với loại ô (cell) cần vẽ.
			switch (i_map[a][b])
			{
			case Cell::Door:
			{
				sprite.setTextureRect(IntRect({2 * CELL_SIZE, CELL_SIZE}, {CELL_SIZE, CELL_SIZE}));

				i_window.draw(sprite);

				break;
			}
			case Cell::Energizer:
			{
				sprite.setTextureRect(IntRect({CELL_SIZE, CELL_SIZE}, {CELL_SIZE, CELL_SIZE}));

				i_window.draw(sprite);

				break;
			}
			case Cell::Pellet:
			{
				sprite.setTextureRect(IntRect({0, CELL_SIZE}, {CELL_SIZE, CELL_SIZE}));

				i_window.draw(sprite);

				break;
			}
			case Cell::Wall:
			{
				// Kiểm tra các ô lân cận để xác định loại tường (nối liền) cần vẽ.
				bool down = 0;
				bool left = 0;
				bool right = 0;
				bool up = 0;
				if (b < MAP_HEIGHT - 1)
				{
					if (Cell::Wall == i_map[a][1 + b])
					{
						down = 1;
					}
				}

				// Xử lý đường hầm (warp tunnels) để vẽ tường như thể chúng được kết nối.
				if (0 < a)
				{
					if (Cell::Wall == i_map[a - 1][b])
					{
						left = 1;
					}
				}

				if (a < MAP_WIDTH - 1)
				{
					if (Cell::Wall == i_map[1 + a][b])
					{
						right = 1;
					}
				}

				if (0 < b)
				{
					if (Cell::Wall == i_map[a][b - 1])
					{
						up = 1;
					}
				}
				// up=8, right=4, left=2, down=1 để chọn 1 trong 16 ô sprite tường.
				sprite.setTextureRect(IntRect({CELL_SIZE * (down + 2 * (left + 2 * (right + 2 * up))), 0}, {CELL_SIZE, CELL_SIZE}));

				i_window.draw(sprite);
			}
			}
		}
	}
}

// Hàm vẽ văn bản sử dụng một texture font
void draw_text(
	const Texture &fontTexture,
	RenderWindow &window,
	const string &text,
	unsigned short x,
	unsigned short y,
	bool center)
{

	int charWidth = fontTexture.getSize().x / CHARS_IN_FONT;
	Sprite characterSprite(fontTexture);
	// Tách chuỗi đầu vào thành các dòng riêng biệt dựa trên ký tự '\n'
	vector<string> lines;
	stringstream ss(text);
	string line;
	while (getline(ss, line, '\n'))
	{
		lines.push_back(line);
	}

	float startY = y;
	if (center)
	{
		float totalHeight = lines.size() * FONT_HEIGHT;
		startY = (CELL_SIZE * MAP_HEIGHT - totalHeight) / 2.0f;
	}
	float currentY = startY;
	for (const auto &currentLine : lines)
	{
		float startX = x;
		if (center)
		{
			float lineWidth = currentLine.length() * charWidth;
			startX = (CELL_SIZE * MAP_WIDTH - lineWidth) / 2.0f;
		}

		float currentX = startX;
		for (char character : currentLine)
		{
			if (character < 32)
				continue;
			characterSprite.setTextureRect(IntRect(
				{static_cast<int>(charWidth * (character - ASCII_OFFSET)), 0},
				{charWidth, FONT_HEIGHT}));
			characterSprite.setPosition({currentX, currentY});
			window.draw(characterSprite);
			currentX += charWidth;
		}
		currentY += FONT_HEIGHT;
	}
}
// tải điểm từ file
void load_high_scores(vector<unsigned int> &scores, const string &filename)
{
	scores.clear();
	ifstream file(filename);
	if (!file.is_open())
	{
		return;
	}
	unsigned int score;
	while (file >> score)
	{
		scores.push_back(score);
	}
	file.close();
	sort(scores.rbegin(), scores.rend());
}
// Lưu điểm vào file
void save_high_scores(const vector<unsigned int> &scores, const string &filename)
{
	ofstream file(filename);
	if (!file.is_open())
	{
		cerr << "Cannot save high scores!" << endl;
		return;
	}

	for (const auto &score : scores)
	{
		file << score << endl;
	}
	file.close();
}
// Thêm, sắp xếp, giới hạn điểm (5 điểm cao nhất)
void add_score(vector<unsigned int> &scores, unsigned int new_score)
{
	if (new_score == 0)
		return;
	scores.push_back(new_score);
	sort(scores.rbegin(), scores.rend());
	if (scores.size() > 5)
	{
		scores.resize(5);
	}
}

// Hàm kiểm tra va chạm với bản đồ.
bool map_collision(bool i_collect_pellets, bool i_use_door, short i_x, short i_y, array<array<Cell, MAP_HEIGHT>, MAP_WIDTH> &i_map, unsigned int &i_score, Sound &eatPelletSound)
{
	bool output = 0;
	float cell_x = i_x / static_cast<float>(CELL_SIZE);
	float cell_y = i_y / static_cast<float>(CELL_SIZE);
	for (unsigned char a = 0; a < 4; a++)
	{
		short x = 0;
		short y = 0;
		// Kiểm tra 4 góc của bounding box của đối tượng.
		switch (a)
		{
		case 0: // Ô trên cùng bên trái
		{
			x = static_cast<short>(floor(cell_x));
			y = static_cast<short>(floor(cell_y));

			break;
		}
		case 1: // Ô trên cùng bên phải
		{
			x = static_cast<short>(ceil(cell_x));
			y = static_cast<short>(floor(cell_y));

			break;
		}
		case 2: // Ô dưới cùng bên trái
		{
			x = static_cast<short>(floor(cell_x));
			y = static_cast<short>(ceil(cell_y));

			break;
		}
		case 3: // Ô dưới cùng bên phải
		{
			x = static_cast<short>(ceil(cell_x));
			y = static_cast<short>(ceil(cell_y));
		}
		}

		if (0 <= x && 0 <= y && MAP_HEIGHT > y && MAP_WIDTH > x)
		{
			if (0 == i_collect_pellets)
			{
				if (Cell::Wall == i_map[x][y])
				{
					output = 1;
				}
				else if (0 == i_use_door && Cell::Door == i_map[x][y])
				{
					output = 1;
				}
			}
			else
			{
				if (Cell::Energizer == i_map[x][y])
				{
					output = 1;
					i_score += ENERGIZER_SCORE;
					i_map[x][y] = Cell::Empty;
				}
				else if (Cell::Pellet == i_map[x][y])
				{
					eatPelletSound.play();
					i_score += PELLET_SCORE;
					i_map[x][y] = Cell::Empty;
				}
			}
		}
	}

	return output;
}

bool Ghost::pacman_collision(const Position &i_pacman_position)
{
	if (position.x > i_pacman_position.x - CELL_SIZE && position.x < CELL_SIZE + i_pacman_position.x)
	{
		if (position.y > i_pacman_position.y - CELL_SIZE && position.y < CELL_SIZE + i_pacman_position.y)
		{
			return 1;
		}
	}

	return 0;
}

float Ghost::get_target_distance(unsigned char i_direction)
{
	short x = position.x;
	short y = position.y;
	switch (i_direction)
	{
	case 0:
	{
		x += GHOST_SPEED;

		break;
	}
	case 1:
	{
		y -= GHOST_SPEED;

		break;
	}
	case 2:
	{
		x -= GHOST_SPEED;

		break;
	}
	case 3:
	{
		y += GHOST_SPEED;
	}
	}

	// tính khoảng cách Euclid (khoảng cách đường chim bay).
	//  Trả về khoảng cách từ vị trí giả định (sau khi di chuyển) đến mục tiêu.
	return static_cast<float>(sqrt(pow(x - target.x, 2) + pow(y - target.y, 2)));
}

void Ghost::switch_mode()
{
	// Chuyển đổi giữa Scatter (0) và Chase (1).
	movement_mode = 1 - movement_mode;
}

void Ghost::update_target(unsigned char i_pacman_direction, const Position &i_ghost_0_position, const Position &i_pacman_position)
{
	// Logic khi ma đang "bị ăn" (chế độ 2) và quay về nhà.
	if (1 == use_door)
	{
		if (position == target)
		{
			if (home_exit == target)
			{
				use_door = 0;
			}
			else if (home == target)
			{
				frightened_mode = 0;

				target = home_exit;
			}
		}
	}

	else
	{
		if (0 == movement_mode) // Chế độ Scatter
		{
			// Mỗi ma đi về một góc cố định của bản đồ.
			switch (id)
			{
			case 0: // Đỏ - Góc trên phải
			{
				target = {CELL_SIZE * (MAP_WIDTH - 1), 0};

				break;
			}
			case 1: // Hồng - Góc trên trái
			{
				target = {0, 0};

				break;
			}
			case 2: // Xanh - Góc dưới phải
			{
				target = {CELL_SIZE * (MAP_WIDTH - 1), CELL_SIZE * (MAP_HEIGHT - 1)};

				break;
			}
			case 3: // Cam - Góc dưới trái
			{
				target = {0, CELL_SIZE * (MAP_HEIGHT - 1)};
			}
			}
		}
		else // Chế độ Chase (Săn đuổi)
		{
			switch (id)
			{
			case 0: // Đỏ - Truy đuổi thẳng Pacman.
			{
				target = i_pacman_position;

				break;
			}
			case 1: // Hồng - Truy đuổi 2 ô phía trước Pacman (theo thiết lập GHOST_1_CHASE).
			{
				target = i_pacman_position;

				switch (i_pacman_direction)
				{
				case 0:
				{
					target.x += CELL_SIZE * GHOST_1_CHASE;

					break;
				}
				case 1:
				{
					target.y -= CELL_SIZE * GHOST_1_CHASE;

					break;
				}
				case 2:
				{
					target.x -= CELL_SIZE * GHOST_1_CHASE;

					break;
				}
				case 3:
				{
					target.y += CELL_SIZE * GHOST_1_CHASE;
				}
				}

				break;
			}
			case 2: // Xanh - Mục tiêu phức tạp.
			{
				target = i_pacman_position;

				switch (i_pacman_direction)
				{
				case 0:
				{
					target.x += CELL_SIZE * GHOST_2_CHASE;

					break;
				}
				case 1:
				{
					target.y -= CELL_SIZE * GHOST_2_CHASE;

					break;
				}
				case 2:
				{
					target.x -= CELL_SIZE * GHOST_2_CHASE;

					break;
				}
				case 3:
				{
					target.y += CELL_SIZE * GHOST_2_CHASE;
				}
				}

				target.x += target.x - i_ghost_0_position.x;
				target.y += target.y - i_ghost_0_position.y;

				break;
			}
			case 3: // Cam - Truy đuổi Pacman nếu ở xa, nhưng về góc scatter (dưới trái) nếu ở quá gần.
			{
				// Tính khoảng cách.
				if (CELL_SIZE * GHOST_3_CHASE <= sqrt(pow(position.x - i_pacman_position.x, 2) + pow(position.y - i_pacman_position.y, 2)))
				{
					// Nếu ở xa (ngoài 4 ô), đuổi Pacman.
					target = i_pacman_position;
				}
				else
				{
					// Nếu ở gần, về góc scatter.
					target = {0, CELL_SIZE * (MAP_HEIGHT - 1)};
				}
			}
			}
		}
	}
}

void Ghost::update(unsigned char i_level, array<array<Cell, MAP_HEIGHT>, MAP_WIDTH> &i_map, Ghost &i_ghost_0, Pacman &i_pacman, unsigned int &i_score, Sound &eatGhostSound, Sound &gameOverSound, Sound &eatPelletSound)
{
	// Cờ (flag) xác định ma có thể di chuyển trong khung hình này không.
	bool move = 0;

	// Đếm số lượng lối đi hợp lệ tại một ngã rẽ.
	unsigned char available_ways = 0;
	unsigned char speed = GHOST_SPEED;

	array<bool, 4> walls{};

	// Xử lý chuyển đổi trạng thái sợ hãi (frightened).
	if (0 == frightened_mode && i_pacman.get_energizer_timer() == ENERGIZER_DURATION / pow(2, i_level))
	{
		frightened_speed_timer = GHOST_FRIGHTENED_SPEED;

		frightened_mode = 1;
	}
	else if (0 == i_pacman.get_energizer_timer() && 1 == frightened_mode)
	{
		frightened_mode = 0;
	}
	// Nếu ma bị ăn (chế độ 2), nó sẽ di chuyển nhanh hơn.
	if (2 == frightened_mode && 0 == position.x % GHOST_ESCAPE_SPEED && 0 == position.y % GHOST_ESCAPE_SPEED)
	{
		speed = GHOST_ESCAPE_SPEED;
	}
	update_target(i_pacman.get_direction(), i_ghost_0.get_position(), i_pacman.get_position());

	walls[0] = map_collision(0, use_door, speed + position.x, position.y, i_map, i_score, eatPelletSound);
	walls[1] = map_collision(0, use_door, position.x, position.y - speed, i_map, i_score, eatPelletSound);
	walls[2] = map_collision(0, use_door, position.x - speed, position.y, i_map, i_score, eatPelletSound);
	walls[3] = map_collision(0, use_door, position.x, position.y + speed, i_map, i_score, eatPelletSound);
	// Logic di chuyển khi không bị sợ hãi (chase/scatter).
	if (1 != frightened_mode)
	{
		// Hướng đi tối ưu, 4 có nghĩa là chưa tìm thấy.
		unsigned char optimal_direction = 4;

		// Ma có thể di chuyển.
		move = 1;

		for (unsigned char a = 0; a < 4; a++)
		{
			// Ma không được phép quay đầu (trừ khi bị bắt buộc).
			if (a == (2 + direction) % 4)
			{
				continue;
			}
			else if (0 == walls[a]) // Nếu hướng này không có tường
			{
				if (4 == optimal_direction)
				{
					optimal_direction = a; // Chọn hướng hợp lệ đầu tiên làm tối ưu tạm thời.
				}

				available_ways++;

				if (get_target_distance(a) < get_target_distance(optimal_direction))
				{
					// Hướng tối ưu là hướng đưa ma đến gần mục tiêu nhất.
					optimal_direction = a;
				}
			}
		}
		if (1 < available_ways) // Nếu đang ở ngã rẽ (nhiều hơn 1 lối đi)
		{
			// Khi ở ngã rẽ, chọn hướng tối ưu.
			direction = optimal_direction;
		}
		else // Nếu chỉ có 1 lối đi (hoặc 0)
		{
			if (4 == optimal_direction)
			{
				// Bị bắt buộc: phải quay đầu lại.
				direction = (2 + direction) % 4;
			}
			else
			{
				// Đi theo lối đi duy nhất.
				direction = optimal_direction;
			}
		}
	}
	// Logic di chuyển khi bị sợ hãi.
	else
	{
		// Chọn hướng ngẫu nhiên.
		unsigned char random_direction = rand() % 4;

		if (0 == frightened_speed_timer)
		{
			// Ma di chuyển chậm hơn khi bị sợ hãi
			move = 1;

			frightened_speed_timer = GHOST_FRIGHTENED_SPEED;

			for (unsigned char a = 0; a < 4; a++)
			{
				if (a == (2 + direction) % 4)
				{
					continue;
				}
				else if (0 == walls[a])
				{
					available_ways++;
				}
			}

			if (0 < available_ways)
			{
				while (1 == walls[random_direction] || random_direction == (2 + direction) % 4)
				{
					// Tiếp tục chọn ngẫu nhiên cho đến khi tìm được hướng hợp lệ.
					random_direction = rand() % 4;
				}

				direction = random_direction;
			}
			else
			{
				// Nếu không còn cách nào khác, quay đầu.
				direction = (2 + direction) % 4;
			}
		}
		else
		{
			// Giảm bộ đếm tốc độ, ma sẽ không di chuyển trong frame này.
			frightened_speed_timer--;
		}
	}
	if (1 == move)
	{
		switch (direction)
		{
		case 0:
		{
			position.x += speed;

			break;
		}
		case 1:
		{
			position.y -= speed;

			break;
		}
		case 2:
		{
			position.x -= speed;

			break;
		}
		case 3:
		{
			position.y += speed;
		}
		}
		if (-CELL_SIZE >= position.x)
		{
			position.x = CELL_SIZE * MAP_WIDTH - speed;
		}
		else if (position.x >= CELL_SIZE * MAP_WIDTH)
		{
			position.x = speed - CELL_SIZE;
		}
	}

	// Kiểm tra va chạm với Pacman.
	if (1 == pacman_collision(i_pacman.get_position()))
	{
		// if (0 == frightened_mode) // Nếu ma không sợ hãi -> Pacman chết.
		// {
		// 	if (i_pacman.get_dead() == 0)
		// 	{
		// 		gameOverSound.play();
		// 	}
		// 	i_pacman.set_dead(1);
		// }
		// else 
		if (frightened_mode !=2) // Nếu ma đang sợ hãi -> Ma bị ăn.
		{
			eatGhostSound.play();

			use_door = 1; // Cho phép ma đi qua cửa nhà ma.

			frightened_mode = 2; // Chuyển sang chế độ "bị ăn".

			target = home; // Đặt mục tiêu là nhà ma.
		}
	}
}

void GhostManager::update(unsigned char i_level, array<array<Cell, MAP_HEIGHT>, MAP_WIDTH> &i_map, Pacman &i_pacman, unsigned int &i_score, Sound &eatGhostSound, Sound &gameOverSound, Sound &eatPelletSound)
{
	if (0 == i_pacman.get_energizer_timer())
	{
		if (0 == wave_timer)
		{
			if (7 > current_wave) // Giới hạn 7 wave (4 scatter, 3 chase)
			{
				current_wave++;

				for (Ghost &ghost : ghosts)
				{
					ghost.switch_mode();
				}
			}

			// Thiết lập thời gian cho wave tiếp theo dựa trên luật của Pacman cổ điển.
			if (1 == current_wave % 2)
			{
				wave_timer = CHASE_DURATION;
			}
			else if (2 == current_wave)
			{
				wave_timer = static_cast<unsigned short>(LONG_SCATTER_DURATION / pow(2, i_level));
			}
			else
			{
				wave_timer = static_cast<unsigned short>(SHORT_SCATTER_DURATION / pow(2, i_level));
			}
		}
		else
		{
			wave_timer--;
		}
	}

	// Cập nhật logic cho từng con ma.
	for (Ghost &ghost : ghosts)
	{
		ghost.update(i_level, i_map, ghosts[0], i_pacman, i_score, eatGhostSound, gameOverSound, eatPelletSound);
	}
}

void Pacman::update(unsigned char i_level, array<array<Cell, MAP_HEIGHT>, MAP_WIDTH> &i_map, unsigned int &i_score, Sound &eatPelletSound)
{
	// Kiểm tra va chạm tường ở 4 hướng
	array<bool, 4> walls{};
	walls[0] = map_collision(0, 0, PACMAN_SPEED + position.x, position.y, i_map, i_score, eatPelletSound);
	walls[1] = map_collision(0, 0, position.x, position.y - PACMAN_SPEED, i_map, i_score, eatPelletSound);
	walls[2] = map_collision(0, 0, position.x - PACMAN_SPEED, position.y, i_map, i_score, eatPelletSound);
	walls[3] = map_collision(0, 0, position.x, PACMAN_SPEED + position.y, i_map, i_score, eatPelletSound);
	if (1 == Keyboard::isKeyPressed(Keyboard::Key::Right))
	{
		if (0 == walls[0])
		{
			direction = 0;
		}
	}
	if (1 == Keyboard::isKeyPressed(Keyboard::Key::Up))
	{
		if (0 == walls[1])
		{
			direction = 1;
		}
	}
	if (1 == Keyboard::isKeyPressed(Keyboard::Key::Left))
	{
		if (0 == walls[2])
		{
			direction = 2;
		}
	}
	if (1 == Keyboard::isKeyPressed(Keyboard::Key::Down))
	{
		if (0 == walls[3])
		{
			direction = 3;
		}
	}
	if (0 == walls[direction])
	{
		switch (direction)
		{
		case 0:
		{
			position.x += PACMAN_SPEED;
			break;
		}
		case 1:
		{
			position.y -= PACMAN_SPEED;
			break;
		}
		case 2:
		{
			position.x -= PACMAN_SPEED;
			break;
		}
		case 3:
		{
			position.y += PACMAN_SPEED;
		}
		}
	}
	if (-CELL_SIZE >= position.x)
	{
		position.x = CELL_SIZE * MAP_WIDTH - PACMAN_SPEED;
	}
	else if (CELL_SIZE * MAP_WIDTH <= position.x)
	{
		position.x = PACMAN_SPEED - CELL_SIZE;
	}

	bool ate_energizer = map_collision(1, 0, position.x, position.y, i_map, i_score, eatPelletSound);
	if (1 == ate_energizer)
	{
		energizer_timer = static_cast<unsigned short>(ENERGIZER_DURATION / pow(2, i_level));
	}
	else
	{
		energizer_timer = max(0, energizer_timer - 1);
	}
}

int main()
{
	// Cờ (flag) báo hiệu đã thắng màn chơi.
	bool game_won = 0;
	bool victory_sound_played = false;
	// Cờ (flag) báo hiệu game đang tạm dừng
	bool is_paused = false;
	// Biến `lag` dùng để tích lũy thời gian, giúp game chạy độc lập với framerate.
	unsigned lag = 0;

	unsigned char level = 0; // Cấp độ hiện tại.
	// Biến điểm
	unsigned int score = 0;
	unsigned int high_score = 0;
	vector<unsigned int> high_scores;
	bool score_saved = false;
	// Biến lưu trữ mốc thời gian của frame trước, dùng để tính delta_time.
	chrono::time_point<chrono::steady_clock> previous_time;

	array<string, MAP_HEIGHT> map_sketch_level_1 = {
		" ################### ",
		" #........#........# ",
		" #o##.###.#.###.##o# ",
		" #.................# ",
		" #.##.#.#####.#.##.# ",
		" #....#...#...#....# ",
		" ####.### # ###.#### ",
		"    #.#   0   #.#    ",
		"#####.# ##=## #.#####",
		"     .  #123#  .     ",
		"#####.# ##### #.#####",
		"    #.#       #.#    ",
		" ####.# ##### #.#### ",
		" #........#........# ",
		" #.##.###.#.###.##.# ",
		" #o.#.....P.....#.o# ",
		" ##.#.#.#####.#.#.## ",
		" #....#...#...#....# ",
		" #.######.#.######.# ",
		" #.................# ",
		" ################### "};

	array<string, MAP_HEIGHT> map_sketch_level_2 = {
		"#####################",
		"#o.................o#",
		"#.#####.#####.#####.#",
		"#.....#.......#.....#",
		"#####.#.#####.#.#####",
		"    #.....#.....#    ",
		"###...### # ###...###",
		"  #.#.#   0   #.#.#  ",
		"###.#.# ##=## #.#.###",
		"#.....# #123#  .....#",
		"#.#.#.# ##### #.#.#.#",
		"#...#.#       #.#...#",
		"##.##.# ##### #.##.##",
		"  ........#........  ",
		"#####.###.#.###.#####",
		"#...#.#...P...#.#...#",
		"#o#.#.#.#####.#.#.#o#",
		"#.#.......#.......#.#",
		"#.###.###.#.###.###.#",
		"#...................#",
		"#####################",
	};

	vector<array<string, MAP_HEIGHT>>
		all_map_sketches;
	all_map_sketches.push_back(map_sketch_level_1);
	all_map_sketches.push_back(map_sketch_level_2);

	array<array<Cell, MAP_HEIGHT>, MAP_WIDTH> map{};

	// Mảng lưu vị trí khởi tạo của 4 con ma
	array<Position, 4> ghost_positions;
	RenderWindow window(VideoMode({CELL_SIZE * MAP_WIDTH * SCREEN_RESIZE, (FONT_HEIGHT + CELL_SIZE * MAP_HEIGHT) * SCREEN_RESIZE}), "Pac-Man", Style::Close);
	window.setView(View(FloatRect({0, 0}, {CELL_SIZE * MAP_WIDTH, FONT_HEIGHT + CELL_SIZE * MAP_HEIGHT})));
	Texture fontTexture;
	if (!fontTexture.loadFromFile("Resources/Images/Font.png"))
	{
		cerr << "Error!" << endl;
	}
	const int charWidth = fontTexture.getSize().x / 96;
	SoundBuffer eatGhostBuffer;
	if (!eatGhostBuffer.loadFromFile("Resources/Sounds/Ghost.ogg"))
	{
		cerr << "Error loading Ghost.ogg" << endl;
	}
	Sound eatGhostSound(eatGhostBuffer);
	SoundBuffer gameOverBuffer;
	if (!gameOverBuffer.loadFromFile("Resources/Sounds/Death.ogg"))
	{
		cerr << "Error loading Death.ogg" << endl;
	}
	Sound gameOverSound(gameOverBuffer);
	SoundBuffer levelUpBuffer;
	if (!levelUpBuffer.loadFromFile("Resources/Sounds/Level.ogg"))
	{
		cerr << "Error loading Level.ogg" << endl;
	}
	Sound levelUpSound(levelUpBuffer);
	SoundBuffer eatPelletBuffer;
	if (!eatPelletBuffer.loadFromFile("Resources/Sounds/Pellet.wav"))
	{
		cerr << "Error loading Pellet.wav" << endl;
	}
	Sound eatPelletSound(eatPelletBuffer);
	GhostManager ghost_manager;
	Pacman pacman;
	// Tạo seed ngẫu nhiên (dùng cho ma khi bị sợ hãi).
	srand(static_cast<unsigned>(time(0)));
	load_high_scores(high_scores, high_score_file);
	if (!high_scores.empty())
	{
		high_score = high_scores[0];
	}

	map = convert_sketch(all_map_sketches[level], ghost_positions, pacman);

	// Thiết lập trạng thái ban đầu cho quản lý ma.
	ghost_manager.reset(level, ghost_positions);
	pacman.reset();

	// Lấy mốc thời gian hiện tại trước khi bắt đầu vòng lặp game.
	previous_time = chrono::steady_clock::now();

	while (window.isOpen())
	{
		// Tính toán thời gian trôi qua (delta_time) từ frame trước.
		unsigned delta_time = chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - previous_time).count();

		// Tích lũy thời gian vào `lag`.
		lag += delta_time;

		// Cập nhật mốc thời gian `previous_time` cho lần lặp kế tiếp.
		previous_time += chrono::microseconds(delta_time);

		// xử lý game bắt kịp thời gian nếu lag
		while (FRAME_DURATION <= lag)
		{
			// giảm lag 1 frame.
			lag -= FRAME_DURATION;
			while (const optional event = window.pollEvent())
			{
				if (event->is<Event::Closed>())
				{
					window.close();
				}
				// Tạm dừng game khi cửa sổ mất focus.
				else if (event->is<Event::FocusLost>())
				{
					is_paused = true;
				}
				// Tiếp tục game khi cửa sổ được focus.
				else if (event->is<Event::FocusGained>())
				{
					is_paused = false;
				}
				// Xử lý sự kiện nhấn phím (dùng cho các hành động 1 lần như Reset).
				else if (const auto *keyPressed = event->getIf<Event::KeyPressed>())
				{
					if (keyPressed->code == Keyboard::Key::Escape)
					{
						window.close();
					}
					// Xử lý sự kiện nhấn phím Enter để chơi lại.
					else if (keyPressed->code == Keyboard::Key::Enter)
					{
						// Chỉ khởi động lại khi game đã kết thúc (thắng hoặc chết).
						if (game_won || pacman.get_dead())
						{
							game_won = 0;

							if (pacman.get_dead()) // Nếu chết, chơi lại từ level 0.
							{
								score = 0;
								level = 0;
							}
							else // Nếu thắng, tăng level.
							{
								level++;
							}
							score_saved = false;
							victory_sound_played = false;

							// Sử dụng modulo (%) để lặp lại các bản đồ
							map = convert_sketch(all_map_sketches[level % all_map_sketches.size()], ghost_positions, pacman);

							ghost_manager.reset(level, ghost_positions);
							pacman.reset();
						}
					}
				}
			}
			// Chỉ cập nhật logic game nếu không tạm dừng.
			if (!is_paused)
			{
				// Chỉ cập nhật khi game đang diễn ra.
				if (0 == game_won && 0 == pacman.get_dead())
				{
					game_won = 1;
					pacman.update(level, map, score, eatPelletSound);
					// Cập nhật logic của 4 con ma.
					ghost_manager.update(level, map, pacman, score, eatGhostSound, gameOverSound, eatPelletSound);
					// cập nhật điểm cao
					if (score > high_score)
					{
						high_score = score;
					}
					// Kiểm tra xem đã ăn hết pellet chưa.
					for (const array<Cell, MAP_HEIGHT> &column : map)
					{
						for (const Cell &cell : column)
						{
							if (Cell::Pellet == cell)
							{
								game_won = 0;
								break;
							}
						}
						if (0 == game_won)
						{
							break;
						}
					}
					if (game_won && !victory_sound_played)
					{
						levelUpSound.play();
						victory_sound_played = true;
					}
				}
				else if (pacman.get_dead() && !score_saved)
				{
					add_score(high_scores, score);
					save_high_scores(high_scores, high_score_file);
					score_saved = true;
				}
			}
			if (FRAME_DURATION > lag)
			{
				window.clear();

				// Nếu game đang diễn ra.
				if (0 == game_won && 0 == pacman.get_dead())
				{
					draw_map(map, window);

					// Vẽ ma (truyền cờ `flash` nếu sắp hết energizer).
					ghost_manager.draw(GHOST_FLASH_START >= pacman.get_energizer_timer(), window);

					string level_text = "LEVEL: " + to_string(1 + level);
					draw_text(fontTexture, window, level_text, 0, 0, 0); // Y = 0

					stringstream ss_score;
					ss_score << "SCORE:" << setw(7) << setfill('0') << score;
					string score_text = ss_score.str();
					draw_text(fontTexture, window, score_text, (CELL_SIZE * MAP_WIDTH - score_text.length() * charWidth) / 2, 0, 0); // Y = 0

					stringstream ss_high_score;
					ss_high_score << "HIGH:" << setw(7) << setfill('0') << high_score;
					string high_score_text = ss_high_score.str();
					draw_text(fontTexture, window, high_score_text, CELL_SIZE * MAP_WIDTH - (high_score_text.length() * charWidth), 0, 0); // Y = 0
				}

				// Vẽ Pacman (hoặc hoạt ảnh chết/thắng).
				pacman.draw(game_won, window);

				if (1 == pacman.get_animation_over())
				{
					if (1 == game_won)
					{
						draw_text(fontTexture, window, "Next level!", 0, 0, 1);
					}
					else
					{
						stringstream ss_game_over;
						ss_game_over << "GAME OVER\n\n"
									 << "HIGH SCORES\n";

						int rank = 1;
						for (unsigned int s : high_scores)
						{
							ss_game_over << setfill('0') << setw(2) << rank << ". " << setw(7) << s << "\n";
							rank++;
							if (rank > 5)
								break; // Chỉ hiển thị top 5
						}
						draw_text(fontTexture, window, ss_game_over.str(), 0, 0, 1);
					}
				}
				// Nếu game đang tạm dừng.
				if (is_paused)
				{
					// Hiển thị thông báo khi tạm dừng.
					draw_text(fontTexture, window, "PAUSED", 0, 0, 1);
				}
				// Hiển thị mọi thứ đã vẽ lên cửa sổ.
				window.display();
			}
		}
	}
}