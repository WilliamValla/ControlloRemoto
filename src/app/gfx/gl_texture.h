#pragma once

#include <cstdint>

class gl_texture
{
public:
    gl_texture();

    void create();

    //Texture must be bound before calling this!
    void update_data(uint8_t* data);

    void bind();
    void unbind();

private:
    uint32_t m_id;
};
