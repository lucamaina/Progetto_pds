#include "symbol.h"

Symbol::Symbol()
{

}

uint Symbol::getNum() const
{
    return num;
}

void Symbol::setNum(const uint &value)
{
    num = value;
}

uint Symbol::getDen() const
{
    return den;
}

void Symbol::setDen(const uint &value)
{
    den = value;
}

uint Symbol::getNumR() const
{
    return numR;
}

void Symbol::setNumR(const uint &value)
{
    numR = value;
}

uint Symbol::getDenR() const
{
    return denR;
}

void Symbol::setDenR(const uint &value)
{
    denR = value;
}

QString Symbol::getUserName() const
{
    return userName;
}

void Symbol::setUserName(const QString &value)
{
    userName = value;
}

uint Symbol::getIndex() const
{
    return index;
}

void Symbol::setIndex(const uint &value)
{
    index = value;
}
