#include "validation_text_box.h"

#include <QVariantAnimation>
#include <QPropertyAnimation>

validation_text_box::initializer validation_text_box::m_initializer;

validation_text_box::validation_text_box(QWidget* parent) :
    QLineEdit(parent)
{
}

validation_text_box::~validation_text_box()
{
}

void validation_text_box::start_error_animation()
{
    QPropertyAnimation* anim = new QPropertyAnimation(this, "styleSheet");
    anim->setDuration(200);
    anim->setKeyValueAt(0.0, "");
    anim->setKeyValueAt(0.5, "border: 1px solid red");
    anim->setKeyValueAt(1.0, "");
    anim->setLoopCount(5);
    QObject::connect(anim, &QPropertyAnimation::finished, this, [this](){ emit on_animations_finished(); });
    anim->start(QPropertyAnimation::DeleteWhenStopped);
}

validation_text_box::initializer::initializer()
{
    qRegisterAnimationInterpolator<QString>(&validation_text_box::initializer::text_compare);
}

QVariant validation_text_box::initializer::text_compare(const QString& start, const QString& end, qreal progress)
{
    return progress < 1 ? start : end;
}
