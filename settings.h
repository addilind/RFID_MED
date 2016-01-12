#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include "datastore.h"
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

private:
    Ui::Settings *ui;
    Datastore* dstore;

    medSettings medications;
    unitSettings units;
};

#endif // SETTINGS_H
