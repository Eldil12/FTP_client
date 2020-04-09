#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(FTPAPI ftpAPI, QString host, QString user, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), ftpAPI(ftpAPI), host(host), user(user) {

    ui->setupUi(this);
    this->setFixedSize(this->width(), this->height());

    ui->label_Infomation->setText("当前主机：" + host + "\t当前用户：" + user);
    ui->lineEdit_LocalFile->setText(QCoreApplication::applicationDirPath());

    connect(ui->listWidget_LocalFile, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
            this,                     SLOT  (slot_enterSubDir (QListWidgetItem *)));

    QDir root(ui->lineEdit_LocalFile->text());
    slot_listAllFile(root);

    QStandardItemModel *model = new QStandardItemModel();
    QStringList labels = QObject::tr("本地/远程,文件名,文件大小,文件路径,当前状态").simplified().split(",");
    model->setHorizontalHeaderLabels(labels);
    ui->tableView_FileDetail->setModel(model);

    connect(ui->listWidget_LocalFile, SIGNAL(itemClicked        (QListWidgetItem *)),
            this,                     SLOT  (slot_showFileDetail(QListWidgetItem *)));
}

MainWindow::~MainWindow() {

    delete ui;
}

void MainWindow::slot_enterSubDir(QListWidgetItem *item) {

    QDir dir;

    dir.setPath(ui->lineEdit_LocalFile->text());
    dir.cd(item->text());

    ui->lineEdit_LocalFile->setText(dir.absolutePath());

    slot_listAllFile(dir.absolutePath());
}

void MainWindow::slot_listAllFile(QDir dir) {

    QStringList stringList;
    stringList << "*";
    QFileInfoList infoList = dir.entryInfoList(stringList, QDir::AllEntries, QDir::DirsFirst);

    ui->listWidget_LocalFile->clear();

    for (int i = 0; i < infoList.count(); i++) {

        ui->listWidget_LocalFile->addItem(new QListWidgetItem(infoList.at(i).fileName()));
    }
}

void MainWindow::slot_showFileDetail(QListWidgetItem *item) {

    QDir dir(ui->lineEdit_LocalFile->text());
    QFileInfo fileInfo(dir, item->text());

    if (!fileInfo.isDir()) {

        QStandardItemModel *model = new QStandardItemModel();
        QStringList labels = QObject::tr("本地/远程,文件名,文件大小,文件路径,当前状态").simplified().split(",");
        model->setHorizontalHeaderLabels(labels);

        QStandardItem *newItem = new QStandardItem(QString("本地"));
        model->setItem(0, 0, newItem);

        newItem = new QStandardItem(item->text());
        model->setItem(0, 1, newItem);

        newItem = new QStandardItem(QString("%1").arg(fileInfo.size()));
        model->setItem(0, 2, newItem);

        newItem = new QStandardItem(ui->lineEdit_LocalFile->text());
        model->setItem(0, 3, newItem);

        newItem = new QStandardItem(QString("未上传到主机"));
        model->setItem(0, 4, newItem);

        ui->tableView_FileDetail->setModel(model);
    }
}
