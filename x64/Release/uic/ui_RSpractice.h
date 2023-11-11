/********************************************************************************
** Form generated from reading UI file 'RSpractice.ui'
**
** Created by: Qt User Interface Compiler version 5.9.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RSPRACTICE_H
#define UI_RSPRACTICE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RSpracticeClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *RSpracticeClass)
    {
        if (RSpracticeClass->objectName().isEmpty())
            RSpracticeClass->setObjectName(QStringLiteral("RSpracticeClass"));
        RSpracticeClass->resize(600, 400);
        menuBar = new QMenuBar(RSpracticeClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        RSpracticeClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(RSpracticeClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        RSpracticeClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(RSpracticeClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        RSpracticeClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(RSpracticeClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        RSpracticeClass->setStatusBar(statusBar);

        retranslateUi(RSpracticeClass);

        QMetaObject::connectSlotsByName(RSpracticeClass);
    } // setupUi

    void retranslateUi(QMainWindow *RSpracticeClass)
    {
        RSpracticeClass->setWindowTitle(QApplication::translate("RSpracticeClass", "RSpractice", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class RSpracticeClass: public Ui_RSpracticeClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RSPRACTICE_H
