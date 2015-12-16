#ifndef HOMESTATION_H
#define HOMESTATION_H

#include <QMainWindow>
#include "datastore.h"
#include "reader.h"

namespace Ui {
class Homestation;
}

class Homestation : public QMainWindow
{
    Q_OBJECT

public:
    explicit Homestation(QWidget *parent = 0);
    ~Homestation();

public slots:
    void ReaderConnected();

private:
    Ui::Homestation *ui;
    Datastore* dstore;
    Reader* reader;

    virtual void showEvent(QShowEvent *event);
};

#endif // HOMESTATION_H
