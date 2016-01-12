#include "settings.h"
#include "ui_settings.h"
#include <set>

Settings::Settings(Datastore *dstore, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings),
    dstore(dstore),

    medications(dstore, this),
    units(dstore, this)
{
    ui->setupUi(this);
    ui->medicationList->setModel(&medications);
    ui->medicationList->resizeColumnsToContents();
    ui->unitList->setModel(&units);

    connect(ui->medAddBtn, SIGNAL(pressed()), this, SLOT(AddNewMed()));
    connect(ui->medDelBtn, SIGNAL(pressed()), this, SLOT(DeleteMed()));
}

Settings::~Settings()
{
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
