/********************************************************************************
** Form generated from reading UI file 'printerdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PRINTERDIALOG_H
#define UI_PRINTERDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_PrinterDialog
{
public:
    QPushButton *flushButton;
    QLabel *authLabel;
    QLabel *iplabel;
    QLineEdit *authleEdit;
    QTextEdit *ipleEdit;

    void setupUi(QDialog *PrinterDialog)
    {
        if (PrinterDialog->objectName().isEmpty())
            PrinterDialog->setObjectName(QStringLiteral("PrinterDialog"));
        PrinterDialog->resize(340, 186);
        PrinterDialog->setMinimumSize(QSize(340, 186));
        PrinterDialog->setMaximumSize(QSize(340, 186));
        PrinterDialog->setStyleSheet(QStringLiteral("background-color: rgb(255, 255, 255);"));
        flushButton = new QPushButton(PrinterDialog);
        flushButton->setObjectName(QStringLiteral("flushButton"));
        flushButton->setGeometry(QRect(276, 150, 47, 25));
        flushButton->setFocusPolicy(Qt::StrongFocus);
        flushButton->setAutoDefault(false);
        authLabel = new QLabel(PrinterDialog);
        authLabel->setObjectName(QStringLiteral("authLabel"));
        authLabel->setGeometry(QRect(9, 120, 111, 22));
        authLabel->setStyleSheet(QStringLiteral("color: rgb(51, 51, 51);"));
        authLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        iplabel = new QLabel(PrinterDialog);
        iplabel->setObjectName(QStringLiteral("iplabel"));
        iplabel->setGeometry(QRect(9, 40, 111, 22));
        iplabel->setLayoutDirection(Qt::LeftToRight);
        iplabel->setStyleSheet(QStringLiteral("color: rgb(51, 51, 51);"));
        iplabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        authleEdit = new QLineEdit(PrinterDialog);
        authleEdit->setObjectName(QStringLiteral("authleEdit"));
        authleEdit->setGeometry(QRect(120, 120, 155, 22));
        ipleEdit = new QTextEdit(PrinterDialog);
        ipleEdit->setObjectName(QStringLiteral("ipleEdit"));
        ipleEdit->setGeometry(QRect(120, 40, 131, 71));
        ipleEdit->setFrameShape(QFrame::NoFrame);
        iplabel->raise();
        flushButton->raise();
        authLabel->raise();
        authleEdit->raise();
        ipleEdit->raise();

        retranslateUi(PrinterDialog);

        QMetaObject::connectSlotsByName(PrinterDialog);
    } // setupUi

    void retranslateUi(QDialog *PrinterDialog)
    {
        PrinterDialog->setWindowTitle(QApplication::translate("PrinterDialog", "Emind Cloud Printer Setting", Q_NULLPTR));
        flushButton->setText(QApplication::translate("PrinterDialog", "Update", Q_NULLPTR));
        authLabel->setText(QApplication::translate("PrinterDialog", "Auth Code\357\274\232", Q_NULLPTR));
        iplabel->setText(QApplication::translate("PrinterDialog", "Server Ip\357\274\232", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class PrinterDialog: public Ui_PrinterDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PRINTERDIALOG_H
