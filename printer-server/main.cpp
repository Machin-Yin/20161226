#include "printerdialog.h"
#include <QApplication>
#include <QTranslator>
#include <QSharedMemory>
#include <QObject>
#include <QDateTime>

//#include "StdAfx.h"
#include <Windows.h>

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    mutex.lock();

    QByteArray localMsg = msg.toLocal8Bit();

    QString strMsg("");
    switch(type)
    {
    case QtDebugMsg:
        strMsg = QString("Debug:");
        break;
    case QtWarningMsg:
        strMsg = QString("Warning:");
        break;
    case QtCriticalMsg:
        strMsg = QString("Critical:");
        break;
    case QtFatalMsg:
        strMsg = QString("Fatal:");
        break;
    }

    QString strDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString strMessage = QString("Message:%1 File:%2  Line:%3  Function:%4  DateTime:%5")
            .arg(localMsg.constData()).arg(context.file).arg(context.line).arg(context.function).arg(strDateTime);

    QFile file("log.txt");
    file.open(QIODevice::ReadWrite | QIODevice::Append);
    QTextStream stream(&file);
    stream << strMessage << "\r\n";
    file.flush();
    file.close();

    mutex.unlock();
}

std::string get_app_run_path()
{
    char szFilePath[MAX_PATH + 1];
    GetModuleFileNameA(NULL, szFilePath, MAX_PATH);
    (strrchr(szFilePath, ('\\')))[1] = 0;//删除文件名，只获得路径
    return std::string(szFilePath);
}

bool GetCurrentProcessDirectory(std::wstring &wstrCurrentProcessPath)
{
    bool is_success = false;

    do
    {
        WCHAR *lpProcessPath = (WCHAR* )malloc(sizeof(WCHAR)*MAX_PATH);
        if (lpProcessPath)
        {
            ZeroMemory(lpProcessPath, MAX_PATH);
            DWORD nBufferLength = MAX_PATH;
            is_success = GetCurrentDirectory(nBufferLength, lpProcessPath);
            wstrCurrentProcessPath = lpProcessPath;
            free(lpProcessPath);
            lpProcessPath = NULL;
        }

    } while (false);

    return is_success;
}



int main(int argc, char *argv[])
{
    std::wstring wstrCurrentProcessPath;
    GetCurrentProcessDirectory(wstrCurrentProcessPath);
    std::string strCurrentProcessPath = get_app_run_path();
    SetCurrentDirectoryA(strCurrentProcessPath.c_str());

    QApplication a(argc, argv);

    //    qInstallMessageHandler(myMessageOutput);

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


