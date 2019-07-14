#include "server_main.h"

server_main::server_main(QObject *parent) : QObject(parent)
{
    mydb = new db();
}
