#pragma once

#include <QStackedWidget>

class fading_stacked_widget : public QStackedWidget
{
    Q_OBJECT

public:
    fading_stacked_widget(QWidget* parent = nullptr);
    ~fading_stacked_widget();

    void set_fade_out_speed(int millis);
    void set_fade_int_speed(int millis);

    void add_widget_page(QWidget* w);

public slots:
    void fade_next();
    void fade_prev();
    void fade_index(int index);

private:
    int m_fade_in_speed;
    int m_fade_out_speed;
};
