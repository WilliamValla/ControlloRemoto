#include "client_socket.h"

#include <vector>
#include "connection_manager.h"
#include "frame_datagram.h"
#include "../gfx/frame.h"
#include "../gfx/frame_buffer.h"


client_socket::client_socket() :
    m_client(new crlib::tcp_client),
    m_udp_socket(nullptr),
    m_stream(m_client),
    m_send_queue(),
    m_recv_event(false, false),
    m_recv_dgram_event(false, false),
    m_connect_event(false, false),
    m_recv_thread(std::bind(&client_socket::recv_thread_method, this)),
    m_send_thread(std::bind(&client_socket::send_thread_method, this)),
    m_connect_thread(new crlib::thread(std::bind(&client_socket::connect_thread_method, this))),
    m_datagram_thread(new crlib::thread(std::bind(&client_socket::recv_datagram_method, this)))
{
    m_recv_thread.start();
    m_send_thread.start();
    m_connect_thread->start();
    m_datagram_thread->start();
}

client_socket::client_socket(crlib::tcp_client* client) :
    m_client(client),
    m_stream(client),
    m_send_queue(),
    m_recv_event(false, true),
    m_recv_dgram_event(false, false),
    m_connect_event(false, false),
    m_recv_thread(std::bind(&client_socket::recv_thread_method, this)),
    m_send_thread(std::bind(&client_socket::send_thread_method, this)),
    m_connect_thread(nullptr)
{
    m_recv_thread.start();
    m_send_thread.start();
}

client_socket::~client_socket()
{
    delete m_connect_thread;
    delete m_client;
}

void client_socket::send(const crlib::string& str)
{
    m_send_queue.push(str);
}

void client_socket::connect_to(const crlib::ip_address& ip)
{
    m_client->remote_ip() = ip;
    m_connect_event.signal();
}

#include <Windows.h>
#include <WinUser.h>

void client_socket::recv_thread_method()
{
    std::vector<crlib::string> split_str;

    while (true)
    {
        m_recv_event.wait();

        while (true)
        {
            crlib::string str = m_stream.read_line();
            split_str.clear();
            str.split(split_str, L' ', 1);

            if (split_str[0] == L"MSG")
            {
                emit connection_manager::instance().on_msg_received(split_str[1]);
            }
            else if (split_str[0] == L"KEYDOWN")
            {
                uint32_t vk = split_str[1].to_number<uint32_t>();

                KEYBDINPUT input_data = {0};
                input_data.wVk = vk;

                INPUT input = {0};
                input.type = INPUT_KEYBOARD;
                input.ki = input_data;

                SendInput(1, &input, sizeof(INPUT));
            }
            else if (split_str[0] == L"KEYUP")
            {
                uint32_t vk = split_str[1].to_number<uint32_t>();

                KEYBDINPUT input_data = {0};
                input_data.wVk = vk;
                input_data.dwFlags = KEYEVENTF_KEYUP;

                INPUT input = {0};
                input.type = INPUT_KEYBOARD;
                input.ki = input_data;

                SendInput(1, &input, sizeof(INPUT));
            }
            else if (split_str[0] == L"MOUSEDOWN")
            {
                uint32_t mouse_btn = split_str[1].to_number<uint32_t>();

                MOUSEINPUT input_data = {0};

                switch (mouse_btn)
                {
                case 1:
                    input_data.dwFlags = MOUSEEVENTF_LEFTDOWN;
                    break;
                case 2:
                    input_data.dwFlags = MOUSEEVENTF_RIGHTDOWN;
                    break;
                case 4:
                    input_data.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
                    break;
                default:
                    std::cout << "Invalid mouse button: " << mouse_btn << std::endl;
                    continue;
                }

                INPUT input = {0};
                input.type = INPUT_MOUSE;
                input.mi = input_data;

                SendInput(1, &input, sizeof(INPUT));
            }
            else if (split_str[0] == L"MOUSEUP")
            {
                uint32_t mouse_btn = split_str[1].to_number<uint32_t>();

                MOUSEINPUT input_data = {0};

                switch (mouse_btn)
                {
                case 1:
                    input_data.dwFlags = MOUSEEVENTF_LEFTUP;
                    break;
                case 2:
                    input_data.dwFlags = MOUSEEVENTF_RIGHTUP;
                    break;
                case 4:
                    input_data.dwFlags = MOUSEEVENTF_MIDDLEUP;
                    break;
                default:
                    std::cout << "Invalid mouse button: " << mouse_btn << std::endl;
                    continue;
                }

                INPUT input = {0};
                input.type = INPUT_MOUSE;
                input.mi = input_data;

                SendInput(1, &input, sizeof(INPUT));
            }
            else if (split_str[0] == L"MOUSEMOVE")
            {
                std::vector<crlib::string> mouse_pos_str;
                split_str[1].split(mouse_pos_str, L' ');

                uint32_t mouseX = mouse_pos_str[0].to_number<uint32_t>();
                uint32_t mouseY = mouse_pos_str[1].to_number<uint32_t>();

                MOUSEINPUT input_data = {0};
                input_data.dx = mouseX;
                input_data.dy = mouseY;
                input_data.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;

                INPUT input = {0};
                input.type = INPUT_MOUSE;
                input.mi = input_data;

                SendInput(1, &input, sizeof(INPUT));
            }
            else if (split_str[0] == L"WHEELROT")
            {
                int32_t rot_delta = split_str[1].to_number<int32_t>();

                MOUSEINPUT input_data = {0};
                input_data.dwFlags = MOUSEEVENTF_WHEEL;
                input_data.mouseData = rot_delta;

                INPUT input = {0};
                input.type = INPUT_MOUSE;
                input.mi = input_data;

                SendInput(1, &input, sizeof(INPUT));
            }
            else
            {
                std::wcout << "Invalid message: " << str << std::endl;
            }
        }
    }
}

void client_socket::send_thread_method()
{
    while (true)
    {
        crlib::string str = m_send_queue.pop();
        m_stream.send(str);
    }
}

void client_socket::connect_thread_method()
{
    while (true)
    {
        m_connect_event.wait();

        try
        {
            m_client->connect(m_client->remote_ip());
        }
        catch (std::exception&)
        {
            emit connection_manager::instance().on_connection_failed();
            continue;
        }

        m_udp_socket = new crlib::udp_socket(m_client->local_ip());
        m_recv_dgram_event.signal();
        m_recv_event.signal();

        emit connection_manager::instance().on_connected();
    }
}

void client_socket::recv_datagram_method()
{
    while (true)
    {
        m_recv_dgram_event.wait();

        frame_datagram dgram;

        while (true)
        {
            dgram.recv();
            frame_buffer::instance().on_frame_data_recv(&dgram);
        }
    }
}
