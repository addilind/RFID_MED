#include "settings.h"
#include "ui_settings.h"

Settings::Settings(Datastore *dstore, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings),
    dstore(dstore)
{
    ui->setupUi(this);
}

Settings::~Settings()
{
    delete ui;
}
