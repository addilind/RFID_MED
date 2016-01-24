#ifndef UNITSETTINGS_H
#define UNITSETTINGS_H

#include <QAbstractTableModel>
#include "../datastore.h"

class unitSettings : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit unitSettings(Datastore* dstore, QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void removeRow(int row);

    Qt::ItemFlags flags(const QModelIndex & index) const override;

    void UpdateBuffer();
private:
    Datastore* dstore;
    static const char* tableHeaders[3];
    std::vector<uint> tagIds;
};

#endif // UNITSETTINGS_H
