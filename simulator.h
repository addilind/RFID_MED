#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <QDialog>

namespace Ui {
class Simulator;
}

class Simulator : public QDialog
{
    Q_OBJECT
public:
    explicit Simulator(QWidget *parent = 0);
    ~Simulator();
    void BeginConnect();

    bool getTagPresent();
    uint getLastTagId();

signals:
    void ConnectionEstablished();
    void ErrorOccured(const std::exception& exception_msg);
    void TagChanged(bool present, uint id);

private slots:
    void changeTag();

private:
    Ui::Simulator *ui;

    bool tagPresent;
    uint tagId;
};

#endif // SIMULATOR_H
