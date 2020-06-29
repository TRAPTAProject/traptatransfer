#include "ratransfert.h"
#include "ui_ratransfert.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include "mystandarditemmodel.h"
#include "sqlscript.h"

RATransfert::RATransfert(int operation, const QStringList &resultList, int startId) :
    QDialog(),
    ui(new Ui::RATransfert)
{
    ui->setupUi(this);
    setWindowTitle("  TRAPTA Transfer 1.8");
    setWindowFlags(Qt::Dialog|Qt::WindowCloseButtonHint);
    _resultList = resultList;
    ui->stackedWidget->setCurrentIndex(0);
    _operation = operation;
    _exitProcess = false;
    _startId = startId;

    QSettings settings;
    ui->lineEditFFTA->setText(settings.value("resultarcpath", "c:/resultarc").toString());

    if (_operation>0) {
        ui->stackedWidget->setCurrentIndex(1);
    }
    else {
        _exitProcess = true;
    }

    connect(ui->okButton, SIGNAL(clicked()), this, SLOT(processOKButton()));
    connect(ui->abortButton, SIGNAL(clicked()), this, SLOT(processAbortButton()));
    connect(ui->chooseDirButton, SIGNAL(clicked()), this, SLOT(processChooseDir()));

}

void RATransfert::setResultList(const QStringList& resultList) {

}

void RATransfert::processOKButton() {
    qDebug() << "OK Button";
    switch (ui->stackedWidget->currentIndex()) {

        case 0: {
            if (ui->radioButton0->isChecked()) _operation=1;
            else _operation = 2;
            ui->stackedWidget->setCurrentIndex(1);
            break;
        }
        case 1: {
            if (_operation==1) chooseCompetition();
            if (_operation==2) {
                if (_exitProcess) {
                    if (!loadResultList()) break;
                }
                createSQLFile();
                QString resultarcPath = getResultARCPath();
                SQLScript* sqlScript = new SQLScript(resultarcPath+"/FFTA.wdd", QDir::currentPath()+"/trapta.sql");
                connect(sqlScript, SIGNAL(processFinished(int,QProcess::ExitStatus)), this, SLOT(sqlScriptFinished(int,QProcess::ExitStatus)));
                sqlScript->run();

            }
            break;
        }
        case 2: {
            int selectedIndex = ui->competitionListWidget->currentIndex().row();
            qDebug() << "Selected index is " << selectedIndex;
            if (selectedIndex<0) {
                qDebug() << "No competition with this index";
                QMessageBox::critical(this, "Sélection d'une compétition", "Aucune compétition sélectionnée. Veuillez sélectionner une compétition.");
                return;
            }
            QString comp = _compList.at(selectedIndex);
            QStringList tab = comp.split(";");
            bool ok;
            int id = tab[0].toInt(&ok);
            if (!ok) return;
            loadArcherList(id, ui->shootIdSpinner->value());
            break;

        }


    }

}

void RATransfert::sqlScriptFinished(int code, QProcess::ExitStatus exitStatus) {

    if (code==0) {
        QMessageBox::information(this, "Mise à jour des scores", "Les scores ont été mis à jour dans Result'arc.");
    }
    else {
        QMessageBox::critical(this, "Erreur mise à jour", "Une erreur est survenue. Tous les scores n'ont pas pu être mis à jour.");
    }

    if (_exitProcess) exit(code);
    else close();

}

bool RATransfert::loadResultList() {
    QSettings settings;
    QString defaultFilename = settings.value("loadFilename", "trapta-results.csv").toString();
    defaultFilename = QFileDialog::getOpenFileName(this, "Liste des résultats de TRAPTA", defaultFilename, tr("CSV (*.csv);; Tout fichier (*.*)"));
    QFileInfo info(defaultFilename);
    if (!info.exists()) return false;

    settings.setValue("loadFilename", defaultFilename);
    QFile traptaFile(defaultFilename);
    if (!traptaFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Erreur lecture fichier", "Impossible de lire le fichier "+defaultFilename);
        return false;
    }
    QTextStream stream(&traptaFile);
    _resultList.clear();
    while (true) {
        QString string = stream.readLine().trimmed();
        if (string.isNull()) break;
        if (string.isEmpty()) continue;
        _resultList << string;
    }
    traptaFile.close();
    return true;
}

