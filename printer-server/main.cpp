#include "printerdialog.h"
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
    QTranslator *translator = new QTranslator;
    translator->load("lang_zh.qm");
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
