#ifndef UTENTE_H
#define UTENTE_H

#include <QObject>

class utente : public QObject
{
    Q_OBJECT
public:
    explicit utente(QString username, QString password, QObject *parent = nullptr);
signals:

public slots:

private:
    QString username;
    QString nickname;
    QString password;

};

#endif // UTENTE_H
