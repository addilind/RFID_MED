#ifndef DATASTORE_H
#define DATASTORE_H

#include <QObject>
#include <QSqlDatabase>

class Datastore : public QObject
{
    Q_OBJECT
public:
    explicit Datastore(QObject *parent = 0);
    ~Datastore();

signals:

public slots:

private:
    QSqlDatabase db;

    void initializeDB();
};

#endif // DATASTORE_H
