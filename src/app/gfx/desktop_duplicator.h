#pragma once

#include <crlib/graphics/d3d_context.h>
#include <crlib/graphics/desktop_duplication.h>
#include <crlib/graphics/render_target.h>
#include <crlib/graphics/screen_quad.h>
#include <crlib/graphics/shader.h>
#include <crlib/threading/thread.h>
#include <crlib/threading/waitable_event.h>
#include "../network/frame_datagram.h"

class desktop_duplicator
{
public:
    static desktop_duplicator& instance();

    void initialize();

protected:
    desktop_duplicator();

    void thread_method();
    void send_thread_method();

private:
    crlib::desktop_duplication m_duplicator;
    crlib::render_target m_render_target;
    crlib::shader m_shader;
    crlib::screen_quad m_quad;
    crlib::waitable_event m_event;
    crlib::thread m_thread;
};
