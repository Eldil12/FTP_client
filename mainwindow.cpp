#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(FTPAPI ftpAPI, QString host, QString user, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), ftpAPI(ftpAPI), host(host), user(user) {

    ui->setupUi(this);
    this->setFixedSize(this->width(), this->height());

    ui->label_Infomation->setText("当前主机：" + host + "\t当前用户：" + user);
    ui->lineEdit_LocalFile->setText(QCoreApplication::applicationDirPath());

    /* 列出本地文件列表 */
    QDir root(ui->lineEdit_LocalFile->text());
    slot_listAllFile(root);

    QStandardItemModel *model = new QStandardItemModel();
    QStringList labels = QObject::tr("本地/远程,文件名,文件大小,文件路径").simplified().split(",");
    model->setHorizontalHeaderLabels(labels);
    ui->tableView_FileDetail->setModel(model);

    connect(ui->listWidget_LocalFile, SIGNAL(itemClicked        (QListWidgetItem *)),
            this,                     SLOT  (slot_showFileDetail(QListWidgetItem *)));
    connect(ui->listWidget_LocalFile, SIGNAL(itemDoubleClicked(QListWidgetItem *)),
            this,                     SLOT  (slot_enterSubDir (QListWidgetItem *)));

    /* 拉取主机文件列表 */
    ui->lineEdit_HostFile->setText("./");

    char data[BUFSIZE];
    ftpAPI.ftp_list(qstring2char(ui->lineEdit_HostFile->text()), data);
    QStringList hostList = QObject::tr(data).simplified().split(" ");

    for (QString s : hostList) {
        ui->listWidget_HostFile->addItem(new QListWidgetItem(s));
    }

    connect(ui->listWidget_HostFile, SIGNAL(itemClicked            (QListWidgetItem *)),
            this,                    SLOT  (slot_showHostFileDetail(QListWidgetItem *)));
    connect(ui->listWidget_HostFile, SIGNAL(itemDoubleClicked      (QListWidgetItem *)),
            this,                    SLOT  (slot_enterHostSubDir   (QListWidgetItem *)));
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

void MainWindow::slot_enterHostSubDir(QListWidgetItem *item)
{
    QString name = ui->lineEdit_HostFile->text() + item->text();

    long size;
    if (ftpAPI.ftp_filesize(qstring2char(name), size) != 0) {
        ui->listWidget_HostFile->clear();
        ui->lineEdit_HostFile->setText(name + "/");

        char data[BUFSIZE];
        ftpAPI.ftp_list(qstring2char(ui->lineEdit_HostFile->text()), data);
        QStringList hostList = QObject::tr(data).simplified().split(" ");

        for (QString s : hostList) {
            ui->listWidget_HostFile->addItem(new QListWidgetItem(s));
        }
    }
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
        QStringList labels = QObject::tr("本地/远程,文件名,文件大小,文件路径, ").simplified().split(",");
        model->setHorizontalHeaderLabels(labels);

        QStandardItem *newItem = new QStandardItem(QString("本地"));
        model->setItem(0, 0, newItem);

        newItem = new QStandardItem(item->text());
        model->setItem(0, 1, newItem);

        newItem = new QStandardItem(QString("%1").arg(fileInfo.size()));
        model->setItem(0, 2, newItem);

        newItem = new QStandardItem(ui->lineEdit_LocalFile->text());
        model->setItem(0, 3, newItem);

        ui->tableView_FileDetail->setModel(model);

        QPushButton *uploadButton = new QPushButton("上传至主机");
        uploadButton->setProperty("path", ui->lineEdit_LocalFile->text());
        uploadButton->setProperty("name", item->text());

        connect(uploadButton, SIGNAL(clicked(bool)), this, SLOT(slot_upload2host()));
        ui->tableView_FileDetail->setIndexWidget(model->index(0, 4), uploadButton);
    }
}

