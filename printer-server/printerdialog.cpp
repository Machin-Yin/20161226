#include "printerdialog.h"
#include "ui_printerdialog.h"
#include <QDebug>
#include <QTimer>
#include <QNetworkInterface>
#include <qfile.h>
#include <QSettings>

#include <winsock2.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#pragma comment(lib, "IPHLPAPI.lib")

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

using namespace std;

PrinterDialog::PrinterDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrinterDialog)
{
    qDebug() << __FUNCTION__ << endl;
    ui->setupUi(this);
    Qt::WindowFlags flags = Qt::Dialog;
    //        flags |= Qt::CustomizeWindowHint;
    flags |= Qt::WindowMinimizeButtonHint;
    flags |= Qt::WindowCloseButtonHint;
    setWindowFlags(flags);

    setAutoStart(true);

    QFile authfile("authcode.txt");
    if (authfile.open(QIODevice::ReadOnly))
    {
        QString authcode = "";
        authcode = QString(authfile.read(100));
        authfile.close();
        qDebug() << "authcode" << authcode << endl;
        ui->authleEdit->setText(authcode);
    }
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(regetIP()));
    timer->start(200);

    /************  systray ****************/
    createActions();
    createTrayIcon();
    setVisible(true);
    trayIcon->show();
    connect(trayIcon , SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconIsActived(QSystemTrayIcon::ActivationReason)));

    tcpPort = 6666;
    if (!printerServer.listen(QHostAddress::Any, tcpPort))
    {
        qDebug() << printerServer.errorString();
        close();
        return;
    }

}


PrinterDialog::~PrinterDialog()
{
    delete ui;
}

void PrinterDialog::regetIP()
{
//    ui->ipleEdit->setText("");
    QLineEdit *lineEdit[8]={ui->ipEdit1,ui->ipEdit2,ui->ipEdit3,ui->ipEdit4,ui->ipEdit5,ui->ipEdit6,ui->ipEdit7,ui->ipEdit8};
    for(int j=0;j<8;j++)
    {
        lineEdit[j]->setText("");
    }

    getIP();
}

void PrinterDialog::getIP()
{
    QLineEdit *lineEdit[8]={ui->ipEdit1,ui->ipEdit2,ui->ipEdit3,ui->ipEdit4,ui->ipEdit5,ui->ipEdit6,ui->ipEdit7,ui->ipEdit8};
    int i = 0;

    PIP_ADAPTER_INFO pAdapterInfo;
    PIP_ADAPTER_INFO pAdapter = NULL;
    DWORD dwRetVal = 0;

    ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
    pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(sizeof(IP_ADAPTER_INFO));
    if (pAdapterInfo == NULL) {
        printf("Error allocating memory needed to call GetAdaptersinfo\n");
        return;
    }
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
        FREE(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(ulOutBufLen);
        if (pAdapterInfo == NULL) {
            printf("Error allocating memory needed to call GetAdaptersinfo\n");
            return;
        }
    }

    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
        pAdapter = pAdapterInfo;
        while (pAdapter) {
            switch (pAdapter->Type) {
            case MIB_IF_TYPE_OTHER:
                printf("Other\n");
                break;
            case MIB_IF_TYPE_ETHERNET:
//                printf("Ethernet\n");
                break;
            case MIB_IF_TYPE_TOKENRING:
                printf("Token Ring\n");
                break;
            case MIB_IF_TYPE_FDDI:
                printf("FDDI\n");
                break;
            case MIB_IF_TYPE_PPP:
                printf("PPP\n");
                break;
            case MIB_IF_TYPE_LOOPBACK:
                printf("Lookback\n");
                break;
            case MIB_IF_TYPE_SLIP:
                printf("Slip\n");
                break;
            default:
//                printf("Unknown type %ld\n", pAdapter->Type);
                break;
            }
            string ipstr0 = "0.0.0.0";
            if(pAdapter->IpAddressList.IpAddress.String != ipstr0)
            {

                lineEdit[i]->setText(pAdapter->IpAddressList.IpAddress.String);
            }
            else
            {
                i--;
            }
            pAdapter = pAdapter->Next;
            i++;
            printf("\n");
        }
    }
    else {
        printf("GetAdaptersInfo failed with error: %d\n", dwRetVal);

    }
    if (pAdapterInfo)
        FREE(pAdapterInfo);
}

