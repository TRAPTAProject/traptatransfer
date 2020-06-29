#ifndef MYSTANDARDITEMMODEL_H
#define MYSTANDARDITEMMODEL_H

#include <QStandardItemModel>
#include <QStringList>

class MyStandardItemModel : public QStandardItemModel {
    Q_OBJECT
public:
    explicit MyStandardItemModel(const QStringList& stringList, const QStringList& headerList);
    

signals:
    
public slots:
    
};

#endif // MYSTANDARDITEMMODEL_H
