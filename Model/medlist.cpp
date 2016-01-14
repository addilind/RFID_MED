#include "medlist.h"
#include <QIcon>
#include <QPixmap>

medList::medList(Datastore* dstore, QObject *parent)
    : QAbstractListModel(parent), dstore(dstore),
      medIds(getMedsForToday())
{
}

int medList::rowCount(const QModelIndex &parent) const
{
    return medIds.size();
}

QVariant medList::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    Medication med = dstore->GetMedication(medIds[index.row()]);
    switch(role) {
        case Qt::DisplayRole:
            return med.GetName();
        case Qt::DecorationRole:
            return med.AnyUnitSeenToday() ?
                        QIcon(":/icons/ok.png").pixmap(16,16) :
                        QIcon(":/icons/missing.png").pixmap(16,16);
    }
    return QVariant();
}

QVariant medList::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch(role) {
        case Qt::DisplayRole:
            return section + 1;
    }
    return QVariant();
}

Qt::ItemFlags medList::flags(const QModelIndex &index) const
{
    return Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
}

void medList::UpdateIcons()
{
    emit dataChanged(createIndex(0,0), createIndex(rowCount(), 0), {Qt::DecorationRole});
}

void medList::UpdateBuffer()
{
    auto newMedIds = getMedsForToday();
    int newrows = newMedIds.size() - medIds.size();
    if(newrows > 0)
        beginInsertRows(QModelIndex(), medIds.size(), medIds.size() + newrows - 1);
            //size der alten Liste ist erste neue Zeilennummer
            //letze neue Zeilennummer ist erste neu Zeilennummer + Anzahl neuer Zeilen - 1
    else if(newrows < 0)
        beginRemoveRows(QModelIndex(), medIds.size() + newrows, medIds.size() - 1);

    medIds = newMedIds;
    emit dataChanged(createIndex(0,0), createIndex(medIds.size()-1, 0));

    if(newrows > 0)
        endInsertRows();
    else if(newrows < 0)
        endRemoveRows();
}

std::vector<uint> medList::getMedsForToday()
{
    auto meds = dstore->GetMedicationIds();
    std::vector<uint> medsToday(meds.size());
    auto dstoreptr = this->dstore;
    auto end = std::copy_if(meds.begin(), meds.end(), medsToday.begin(), [&dstoreptr](const uint& m) {
        return dstoreptr->GetMedication(m).GetDailyCount(QDate::currentDate().dayOfWeek() - 1) > 0;
    } );
    medsToday.resize(std::distance(medsToday.begin(),end)); //Überflüssige Plätze im Zielvector entfernen
    return medsToday;
}
