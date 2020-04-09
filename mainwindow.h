#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDir>
#include <QMainWindow>
#include <QListWidgetItem>
#include <QStandardItemModel>

#include "ftpapi.h"

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

private:
    Ui::MainWindow *ui;

    FTPAPI ftpAPI;

    QString host;
    QString user;
};

#endif // MAINWINDOW_H
