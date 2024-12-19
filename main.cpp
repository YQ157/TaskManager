#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <Windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <QString>
#include <QDebug>
void getRunningApplications() {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        qDebug() << "无法获取进程快照!";
        return;
    }

    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe32)) {
        do {
            QString processName = QString::fromWCharArray(pe32.szExeFile);
            DWORD pid = pe32.th32ProcessID;

            // 跳过系统进程和后台服务进程
            if (processName == "System" || processName == "smss.exe" || processName == "csrss.exe") {
                continue;
            }

            // 输出进程名称和PID
            qDebug() << "进程名称: " << processName << ", PID: " << pid;

        } while (Process32Next(hSnapshot, &pe32));  // 获取下一个进程
    }

    CloseHandle(hSnapshot);
}
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "TaskManager_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    //getRunningApplications();
    QProcess process;
    MainWindow w;
    w.init(process);
    w.show();
    return a.exec();
}
