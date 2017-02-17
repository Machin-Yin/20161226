#include "printerdialog.h"
#include <QApplication>
#include <QTranslator>
#include <QSharedMemory>
#include <QObject>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

    QFile styleSheet(":/mystyle.qss");
    if (!styleSheet.open(QIODevice::ReadOnly))
    {
        qWarning("Can't open the style sheet file.");
        return 0;
    }
    a.setStyleSheet(styleSheet.readAll());

    QTranslator *translator = new QTranslator;
    translator->load(":/lang_zh.qm");
    a.installTranslator(translator);
    QSharedMemory shared_memory;
    shared_memory.setKey("emindsoft.com.cn");
    if(shared_memory.attach())
    {
        QMessageBox::information(0,QObject::tr("Information"),QObject::tr("This program is running already!"),QMessageBox::Yes);
        return 0;
    }
    if(shared_memory.create(1))
    {

        PrinterDialog w;
        if ( argc>1 && (argv[1]==(char *)"//min" ))
        {
            w.hide();
        }
        else
        {

            w.showNormal();
        }
    //	w.show();
        return a.exec();
    }
    return -1;
}
