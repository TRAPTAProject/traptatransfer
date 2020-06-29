#include "mystandarditemmodel.h"

MyStandardItemModel::MyStandardItemModel(const QStringList& stringList, const QStringList& headerList) :
    QStandardItemModel() {

    setRowCount(stringList.size());
    int row = 0;
    foreach (QString string, stringList) {
        QStringList tab = string.split(";");
        if (tab.length()==0) tab = string.split(",");
        for (int index=0; index<tab.length(); index++) {
            QStandardItem* cell = new QStandardItem(tab[index]);
            setItem(row, index, cell);
        }
        row++;
    }
    setHorizontalHeaderLabels(headerList);

}
