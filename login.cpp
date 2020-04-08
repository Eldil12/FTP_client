#include "login.h"
#include "ui_login.h"

Login::Login( FTPAPI ftpAPI, QWidget *parent )
    : QWidget( parent ), ui( new Ui::Login ), ftpAPI( ftpAPI )
{
    ui->setupUi(this);
}

Login::~Login()
{
    delete ui;
}

void Login::on_pushButton_Login_clicked()
{
    QString userName = ui->lineEdit_UserName->text();
    QString password = ui->lineEdit_Password->text();

    if ( ftpAPI.login_server( string2char(userName), string2char(password) ) ) {

        /* 登录失败时弹出信息框，提示密码错误 */
        QMessageBox::about( nullptr, "登录失败", "用户名或密码不正确，请检查后再次尝试");

    } else {

        /* 登录成功时关闭登录页面，打开主页面 */
        // TODO：Login如何调用主页面？

    }
}

char *Login::string2char(QString s)
{
    QByteArray b = s.toLatin1();
    return b.data();
}
