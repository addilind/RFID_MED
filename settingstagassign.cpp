#include "settingstagassign.h"
#include "ui_settingstagassign.h"

SettingsTagAssign::SettingsTagAssign(Datastore *dstore, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsTagAssign),
    currentTarget(nullptr),
    dstore(dstore)
{
    ui->setupUi(this);
    connect(dstore, SIGNAL(UnitChanged(Unit*)), this, SLOT(UnitChanged(Unit*)));
    connect(ui->applyBtn, SIGNAL(pressed()), this, SLOT(Apply()));
}

SettingsTagAssign::~SettingsTagAssign()
{
    if(currentTarget != nullptr)
    {
        delete currentTarget;
        currentTarget = nullptr;
    }
    delete ui;
}

void SettingsTagAssign::UnitChanged(Unit *u)
{
    if(u == nullptr)
        return; //Bei Entfernen eines Tags zuletzt gesehenes beibehalten
    ui->tagIdBox->setText( QString("%1").arg(u->GetTagId(), 8, 16, QChar('0')) );
            //Tag-ID zu 8-Stelligem Hex-String mit führenden nullen
    if(currentTarget != nullptr)
        delete currentTarget;
    currentTarget = new Unit(*u); //Kopieren, da Pointer ungültig wird
    ui->applyBtn->setEnabled(true);
}

void SettingsTagAssign::Apply()
{
    currentTarget->SetMedication(dstore->GetMedication(ui->medicationBox->currentData().toUInt()));
    close();
}

void SettingsTagAssign::showEvent(QShowEvent *event)
{
    /*ui->tagIdBox->setText(""); //Zurücksetzen
    if(currentTarget != nullptr)
    {
        delete currentTarget;
        currentTarget = nullptr;
    }
    currentTarget = nullptr;
    ui->medicationBox->clear();
    ui->applyBtn->setEnabled(false);*/

    auto meds = dstore->GetMedicationIds();
    for(auto med : meds)
        ui->medicationBox->addItem(dstore->GetMedication(med).GetName(), med);
}
