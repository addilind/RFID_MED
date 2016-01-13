#ifndef SETTINGSTAGASSIGN_H
#define SETTINGSTAGASSIGN_H

#include <QDialog>
#include "Model/unit.h"
#include "datastore.h"
#include <vector>
#include "Model/medication.h"

namespace Ui {
class SettingsTagAssign;
}

class SettingsTagAssign : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsTagAssign(Datastore* dstore, QWidget *parent = 0);
    ~SettingsTagAssign();

private slots:
    void UnitChanged(Unit* u);
    void Apply();

private:
    Ui::SettingsTagAssign *ui;
    Unit* currentTarget;

    Datastore* dstore;

    virtual void showEvent(QShowEvent *event) override;
};

#endif // SETTINGSTAGASSIGN_H
