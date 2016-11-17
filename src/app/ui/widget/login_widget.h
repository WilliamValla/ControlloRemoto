#pragma once

#include <QWidget>
#include "crlib/network/ip_address.h"
#include "loading_widget.h"

namespace Ui {
class login_widget;
}

class login_widget : public QWidget
{
    Q_OBJECT

public:
    explicit login_widget(QWidget *parent = 0);
    ~login_widget();

signals:
    void begin_host(const crlib::ip_address& ip);
    void begin_connect(const crlib::ip_address& ip);

private slots:
    void on_btnConnect_clicked();
    void on_btnHost_clicked();

protected:
    bool try_get_ip(crlib::ip_address& out_ip);

private:
    Ui::login_widget* ui;
    loading_widget* m_loading_widget;

};
