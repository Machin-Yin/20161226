#include "printerthread.h"
#include <QPrinterInfo>
#include <Tlhelp32.h>

//#define PRINTER_NUM 0
#define COPIES_NUM 1
#define DMPAPER_SIZE DMPAPER_A4


PrinterThread::PrinterThread(int socketDescriptor, QObject *parent)
	:QThread(parent), socketDescriptor(socketDescriptor)
{
	totalBytes = 0;
	bytesReceived = 0;
	fileNameSize = 0;
	blockSize = 0;
	cliPnum = 0;
}


void PrinterThread::run()
{
	qDebug() << __FUNCTION__ << endl;
	tcpSocket = new QTcpSocket();
	if (!tcpSocket->setSocketDescriptor(socketDescriptor))
	{
		emit error(tcpSocket->error());
		tcpSocket->close();
		return;
	}
	while (tcpSocket->waitForReadyRead())
	{
		QString bl = recMessage();
		if (bl == "Request printer list!")
		{
			qDebug() << "request printer list";
			QPrinterInfo pInfo;
			QStringList pname;
			pname = pInfo.availablePrinterNames();
			qDebug() << "panme:" << pname;
			foreach(auto a, pname)
			{
				qDebug() << a;
			}
			QString pstr = pname.join(",");
			sendMessage(pstr);

		}
		else if (bl == "begin send file") {
			qDebug() << "begin send file";
			while (tcpSocket->waitForReadyRead())
			{
				if (recFile())
					break;
			}
		}
		else if (bl == "DefaultPrinter") {
			while (tcpSocket->waitForReadyRead())
			{
				QString index = recMessage();
				cliPnum = index.toInt();
				qDebug() << "cliPnum==" << cliPnum << endl;
				break;
			}

		}
		else {
			qDebug() << "authcode" << bl;
			QFile authfile("authcode.txt");
			if (authfile.open(QIODevice::ReadOnly))
			{
				QString authcode = "";
				authcode = QString(authfile.read(10));
				authfile.close();
				if (bl != authcode)
				{
					sendMessage("AUTH WRONG");
					return;
				}
			}
			sendMessage("OK");
		}
	}
}


void PrinterThread::sendMessage(QString messtr)
{
	qDebug() << __FUNCTION__ << endl;
	QByteArray authblock;
	QDataStream out(&authblock, QIODevice::WriteOnly);
	out << (quint16)0 <<messtr;
	out.device()->seek(0);
	out << (quint16)(authblock.size() - sizeof(quint16));
	tcpSocket->write(authblock);
	tcpSocket->flush();
}

QString PrinterThread::recMessage()
{
	qDebug() << __FUNCTION__ << endl;
	QDataStream in(tcpSocket);
	if (blockSize == 0)
	{
		qDebug() <<"blockSize == 0 tcpSocket->bytesAvailable()"<< tcpSocket->bytesAvailable() << endl;
		if (tcpSocket->bytesAvailable() < (int)sizeof(quint16))
		{
			qDebug() << "return" << endl;
			return 0;
		}
		in >> blockSize;
	}
	if (tcpSocket->bytesAvailable() < blockSize)
	{
		return 0;
	}
	in >> message;
	qDebug() << "message ==" << message << endl;
	blockSize = 0;
	return message;
}


