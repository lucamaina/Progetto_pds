#include "symbol.h"

QChar Symbol::getCar() const
{
    return car;
}

QByteArray Symbol::getFormato() const
{
    return formato;
}

void Symbol::setFormato(const QByteArray &value)
{
    formato = value;
}

QVector<qint32> Symbol::getIndici() const
{
    return indici;
}

QMap<QString, QString> Symbol::toMap()
{
    QMap<QString, QString> map;
    map.insert(UNAME, this->userName);
    map.insert(CAR, this->car);
    QString idx;
    for (qint32 val : this->indici){
        idx.append(QString::number(val) + ";");
    }
    map.insert(IDX, idx);
    return map;
}

QTextCharFormat Symbol::getFormat(void) const
{
    QTextCharFormat myformat;
    QByteArray myarray(this->formato);
    QDataStream in(&myarray, QIODevice::ReadWrite);
   in >> myformat;
    return myformat;
}

void Symbol::setFormat(const QTextCharFormat &format)
{
    formato = serialize(format);
    //formato = format; DEBUG
}

QString Symbol::getUserName() const
{
    return userName;
}

void Symbol::setUserName(const QString &value)
{

    userName = value;
}


QDataStream &operator<<(QDataStream &out, const Symbol &sym)
{
    out.setVersion(QDataStream::Qt_5_12);
    out << sym.getUserName()<< sym.getIndici() << sym.getCar() << sym.getFormato();
    return out;
}

QDataStream &operator >>(QDataStream &in, Symbol &sym)
{
    in.setVersion(QDataStream::Qt_5_12);
    in  >> sym.userName     >> sym.indici    >> sym.car      >> sym.formato;
    return in;
}

bool Symbol::operator ==(const Symbol &s)
{
    return s.getIndici() == this->getIndici();
}

