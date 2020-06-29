#ifndef RATRANSFERT_H
#define RATRANSFERT_H

#include <QWidget>
#include "wdrequest.h"
#include <QDialog>

namespace Ui {
class RATransfert;
}

class RATransfert : public QDialog
{
    Q_OBJECT

public:
    explicit RATransfert(int operation, const QStringList& resultList = QStringList(), int startId=1);
    QStringList archerList() const;
    void setResultList(const QStringList& resultList);
    ~RATransfert();


private:
    Ui::RATransfert *ui;
    WDRequest* _wdRequest;

    bool _exitProcess;
    int _operation; // 0: choose, 1:extract, 2: modify results
    QStringList _compList;
    QStringList _archerList;
    QStringList _resultList;
    int _startId;

    void chooseCompetition();
    bool loadResultList();
    void loadArcherList(int competitionId, int shootId);
    void displayResultList();
    QString getResultARCPath();
    QStringList formatCompetitionList(const QStringList& list) const;
    void loadListResult();
    void createSQLFile();

private slots:

    void processSQLCompetitionResult(const QStringList&);
    void processSQLArcherResult(const QStringList&);
    void processOKButton();
    void processAbortButton();
    void processChooseDir();
    void sqlScriptFinished(int code, QProcess::ExitStatus exitStatus);

};

#endif // RATRANSFERT_H
