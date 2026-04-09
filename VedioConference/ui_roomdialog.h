/********************************************************************************
** Form generated from reading UI file 'roomdialog.ui'
**
** Created by: Qt User Interface Compiler version 5.12.11
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ROOMDIALOG_H
#define UI_ROOMDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <usershow.h>

QT_BEGIN_NAMESPACE

class Ui_RoomDialog
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *wdg_title;
    QHBoxLayout *horizontalLayout_3;
    QLabel *lb_title;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *pb_min;
    QPushButton *pb_max;
    QPushButton *pb_close;
    QHBoxLayout *horizontalLayout;
    QWidget *wdg_left;
    QHBoxLayout *horizontalLayout_5;
    UserShow *wdg_userShow;
    QWidget *wdg_right;
    QVBoxLayout *verticalLayout_2;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_3;
    QWidget *wdg_list;
    QSpacerItem *verticalSpacer;
    QWidget *wdg_bottom;
    QHBoxLayout *horizontalLayout_4;
    QCheckBox *cb_vedio;
    QCheckBox *cb_audio;
    QCheckBox *cb_desk;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *pb_exit;

    void setupUi(QDialog *RoomDialog)
    {
        if (RoomDialog->objectName().isEmpty())
            RoomDialog->setObjectName(QString::fromUtf8("RoomDialog"));
        RoomDialog->resize(881, 642);
        RoomDialog->setMinimumSize(QSize(800, 600));
        RoomDialog->setSizeGripEnabled(true);
        verticalLayout = new QVBoxLayout(RoomDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        wdg_title = new QWidget(RoomDialog);
        wdg_title->setObjectName(QString::fromUtf8("wdg_title"));
        wdg_title->setMinimumSize(QSize(800, 60));
        wdg_title->setMaximumSize(QSize(16777215, 60));
        horizontalLayout_3 = new QHBoxLayout(wdg_title);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        lb_title = new QLabel(wdg_title);
        lb_title->setObjectName(QString::fromUtf8("lb_title"));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font.setPointSize(10);
        lb_title->setFont(font);

        horizontalLayout_3->addWidget(lb_title);

        horizontalSpacer = new QSpacerItem(536, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        pb_min = new QPushButton(wdg_title);
        pb_min->setObjectName(QString::fromUtf8("pb_min"));
        pb_min->setMinimumSize(QSize(40, 40));
        pb_min->setMaximumSize(QSize(40, 40));

        horizontalLayout_2->addWidget(pb_min);

        pb_max = new QPushButton(wdg_title);
        pb_max->setObjectName(QString::fromUtf8("pb_max"));
        pb_max->setMinimumSize(QSize(40, 40));
        pb_max->setMaximumSize(QSize(40, 40));
        QFont font1;
        font1.setFamily(QString::fromUtf8("\345\271\274\345\234\206"));
        font1.setPointSize(10);
        pb_max->setFont(font1);

        horizontalLayout_2->addWidget(pb_max);

        pb_close = new QPushButton(wdg_title);
        pb_close->setObjectName(QString::fromUtf8("pb_close"));
        pb_close->setMinimumSize(QSize(40, 40));
        pb_close->setMaximumSize(QSize(40, 40));
        QFont font2;
        font2.setFamily(QString::fromUtf8("\345\256\213\344\275\223"));
        font2.setPointSize(17);
        pb_close->setFont(font2);

        horizontalLayout_2->addWidget(pb_close);


        horizontalLayout_3->addLayout(horizontalLayout_2);


        verticalLayout->addWidget(wdg_title);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, -1, -1, -1);
        wdg_left = new QWidget(RoomDialog);
        wdg_left->setObjectName(QString::fromUtf8("wdg_left"));
        wdg_left->setStyleSheet(QString::fromUtf8(""));
        horizontalLayout_5 = new QHBoxLayout(wdg_left);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        wdg_userShow = new UserShow(wdg_left);
        wdg_userShow->setObjectName(QString::fromUtf8("wdg_userShow"));

        horizontalLayout_5->addWidget(wdg_userShow);


        horizontalLayout->addWidget(wdg_left);

        wdg_right = new QWidget(RoomDialog);
        wdg_right->setObjectName(QString::fromUtf8("wdg_right"));
        wdg_right->setMinimumSize(QSize(251, 479));
        wdg_right->setMaximumSize(QSize(251, 16777215));
        verticalLayout_2 = new QVBoxLayout(wdg_right);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        scrollArea = new QScrollArea(wdg_right);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setFrameShape(QFrame::NoFrame);
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 251, 484));
        verticalLayout_3 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(0, -1, -1, 0);
        wdg_list = new QWidget(scrollAreaWidgetContents);
        wdg_list->setObjectName(QString::fromUtf8("wdg_list"));

        verticalLayout_3->addWidget(wdg_list);

        verticalSpacer = new QSpacerItem(20, 460, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer);

        scrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout_2->addWidget(scrollArea);


        horizontalLayout->addWidget(wdg_right);


        verticalLayout->addLayout(horizontalLayout);

        wdg_bottom = new QWidget(RoomDialog);
        wdg_bottom->setObjectName(QString::fromUtf8("wdg_bottom"));
        wdg_bottom->setMinimumSize(QSize(800, 60));
        wdg_bottom->setMaximumSize(QSize(16777215, 60));
        wdg_bottom->setFont(font);
        horizontalLayout_4 = new QHBoxLayout(wdg_bottom);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(-1, 0, -1, -1);
        cb_vedio = new QCheckBox(wdg_bottom);
        cb_vedio->setObjectName(QString::fromUtf8("cb_vedio"));

        horizontalLayout_4->addWidget(cb_vedio);

        cb_audio = new QCheckBox(wdg_bottom);
        cb_audio->setObjectName(QString::fromUtf8("cb_audio"));

        horizontalLayout_4->addWidget(cb_audio);

        cb_desk = new QCheckBox(wdg_bottom);
        cb_desk->setObjectName(QString::fromUtf8("cb_desk"));

        horizontalLayout_4->addWidget(cb_desk);

        horizontalSpacer_2 = new QSpacerItem(438, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);

        pb_exit = new QPushButton(wdg_bottom);
        pb_exit->setObjectName(QString::fromUtf8("pb_exit"));
        pb_exit->setFont(font);

        horizontalLayout_4->addWidget(pb_exit);


        verticalLayout->addWidget(wdg_bottom);


        retranslateUi(RoomDialog);

        QMetaObject::connectSlotsByName(RoomDialog);
    } // setupUi

    void retranslateUi(QDialog *RoomDialog)
    {
        RoomDialog->setWindowTitle(QApplication::translate("RoomDialog", "Dialog", nullptr));
        lb_title->setText(QApplication::translate("RoomDialog", "\346\210\277\351\227\264\345\217\267\357\274\23288888888", nullptr));
        pb_min->setText(QApplication::translate("RoomDialog", "\344\270\200", nullptr));
        pb_max->setText(QApplication::translate("RoomDialog", "\345\217\243", nullptr));
        pb_close->setText(QApplication::translate("RoomDialog", "X", nullptr));
        cb_vedio->setText(QApplication::translate("RoomDialog", "\350\247\206\351\242\221", nullptr));
        cb_audio->setText(QApplication::translate("RoomDialog", "\351\237\263\351\242\221", nullptr));
        cb_desk->setText(QApplication::translate("RoomDialog", "\346\241\214\351\235\242", nullptr));
        pb_exit->setText(QApplication::translate("RoomDialog", "\351\200\200\345\207\272\346\210\277\351\227\264", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RoomDialog: public Ui_RoomDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ROOMDIALOG_H
