#include "printerdialog.h"
#include <QApplication>
#include <QTranslator>
#include <QSharedMemory>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

    QSharedMemory shared_memory;
    shared_memory.setKey("emindsoft.com.cn");
    if(shared_memory.attach())
    {
        QMessageBox::information(0,"Information","This program is running already.",QMessageBox::Yes);
        return 0;
    }
    if(shared_memory.create(1))
    {
        QTranslator *translator = new QTranslator;
        translator->load(":/lang_zh.qm");
        a.installTranslator(translator);
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
