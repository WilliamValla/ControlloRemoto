#pragma once

#include <cstdint>
#include <initializer_list>
#include <crlib/string/string.h>
#include "connection_manager.h"

class network_out_message
{
public:
    network_out_message(const crlib::string& opcode, const std::initializer_list<crlib::string>& list);

    void send();

private:
    crlib::string m_str;
};

struct network_out_msg : public network_out_message
{
    network_out_msg(const crlib::string& msg) : network_out_message(L"MSG", {msg}) {}
};

struct network_out_keyupdate : public network_out_message
{
    network_out_keyupdate(bool pressed, int vk) : network_out_message(pressed ? L"KEYDOWN" : L"KEYUP", { crlib::string_cast<uint32_t>(vk) }) {}
};

struct network_out_mousepos_update : public network_out_message
{
    network_out_mousepos_update(int mouseX, int mouseY, int width, int height) :
        network_out_message(L"MOUSEMOVE", { crlib::string_cast<uint32_t>(mouseX * 65535 / width), crlib::string_cast<uint32_t>(mouseY * 65535 / height) }) {}
};

struct network_out_mousebtn_update : public network_out_message
{
    network_out_mousebtn_update(bool pressed, int btn) : network_out_message(pressed ? L"MOUSEDOWN" : L"MOUSEUP", { crlib::string_cast<uint32_t>(btn) }) {}
};

struct network_out_wheel_rotation : public network_out_message
{
    network_out_wheel_rotation(int delta) : network_out_message(L"WHEELROT", { crlib::string_cast<int32_t>(delta) }) {}
};
