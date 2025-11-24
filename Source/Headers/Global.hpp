#pragma once

// Kích thước (chiều rộng và cao) của mỗi ô (cell) trên bản đồ, tính bằng pixel.
constexpr unsigned char CELL_SIZE = 16;
// Chiều cao của mỗi ký tự trong font chữ, tính bằng pixel.
constexpr unsigned char FONT_HEIGHT = 16;
// Xác định ô mục tiêu của Pinky (Ghost 1) phía trước Pacman.
constexpr unsigned char GHOST_1_CHASE = 2;
// Xác định ô mục tiêu của Inky (Ghost 2) phía trước Pacman để tính toán vector.
constexpr unsigned char GHOST_2_CHASE = 1;
// Khoảng cách (tính bằng ô) mà Clyde (Ghost 3) sẽ chuyển sang chế độ "scatter" nếu ở quá gần Pacman.
constexpr unsigned char GHOST_3_CHASE = 4;
// Tổng số khung hình (frame) trong hoạt ảnh di chuyển của ma.
constexpr unsigned char GHOST_ANIMATION_FRAMES = 6;
// Số frame của game trước khi chuyển sang khung hình hoạt ảnh tiếp theo của ma.
constexpr unsigned char GHOST_ANIMATION_SPEED = 1;
// Tốc độ di chuyển của ma khi bị ăn (chỉ còn mắt) và quay về nhà, tính bằng pixel mỗi frame logic.
constexpr unsigned char GHOST_ESCAPE_SPEED = 4;
// Tốc độ di chuyển của ma khi bị "frightened" (sợ hãi). Đây là một bộ đếm, giá trị càng cao, ma di chuyển càng chậm.
constexpr unsigned char GHOST_FRIGHTENED_SPEED = 3;
// Tốc độ di chuyển chuẩn của ma, tính bằng pixel mỗi frame logic.
constexpr unsigned char GHOST_SPEED = 1;
// Chiều cao của bản đồ, tính bằng số ô.
constexpr unsigned char MAP_HEIGHT = 21;
// Chiều rộng của bản đồ, tính bằng số ô.
constexpr unsigned char MAP_WIDTH = 21;
// Tổng số khung hình (frame) trong hoạt ảnh di chuyển của Pacman.
constexpr unsigned char PACMAN_ANIMATION_FRAMES = 6;
// Số frame của game trước khi chuyển sang khung hình hoạt ảnh tiếp theo của Pacman.
constexpr unsigned char PACMAN_ANIMATION_SPEED = 2;
// Tổng số khung hình (frame) trong hoạt ảnh Pacman chết.
constexpr unsigned char PACMAN_DEATH_FRAMES = 12;
// Tốc độ di chuyển của Pacman, tính bằng pixel mỗi frame logic.
constexpr unsigned char PACMAN_SPEED = 2;
// Tỷ lệ nhân kích thước cửa sổ game.
constexpr unsigned char SCREEN_RESIZE = 2;
// Thời lượng (tính bằng frame logic) của chế độ "chase" (săn đuổi).
constexpr unsigned short CHASE_DURATION = 1024;
// Thời lượng (tính bằng frame logic) của trạng thái "energizer" (Pacman ăn hạt năng lượng).
constexpr unsigned short ENERGIZER_DURATION = 512;
// Thời lượng mục tiêu cho mỗi frame logic của game, tính bằng micro giây (khoảng 60 FPS).
constexpr unsigned short FRAME_DURATION = 16667;
// Mốc thời gian (còn lại) của trạng thái "energizer" khi ma bắt đầu nhấp nháy.
constexpr unsigned short GHOST_FLASH_START = 64;
// Thời lượng (tính bằng frame logic) của chế độ "scatter" (phân tán) dài.
constexpr unsigned short LONG_SCATTER_DURATION = 512;
// Thời lượng (tính bằng frame logic) của chế độ "scatter" (phân tán) ngắn.
constexpr unsigned short SHORT_SCATTER_DURATION = 256;
// Hằng số điểm
constexpr unsigned int PELLET_SCORE = 10;
constexpr unsigned int ENERGIZER_SCORE = 50;
const unsigned int CHARS_IN_FONT = 96;
const unsigned int ASCII_OFFSET = 32; // Texture font bắt đầu từ ký tự ' ' (mã ASCII 32)
// Tên tệp lưu điểm
const std::string high_score_file = "Resources/highscores.txt";
// Định nghĩa các loại ô có thể có trên bản đồ.
enum Cell
{
	Door,	   // Cửa nhà ma
	Empty,	   // Ô trống
	Energizer, // Hạt năng lượng (viên to)
	Pellet,	   // Hạt thường (viên nhỏ)
	Wall	   // Tường
};
// Cấu trúc lưu trữ vị trí 2D (tọa độ pixel).
struct Position
{
	short x;
	short y;
	// Toán tử so sánh bằng để kiểm tra hai vị trí có trùng nhau không.
	bool operator==(const Position &i_position)
	{
		return this->x == i_position.x && this->y == i_position.y;
	}
};