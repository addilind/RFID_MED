#ifndef HOMESTATION_H
#define HOMESTATION_H

#include <QMainWindow>

namespace Ui {
class Homestation;
}

class Homestation : public QMainWindow
{
    Q_OBJECT

public:
    explicit Homestation(QWidget *parent = 0);
    ~Homestation();

private:
    Ui::Homestation *ui;
};

#endif // HOMESTATION_H
