#include "login_widget.h"
#include "ui_login_widget.h"

#include "crlib/string/string.h"

login_widget::login_widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::login_widget)
{
    ui->setupUi(this);
}

login_widget::~login_widget()
{
    delete ui;
}

void login_widget::on_btnConnect_clicked()
{
    crlib::ip_address ip;

    if (try_get_ip(ip))
    {
        emit begin_connect(ip);
    }
}

void login_widget::on_btnHost_clicked()
{
    crlib::ip_address ip;

    if (try_get_ip(ip))
    {
        emit begin_host(ip);
    }
}

bool login_widget::try_get_ip(crlib::ip_address &out_ip)
{
    bool result = true;

    crlib::string port_str(ui->txtPort->text().toStdWString());
    port_str.trim();

    uint16_t port;

    try
    {
        port = port_str.to_number<uint16_t>();
    }
    catch (std::exception&)
    {
        ui->txtPort->start_error_animation();
        result = false;
    }

    crlib::string ip_str(ui->txtIP->text().toStdWString());
    ip_str.trim();

    if (ip_str.to_lower_new() == L"localhost")
    {
        ip_str = L"127.0.0.1";
    }

    try
    {
        out_ip = crlib::ip_address(ip_str, port);
    }
    catch (std::exception&)
    {
        ui->txtIP->start_error_animation();
        result = false;
    }

    return result;
}
