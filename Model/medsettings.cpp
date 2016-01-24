#include "medsettings.h"

const char* medSettings::tableHeaders[8] = {"Name", "Mo", "Di", "Mi", "Do", "Fr", "Sa", "So" };

medSettings::medSettings(Datastore* dstore, QObject *parent)
    : QAbstractTableModel(parent), dstore(dstore)
{
    UpdateBuffer();
}

int medSettings::rowCount(const QModelIndex &parent) const
{
    return medIds.size();
}

int medSettings::columnCount(const QModelIndex &parent) const
{
    return sizeof(tableHeaders)/sizeof(*tableHeaders);
}

QVariant medSettings::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    if(role != Qt::DisplayRole  && role != Qt::EditRole)
        return QVariant();

    Medication med = dstore->GetMedication(medIds[index.row()]);
    if(index.column() == 0) {
        auto name = med.GetName();
        return name;
    }
    else if(index.column() <= 7)
        return static_cast<uint>(med.GetDailyCount(index.column() - 1));
            //Spaltenindex 1 = Montag (0); Spalte 7 = Sonntag (6)
    return QVariant();
}

QVariant medSettings::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch(role) {
        case Qt::DisplayRole:
            if(orientation == Qt::Horizontal) {
                if( (section>=0 && section < columnCount()) )
                    return tr(tableHeaders[section]);
            }
            else
                return section + 1;
            break;
    }
    return QVariant();
}

bool medSettings::setData(const QModelIndex &index, const QVariant &value, int role)
{
    switch(role) {
        case Qt::EditRole:
            {
                Medication med = dstore->GetMedication(medIds[index.row()]);
                if(index.column() == 0) {
                    med.SetName(value.toString());
                    emit dataChanged(index, index, { Qt::DisplayRole });
                    return true;
                }
                else if(index.column() <= 7)
                {
                    bool isValid = false;
                    uint val = value.toUInt(&isValid);
                    if(isValid)
                    {
                        if(val>255)
                            val = 255;
                        med.SetDailyCount(index.column()-1, val);
                        emit dataChanged(index, index, { Qt::DisplayRole });
                        return true;
                    }
                }
            }
            break;
    }
    return false;
}

void medSettings::insertRow()
{
    beginInsertRows(QModelIndex(), medIds.size(), medIds.size());
    medIds.push_back(
                dstore->AddMedication(QApplication::tr("Neues Medikament"))
                .GetDbId());
    endInsertRows();
}

void medSettings::removeRow(int row)
{
    assert(row < medIds.size());
    beginRemoveRows(QModelIndex(), row, row);

    Medication med = dstore->GetMedication(medIds[row]);
    auto associatedUnits = dstore->GetUnitsByMedication(med);
    std::for_each(associatedUnits.begin(), associatedUnits.end(),
                  [](Unit &u){ u.UnsetMedication(); });
        //Alle Schachteln, die zu diesem Medikament gehören, zurücksetzen

    med.Delete();
    medIds.erase(medIds.begin() + row);
    endRemoveRows();
}

Qt::ItemFlags medSettings::flags(const QModelIndex &index) const
{
    return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemNeverHasChildren;
}

void medSettings::UpdateBuffer()
{
    medIds = dstore->GetMedicationIds();
}