void PrinterDialog::on_flushButton_clicked()
{    
    QString authcode = ui->authleEdit->text();
    if(authcode == "")
    {
        QMessageBox::information(0,QObject::tr("Information"),QObject::tr("The authcode can't null!"),QMessageBox::Yes);
        QFile authfile("authcode.txt");
        if (authfile.open(QIODevice::ReadOnly))
        {
            QString oldcode = "";
            oldcode = QString(authfile.read(100));
            authfile.close();
            ui->authleEdit->setText(oldcode);
        }
    }
    else
    {
        QFile file("authcode.txt");
        if (file.open(QIODevice::WriteOnly))
        {
            file.write(authcode.toStdString().c_str());
            file.close();
        }
    }
}

//void PrinterDialog::setAutoStart(bool is_auto_start)
//{
//    QString application_name = QApplication::applicationName();
//    QSettings *settings = new QSettings("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
//    if(is_auto_start)
//    {
//        QString application_path = QApplication::applicationFilePath();
//        settings->setValue(application_name, application_path.replace('/', '\\'));
//    }
//    else
//    {
//        settings->remove(application_name);
//    }
//    delete settings;
//}

#include <QDir>

void PrinterDialog::setAutoStart(bool bAutoRun)
{
    //HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Run
    QSettings  reg("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",QSettings::NativeFormat);

    if (bAutoRun)
    {
         QString strAppPath=QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
        //strAppPath.replace(QChar('/'),QChar('\\'),Qt::CaseInsensitive);
        reg.setValue("wirtepad",strAppPath);
    }
    else
    {
        reg.setValue("wirtepad","");
    }

}

void PrinterDialog::iconIsActived(QSystemTrayIcon::ActivationReason reason)
{
    qDebug()<<__FUNCTION__<<endl;
    switch(reason)
    {
    case QSystemTrayIcon::Trigger:
    {
        if(this->isMinimized()|| this->isHidden())
        {
            showNormal();
            QFile authfile("authcode.txt");
            if (authfile.open(QIODevice::ReadOnly))
            {
                QString authcode = "";
                authcode = QString(authfile.read(100));
                authfile.close();
                ui->authleEdit->setText(authcode);
            }
            break;
        }
        else if(this->isVisible())
        {
            showMinimized();
            break;
        }
    }
    case QSystemTrayIcon::MiddleClick:
    {
        if(this->isMinimized()|| this->isHidden())
        {
            showNormal();
            QFile authfile("authcode.txt");
            if (authfile.open(QIODevice::ReadOnly))
            {
                QString authcode = "";
                authcode = QString(authfile.read(100));
                authfile.close();
                ui->authleEdit->setText(authcode);
            }
            break;
        }
        else if(this->isVisible())
        {
            showMinimized();
            break;
        }
    }
    default:
        break;
    }
}


void PrinterDialog::closeEvent(QCloseEvent *event)
{

    if (trayIcon->isVisible()) {
//        QMessageBox::information(this, tr("Systray"),
//                                 tr("The program will keep running in the "
//                                    "system tray. To terminate the program, "
//                                    "choose <b>Quit</b> in the context menu "
//                                    "of the system tray entry."));
        hide();
        event->ignore();
    }
}


void PrinterDialog::setVisible(bool visible)
{
    restoreAction->setEnabled(isMaximized() || !visible);
    QDialog::setVisible(visible);
}

void PrinterDialog::createActions()
{
    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, &QAction::triggered, this, &QWidget::showNormal);

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void PrinterDialog::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon ->setIcon(QIcon(":/image/tray.png"));
    trayIcon->setToolTip(tr("EmindCloudPrinter"));

}



