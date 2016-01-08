#include "homestation.h"
#include "ui_homestation.h"

#include "Model/medication.h"

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
    if(reader != nullptr)
        delete reader;
    delete ui;
}

void Homestation::SetSerialPort(const QString& portname)
{
    serialPort = portname;
}

void Homestation::SetDBFile(const QString &fileName)
{
    dbFileName = fileName;
}

void Homestation::Ready()
{
    ui->mainPrompt->setText(tr("Bereit"));
    ui->explanation->setText(tr("Medikament in Lesebereich legen"));
    ui->progressBar->setEnabled(false);
}

void Homestation::SubmoduleError(const std::exception& ex)
{
    ui->mainPrompt->setText(tr("Fehler"));
    ui->explanation->setText(ex.what());
    ui->progressBar->setEnabled(false);
}

void Homestation::UnitChanged(Unit *u)
{
    if(u == nullptr)
    {
        Ready();
        return;
    }
    if(!u->IsMedicationSet())
    {
        unknownUnit();
        return;
    }

    ui->mainPrompt->setText(tr("Medi"));
    ui->explanation->setText(tr("WTF"));
    ui->progressBar->setEnabled(false);
}

void Homestation::showEvent(QShowEvent *event)
{
    try {
        if(dstore == nullptr)
        {
            dstore = new Datastore(dbFileName, this);
            connect(dstore, SIGNAL(UnitChanged(Unit*)), this, SLOT(UnitChanged(Unit*)));
        }

        ui->explanation->setText(tr("Verbinde zu Lesegerät"));

        if(reader == nullptr)
        {
#ifdef USESIM
            reader = new Simulator(this);
            reader->show();
#else
            reader = new Reader(serialPort, this);
#endif
            connect(reader, SIGNAL(ConnectionEstablished()), this, SLOT(Ready()));
            connect(reader, SIGNAL(ErrorOccured(std::exception)), this, SLOT(SubmoduleError(std::exception)));
            connect(reader, SIGNAL(TagChanged(bool,uint)), dstore, SLOT(TagChanged(bool,uint)));
            reader->BeginConnect();
        }
    }
    catch(std::exception& ex){
        ui->mainPrompt->setText(tr("Fehler"));
        ui->explanation->setText(ex.what());
        ui->progressBar->setEnabled(false);
    }
}

void Homestation::unknownUnit()
{
    ui->mainPrompt->setText(tr("Unbekanntes\nMedikament\nerfasst"));
    ui->explanation->setText(tr("Bitte Packung in den Einstellungen eintragen."));
    ui->progressBar->setEnabled(false);
}