void RATransfert::createSQLFile() {
    // cp Resultat.FIC
    QString resultARCPath = getResultARCPath();
    if (resultARCPath.isEmpty()) return;
    QString filepath1 = resultARCPath+"/Resultat.FIC";
    QString filepath2 = resultARCPath+"/Resultat.ndx";
    if (filepath1.isEmpty() || filepath2.isEmpty()) return;

    QString path = QFileInfo(filepath1).canonicalPath();
    qDebug() << "Canonical path to resultarc is: " << path;

    QFile oldfic(path+"/Resultat-old.FIC");
    QFile oldndx(path+"/Resultat-old.ndx");
    if (!oldfic.remove()) qDebug() << "File " << oldfic.fileName() << "has not been deleted";
    if (!oldndx.remove()) qDebug() << "File " << oldndx.fileName() << "has not been deleted";

    if (!QFile::copy(filepath1, path+"/Resultat-old.FIC")) {
        QMessageBox::critical(this, "Copie impossible", "Impossible de copier le fichier Resultat.FIC");
        return;
    }
    if (!QFile::copy(filepath2, path+"/Resultat-old.ndx")) {
        QMessageBox::critical(this, "Copie impossible", "Impossible de copier le fichier Resultat.ndx");
        return;
    }


    QFile traptaSQL(QDir::currentPath()+"/trapta.sql");
    if (!traptaSQL.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Erreur ouverture fichier", "Impossible de créer le fichier "+traptaSQL.fileName());
        return;
    }
    qDebug() << "Output to "+traptaSQL.fileName();
    QTextStream out(&traptaSQL);
    int line = 0;

    foreach (QString string, _resultList) {
        line++;
        qDebug() << string;
        QStringList tab;
        if (string.indexOf(';')>-1) tab = string.split(';'); else tab = string.split(',');
        if (tab.size()<7) {
            qDebug() << "Too few values. I need 7";
            QMessageBox::critical(this, "Erreur ligne "+QString::number(line), "Trop peu de valeurs: "+string);
            continue;
        }
        bool ok;
        int id = tab[0].toInt(&ok);
        if (!ok) {
            qDebug() << "Error while parsing id";
            QMessageBox::critical(this, "Erreur ligne "+QString::number(line), "Id illisible: "+string);
            continue;
        }
        int nb10 = tab[1].toInt(&ok);
        if (!ok) {
            qDebug() << "Error while parsing nb10";
            QMessageBox::critical(this, "Erreur ligne "+QString::number(line), "nb10 illisible: "+string);
            continue;
        }
        int nb9 = tab[2].toInt(&ok);
        if (!ok) {
            qDebug() << "Error while parsing nb9";
            QMessageBox::critical(this, "Erreur ligne "+QString::number(line), "nb9 illisible: "+string);
            continue;
        }
        int score1 = tab[3].toInt(&ok);
        if (!ok) {
            qDebug() << "Error while parsing score1";
            QMessageBox::critical(this, "Erreur ligne "+QString::number(line), "score1 illisible: "+string);
            continue;
        }
        int score2 = tab[4].toInt(&ok);
        if (!ok) {
            qDebug() << "Error while parsing score2";
            QMessageBox::critical(this, "Erreur ligne "+QString::number(line), "score2 illisible: "+string);
            continue;
        }
        int score3 = tab[5].toInt(&ok);
        if (!ok) {
            qDebug() << "Error while parsing score3";
            QMessageBox::critical(this, "Erreur ligne "+QString::number(line), "score3 illisible: "+string);
            continue;
        }
        int score4 = tab[6].toInt(&ok);
        if (!ok) {
            qDebug() << "Error while parsing score4";
            QMessageBox::critical(this, "Erreur ligne "+QString::number(line), "score4 illisible: "+string);
            continue;
        }

        QString sqlDelete = QString("DELETE FROM Resultat WHERE Resultat.IDCibles = %0").arg(id);
        QString sqlInsert = QString("INSERT INTO Resultat ( IDCibles, score_total, nb10, nb9, Result_serie[1], Result_serie[2], Result_serie[3], Result_serie[4]) VALUES (%0, %1, %2, %3, %4, %5, %6, %7)")
                .arg(id).arg(score1+score2+score3+score4).arg(nb10).arg(nb9).arg(score1).arg(score2).arg(score3).arg(score4);
        qDebug() << sqlDelete;
        qDebug() << sqlInsert;
        out << sqlDelete << "\n";
        out << sqlInsert << "\n";

    }

    traptaSQL.close();


}

void RATransfert::loadArcherList(int competitionId, int shootId) {
    QString path = getResultARCPath();
    if (path.isEmpty()) return;
    _wdRequest = new WDRequest(path+"/FFTA.wdd");
    connect(_wdRequest, SIGNAL(wdRequestResult(QStringList)), SLOT(processSQLArcherResult(QStringList)));
    QString sql = QString("SELECT   Cibles.IDCibles,\
                                    Licence.IDLicence, \
                                    abv_categorie_classement, \
                                    NOM, PRENOM, CIE, \
                                    Engagement.no_depart, \
                                    No_cibles, Trispot \
                            FROM \
                                    categorie_Classement, \
                                    Engagement, \
                                    Licence, \
                                    Cibles \
                            WHERE \
                                    (Cibles.TYPe_match=0 AND \
                                    Engagement.IDLicence=Licence.IDLicence AND \
                                    Engagement.IDEngagement = Cibles.IDEngagement AND \
                                    Engagement.IDCategorie_classement = categorie_Classement.IDCategorie_classement AND \
                                    Engagement.IDCompetition=%0 AND \
                                    Engagement.no_depart=%1)")
            .arg(competitionId).arg(shootId);
    qDebug() << sql;
    _wdRequest->sqlExec(sql);
}

