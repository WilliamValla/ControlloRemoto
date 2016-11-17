#pragma once

#include <QLineEdit>

class validation_text_box : public QLineEdit
{
private:
    struct initializer
    {
        initializer();
        static QVariant text_compare(const QString& start, const QString& end, qreal progress);
    };

    Q_OBJECT

public:
    explicit validation_text_box(QWidget* parent = 0);
    ~validation_text_box();

signals:
    void on_animations_finished();

public slots:
    void start_error_animation();

private:
    static initializer m_initializer;
};
