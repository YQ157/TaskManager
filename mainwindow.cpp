#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QProcess>
#include <QTimer>
#include <windows.h>
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
    taskList.removeAt(0);
    taskList.removeAt(1);
    std::sort(taskList.begin(),taskList.end(),[](QString x,QString y){
        return x.toLower()<y.toLower();
    });
    //ui->listWidget->setStyleSheet("QListWidget::item { text-align: center; }");
    ui->tableWidget->setRowCount(taskList.size());
    ui->tableWidget->setColumnCount(5);
    ui->tableWidget->setHorizontalHeaderLabels({"映像名称", "PID", "会话名", "会话#", "内存使用"});
    QStringList userList = getList(QString("query"),QStringList()<<"user");
    userList.removeAt(0);
    qDebug()<<userList[0];
    qDebug()<<userList.size();
    /*ui->tableWidget_2->setRowCount(userList.size());
    ui->tableWidget_2->setColumnCount(6);
    ui->tableWidget_2->setHorizontalHeaderLabels({"用户名", "会话名", "ID", "状态", "空闲时间","登录时间"});*/
    ui->listWidget->addItem("用户名                会话名             ID  状态    空闲时间   登录时间");
    for(auto i:userList)
    {
        if(!i.isEmpty())
        ui->listWidget->addItem(i);
    }

    /*for(auto &i:taskList)
    {
        QListWidgetItem *item = new QListWidgetItem(i);//用new而不直接构造函数，不然局部变量创建在栈区上，生命周期不由Listwidget控制
        item->setTextAlignment(Qt::AlignCenter);
        ui->listWidget->addItem(item);
    }*/
    QRegularExpression regex("(\\S[\\w\\s\\-]+(?:\\.exe)?)\\s+(\\d+)\\s+(\\S+)\\s+(\\d+)\\s+(\\S+\\s?\\S+)");
    int column=0;
    // 解析每行数据并填充表格
    for (int row = 0; row < taskList.size(); ++row) {
        QRegularExpressionMatch match = regex.match(taskList[row]);
        if (match.hasMatch()) {
            // 正则匹配成功，提取分组
            if(match.captured(1).endsWith(".exe"))
            {
                ui->tableWidget->setItem(column, 0, new QTableWidgetItem(match.captured(1))); // 映像名称
                ui->tableWidget->setItem(column, 1, new QTableWidgetItem(match.captured(2))); // PID
                ui->tableWidget->setItem(column, 2, new QTableWidgetItem(match.captured(3))); // 会话名
                ui->tableWidget->setItem(column, 3, new QTableWidgetItem(match.captured(4))); // 会话#
                ui->tableWidget->setItem(column, 4, new QTableWidgetItem(match.captured(5))); // 内存使用
                column++;
            }
        }
    }
    ui->label_5->setText(QString::number(taskList.size()));
    ui->tableWidget->setRowCount(column);
    //ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->setSortingEnabled(true);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);  // 只允许单选
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
}
void updateCpuUsage()
    {
        // 创建 QProcess 对象
        QProcess process;

        // 设置执行命令
        process.start(R"(typeperf "\Processor(_Total)\% Processor Time")");
        QObject::connect(&process, &QProcess::readyReadStandardOutput, [&process]() {
            QByteArray output = process.readAllStandardOutput();
            qDebug() << output;
        });

        QObject::connect(&process, &QProcess::readyReadStandardError, [&process]() {
            QByteArray error = process.readAllStandardError();
            qDebug() << error;
        });
        // 等待进程执行完成
        if (process.waitForFinished()) {
            QByteArray output = process.readAllStandardOutput();  // 获取标准输出
            QString result = QString(output).trimmed();

            // 更新标签显示 CPU 使用率
            QRegularExpression regex("\"([^\"]+)\"");
            QRegularExpressionMatch match=regex.match(result);
            qDebug()<<result<<'\n';
            if(match.hasMatch())
            {
            qDebug()<<match.captured(1)<<'\n';
            }
            //qDebug()<<("CPU 使用率: " + result + "%");
        } else {
            //qDebug()<<("无法获取 CPU 使用率");
        }
    }
void MainWindow::init(QProcess&process)
{
    connect(ui->logoffBtn,&QPushButton::clicked,[this](){
        getString(QString("logoff "+QString::number(ui->listWidget->currentRow())));
    });
    connect(ui->stopBtn, &QPushButton::clicked, [this]() {
                //qDebug()<<"点击"<<'\n';
                auto t=ui->tableWidget->selectedItems();
                if(!t.isEmpty())
                {
                    qDebug()<<t[0]->row()<<'\n';
                    getString(QString("taskkill"),QStringList()<<"/f"<<"/pid"<<QString(ui->tableWidget->item(t[0]->row(),1)->text()));
                }
            });
    connect(ui->winrBtn,&QPushButton::clicked,[this](){
        keybd_event(VK_LWIN,0,0,0);
        keybd_event(0x52,0,0,0);
        keybd_event(0x52, 0, KEYEVENTF_KEYUP, 0);
        keybd_event(VK_LWIN, 0, KEYEVENTF_KEYUP, 0);
    });
    /*QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, updateCpuUsage);
    timer->start(100000);  // 每 1 秒更新一次*/

    // 设置执行命令
    process.start(R"(typeperf "\Processor(_Total)\% Processor Time")");
    bool flag=false;
    QObject::connect(&process, &QProcess::readyReadStandardOutput, [&process,this,&flag]() {
        QByteArray output = process.readAllStandardOutput();
        //qDebug() << output;
        QRegularExpression regex("\"([^\"]+)\"");
        QRegularExpressionMatch match=regex.match(output);
        if(match.hasMatch())
        {
            qDebug()<<match.captured(1)<<'\n';
            QString res=match.captured(1);
            ui->label_2->setText(res);
        }
        QRegularExpression regax("[\\r\\n]+");
        QString result=QString::fromLocal8Bit(output);
        //result.replace(regax, " ");
        if(result.contains("2024"))ui->label_3->setText(result+ui->label_3->text());
        //QListWidgetItem*item=new QListWidgetItem(result);
        /*if(!flag)ui->listWidget->addItem(result);
        flag=!flag;*/
    });

    QObject::connect(&process, &QProcess::readyReadStandardError, [&process]() {
        QByteArray error = process.readAllStandardError();
        qDebug() << error;
    });
    ui->label_3->setAlignment(Qt::AlignTop);
    //ui->listWidget->setTextElideMode(Qt::ElideRight);
}
MainWindow::~MainWindow()
{
    delete ui;
}

