//
// Created by mnalc on 06/05/2019.
//

#include <algorithm>
#include "NetworkServer.h"
#include "SharedEditor.h"
#include "Message.h"

/**
 *  Registra, nel proprio vettore di puntatori a SharedEditor, il puntatore ricevuto e restituisce un identificatore
 *  univoco mediante il quale l’editor potrà distinguersi dalle altre istanze della stessa classe
 *  Ritorna indice del vettorre
 */
int NetworkServer::connect(SharedEditor* sharedEditor) {
    if (sharedEditor == nullptr) {
        return -1;
    }
    key_count++;
    _map_editor.insert( {key_count , sharedEditor} );
    return key_count;
}

/**
 *  Elimina il puntatore dal vettore degli editor collegati.
 */
void NetworkServer::disconnect(SharedEditor *sharedEditor) {
    if (sharedEditor == nullptr)
        return;

    for (auto i = _map_editor.begin(); i != _map_editor.end(); i++){
        if (i->second == sharedEditor) {
            // remove
            _map_editor.erase(i);
        }
    }
}

/**
 *  Aggiunge il messaggio m alla coda dei messaggi da distribuire, senza ancora mandarlo.
 */
void NetworkServer::send(const Message& m) {
    _msg_queue.push(m);
}

/**
 *  Distribuisce tutti i messaggi accodati a tutti gli editor attualmente collegati, fatta eccezione, però,
 *  per l’originatore del messaggio stesso.
 */
void NetworkServer::dispatchMessages() {
    int size = _msg_queue.size(); // dimensione iniziale perchè viene modificata
    for (int i = 0; i < size; i++){
        Message tmp_msg = _msg_queue.front();
        _msg_queue.pop();   // elimino msg dalla coda
        int siteId = tmp_msg.get_sym().getSymID();

        for ( auto editor : _map_editor){
            if ( editor.first != siteId) {
                editor.second->process(tmp_msg);
            }
            // else siteId di provenienza
        }
    }
}
