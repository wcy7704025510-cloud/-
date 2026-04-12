/********************************************************************************
** Form generated from reading UI file 'logindialog.ui'
**
** Created by: Qt User Interface Compiler version 5.12.11
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LOGINDIALOG_H
#define UI_LOGINDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LoginDialog
{
public:
    QTabWidget *tw_page;
    QWidget *page_login;
    QLabel *lb_tel;
    QLabel *lb_pass;
    QLineEdit *le_tel;
    QLineEdit *le_pass;
    QPushButton *pb_clean;
    QPushButton *pb_commit;
    QWidget *page_register;
    QLineEdit *le_confirm_pass_register;
    QPushButton *pb_commit_register;
    QLabel *lb_confirm_register;
    QLabel *lb_tel_register;
    QPushButton *pb_clean_register;
    QLineEdit *le_tel_register;
    QLineEdit *le_name_register;
    QLabel *lb_name_register;
    QLineEdit *le_pass_register;
    QLabel *lb_pass_register;
    QLabel *label;

    void setupUi(QDialog *LoginDialog)
    {
        if (LoginDialog->objectName().isEmpty())
            LoginDialog->setObjectName(QString::fromUtf8("LoginDialog"));
        LoginDialog->resize(600, 400);
        LoginDialog->setMinimumSize(QSize(600, 400));
        LoginDialog->setMaximumSize(QSize(600, 400));
        tw_page = new QTabWidget(LoginDialog);
        tw_page->setObjectName(QString::fromUtf8("tw_page"));
        tw_page->setGeometry(QRect(220, 0, 381, 401));
        page_login = new QWidget();
        page_login->setObjectName(QString::fromUtf8("page_login"));
        lb_tel = new QLabel(page_login);
        lb_tel->setObjectName(QString::fromUtf8("lb_tel"));
        lb_tel->setGeometry(QRect(20, 70, 111, 30));
        lb_pass = new QLabel(page_login);
        lb_pass->setObjectName(QString::fromUtf8("lb_pass"));
        lb_pass->setGeometry(QRect(20, 170, 111, 30));
        le_tel = new QLineEdit(page_login);
        le_tel->setObjectName(QString::fromUtf8("le_tel"));
        le_tel->setGeometry(QRect(130, 70, 150, 30));
        le_pass = new QLineEdit(page_login);
        le_pass->setObjectName(QString::fromUtf8("le_pass"));
        le_pass->setGeometry(QRect(130, 170, 150, 30));
        le_pass->setEchoMode(QLineEdit::Password);
        le_pass->setClearButtonEnabled(true);
        pb_clean = new QPushButton(page_login);
        pb_clean->setObjectName(QString::fromUtf8("pb_clean"));
        pb_clean->setGeometry(QRect(60, 260, 89, 24));
        pb_commit = new QPushButton(page_login);
        pb_commit->setObjectName(QString::fromUtf8("pb_commit"));
        pb_commit->setGeometry(QRect(200, 260, 89, 24));
        tw_page->addTab(page_login, QString());
        page_register = new QWidget();
        page_register->setObjectName(QString::fromUtf8("page_register"));
        le_confirm_pass_register = new QLineEdit(page_register);
        le_confirm_pass_register->setObjectName(QString::fromUtf8("le_confirm_pass_register"));
        le_confirm_pass_register->setGeometry(QRect(160, 130, 161, 30));
        le_confirm_pass_register->setEchoMode(QLineEdit::Password);
        le_confirm_pass_register->setClearButtonEnabled(true);
        pb_commit_register = new QPushButton(page_register);
        pb_commit_register->setObjectName(QString::fromUtf8("pb_commit_register"));
        pb_commit_register->setGeometry(QRect(210, 250, 89, 24));
        lb_confirm_register = new QLabel(page_register);
        lb_confirm_register->setObjectName(QString::fromUtf8("lb_confirm_register"));
        lb_confirm_register->setGeometry(QRect(50, 130, 111, 30));
        lb_tel_register = new QLabel(page_register);
        lb_tel_register->setObjectName(QString::fromUtf8("lb_tel_register"));
        lb_tel_register->setGeometry(QRect(50, 30, 111, 30));
        pb_clean_register = new QPushButton(page_register);
        pb_clean_register->setObjectName(QString::fromUtf8("pb_clean_register"));
        pb_clean_register->setGeometry(QRect(80, 250, 89, 24));
        le_tel_register = new QLineEdit(page_register);
        le_tel_register->setObjectName(QString::fromUtf8("le_tel_register"));
        le_tel_register->setGeometry(QRect(160, 30, 161, 30));
        le_name_register = new QLineEdit(page_register);
        le_name_register->setObjectName(QString::fromUtf8("le_name_register"));
        le_name_register->setGeometry(QRect(160, 170, 161, 30));
        lb_name_register = new QLabel(page_register);
        lb_name_register->setObjectName(QString::fromUtf8("lb_name_register"));
        lb_name_register->setGeometry(QRect(50, 170, 111, 30));
        le_pass_register = new QLineEdit(page_register);
        le_pass_register->setObjectName(QString::fromUtf8("le_pass_register"));
        le_pass_register->setGeometry(QRect(160, 80, 161, 30));
        le_pass_register->setEchoMode(QLineEdit::Password);
        le_pass_register->setClearButtonEnabled(true);
        lb_pass_register = new QLabel(page_register);
        lb_pass_register->setObjectName(QString::fromUtf8("lb_pass_register"));
        lb_pass_register->setGeometry(QRect(50, 80, 111, 30));
        tw_page->addTab(page_register, QString());
        label = new QLabel(LoginDialog);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(0, 0, 220, 400));
        label->setPixmap(QPixmap(QString::fromUtf8(":/images/register.png")));
        label->setScaledContents(true);
        QWidget::setTabOrder(le_tel_register, le_pass_register);
        QWidget::setTabOrder(le_pass_register, le_confirm_pass_register);
        QWidget::setTabOrder(le_confirm_pass_register, le_name_register);
        QWidget::setTabOrder(le_name_register, pb_commit_register);
        QWidget::setTabOrder(pb_commit_register, pb_clean_register);
        QWidget::setTabOrder(pb_clean_register, tw_page);
        QWidget::setTabOrder(tw_page, le_tel);
        QWidget::setTabOrder(le_tel, le_pass);
        QWidget::setTabOrder(le_pass, pb_commit);
        QWidget::setTabOrder(pb_commit, pb_clean);

        retranslateUi(LoginDialog);

        tw_page->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(LoginDialog);
    } // setupUi

    void retranslateUi(QDialog *LoginDialog)
    {
        LoginDialog->setWindowTitle(QApplication::translate("LoginDialog", "Dialog", nullptr));
        lb_tel->setText(QApplication::translate("LoginDialog", "\346\211\213\346\234\272\345\217\267", nullptr));
        lb_pass->setText(QApplication::translate("LoginDialog", "\345\257\206\347\240\201", nullptr));
        le_tel->setText(QApplication::translate("LoginDialog", "13659270451", nullptr));
        le_pass->setText(QApplication::translate("LoginDialog", "wcy123", nullptr));
        pb_clean->setText(QApplication::translate("LoginDialog", "\346\270\205\347\251\272", nullptr));
        pb_commit->setText(QApplication::translate("LoginDialog", "\347\241\256\350\256\244", nullptr));
        tw_page->setTabText(tw_page->indexOf(page_login), QApplication::translate("LoginDialog", "\347\231\273\345\275\225", nullptr));
#ifndef QT_NO_TOOLTIP
        le_confirm_pass_register->setToolTip(QApplication::translate("LoginDialog", "<html><head/><body><p>\350\257\267\347\241\256\350\256\244\345\257\206\347\240\201\357\274\214\344\277\235\350\257\201\344\270\244\346\254\241\350\276\223\345\205\245\344\270\200\350\207\264</p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
        pb_commit_register->setText(QApplication::translate("LoginDialog", "\347\241\256\350\256\244", nullptr));
        lb_confirm_register->setText(QApplication::translate("LoginDialog", "\347\241\256\350\256\244\345\257\206\347\240\201", nullptr));
        lb_tel_register->setText(QApplication::translate("LoginDialog", "\346\211\213\346\234\272\345\217\267", nullptr));
        pb_clean_register->setText(QApplication::translate("LoginDialog", "\346\270\205\347\251\272", nullptr));
#ifndef QT_NO_TOOLTIP
        le_tel_register->setToolTip(QApplication::translate("LoginDialog", "<html><head/><body><p>\350\257\267\350\276\223\345\205\2458-11\344\275\215\346\211\213\346\234\272\345\217\267</p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        le_name_register->setToolTip(QApplication::translate("LoginDialog", "<html><head/><body><p>\350\257\267\350\276\223\345\205\24510\344\275\215\344\273\245\345\206\205\347\232\204\346\230\265\347\247\260</p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
        lb_name_register->setText(QApplication::translate("LoginDialog", "\346\230\265\347\247\260", nullptr));
#ifndef QT_NO_TOOLTIP
        le_pass_register->setToolTip(QApplication::translate("LoginDialog", "<html><head/><body><p>\350\257\267\350\276\223\345\205\24520\344\275\215\344\273\245\345\206\205\347\232\204\345\257\206\347\240\201</p><p>\345\217\257\344\273\245\346\230\257\346\225\260\345\255\227\346\210\226\345\255\227\347\254\246</p></body></html>", nullptr));
#endif // QT_NO_TOOLTIP
        le_pass_register->setText(QString());
        lb_pass_register->setText(QApplication::translate("LoginDialog", "\345\257\206\347\240\201", nullptr));
        tw_page->setTabText(tw_page->indexOf(page_register), QApplication::translate("LoginDialog", "\346\263\250\345\206\214", nullptr));
        label->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class LoginDialog: public Ui_LoginDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LOGINDIALOG_H
