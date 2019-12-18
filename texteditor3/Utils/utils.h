#ifndef UTILS_H
#define UTILS_H
#include "QByteArray"
#include "QDataStream"

template<class T>

QByteArray serialize(T toserial){
    QByteArray s;
    QDataStream out(&s, QIODevice::ReadWrite);
    out << toserial;
    return s;
}

template<class T>

T deserialize(QByteArray qba){
    T deserial;
    QDataStream in(&qba, QIODevice::ReadWrite);
    in >> deserial;
    return deserial;
}
#endif // UTILS_H
