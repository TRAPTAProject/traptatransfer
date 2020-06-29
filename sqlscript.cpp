#include "sqlscript.h"
#include <QFileInfo>
#include <QDebug>
#include <QApplication>

SQLScript::SQLScript(const QString& wddFilename, const QString &filename) :
    QObject()
{
    _filename = filename;
    _wddFilename = wddFilename;
    connect(&_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SIGNAL(processFinished(int,QProcess::ExitStatus)));
}


void SQLScript::run() {
    QFileInfo info(_wddFilename);
    QString workingPath = info.absolutePath();
    qDebug() << "WDD file is " << _wddFilename;
    qDebug() << "SQL file is " << _filename;
    QStringList argList;
    argList << _wddFilename << _filename;
    _process.setWorkingDirectory(workingPath);
    _process.start(QApplication::applicationDirPath()+"/SQLSelect/SQLScript.exe", argList);
    _pid = _process.pid();

}
