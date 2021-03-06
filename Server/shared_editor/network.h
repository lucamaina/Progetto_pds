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
    bool createEditor(QString fileID, QString nomeFile, utente& userName, QSharedPointer<MySocket> &sock);
    bool createEditor(QMap<QString, QString> cmd);
    void sendToEdit(Message& msg);
    bool filePresent(QString fileId);
    bool addRefToEditor(QString fileId, utente& user, QSharedPointer<MySocket> &sock);
    bool remRefToEditor(QString fileId, QString user);
    Editor& getEditor(QString docId);
    void salvaTutto();

signals:
    void sigSend(void);

public slots:
    void dispatch();

private:
    std::mutex mQueue;
    QMap<QString, Editor*> editorMap;   // chiave = DocId
    QMap<QString, QPointer<Editor>> eMap;
    QQueue<Message> msgQueue;

    explicit Network(QObject *parent = nullptr);

    // metodi non definiti
    Network(const Network & hold) = delete;
    const Network &operator=(const Network & hold) = delete;
};

#endif // NETWORK_H
