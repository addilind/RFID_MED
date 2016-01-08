#include "simulator.h"
#include "ui_simulator.h"

Simulator::Simulator(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Simulator),
    tagPresent(false), tagId(0)
{
    ui->setupUi(this);
    connect(ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(changeTag()));
}

Simulator::~Simulator()
{
    delete ui;
}

void Simulator::BeginConnect()
{
    emit ConnectionEstablished();
}

bool Simulator::getTagPresent()
{
    return tagPresent;
}

uint Simulator::getLastTagId()
{
    return tagId;
}

void Simulator::changeTag()
{
    auto idstr = ui->lineEdit->text();
    bool isInt = false;
    uint id = idstr.toUInt(&isInt, 16);
    if(isInt)
    {
        if(tagPresent && tagId == id)
            return; //Selber Tag

        tagPresent = true;
        tagId = id;
        emit TagChanged(true, tagId);
    }
    else
    {
        if(tagPresent)
        {
            tagPresent = false;
            emit TagChanged(false, tagId);
        }
    }
    ui->lineEdit->selectAll();
}
