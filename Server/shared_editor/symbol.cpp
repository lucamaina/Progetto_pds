#include "symbol.h"

QChar Symbol::getCar() const
{
    return car;
}

void Symbol::setCar(const QChar &value)
{
    car = value;
}

QByteArray Symbol::getFormato() const
{
    return formato;
}

void Symbol::setFormato(const QByteArray &value)
{
    formato = value;
}

QVector<qint32> Symbol::getIndex() const
{
    return index;
}

void Symbol::setIndex(const QVector<qint32> &value)
{
    index = value;
}

Symbol::Symbol()
{
    
}

QMap<QString, QString> Symbol::toMap()
{
    QMap<QString, QString> map;
    {   // elementi sempre presenti
        map.insert(UNAME, this->userName);
        QString indici;
        for (qint32 val : this->index){
            indici.append(QString::number(val) + ";");
        }
        map.insert(IDX, indici);
    }
    if ( !this->car.isNull() ){
        map.insert(CAR, this->car);
    }
    if ( !this->formato.isEmpty() && !this->formato.isNull() ){
        map.insert(FORMAT, QString(this->formato));
    }
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

bool Symbol::operator ==(const Symbol &s)
{
    return s.getIndex() == this->getIndex();
}



QDataStream &operator<<(QDataStream &out, const Symbol &sym)
{
    out.setVersion(QDataStream::Qt_5_12);
    out << sym.getUserName()<< sym.getIndex() << sym.getCar() << sym.getFormato();
    return out;
}

QDataStream &operator >>(QDataStream &in, Symbol &sym)
{
    in.setVersion(QDataStream::Qt_5_12);
    in  >> sym.userName     >> sym.index    >> sym.car      >> sym.formato;
    return in;
}

