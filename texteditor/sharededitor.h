#ifndef SHAREDEDITOR_H
#define SHAREDEDITOR_H

#include <QObject>
class Message;
class Symbol;

class sharedEditor : public QObject
{
    Q_OBJECT
public:
    sharedEditor();
    void localInsert(int index, char value);
    void localErase(int index);
    void process(const Message& m);
    std::string to_string();

    int remoteInsert(Symbol sym);
    int remoteErase(Symbol sym);

private:
    int _siteId;
    std::vector<Symbol> _symbols;
    int _counter;
};

#endif // SHAREDEDITOR_H

/**

#include "Message.h"
#include "Symbol.h"

class NetworkServer;

class SharedEditor {

public:
    explicit SharedEditor(NetworkServer& netserv);

    void localInsert(int index, char value);
    void localErase(int index);
    void process(const Message& m);
    std::string to_string();

    int remoteInsert(Symbol sym);
    int remoteErase(Symbol sym);

private:
    NetworkServer& _server;
    int _siteId;
    std::vector<Symbol> _symbols;
    int _counter;
};

*/
