#include "frame.h"

#include <cstring>
#include <crlib/utils/time.h>

frame::frame() :
    m_data(new uint8_t[frame::size()]),
    m_num_fragments(0),
    m_time(crlib::time_stamp<crlib::millis>())
{
}

frame::~frame()
{
    delete[] m_data;
}

uint8_t* frame::data()
{
    return m_data;
}

void frame::add_fragment(size_t offset, uint8_t* data, size_t size)
{
    std::memcpy(m_data + offset, data, size);
    ++m_num_fragments;
}

void frame::reset()
{
    m_num_fragments = 0;
}

bool frame::completed()
{
    return m_num_fragments == frame::req_fragments();
}
