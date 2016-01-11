#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include "datastore.h"

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(Datastore* dstore, QWidget *parent = 0);
    ~Settings();

private:
    Ui::Settings *ui;
    Datastore* dstore;
};

#endif // SETTINGS_H
