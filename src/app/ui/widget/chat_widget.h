#pragma once

#include <QWidget>
#include <crlib/string/string.h>

namespace Ui {
class chat_widget;
}

class chat_widget : public QWidget
{
    Q_OBJECT

public:
    explicit chat_widget(QWidget *parent = 0);
    ~chat_widget();

public slots:
    void on_msg_received(const crlib::string& str);

private slots:
    void on_lineEdit_returnPressed();

private:
    Ui::chat_widget *ui;
};

