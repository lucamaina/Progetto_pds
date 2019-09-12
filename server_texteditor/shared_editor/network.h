#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QMap>
#include <QQueue>
#include <mutex>
#include <memory>
#include "editor.h"

class Network : public QObject
{
    Q_OBJECT
public:
    static Network& getNetwork();
    void send(Message &msg);

signals:
    void sigSend();     // segnale

public slots:
    void dispatch();

private:
    std::mutex mQueue;
    QMap<QString, Editor*> editorMap;
    QQueue<Message> msgQueue;


     explicit Network(QObject *parent = nullptr);

    // metodi non definiti
    Network(const Network & hold) = delete;
    const Network &operator=(const Network & hold) = delete;
};

#endif // NETWORK_H
