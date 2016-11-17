#include "loading_widget.h"
#include "ui_loading_widget.h"

#include <QMovie>

loading_widget::loading_widget(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::loading_widget)
{
    ui->setupUi(this);

    ui->lblAnimation->setAttribute(Qt::WA_TranslucentBackground, true);

    QMovie* anim = new QMovie(ui->lblAnimation);
    anim->setFileName(":/animations/loading.gif");
    ui->lblAnimation->setMovie(anim);
    anim->start();
}

loading_widget::~loading_widget()
{
    delete ui;
}

void loading_widget::update_text(const QString& text)
{
    ui->lblText->setText(text);
}
