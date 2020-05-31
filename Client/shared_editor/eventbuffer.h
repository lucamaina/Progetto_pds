#ifndef EVENTBUFFER_H
#define EVENTBUFFER_H

#include <QObject>

class EventBuffer
{
public:
    QEvent *evento;
    int pos;

    EventBuffer(QEvent *e, int pos):evento(e), pos(pos){}


};

#endif // EVENTBUFFER_H
