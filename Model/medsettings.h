#ifndef MEDSETTINGS_H
#define MEDSETTINGS_H

#include <QAbstractTableModel>
#include "../datastore.h"

class medSettings : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit medSettings(Datastore* dstore, QObject *parent = 0);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole) override;

    void insertRow();
    void removeRow(int row);

    Qt::ItemFlags flags(const QModelIndex & index) const override;
private:
    Datastore* dstore;
    constexpr static const char* tableHeaders[] = {"Name", "Mo", "Di", "Mi", "Do", "Fr", "Sa", "So" };
    std::vector<uint> medIds;

    void UpdateBuffer();
};

#endif // MEDSETTINGS_H
