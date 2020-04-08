#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ftpapi.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow( FTPAPI ftpAPI, QWidget *parent = nullptr );
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    FTPAPI ftpAPI;
};

#endif // MAINWINDOW_H
