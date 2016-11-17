#pragma once

#include <QWidget>

namespace Ui {
class loading_widget;
}

class loading_widget : public QWidget
{
    Q_OBJECT

public:
    explicit loading_widget(QWidget* parent = 0);
    ~loading_widget();

public slots:
    void update_text(const QString& text);

private:
    Ui::loading_widget* ui;
};
