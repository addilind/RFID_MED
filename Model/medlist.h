#ifndef MEDLIST_H
#define MEDLIST_H

#include <QAbstractTableModel>
#include "../datastore.h"

class medList : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit medList(Datastore* dstore, QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void removeRow(int row);

    Qt::ItemFlags flags(const QModelIndex & index) const override;
public slots:
    void UpdateIcons();
    void UpdateBuffer();
private:
    Datastore* dstore;
    std::vector<uint> medIds;
    std::vector<uint> getMedsForToday();
};

#endif // MEDLIST_H
