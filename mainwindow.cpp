#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QProcess>
QStringList getList(QString cmd,QStringList arguments=QStringList())
{
    QProcess process;
    process.start(cmd,arguments);
    if (!process.waitForStarted()) {
        qDebug() << "Failed to start process.";
        return QStringList();
    }
    if (!process.waitForFinished()) {
        qDebug() << "Process did not finish.";
        return QStringList();
    }
    QByteArray output = process.readAllStandardOutput();
    QStringList res = QString::fromLocal8Bit(output).split("\r\n");
    return res;
}
QString getString(QString cmd,QStringList arguments=QStringList())
{
    QProcess process;
    process.start(cmd,arguments);
    if (!process.waitForStarted()) {
        qDebug() << "Failed to start process.";
        return QString();
    }
    if (!process.waitForFinished()) {
        qDebug() << "Process did not finish.";
        return QString();
    }
    QByteArray output = process.readAllStandardOutput();
    QString res = QString::fromLocal8Bit(output);
    return res;
}
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QStringList taskList = getList(QString("tasklist"));
    std::sort(taskList.begin()+3,taskList.end(),[](QString x,QString y){
        return x.toLower()<y.toLower();
    });
    //ui->listWidget->setStyleSheet("QListWidget::item { text-align: center; }");
    for(auto &i:taskList)
    {
        QListWidgetItem *item = new QListWidgetItem(i);//用new而不直接构造函数，不然局部变量创建在栈区上，生命周期不由Listwidget控制
        item->setTextAlignment(Qt::AlignCenter);
        ui->listWidget->addItem(item);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

