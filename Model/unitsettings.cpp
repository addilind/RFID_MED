#include "unitsettings.h"

constexpr const char* unitSettings::tableHeaders[];

unitSettings::unitSettings(Datastore* dstore, QObject *parent)
    : QAbstractTableModel(parent), dstore(dstore)
{
    UpdateBuffer();
}

int unitSettings::rowCount(const QModelIndex &parent) const
{
    return tagIds.size();
}

int unitSettings::columnCount(const QModelIndex &parent) const
{
    return sizeof(tableHeaders)/sizeof(*tableHeaders);
}

QVariant unitSettings::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if(role != Qt::DisplayRole)
        return QVariant();

    Unit u = dstore->GetUnit(tagIds[index.row()]);
    if(index.column() == 0) { //zugeordnetes Medikament
        if(u.IsMedicationSet())
            return u.GetMedication().GetName();
        else
            return tr("<leer>");
    }
    else if(index.column() == 1)
        return u.GetTimesSeen();
    return QVariant();
}

QVariant unitSettings::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch(role) {
        case Qt::DisplayRole:
            if(orientation == Qt::Horizontal) {
                if( (section>=0 && section < columnCount()) )
                    return tr(tableHeaders[section]);
            }
            else if(section < tagIds.size())
                return tagIds[section];
            break;
    }
    return QVariant();
}

void unitSettings::removeRow(int row)
{
    assert(row < tagIds.size());
    beginRemoveRows(QModelIndex(), row, row);

    endRemoveRows();
}

Qt::ItemFlags unitSettings::flags(const QModelIndex &index) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
}

void unitSettings::UpdateBuffer()
{
    tagIds = dstore->GetUnitTagIds();
}
