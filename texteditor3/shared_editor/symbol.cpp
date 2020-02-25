#include "symbol.h"

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

double Symbol::getIndex() const
{
    return index;
}

void Symbol::setIndex(const int &value)
{
    index = value;
}

QDataStream &operator<<(QDataStream& out, const Symbol &sym){
    out << sym.getUserName()<<sym.getIndex()<<sym.car<<sym.qbformat;
    return out;
}

QDataStream &operator>>(QDataStream& in, Symbol &sym){
    in >> sym.userName>>sym.index>>sym.car>>sym.qbformat;
    return in;
}

