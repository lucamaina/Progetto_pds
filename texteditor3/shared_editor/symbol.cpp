#include "symbol.h"

QByteArray Symbol::getQbformat() const
{
    return qbformat;
}

void Symbol::setQbformat(const QByteArray &value)
{
    qbformat = value;
}

QChar Symbol::getCar() const
{
    return car;
}

void Symbol::setCar(const QChar &value)
{
    car = value;
}

double Symbol::getIndex() const
{
    return index;
}

void Symbol::setIndex(double value)
{
    index = value;
}

Symbol::Symbol()
{

}

QMap<QString, QString> Symbol::toMap()
{
    QMap<QString, QString> map;
    map.insert(UNAME, this->userName);
    map.insert(CAR, this->car);
    map.insert(IDX, QString::number(this->index));
    return map;
}

QTextCharFormat Symbol::getFormat(void) const
{
    QTextCharFormat myformat;
    QByteArray myarray(qbformat);
    QDataStream in(&myarray, QIODevice::ReadWrite);
   in >> myformat;
    return myformat;
}

void Symbol::setFormat(const QTextCharFormat &format)
{
    qbformat=serialize(format);
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
    out.setFloatingPointPrecision(QDataStream::FloatingPointPrecision::DoublePrecision);
    out << sym.getUserName() << sym.getIndex() << sym.getCar() << sym.getQbformat();
    return out;
}

QDataStream &operator >>(QDataStream &in, Symbol &sym)
{
    in.setVersion(QDataStream::Qt_5_12);
    in.setFloatingPointPrecision(QDataStream::DoublePrecision);
    in >> sym.userName >> sym.index >> sym.car >> sym.qbformat;
    return in;
}

