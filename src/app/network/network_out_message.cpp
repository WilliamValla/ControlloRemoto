#include "network_out_message.h"

network_out_message::network_out_message(const crlib::string& opcode, const std::initializer_list<crlib::string>& list) :
    m_str(opcode)
{
    for (const crlib::string& str : list)
    {
        m_str += crlib::string(L" ");
        m_str += str;
    }
}

void network_out_message::send()
{
    connection_manager::instance().send_message(m_str);
}
