#include "network.h"

Network::Network(QObject *parent) : QObject(parent)
{
    connect(this, SIGNAL(sigSend()), this, SLOT(dispatch()));
}

Network& Network::getNetwork(){
    static std::unique_ptr<Network> p (new Network());
    return *p;
}


void Network::send(Message &msg)
{
    std::lock_guard<std::mutex> lg(mQueue);
    //this->msgQueue.push_back(msg);
    emit sigSend();
}


void Network::dispatch()
{
    std::lock_guard<std::mutex> lg(mQueue);
    if (!msgQueue.isEmpty()){
        while (msgQueue.size() > 0){
            //Message msg = msgQueue.dequeue();
        }
    }
}
