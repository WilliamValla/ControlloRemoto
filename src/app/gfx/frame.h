#pragma once

#include <cstdint>

class frame
{
public:
    static constexpr size_t size() { return 1920000; } //800 * 600 * 4 BPP
    static constexpr size_t req_fragments() { return 1920; } //ceil(size() / 1000)

    frame();
    ~frame();

    uint8_t* data();
    uint64_t time() { return m_time; }

    void add_fragment(size_t offset, uint8_t* data, size_t size);

    void reset();
    bool completed();

private:
    uint8_t* m_data;
    uint64_t m_time;
    size_t m_num_fragments;
};
