#pragma once

#include <map>
#include <QObject>
#include <crlib/threading/spin_lock.h>
#include <crlib/threading/thread.h>
#include <crlib/threading/waitable_event.h>
#include "frame.h"
#include "gl_texture.h"
#include "../network/frame_datagram.h"

class frame_buffer : public QObject
{
    Q_OBJECT

public:
    static frame_buffer& instance();

    void on_frame_data_recv(frame_datagram* datagram);

signals:
    void signal_texture_update(frame* f);

protected:
    frame_buffer();

    void thread_method();

private:
    crlib::spin_lock m_lock;
    crlib::waitable_event m_event;
    crlib::thread m_thread;
    std::map<uint64_t, frame*> m_buffer;
    uint64_t m_last_frame_time;
    uint64_t m_last_frame_num;
};