void MainWindow::slot_showHostFileDetail(QListWidgetItem *item)
{
    QStandardItemModel *model = new QStandardItemModel();
    QStringList labels = QObject::tr("本地/远程,文件名,文件大小,文件路径, , ").simplified().split(",");
    model->setHorizontalHeaderLabels(labels);

    QStandardItem *newItem = new QStandardItem(QString("远程"));
    model->setItem(0, 0, newItem);

    newItem = new QStandardItem(item->text());
    model->setItem(0, 1, newItem);

    long size = 0;
    ftpAPI.ftp_filesize(qstring2char(ui->lineEdit_HostFile->text()+item->text()), size);
    newItem = new QStandardItem(QString("%1").arg(size));
    model->setItem(0, 2, newItem);

    newItem = new QStandardItem(ui->lineEdit_HostFile->text());
    model->setItem(0, 3, newItem);

    ui->tableView_FileDetail->setModel(model);

    if (size >= 0) {

        QPushButton *downloadButton = new QPushButton("下载到本地");
        downloadButton->setProperty("path", ui->lineEdit_HostFile->text());
        downloadButton->setProperty("name", item->text());
        connect(downloadButton, SIGNAL(clicked(bool)), this, SLOT(slot_download2local()));
        ui->tableView_FileDetail->setIndexWidget(model->index(0, 4), downloadButton);

        QPushButton *deleteButton = new QPushButton("从主机删除");
        deleteButton->setProperty("path", ui->lineEdit_HostFile->text());
        deleteButton->setProperty("name", item->text());
        connect(deleteButton, SIGNAL(clicked(bool)), this, SLOT(slot_deleteFileFromHost()));
        ui->tableView_FileDetail->setIndexWidget(model->index(0, 5), deleteButton);

    }
}

void MainWindow::slot_upload2host()
{
    QPushButton *b = (QPushButton *)sender();
    ftpAPI.ftp_append(qstring2char(b->property("path").toString()+"/"+b->property("name").toString()),
                      qstring2char(ui->lineEdit_HostFile->text()+b->property("name").toString()));
    fresh();
}

void MainWindow::slot_download2local()
{
    QPushButton *b = (QPushButton *)sender();
    ftpAPI.ftp_download(qstring2char(b->property("path").toString()+"/"+b->property("name").toString()),
                        qstring2char(ui->lineEdit_LocalFile->text()+b->property("name").toString()));
    fresh();
}

void MainWindow::slot_deleteFileFromHost()
{
    QPushButton *b = (QPushButton *)sender();
    QString name = b->property("path").toString()+b->property("name").toString();

    long size;
    if (ftpAPI.ftp_filesize(qstring2char(name), size) != 0) {

        ftpAPI.ftp_deletefolder(qstring2char(name));

    } else {

        ftpAPI.ftp_deletefile(qstring2char(name));

    }

    fresh();
}

void MainWindow::fresh()
{
    ui->listWidget_HostFile->clear();
    ui->listWidget_LocalFile->clear();

    QDir root(ui->lineEdit_LocalFile->text());
    slot_listAllFile(root);

    char data[BUFSIZE];
    ftpAPI.ftp_list(qstring2char(ui->lineEdit_HostFile->text()), data);
    QStringList hostList = QObject::tr(data).simplified().split(" ");

    for (QString s : hostList) {
        ui->listWidget_HostFile->addItem(new QListWidgetItem(s));
    }
}

void MainWindow::on_pushButton_NewFolder_clicked()
{
    QString newFolder = ui->lineEdit_HostFile->text() + ui->lineEdit_NewFolder->text() + "/";
    ftpAPI.ftp_mkd(qstring2char(newFolder));
    fresh();
}

void MainWindow::on_pushButton_PreFolder_clicked()
{
    ftpAPI.ftp_cdup();
    char path[BUFSIZE];
    ftpAPI.ftp_cwd(path);
    ui->lineEdit_HostFile->setText(QString::fromLocal8Bit(path));
    fresh();
}
