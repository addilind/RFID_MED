#include "homestation.h"
#include "ui_homestation.h"

Homestation::Homestation(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Homestation),
    dstore(nullptr),
    reader(nullptr)
{
    ui->setupUi(this);
}

Homestation::~Homestation()
{
    if(dstore != nullptr)
        delete dstore;
    delete ui;
}

void Homestation::ReaderConnected()
{
    ui->mainPrompt->setText(tr("Bereit"));
    ui->explanation->setText(tr("Medikament in Lesebereich legen"));
    ui->progressBar->setEnabled(false);
}

void Homestation::showEvent(QShowEvent *event)
{
    try {
        if(dstore == nullptr)
        {
            dstore = new Datastore(this);
        }

        ui->explanation->setText(tr("Verbinde zu Lesegerät"));

        if(reader == nullptr)
        {
            reader = new Reader(this);
            connect(reader, SIGNAL(ConnectionEstablished()), this, SLOT(ReaderConnected()));
        }
    }
    catch(std::exception& ex){
        ui->mainPrompt->setText(tr("Fehler"));
        ui->explanation->setText(ex.what());
        ui->progressBar->setEnabled(false);
    }
}
