#ifndef SYMBOL_H
#define SYMBOL_H

#include <QObject>
#include <QMap>
#include <QTextCharFormat>
#include "Client/cmdstring.h"
#include "QDebug"
#include "Utils/utils.h"
class Editor;

class Symbol
{
public:
    QChar car;
    QString userName;           // identifica utente che inserische il carattere
    QTextCharFormat textFormat;
    QByteArray formato;
    // versione con QVector
    QVector<qint32> indici;


friend QDataStream &operator <<(QDataStream& out, const Symbol& sym);
friend QDataStream &operator >>(QDataStream& in, Symbol& sym);
bool operator ==(const Symbol &s);

public:
    explicit Symbol();
    explicit Symbol(QString user, QChar car, QVector<qint32> idx, QByteArray formato) :  car(car), userName(user), formato(formato), indici(idx)
    { textFormat = deserialize<QTextCharFormat>(formato);}

    explicit Symbol(QString user, QChar car, QVector<qint32> idx, QTextCharFormat format) :  car(car), userName(user), textFormat(format),indici(idx)
    { formato = serialize(format); }

    QMap<QString, QString> toMap();
    QList<Symbol> & read(QList<Symbol> &testo);

    void setFormat(const QTextCharFormat &format);
    QTextCharFormat getFormat(void) const;

    QString getUserName() const;
    void setUserName(const QString &value);

    QChar getCar() const;

    QByteArray getFormato() const;
    void setFormato(const QByteArray &value);

    QVector<qint32> getIndici() const;

signals:
    void s_read(QChar c, QTextCharFormat f, qint32 pos);

};


#endif // SYMBOL_H
