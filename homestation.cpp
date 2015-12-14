#include "homestation.h"
#include "ui_homestation.h"

Homestation::Homestation(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Homestation)
{
    ui->setupUi(this);
}

Homestation::~Homestation()
{
    delete ui;
}
