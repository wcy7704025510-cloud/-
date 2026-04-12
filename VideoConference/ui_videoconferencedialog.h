/********************************************************************************
** Form generated from reading UI file 'videoconferencedialog.ui'
**
** Created by: Qt User Interface Compiler version 5.12.11
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIDEOCONFERENCEDIALOG_H
#define UI_VIDEOCONFERENCEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_VedioConferenceDialog
{
public:
    QPushButton *pb_icon;
    QLabel *lb_name;
    QPushButton *pb_set;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QToolButton *tb_join;
    QToolButton *tb_create;
    QToolButton *tb_tool3;

    void setupUi(QDialog *VedioConferenceDialog)
    {
        if (VedioConferenceDialog->objectName().isEmpty())
            VedioConferenceDialog->setObjectName(QString::fromUtf8("VedioConferenceDialog"));
        VedioConferenceDialog->resize(450, 800);
        QFont font;
        font.setPointSize(10);
        VedioConferenceDialog->setFont(font);
        pb_icon = new QPushButton(VedioConferenceDialog);
        pb_icon->setObjectName(QString::fromUtf8("pb_icon"));
        pb_icon->setGeometry(QRect(30, 20, 60, 60));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/tx/6.png"), QSize(), QIcon::Normal, QIcon::Off);
        pb_icon->setIcon(icon);
        pb_icon->setIconSize(QSize(60, 60));
        pb_icon->setFlat(true);
        lb_name = new QLabel(VedioConferenceDialog);
        lb_name->setObjectName(QString::fromUtf8("lb_name"));
        lb_name->setGeometry(QRect(100, 40, 291, 30));
        QFont font1;
        font1.setPointSize(12);
        lb_name->setFont(font1);
        pb_set = new QPushButton(VedioConferenceDialog);
        pb_set->setObjectName(QString::fromUtf8("pb_set"));
        pb_set->setGeometry(QRect(410, 40, 30, 30));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/images/sett.png"), QSize(), QIcon::Normal, QIcon::Off);
        pb_set->setIcon(icon1);
        pb_set->setIconSize(QSize(30, 30));
        pb_set->setFlat(true);
        scrollArea = new QScrollArea(VedioConferenceDialog);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setGeometry(QRect(30, 240, 391, 521));
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 391, 521));
        scrollArea->setWidget(scrollAreaWidgetContents);
        layoutWidget = new QWidget(VedioConferenceDialog);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(0, 120, 451, 90));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        tb_join = new QToolButton(layoutWidget);
        tb_join->setObjectName(QString::fromUtf8("tb_join"));
        tb_join->setMinimumSize(QSize(100, 88));
        tb_join->setMaximumSize(QSize(100, 88));
        QFont font2;
        font2.setFamily(QString::fromUtf8("\345\256\213\344\275\223"));
        font2.setPointSize(10);
        font2.setBold(true);
        font2.setWeight(75);
        tb_join->setFont(font2);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/images/history.png"), QSize(), QIcon::Normal, QIcon::Off);
        tb_join->setIcon(icon2);
        tb_join->setIconSize(QSize(60, 60));
        tb_join->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout->addWidget(tb_join);

        tb_create = new QToolButton(layoutWidget);
        tb_create->setObjectName(QString::fromUtf8("tb_create"));
        tb_create->setMinimumSize(QSize(100, 88));
        tb_create->setMaximumSize(QSize(100, 88));
        tb_create->setFont(font2);
        tb_create->setIcon(icon2);
        tb_create->setIconSize(QSize(60, 60));
        tb_create->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout->addWidget(tb_create);

        tb_tool3 = new QToolButton(layoutWidget);
        tb_tool3->setObjectName(QString::fromUtf8("tb_tool3"));
        tb_tool3->setMinimumSize(QSize(100, 88));
        tb_tool3->setMaximumSize(QSize(100, 88));
        tb_tool3->setFont(font2);
        tb_tool3->setIcon(icon2);
        tb_tool3->setIconSize(QSize(60, 60));
        tb_tool3->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

        horizontalLayout->addWidget(tb_tool3);


        retranslateUi(VedioConferenceDialog);

        QMetaObject::connectSlotsByName(VedioConferenceDialog);
    } // setupUi

    void retranslateUi(QDialog *VedioConferenceDialog)
    {
        VedioConferenceDialog->setWindowTitle(QApplication::translate("VedioConferenceDialog", "VedioConferenceDialog", nullptr));
        pb_icon->setText(QString());
        lb_name->setText(QApplication::translate("VedioConferenceDialog", "\346\265\213\350\257\225\347\224\250\346\210\267", nullptr));
        pb_set->setText(QString());
        tb_join->setText(QApplication::translate("VedioConferenceDialog", "\345\212\240\345\205\245\344\274\232\350\256\256", nullptr));
        tb_create->setText(QApplication::translate("VedioConferenceDialog", "\345\277\253\351\200\237\344\274\232\350\256\256", nullptr));
        tb_tool3->setText(QApplication::translate("VedioConferenceDialog", "\351\242\204\345\256\232\344\274\232\350\256\256", nullptr));
    } // retranslateUi

};

namespace Ui {
    class VedioConferenceDialog: public Ui_VedioConferenceDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIDEOCONFERENCEDIALOG_H
