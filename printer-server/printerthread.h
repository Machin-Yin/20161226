#ifndef PRINTERTHREAD_H
#define PRINTERTHREAD_H

#include <QThread>
#include <QtNetwork>
#include <QTcpSocket>
#include <QPrinter>
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <QMutex>


class PrinterThread : public QThread
{
	Q_OBJECT
public:
	PrinterThread(int socketDescriptor, QObject *parent = 0);
	void run() ;
	void sendMessage(QString messtr);
	QString recMessage();
	bool recFile();
	void setDefPrinter(int num, QString fileName1);
    void setDefPrinter(QString printer_name, QString fileName1);

    void doPrint(QString fileName2);
	void terminatePrg();
	void remTerm(QString fileName3);
	int cliPnum;
    QString prn_name;
    short copyCount;
signals:
	void error(QTcpSocket::SocketError socketError);

private:
	int socketDescriptor;
	QTcpServer tcpServer;
	QTcpSocket *tcpSocket;
	qint64 totalBytes;
	qint64 bytesReceived;
	qint64 fileNameSize;
	QString fileName;
    QString tempFolder;
    QString tempFile;
	QFile *localFile;
	QByteArray inBlock;
	quint16 blockSize;
	QString message;
    QMutex mutex;

};

#endif // PRINTERTHREAD_H
