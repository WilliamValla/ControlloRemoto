#include <iostream>
#include <QDebug>
#include <QApplication>
#include <QFile>
#include <QTextStream>

#include <crlib/string/string.h>
#include "ui/login_window.h"
#include "ui/main_window.h"

void load_style()
{
    QFile f(":qdarkstyle/style.qss");

    if (!f.exists())
    {
        std::cout << "Failed to load stylesheet: file not found\n";
    }
    else
    {
        try
        {
            f.open(QFile::ReadOnly | QFile::Text);
            QTextStream ts(&f);
            qApp->setStyleSheet(ts.readAll());

            qDebug("Loaded stylesheet");
        }
        catch (std::exception&)
        {
            qDebug("Loaded stylesheet");
        }
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qDebug("Application started");
    load_style();

    qRegisterMetaType<crlib::string>("crlib::string");

    login_window::instance().show();

    return a.exec();

}
