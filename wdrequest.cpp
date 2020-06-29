#include "wdrequest.h"
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QDebug>
#include <QApplication>

WDRequest::WDRequest(const QString& wddFilename, QObject *parent) :
    QObject(parent)
{
    _wddFilename = wddFilename;
    connect(&_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processFinished(int,QProcess::ExitStatus)));

}


void WDRequest::processFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    qDebug() << "WDRequest: process finished";
    // if error, retun empty string list
    if (exitCode>0) {
        emit wdRequestResult(QStringList());
        return;
    }
    QFile file("output.csv");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit wdRequestResult(QStringList());
        return;
    }
    QTextStream stream(&file);
    QString string;
    QStringList list;
    while (true) {
        string = stream.readLine().trimmed();
        if (string.isNull()) break;
        if (string.isEmpty()) continue;
        list << string;
    }
    file.close();
    emit wdRequestResult(list);
}


bool WDRequest::sqlExec(const QString& request) {
    qDebug() << "Requesting " << request << " on " << _wddFilename;
    QFileInfo info(_wddFilename);
    if (!info.exists()) {
        qDebug() << "Cannot find wdd file";
        return false;
    }
    QString path = info.absolutePath();
    QStringList argList;
    argList << _wddFilename << request << QDir::currentPath()+"/output.csv";
    QString execPath = QApplication::applicationDirPath()+"/SQLSelect/SQLSelect.exe";
    qDebug() << "Working directory: " << path;
    qDebug() << execPath << argList;
    _process.setWorkingDirectory(path);
    _process.start(execPath, argList);
    _pid = _process.pid();

}
