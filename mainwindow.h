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
    void slot_listAllFile(QDir);
    void slot_enterSubDir(QListWidgetItem *);
    void slot_enterHostSubDir(QListWidgetItem *);

    void slot_showFileDetail(QListWidgetItem *);
    void slot_showHostFileDetail(QListWidgetItem *);

    void slot_upload2host();
    void slot_download2local();
    void slot_deleteFileFromHost();

    void on_pushButton_NewFolder_clicked();

    void on_pushButton_PreFolder_clicked();

private:
    Ui::MainWindow *ui;

    FTPAPI ftpAPI;

    QString host;
    QString user;

    void fresh();
};

#endif // MAINWINDOW_H
