#include "logindialog.h"
#include "ui_logindialog.h"
#include <QMessageBox>
#include <QRegExp>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("注册&登录");   // 修改登录注册界面标题

    /* [标注] tw_page 通常是一个 QTabWidget 或 QStackedWidget */
    ui->tw_page->setCurrentIndex(0);    // 第一时间显示注册页
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

// 登录注册界面关闭事件处理
void LoginDialog::closeEvent(QCloseEvent *events)
{
    // 忽略关闭事件：点击窗口关闭按钮时，窗口不会真正消失
    events->ignore();

    /* [纠正乱码] 向核心处理类（通常是 CKernel）发送关闭信号 */
    Q_EMIT SIG_close();
}

// 提交登录信息
void LoginDialog::on_pb_commit_clicked()
{
    /* [标注] __func__ 是预定义宏，用于输出当前函数的名称，方便调试 */
    qDebug()<<__func__;

    // 获取用户输入信息
    QString strTel = ui->le_tel->text();
    QString strPass = ui->le_pass->text();

    // 数据校验
    // 1、非空校验：清除空格之后，不能是空字符串
    QString tempTel = strTel;
    QString tempPass = strPass;

    /* [标注] remove(" ") 会修改原字符串，所以这里用了临时变量 */
    if((tempTel.remove(" ").isEmpty()) || (tempPass.remove(" ").isEmpty())){
        QMessageBox::about(this, "提示", "手机号密码不能为空");
        return;
    }

    // 2、校验手机号：正则表达式
    /* [标注] QRegExp 在新版本 Qt 中逐渐被 QRegularExpression 替代 */
    QRegExp reg("^1[3-8][0-9]{6,9}$");
    bool res = reg.exactMatch(tempTel);
    if(!res){
        QMessageBox::about(this, "提示", "手机号格式错误，8-11位手机号");
        return;
    }

    // 3、校验密码，长度不能超过20
    if(tempPass.length() > 20){
        QMessageBox::about(this, "提示", "密码过长，密码长度不超过20");
        return;
    }

    /* [纠正乱码] 通过信号发送用户登录信息至 CKernel 类处理 */
    Q_EMIT SIG_loginCommit(strTel, strPass);
}

// 清空注册信息
void LoginDialog::on_pb_clean_register_clicked()
{
    qDebug()<<__func__;
    ui->le_name_register->setText("");
    ui->le_confirm_pass_register->setText("");
    ui->le_pass_register->setText("");
    ui->le_tel_register->setText("");
}

// 提交注册信息
void LoginDialog::on_pb_commit_register_clicked()
{
    qDebug()<<__func__;
    // 获取用户输入信息
    QString strTel = ui->le_tel_register->text();
    QString strPass = ui->le_pass_register->text();
    QString strConfirm = ui->le_confirm_pass_register->text();
    QString strName = ui->le_name_register->text();

    // 数据校验
    // 1、非空校验
    QString tempTel = strTel;
    QString tempPass = strPass;
    QString tempName = strName;
    if((tempTel.remove(" ").isEmpty()) ||
       (tempPass.remove(" ").isEmpty()) ||
       (tempName.remove(" ").isEmpty())){
        QMessageBox::about(this, "提示", "手机号密码昵称不能为空");
        return;
    }

    // 2、校验手机号
    QRegExp reg("^1[3-8][0-9]{6,9}$");
    bool res = reg.exactMatch(tempTel);
    if(!res){
        QMessageBox::about(this, "提示", "手机号格式错误，8-11位手机号");
        return;
    }

    // 3、校验密码长度
    if(tempPass.length() > 20){
        QMessageBox::about(this, "提示", "密码过长，密码长度不超过20");
        return;
    }

    /* [纠正乱码] 4、校验两次密码输入是否一致 */
    if(strPass != strConfirm){
        QMessageBox::about(this, "提示", "输入密码不一致");
        return;
    }

    // 5、昵称长度不超过10
    if(strName.length() > 10){
        QMessageBox::about(this, "提示", "昵称过长，昵称长度不超过10");
        return;
    }

    /* [纠正乱码] 通过信号发送用户注册信息至 CKernel 类处理 */
    Q_EMIT SIG_registerCommit(strTel, strPass, strName);
}

// 清空登录信息
void LoginDialog::on_pb_clean_clicked()
{
    qDebug()<<__func__;
    ui->le_pass->setText("");
    ui->le_tel->setText("");
}
