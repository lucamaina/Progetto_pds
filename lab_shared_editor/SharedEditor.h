//
// Created by mnalc on 06/05/2019.
//

#ifndef LAB_3_SHAREDEDITOR_H
#define LAB_3_SHAREDEDITOR_H

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


#endif //LAB_3_SHAREDEDITOR_H
