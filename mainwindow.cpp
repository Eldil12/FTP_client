#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow( FTPAPI ftpAPI, QWidget *parent )
    : QMainWindow(parent), ui(new Ui::MainWindow), ftpAPI(ftpAPI)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}
