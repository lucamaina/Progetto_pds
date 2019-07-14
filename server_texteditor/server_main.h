#ifndef SERVER_MAIN_H
#define SERVER_MAIN_H

#include <QObject>
#include <database/db.h>

class server_main : public QObject
{
    Q_OBJECT
public:
    explicit server_main(QObject *parent = nullptr);

private:
    db* mydb;

signals:

public slots:
};

#endif // SERVER_MAIN_H
