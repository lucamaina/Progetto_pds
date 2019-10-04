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


double Symbol::getNum() const
{
    return num;
}

void Symbol::setNum(const double &value)
{
    num = value;
}

double Symbol::getDen() const
{
    return den;
}

void Symbol::setDen(const double &value)
{
    den = value;
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
