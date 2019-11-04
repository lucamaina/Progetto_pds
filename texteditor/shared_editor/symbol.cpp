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


int Symbol::getNum() const
{
    return num;
}

void Symbol::setNum(const int &value)
{
    num = value;
}

int Symbol::getDen() const
{
    return den;
}

void Symbol::setDen(const int &value)
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

int Symbol::getIndex() const
{
    return index;
}

void Symbol::setIndex(const int &value)
{
    index = value;
}
