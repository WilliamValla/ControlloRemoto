#include "chat_widget.h"
#include "ui_chat_widget.h"
#include "../../network/network_out_message.h"

chat_widget::chat_widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::chat_widget)
{
    ui->setupUi(this);
}

chat_widget::~chat_widget()
{
    delete ui;
}

void chat_widget::on_msg_received(const crlib::string& str)
{
    ui->textEdit->append("user: " + QString::fromWCharArray(str.data(), str.size()));
}

void chat_widget::on_lineEdit_returnPressed()
{
    QString str = ui->lineEdit->text().trimmed();

    if (!str.isEmpty())
    {
        ui->textEdit->append("me: " + str);
        network_out_msg(crlib::string(str.toStdWString())).send();
    }

    ui->lineEdit->clear();
}
