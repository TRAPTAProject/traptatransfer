#ifndef WDREQUEST_H
#define WDREQUEST_H

#include <QObject>
#include <QStringList>
#include <QProcess>

class WDRequest : public QObject {
    Q_OBJECT
public:
    explicit WDRequest(const QString& wddFilename, QObject *parent = 0);
    bool sqlExec(const QString& request);

signals:

    void wdRequestResult(QStringList);

public slots:

    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:

    QString _wddFilename;
    QProcess _process;
    Q_PID _pid;


};

#endif // WDREQUEST_H
