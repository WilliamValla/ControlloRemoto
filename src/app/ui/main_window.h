#pragma once

#include <QMainWindow>
#include "widget/opengl_widget.h"

namespace Ui {
class main_window;
}

class main_window : public QMainWindow
{
    Q_OBJECT

public:
    static main_window& instance();
    ~main_window();

protected:
    explicit main_window(QWidget *parent = 0);

private:
    Ui::main_window *ui;
    opengl_widget* m_gl_widget;
};
