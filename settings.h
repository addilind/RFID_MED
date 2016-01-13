#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include "datastore.h"
#include "settingstagassign.h"
#include "Model/medsettings.h"
#include "Model/unitsettings.h"

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(Datastore* dstore, QWidget *parent = 0);
    ~Settings();

private slots:
    void AddNewMed();
    void DeleteMed();
    void AssignUnit();
    void DeleteUnit();
    void UnitChanged(Unit* u);

private:
    Ui::Settings *ui;
    Datastore* dstore;
    SettingsTagAssign* tagAssign;

    medSettings medications;
    unitSettings units;
};

#endif // SETTINGS_H
