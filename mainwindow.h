#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDir>
#include <QDebug>
#include <QMainWindow>
#include <QPushButton>
#include <QListWidgetItem>
#include <QStandardItemModel>

#include "ftpapi.h"
#include "login.h"

QT_BEGIN_NAMESPACE
    namespace Ui {
        class MainWindow;
    }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {

    Q_OBJECT

public:
    explicit MainWindow(FTPAPI ftpAPI, QString host, QString user, QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void slot_enterSubDir(QListWidgetItem *);
    void slot_listAllFile(QDir);
    void slot_showFileDetail(QListWidgetItem *);
    void slot_upload2host();

private:
    Ui::MainWindow *ui;

    FTPAPI ftpAPI;

    QString host;
    QString user;
};

#endif // MAINWINDOW_H