QString RATransfert::getResultARCPath() {
    QString wdd = ui->lineEditFFTA->text();
    QString ffta = wdd;
    QFileInfo info(wdd);
    if (info.isFile()) {
        wdd = info.canonicalPath()+"/FFTA.wdd";
        ffta = info.canonicalPath()+"/ffta.exe";
    }
    else {
        wdd.append("/FFTA.wdd");
        ffta.append("/ffta.exe");
    }
    qDebug() << "Checking " << ffta;
    qDebug() << "Checking " << wdd;
    QFileInfo infoFFTA(ffta);
    if (!infoFFTA.exists()) {
        QMessageBox::critical(this, "Fichier ffta.exe introuvable", "Ce dossier ne semble pas contenir Resultarc.");
        return QString();
    }
    // check FFTA.wdd
    QFileInfo infoWDD(wdd);
    if (!infoWDD.exists()) {
        qDebug() << "FFTA.wdd is not there, let's copy it";
        if (!QFile::copy(":/FFTA.wdd", wdd)) {
            QMessageBox::critical(this, "Copie impossible", "Impossible de copier le fichier FFTA.wdd dans "+wdd);
            return QString();
        }
    }

    QSettings settings;
    settings.setValue("resultarcpath", ui->lineEditFFTA->text());
    return infoWDD.canonicalPath();
}


void RATransfert::chooseCompetition() {
    QString path = getResultARCPath();
    if (path.isEmpty()) return;
    _wdRequest = new WDRequest(path+"/FFTA.wdd");
    connect(_wdRequest, SIGNAL(wdRequestResult(QStringList)), this, SLOT(processSQLCompetitionResult(QStringList)));
    _wdRequest->sqlExec("SELECT IDCompetition, lb_competition, Lieu_competition, date_debut_competition FROM Competition");
}

void RATransfert::processAbortButton() {
    qDebug() << "Abort Button";
    if (_exitProcess) {
        exit(0);
    }
    else {
        close();
    }
}

QStringList RATransfert::formatCompetitionList(const QStringList& list) const {
    QStringList competitionList;
    foreach (QString string, list) {
        QStringList tab = string.split(";");
        if (tab.length()<4) continue;
        QString competition = tab[1]+" "+tab[2]+" "+tab[3];
        competitionList << competition;
    }
    return  competitionList;
}

void RATransfert::processSQLCompetitionResult(const QStringList& list) {
    qDebug() << "Got result list for competition";

    _compList.clear();
    if (list.isEmpty()) {
        QMessageBox::critical(this, "Aucune compétition", "Aucune compétition n'a été trouvée dans Result'arc");
        return;
    }
    _compList = list;
    ui->competitionListWidget->addItems(formatCompetitionList(list));
    ui->stackedWidget->setCurrentIndex(2);

}

QStringList RATransfert::archerList() const {
    return _archerList;
}

void RATransfert::processSQLArcherResult(const QStringList& list) {
    qDebug() << "Got result list for archers!";
    if (!_exitProcess) {
        _archerList = list;
        close();
        return;
    }
    else {
        QSettings settings;
        QString defaultFilename = settings.value("saveFilename", "resultarc-export.csv").toString();
        QString filename = QFileDialog::getSaveFileName(this, "Enregistrer la liste des archers",defaultFilename);
        if (filename.isEmpty()) {
            QMessageBox::critical(this, "Erreur fichier", "Le nom de fichier sélectionné est vide.");
            return;
        }
        settings.setValue("saveFilename", filename);
        QFile file(filename);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
           QMessageBox::critical(this, "Erreur fichier", "Erreur durant l'ouverture de "+filename);
            return;
        }
        QTextStream out(&file);
        foreach (QString string, list) {
            out << string.trimmed() << "\n";
        }
        file.close();
        QMessageBox::information(this, "Liste des archers", "La liste des archers a été enregistrée dans le fichier "+filename);
        exit(0);
    }
}

void RATransfert::processChooseDir() {
    QString file = QFileDialog::getExistingDirectory(this, "Où est le répertoire de Result'Arc ?", "c:/", QFileDialog::ShowDirsOnly
                                                     | QFileDialog::DontResolveSymlinks);

    qDebug() << "Chosen dir=" << file;
    if (!file.isEmpty()) ui->lineEditFFTA->setText(file);
}

RATransfert::~RATransfert() {
    delete ui;
}
