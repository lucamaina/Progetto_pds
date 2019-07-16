#ifndef SERVER_H
#define SERVER_H

#include <QObject>

class server : public QObject
{
    Q_OBJECT
public:
    explicit server(QObject *parent = nullptr);

signals:

public slots:
};

#endif // SERVER_H
