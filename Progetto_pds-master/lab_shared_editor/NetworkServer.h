//
// Created by mnalc on 06/05/2019.
//

#ifndef LAB_3_NETWORKSERVER_H
#define LAB_3_NETWORKSERVER_H

#include <queue>
#include <map>
#include "Message.h"

class SharedEditor;

class NetworkServer {
private:
    std::map<int , SharedEditor*> _map_editor;
    std::queue<Message> _msg_queue;
    int key_count = 0;

public:
    int connect(SharedEditor* sharedEditor);
    void disconnect(SharedEditor* sharedEditor);
    void send(const Message& m);
    void dispatchMessages();
};


#endif //LAB_3_NETWORKSERVER_H
