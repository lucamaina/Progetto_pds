#ifndef UTENTE_H
#define UTENTE_H

#include <QObject>

class utente : public QObject
{
    Q_OBJECT
public:
    explicit utente(QObject *parent = nullptr);

signals:

public slots:
};

#endif // UTENTE_H
