// Headers/Entity.hpp
#pragma once


// Lớp cha (Base Class)
class Entity
{
    // Dùng 'protected' để các lớp con (Pacman, Ghost) có thể truy cập trực tiếp
protected:
    unsigned char direction;
    unsigned short animation_timer;
    Position position;

public:
    // Hàm khởi tạo: Đặt các giá trị mặc định
    Entity() : direction(0),
               animation_timer(0),
               position({0, 0})
    {
    }

    // Hàm ảo (virtual destructor) - nên có cho mọi lớp cha
    virtual ~Entity() {}

    // Các hàm dùng chung
    void set_position(short i_x, short i_y)
    {
        position = {i_x, i_y};
    }

    Position get_position()
    {
        return position;
    }

    unsigned char get_direction()
    {
        return direction;
    }

    unsigned short get_animation_timer()
    {
        return animation_timer;
    }

    void set_animation_timer(unsigned short i_animation_timer)
    {
        animation_timer = i_animation_timer;
    }

    // Hàm ảo thuần túy (pure virtual function)
    // Bắt buộc các lớp con (Pacman, Ghost) phải tự định nghĩa hàm reset()
    virtual void reset() = 0;
};