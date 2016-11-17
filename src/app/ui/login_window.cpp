#include "login_window.h"
#include "ui_login_window.h"

#include <QDebug>
#include "main_window.h"
#include "../network/connection_manager.h"

login_window& login_window::instance()
{
    static login_window w;
    return w;
}

login_window::login_window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::login_window),
    m_loading_widget(new loading_widget),
    m_login_widget(new login_widget)
{
    ui->setupUi(this);

    connect(m_login_widget, &login_widget::begin_connect, this, [this](const crlib::ip_address& ip)
    {
        connection_manager::instance().connect_to_server(ip);

        m_loading_widget->update_text("Connecting to host...");
        ui->stackedWidget->fade_index(1);
    });

    connect(m_login_widget, &login_widget::begin_host, this, [this](const crlib::ip_address& ip)
    {
        connection_manager::instance().host_server(ip);

        crlib::string str = ip.to_string();
        m_loading_widget->update_text("Waiting for client (" + QString::fromWCharArray(str.data(), str.size()) + ":" + QString::number(ip.port()) + ")");
        ui->stackedWidget->fade_index(1);
    });

    ui->stackedWidget->add_widget_page(m_login_widget);
    ui->stackedWidget->add_widget_page(m_loading_widget);
    ui->stackedWidget->widget(1)->setStyleSheet("border: 0px");
}

void login_window::on_connected()
{
    qDebug("Connected to remote host");
    hide();
    main_window::instance().show();
}

void login_window::on_connection_failed()
{
    qDebug("Connection to remote host timed out");
    ui->stackedWidget->fade_index(0);
}

void login_window::on_client_connected()
{
    qDebug("Remote client connected");
    hide();
    main_window::instance().show();
}

login_window::~login_window()
{
    delete ui;
}
