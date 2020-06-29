#ifndef SQLSCRIPT_H
#define SQLSCRIPT_H

#include <QObject>
#include <QProcess>

class SQLScript : public QObject {
    Q_OBJECT
public:
    explicit SQLScript(const QString& wddFilename, const QString& filename);
    void run();

signals:
    void processFinished(int,QProcess::ExitStatus);


private:
    QProcess _process;
    Q_PID _pid;
    QString _filename;
    QString _wddFilename;


};

#endif // SQLSCRIPT_H
