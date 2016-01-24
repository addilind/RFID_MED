#include "unitsettings.h"

const char* unitSettings::tableHeaders[] = {"Medikament", "Erfassungen", "Zuletzt"};

unitSettings::unitSettings(Datastore* dstore, QObject *parent)
    : QAbstractTableModel(parent), dstore(dstore), tagIds(dstore->GetUnitTagIds())
{
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
    switch(index.column()) {
        case 0://zugeordnetes Medikament
            if(u.IsMedicationSet())
                return u.GetMedication().GetName();
            return tr("<nicht zugewiesen>");
        case 1:
            return u.GetTimesSeen();
        case 2:
            return u.GetLastSeen();
        default:
            return QVariant();
    }
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
                return QString("%1").arg(tagIds[section], 8, 16, QChar('0'));
                    //Tag-ID zu 8-Stelligem Hex-String mit führenden nullen
            break;
    }
    return QVariant();
}

void unitSettings::removeRow(int row)
{
    assert(row < tagIds.size());
    beginRemoveRows(QModelIndex(), row, row);
    dstore->GetUnit(tagIds[row]).Delete();
    tagIds.erase(tagIds.begin() + row);
    endRemoveRows();
}

Qt::ItemFlags unitSettings::flags(const QModelIndex &index) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
}

void unitSettings::UpdateBuffer()
{
    auto newTagIds = dstore->GetUnitTagIds();
    int newrows = newTagIds.size() - tagIds.size();
    if(newrows > 0)
        beginInsertRows(QModelIndex(), tagIds.size(), tagIds.size() + newrows - 1);
            //size der alten Liste ist erste neue Zeilennummer
            //letze neue Zeilennummer ist erste neu Zeilennummer + Anzahl neuer Zeilen - 1
    else if(newrows < 0)
        beginRemoveRows(QModelIndex(), tagIds.size() + newrows, tagIds.size() - 1);

    tagIds = newTagIds;
    emit dataChanged(createIndex(0,0), createIndex(tagIds.size()-1, columnCount()));

    if(newrows > 0)
        endInsertRows();
    else if(newrows < 0)
        endRemoveRows();
}
