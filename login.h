/**
 * 登录界面
 *
 */

#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <QMessageBox>

#include "ftpapi.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Login; }
QT_END_NAMESPACE

class Login : public QWidget
{
    Q_OBJECT

public:
    Login( FTPAPI ftpAPI, QWidget *parent = nullptr );
    ~Login();

private slots:
    void on_pushButton_Login_clicked();

private:
    Ui::Login *ui;

    FTPAPI ftpAPI;

    char *string2char( QString s );
};
#endif // LOGIN_H
