#include "desktop_duplicator.h"

#include <iostream>
#include <crlib/utils/time.h>
#include "frame.h"

const char* RESIZE_SHADER_SRC = R"(
Texture2D tx;
SamplerState sampl
{
    Filter = MAXIMUM_ANISOTROPIC;
    AddressU = Wrap;
    AddressV = Wrap;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXTCOORD;
};

VS_OUTPUT VS(float4 inPos : POSITION, float2 inTexCoord : TEXTCOORD)
{
    VS_OUTPUT output;
    output.Pos = inPos;
    output.Tex = inTexCoord;
    return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET0
{
    return tx.Sample(sampl, input.Tex);
})";

desktop_duplicator &desktop_duplicator::instance()
{
    static desktop_duplicator duplicator;
    return duplicator;
}


void desktop_duplicator::initialize()
{
    m_render_target.set_active();
    m_shader.set_active();
    m_quad.set_active();
    m_event.signal();
}

desktop_duplicator::desktop_duplicator() :
    m_duplicator(),
    m_render_target(800, 600),
    m_shader(RESIZE_SHADER_SRC),
    m_quad(m_shader),
    m_event(),
    m_thread(std::bind(&desktop_duplicator::thread_method, this))
{
    m_thread.start();
}

void desktop_duplicator::thread_method()
{
    while (true)
    {
        m_event.wait();

        crlib::gpu_texture* frame;
        uint64_t frame_num = 0;
        uint32_t total = frame::size();

        frame = m_duplicator.aquire_frame(INFINITE);
        delete frame;

        uint64_t last_time = crlib::time_stamp<crlib::millis>();
        uint64_t current_time = 0;
        uint64_t elapsed = 0;
        int fps = 0;

        while (true)
        {
            frame = m_duplicator.aquire_frame(INFINITE);
            current_time = crlib::time_stamp<crlib::millis>();
            elapsed = current_time - last_time;

            if (elapsed >= 1000)
            {
                //std::cout << "FPS: " << fps << std::endl;
                fps = 0;
                last_time = current_time;
            }
            else if (fps >= 30)
            {
                delete frame;
                continue;
            }

            if (frame == nullptr)
            {
                crlib::thread::yield();
                continue;
            }

            m_quad.draw(*frame);
            delete frame;

            crlib::gpu_texture& scaled_frame = m_render_target.copy_to_cpu();
            uint8_t* frame_data = scaled_frame.map();

            uint32_t sent = 0;
            size_t size;

            do
            {
                size = 1000 > total - sent ? total - sent : 1000;
                frame_datagram dgram(frame_num, sent, frame_data + sent, size);
                dgram.send();
                sent += size;
            }
            while (sent != total);

            scaled_frame.unmap();

            ++frame_num;
            ++fps;
        }
    }
}

