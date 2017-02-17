#ifndef PRINTERDIALOG_H
#define PRINTERDIALOG_H

#include "printerserver.h"
#include <QDialog>
#include <QSystemTrayIcon>
#include <QAction>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QCloseEvent>
//#include <QPrinter>


namespace Ui {
	class PrinterDialog;
}

class PrinterDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PrinterDialog(QWidget *parent = 0);
	~PrinterDialog();
    void setAutoStart(bool is_auto_start);

private:
	Ui::PrinterDialog *ui;
	PrinterServer printerServer;
	qint16 tcpPort;
    void getIP();

/*************** systray *************/
    QAction *restoreAction;
    QAction *quitAction;
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    void createTrayIcon();
    void createActions();
    void setVisible(bool visible);
    void setIcon();
    void closeEvent(QCloseEvent *event);

private slots:
    void regetIP();
	void on_flushButton_clicked();
    void iconIsActived(QSystemTrayIcon::ActivationReason reason);
};

#endif // PRINTERDIALOG_H
