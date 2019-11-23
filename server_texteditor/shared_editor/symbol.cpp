#include "symbol.h"

QChar Symbol::getCar() const
{
    return car;
}

void Symbol::setCar(const QChar &value)
{
    car = value;
}

int Symbol::getPosX() const
{
    return posX;
}

void Symbol::setPosX(int value)
{
    posX = value;
}

int Symbol::getPosY() const
{
    return posY;
}

void Symbol::setPosY(int value)
{
    posY = value;
}

QByteArray Symbol::getFormato() const
{
    return formato;
}

void Symbol::setFormato(const QByteArray &value)
{
    formato = value;
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

void Symbol::setIndex(const double &value)
{
    index = value;
}


QDataStream &operator<<(QDataStream &out, const Symbol &sym)
{
    out << sym.getUserName() << sym.getIndex() << sym.getCar() << sym.getPosX() << sym.getPosY() << sym.getFormato();
    return out;
}

QDataStream &operator >>(QDataStream &in, Symbol &sym)
{
    in.setFloatingPointPrecision(QDataStream::DoublePrecision);
    in >> sym.userName >> sym.index >> sym.car >> sym.posX >> sym.posY >> sym.formato;
    return in;
}