bool PrinterThread::recFile() 
{
	qDebug() << __FUNCTION__ << endl;
	QDataStream in(tcpSocket);
	qDebug() << "tcpSocket->bytesAvailable()" << tcpSocket->bytesAvailable() << endl;
	while (tcpSocket->bytesAvailable() > 0)
	{

		//in.setVersion(QDataStream::Qt_4_6);
		qDebug() << "rec begin" << endl;
		qDebug() << "bytesReceived======" << bytesReceived << endl;
		if (bytesReceived < sizeof(qint64) * 2)
		{
			if ((tcpSocket->bytesAvailable() >= sizeof(qint64) * 2) && (fileNameSize == 0))
			{
				in >> totalBytes >> fileNameSize;
				bytesReceived += sizeof(qint64) * 2;
				qDebug() << "totalBytes=" << totalBytes << endl;

				qDebug() << "bytesReceived=" << bytesReceived << endl;
			}
			if ((tcpSocket->bytesAvailable() >= fileNameSize)
				&& (fileNameSize != 0))
			{
				qDebug() << "(tcpSocket->bytesAvailable() >= fileNameSize)" << endl;
				in >> fileName;
				qDebug() << "fileName=" << fileName << endl;
				bytesReceived += fileNameSize;
				localFile = new QFile(fileName);
				if (!localFile->open(QFile::ReadWrite))
				{
					qDebug() << "open file error!";
					return true;
				}
			}
			else
			{
				return true;
			}
		}
		if (bytesReceived < totalBytes)
		{
			bytesReceived += tcpSocket->bytesAvailable();
			inBlock = tcpSocket->readAll();
			localFile->write(inBlock);
			inBlock.resize(0);
		}

		qDebug() << "bytesReceived" << bytesReceived << endl;
		if (bytesReceived == (totalBytes + 16))

		{
			qDebug() << "bytesReceived == totalBytes:  Receive" << fileName << "success!" << endl;

			bytesReceived = 0;
			//tcpSocket->close();
			fileNameSize = 0;
			localFile->close();
			delete localFile;
			setDefPrinter(cliPnum, fileName);
			return true;
		}
	}
	return false;
}


void PrinterThread::setDefPrinter(int num,QString fileName1)
{
	qDebug() << __FUNCTION__ << endl;

	QPrinterInfo pInfo;
	QStringList pname;
	pname = pInfo.availablePrinterNames();
	qDebug() << "panme:" << pname;
	foreach(auto a, pname)
	{
		qDebug() << a;
	}

	int printer_num = num;
	QPrinter printer;
	QString printer_name;
	printer_name = pname.at(printer_num);
	printer.setPrinterName(printer_name);
	qDebug() << "class::printerName:" << printer.printerName() << endl;

	TCHAR szBufferDefaultPrinterName[256] = { 0 };
	memset(szBufferDefaultPrinterName, 0, 256);
	DWORD length = 256;
	GetDefaultPrinter(szBufferDefaultPrinterName, &length);
	qDebug() << "szBufferDefaultPrinterName===" << szBufferDefaultPrinterName << endl;
	LPCWSTR printerName = (const wchar_t*)printer_name.utf16();
	SetDefaultPrinter(printerName);

	/****** Set printer property! ******/

	LPDEVMODE lpDevMode = NULL;
	HANDLE hPrinter;
	DWORD dwNeeded, dwRet;

	TCHAR defPrinter[256] = { 0 };
	memset(defPrinter, 0, 256);
	DWORD lengthDefpr = 256;
	GetDefaultPrinter(defPrinter, &lengthDefpr);
	qDebug() << "defPrinter===" << defPrinter << endl;
	qDebug() << "defPrinter===" << defPrinter << endl;
	if (!OpenPrinter(defPrinter, &hPrinter, NULL))
	{
		qDebug() << "OpenPrinter==" << !OpenPrinter(defPrinter, &hPrinter, NULL) << endl;
		return;
	}
	//get real size of DEVMODE
	dwNeeded = DocumentProperties(NULL, hPrinter, defPrinter, NULL, NULL, 0);
	lpDevMode = (LPDEVMODE)malloc(dwNeeded);  
	dwRet = DocumentProperties(NULL, hPrinter, defPrinter, lpDevMode, NULL, DM_OUT_BUFFER);
	qDebug() << "dwRet==" << dwRet << endl;
	if (dwRet != IDOK)
	{
		free(lpDevMode);
		ClosePrinter(hPrinter);
		return;
	}
	if (lpDevMode->dmFields & DM_COPIES)
	{
		lpDevMode->dmCopies = COPIES_NUM;
		lpDevMode->dmFields |= DM_COPIES;
	}
	//if (lpDevMode->dmFields & DM_ORIENTATION)
	//{
	//	lpDevMode->dmOrientation = DMORIENT_LANDSCAPE;  //landscape:ºá    portrait:×Ý
	//	lpDevMode->dmOrientation |= DM_ORIENTATION;
	//}
	//if (lpDevMode->dmFields & DM_PAPERSIZE)
	//{
	//	lpDevMode->dmPaperSize = DMPAPER_SIZE;
	//	lpDevMode->dmOrientation |= DM_PAPERSIZE;
	//}
	dwRet = DocumentProperties(NULL, hPrinter, defPrinter, lpDevMode, lpDevMode, DM_IN_BUFFER | DM_OUT_BUFFER);
	if (dwRet != IDOK)
	{
		free(lpDevMode);
		return;
	}

	DWORD dw;
	PRINTER_INFO_2 *pi2;
	GetPrinter(hPrinter, 2, NULL, 0, &dw);
	pi2 = (PRINTER_INFO_2*)GlobalAllocPtr(GHND, dw);
	GetPrinter(hPrinter, 2, (LPBYTE)pi2, dw, &dw);

	qDebug() << "pi2->pDevMode before" << pi2->pDevMode << endl;
	qDebug() << "lpDevMode before" << lpDevMode << endl;

	pi2->pDevMode = lpDevMode;
	SetPrinter(hPrinter, 2, (LPBYTE)pi2, 0);

	QString filePath = fileName1;
	doPrint(&printer,filePath);

	ClosePrinter(hPrinter);
	GlobalFreePtr(pi2);

	//_sleep(10 * 1000);
	SetDefaultPrinter(szBufferDefaultPrinterName);
	//	setDefaultPdevmode(hPrinter, defdevmode);   ////

}


