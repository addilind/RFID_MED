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

void Homestation::SetSerialPort(const QString& portname)
{
    serialPort = portname;
}

void Homestation::SetDBFile(const QString &fileName)
{
    dbFileName = fileName;
}

void Homestation::ReaderConnected()
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

void Homestation::showEvent(QShowEvent *event)
{
    try {
        if(dstore == nullptr)
        {
            dstore = new Datastore(dbFileName, this);
        }

        ui->explanation->setText(tr("Verbinde zu Lesegerät"));

        if(reader == nullptr)
        {
            reader = new Reader(serialPort, this);
            connect(reader, SIGNAL(ConnectionEstablished()), this, SLOT(ReaderConnected()));
            connect(reader, SIGNAL(ErrorOccured(std::exception)), this, SLOT(SubmoduleError(std::exception)));
            reader->BeginConnect();
        }
    }
    catch(std::exception& ex){
        ui->mainPrompt->setText(tr("Fehler"));
        ui->explanation->setText(ex.what());
        ui->progressBar->setEnabled(false);
    }
}
