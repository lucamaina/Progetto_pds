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

Symbol::Symbol()
{

}

QMap<QString, QString> Symbol::toMap()
{
    QMap<QString, QString> map;
    map.insert(UNAME, this->userName);
    map.insert(CAR, this->car);
    map.insert(IDX, QString::number(this->index));
    map.insert(FORMAT, QString(this->formato));
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
    out.setVersion(QDataStream::Qt_5_12);
    out.setFloatingPointPrecision(QDataStream::FloatingPointPrecision::DoublePrecision);
    out << sym.getUserName() << sym.getIndex() << sym.getCar() << sym.getFormato();
    return out;
}

QDataStream &operator >>(QDataStream &in, Symbol &sym)
{
    in.setVersion(QDataStream::Qt_5_12);
    in.setFloatingPointPrecision(QDataStream::DoublePrecision);
    in >> sym.userName >> sym.index >> sym.car >> sym.formato;
    return in;
}

QJsonObject Symbol::toJson(){
  QJsonObject j;
  /*
    j.insert("char",QJsonValue(car));
    j.insert("index",QJsonValue(index));
    j.insert("user",QJsonValue(userName));
    j.insert("font",QJsonValue(ff.fontFamily()));
    j.insert("size",QJsonValue(ff.fontWeight()));
    j.insert("italic",QJsonValue(ff.fontItalic()));
    j.insert("underline",QJsonValue(ff.fontUnderline()));
*/
    return j;
}
