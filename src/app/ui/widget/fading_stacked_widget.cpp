#include "fading_stacked_widget.h"

#include <QGraphicsEffect>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QBoxLayout>

fading_stacked_widget::fading_stacked_widget(QWidget* parent) :
    QStackedWidget(parent),
    m_fade_in_speed(500),
    m_fade_out_speed(500)
{
}

fading_stacked_widget::~fading_stacked_widget()
{   
}

void fading_stacked_widget::set_fade_out_speed(int millis)
{
    m_fade_in_speed = millis;
}

void fading_stacked_widget::set_fade_int_speed(int millis)
{
    m_fade_out_speed = millis;
}

void fading_stacked_widget::add_widget_page(QWidget* w)
{
    QWidget* page = new QWidget(this);
    QHBoxLayout* h_layout = new QHBoxLayout(page);
    QVBoxLayout* v_layout = new QVBoxLayout();
    v_layout->addWidget(w);
    h_layout->addLayout(v_layout);
    //page->setLayout(h_layout);
    this->addWidget(page);
}

void fading_stacked_widget::fade_next()
{
    int index = currentIndex() + 1;

    if (index >= count())
    {
        index = 0;
    }

    fade_index(index);
}

void fading_stacked_widget::fade_prev()
{
    int index = currentIndex();

    if (index == 0)
    {
        index = count() - 1;
    }

    fade_index(index);
}

void fading_stacked_widget::fade_index(int index)
{
    QSequentialAnimationGroup* anim_group = new QSequentialAnimationGroup;
    QWidget* current = this->currentWidget();
    QWidget* next = this->widget(index);

    QGraphicsOpacityEffect* current_opacity = new QGraphicsOpacityEffect(current);
    current_opacity->setOpacity(1.0);
    current->setGraphicsEffect(current_opacity);

    QPropertyAnimation* fade_out = new QPropertyAnimation(current_opacity, "opacity");
    fade_out->setDuration(m_fade_out_speed);
    fade_out->setStartValue(1.0);
    fade_out->setEndValue(0.0);

    anim_group->addAnimation(fade_out);

    QGraphicsOpacityEffect* next_opacity = new QGraphicsOpacityEffect(next);
    next_opacity->setOpacity(1.0);
    next->setGraphicsEffect(next_opacity);

    QPropertyAnimation* fade_in = new QPropertyAnimation(next_opacity, "opacity");
    fade_in->setDuration(m_fade_in_speed);
    fade_in->setStartValue(0.0);
    fade_in->setEndValue(1.0);

    anim_group->addAnimation(fade_in);

    QObject::connect(fade_out, &QPropertyAnimation::finished, this, [this, index](){ this->setCurrentIndex(index); });
    anim_group->start(QAbstractAnimation::DeletionPolicy::DeleteWhenStopped);
}
