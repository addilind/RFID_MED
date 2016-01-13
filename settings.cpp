#include "settings.h"
#include "ui_settings.h"
#include <set>

Settings::Settings(Datastore *dstore, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings),
    dstore(dstore),
    tagAssign(new SettingsTagAssign(dstore, this)),
    medications(dstore, this),
    units(dstore, this)
{
    ui->setupUi(this);
    ui->medicationList->setModel(&medications);
    ui->medicationList->resizeColumnsToContents();
    ui->unitList->setModel(&units);

    connect(ui->medAddBtn, SIGNAL(pressed()), this, SLOT(AddNewMed()));
    connect(ui->medDelBtn, SIGNAL(pressed()), this, SLOT(DeleteMed()));
    connect(ui->unitAssignBtn, SIGNAL(pressed()), this, SLOT(AssignUnit()));
    connect(ui->unitDelBtn, SIGNAL(pressed()), this, SLOT(DeleteUnit()));
    connect(dstore, SIGNAL(UnitChanged(Unit*)), this, SLOT(UnitChanged(Unit*)));
}

Settings::~Settings()
{
    delete tagAssign;
    delete ui;
}

void Settings::AddNewMed()
{
    medications.insertRow();
    ui->medicationList->focusWidget();
}

void Settings::DeleteMed()
{
    std::set<int> selectedRows;
    auto selectedCells = ui->medicationList->selectionModel()->selectedIndexes();
    std::for_each(selectedCells.begin(), selectedCells.end(),
         [&selectedRows](const QModelIndex& index){ selectedRows.insert(index.row()); });
    for(auto it = selectedRows.rbegin(); it != selectedRows.rend(); ++it)
    {//Reverse iterator, damit die letzte Zeile zuerst gelöscht wird
        //(ansonsten ändern sich die Zeilennummern)
        medications.removeRow(*it);
    }
    ui->medicationList->focusWidget();
}

void Settings::AssignUnit()
{
    tagAssign->show();
}

void Settings::DeleteUnit()
{
    std::set<int> selectedRows;
    auto selectedCells = ui->unitList->selectionModel()->selectedIndexes();
    std::for_each(selectedCells.begin(), selectedCells.end(),
         [&selectedRows](const QModelIndex& index){ selectedRows.insert(index.row()); });
    for(auto it = selectedRows.rbegin(); it != selectedRows.rend(); ++it)
    {//Reverse iterator, damit die letzte Zeile zuerst gelöscht wird
        //(ansonsten ändern sich die Zeilennummern)
        units.removeRow(*it);
    }
    ui->unitList->focusWidget();
}

void Settings::UnitChanged(Unit *u)
{
    if(u != nullptr)
        units.UpdateBuffer();
}
