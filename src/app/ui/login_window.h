#pragma once

#include <QMainWindow>
#include "widget/login_widget.h"
#include "widget/loading_widget.h"

namespace Ui {
class login_window;
}

class login_window : public QMainWindow
{
    Q_OBJECT

public:
    static login_window& instance();

public slots:
    void on_connected();
    void on_connection_failed();

    void on_client_connected();

protected:
    explicit login_window(QWidget *parent = 0);

public:
    ~login_window();

private:
    Ui::login_window* ui;
    login_widget* m_login_widget;
    loading_widget* m_loading_widget;
};
