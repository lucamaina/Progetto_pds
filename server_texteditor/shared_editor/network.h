#ifndef NETWORK_H
#define NETWORK_H

#include <QObject>
#include <QMap>
#include <QQueue>
#include <mutex>
#include <memory>
#include <exception>
#include "editor.h"

class Network : public QObject
{
    Q_OBJECT
public:
    static Network& getNetwork();
    void push(Message &msg);
    void msgRead(Message &msg);
    void createEditor(QString nomeFile, utente& userName);
    void createEditor(QMap<QString, QString> cmd);
    void addUserToEditor(Editor& editor, utente& user); //
    void sendToEdit(Message &msg);

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