void PrinterThread::doPrint(QPrinter *printer, QString fileName2)
{
	int ret = 0;
	ret = (int)ShellExecute(NULL,
		QString("print").toStdWString().c_str(),
		fileName2.toStdWString().c_str(),
		NULL,
		NULL,
		SW_HIDE);

	qDebug() << "ret====" << ret << endl;

	remTerm(fileName2);

}

void PrinterThread::remTerm(QString fileName3)
{
	_sleep(1 * 1000);
	qDebug() << "*****  1s *****" << endl;
	if (QFile::remove(fileName3) == 1)
	{
		qDebug() << "delete" << fileName3 << "success!" << endl;
		terminatePrg();
		qDebug() << "kill AcroRd32.exe success!" << endl;
	}
	else
	{
		qDebug() << "delete" << fileName3 << "failure!" << endl;
		remTerm(fileName3);
	}
}

void PrinterThread::terminatePrg()
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32* processInfo = new PROCESSENTRY32;
	processInfo->dwSize = sizeof(PROCESSENTRY32);
	int index = 0;
	int ID = 0;
	while (Process32Next(hSnapShot, processInfo) != FALSE)
	{
		index++;
		int size = WideCharToMultiByte(CP_ACP, 0, processInfo->szExeFile, -1, NULL, 0, NULL, NULL);
		char *ch = new char[size + 1];
		if (WideCharToMultiByte(CP_ACP, 0, processInfo->szExeFile, -1, ch, size, NULL, NULL))
		{
			if (strstr(ch, "AcroRd32.exe"))
			{
				ID = processInfo->th32ProcessID;
				HANDLE hProcess;
				hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, ID);
				if (hProcess == NULL)
				{
					printf("Unable to get handle of process: ");
					printf("Error is: %d", GetLastError());
				}
				TerminateProcess(hProcess, 0);
				CloseHandle(hProcess);
			}
		}
	}
	CloseHandle(hSnapShot);
	delete processInfo;
}


