/********************************************************************************
** Form generated from reading UI file 'usershow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.11
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_USERSHOW_H
#define UI_USERSHOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_UserShow
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *lb_name;

    void setupUi(QWidget *UserShow)
    {
        if (UserShow->objectName().isEmpty())
            UserShow->setObjectName(QString::fromUtf8("UserShow"));
        UserShow->resize(240, 180);
        UserShow->setMinimumSize(QSize(220, 180));
        UserShow->setMaximumSize(QSize(16777215, 16777215));
        UserShow->setStyleSheet(QString::fromUtf8(""));
        verticalLayout = new QVBoxLayout(UserShow);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        lb_name = new QLabel(UserShow);
        lb_name->setObjectName(QString::fromUtf8("lb_name"));
        lb_name->setMinimumSize(QSize(220, 30));
        lb_name->setMaximumSize(QSize(16777215, 30));
        lb_name->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 0, 0);\n"
"color: rgb(255, 255, 255);"));
        lb_name->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(lb_name);


        retranslateUi(UserShow);

        QMetaObject::connectSlotsByName(UserShow);
    } // setupUi

    void retranslateUi(QWidget *UserShow)
    {
        UserShow->setWindowTitle(QApplication::translate("UserShow", "Form", nullptr));
        lb_name->setText(QApplication::translate("UserShow", "\347\224\250\346\210\267\345\220\215:\346\265\213\350\257\225\347\224\250\346\210\267", nullptr));
    } // retranslateUi

};

namespace Ui {
    class UserShow: public Ui_UserShow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_USERSHOW_H
