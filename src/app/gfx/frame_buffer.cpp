#include "frame_buffer.h"

#include <iostream>
#include <crlib/utils/time.h>

frame_buffer& frame_buffer::instance()
{
    static frame_buffer buffer;
    return buffer;
}

frame_buffer::frame_buffer() :
    m_thread(std::bind(&frame_buffer::thread_method, this)),
    m_lock(),
    m_buffer(),
    m_last_frame_num(0),
    m_last_frame_time(0)
{
    m_thread.start();
}

void frame_buffer::on_frame_data_recv(frame_datagram* datagram)
{
    m_lock.lock();

    frame_datagram_header header = datagram->read_header();

    if (header.m_frame_num < m_last_frame_num)
    {
        //Frame was dropped, ignore
        return;
    }

    auto iter = m_buffer.find(header.m_frame_num);
    frame* f;

    if (iter == m_buffer.end())
    {
        f = new frame;
        f->add_fragment(header.m_fragment_offset, datagram->frame_data(), header.m_fragment_size);
        m_buffer.insert({header.m_frame_num, f});
    }
    else
    {
        f = iter->second;
        f->add_fragment(header.m_fragment_offset, datagram->frame_data(), header.m_fragment_size);
    }

    m_lock.unlock();
}

void frame_buffer::thread_method()
{
    uint64_t last_time = crlib::time_stamp<crlib::millis>();
    uint64_t current_time = 0;
    uint64_t elapsed = 0;

    long double accum = 0;
    long double MS_PER_FRAME = 1000.0 / 30.0;
    int fps = 0;

    uint64_t last_fps =  crlib::time_stamp<crlib::millis>();

    while (true)
    {
        current_time = crlib::time_stamp<crlib::millis>();
        elapsed = std::min((current_time - last_time), 1000ull);
        accum += elapsed;
        last_time = current_time;

        while (accum >= MS_PER_FRAME)
        {
            m_lock.lock();

            frame* f;

            for (auto iter = m_buffer.begin(); iter != m_buffer.end();)
            {
                f = iter->second;

                if (f->completed())
                {
                    emit signal_texture_update(f);

                    ++fps;
                    m_buffer.erase(iter);
                    break;
                }
                else if (current_time - f->time() >= 200)
                {
                    //std::cout << "Frame dropped, too much time for completion" << std::endl;
                    delete f;
                    iter = m_buffer.erase(iter);
                }
                else
                {
                    break;
                }
            }

            m_lock.unlock();

            accum -= MS_PER_FRAME;
        }

        if (current_time - last_fps >= 1000)
        {
            //std::cout << "FPS: "  << fps << std::endl;
            last_fps = current_time;
            fps = 0;

            //m_lock.lock();
            //std::cout << m_buffer.size() << std::endl;
            //m_lock.unlock();
        }
    }
}
