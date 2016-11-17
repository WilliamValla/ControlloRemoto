#include "main_window.h"
#include "ui_main_window.h"

#include "../network/connection_manager.h"
#include "../gfx/desktop_duplicator.h"
#include "../gfx/frame_buffer.h"

main_window& main_window::instance()
{
    static main_window w;
    return w;
}

main_window::main_window(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::main_window),
    m_gl_widget(nullptr)
{
    ui->setupUi(this);

    QObject::connect(&connection_manager::instance(), &connection_manager::on_msg_received, ui->widget, &chat_widget::on_msg_received);

    if (connection_manager::instance().is_server())
    {
        desktop_duplicator::instance().initialize();
    }
    else
    {
        m_gl_widget = new opengl_widget(this);
        ui->centralwidget->layout()->addWidget(m_gl_widget);
        this->resize(800, 600);

        QObject::connect(&frame_buffer::instance(), &frame_buffer::signal_texture_update, m_gl_widget, &opengl_widget::update_texture);
    }
}

main_window::~main_window()
{
    delete ui;
}
