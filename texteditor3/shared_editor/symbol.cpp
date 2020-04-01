#include "symbol.h"

qint32 Symbol::getIndex() const
{
    return index;
}

void Symbol::setIndex(const qint32 &value)
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



QList<Symbol> & Symbol::read(QList<Symbol>& testo)
{
    if (this->nextLevel.isEmpty()){
        testo.append(*this);
    } else {
        for (Symbol s : nextLevel.values()){
            s.read(testo);
        }
    }
    return testo;
}

/**
 * @brief Symbol::getLocalIndex
 * @param posCur
 * @param vec
 * @return
 * scorre le liste contando i symboli che non hanno next level
 */
qint32 Symbol::getLocalIndex(qint32 posCur, QVector<qint32> &vec)
{
    if (posCur == 0){
        vec.push_front(this->getIndex());
        return 0;
    }
    std::for_each(nextLevel.begin(),
                  nextLevel.end(),
                  [&](Symbol s){
        if (s.getLocalIndex(posCur, vec) == 0){
            vec.push_front(s.index);
        }
                  });

    return posCur--;
}

QDataStream &operator<<(QDataStream& out, const Symbol &sym){
    out << sym.getUserName()<<sym.getIndex()<<sym.car<<sym.formato;
    return out;
}

QDataStream &operator>>(QDataStream& in, Symbol &sym){
    in >> sym.userName>>sym.index>>sym.car>>sym.formato;
    return in;
}

