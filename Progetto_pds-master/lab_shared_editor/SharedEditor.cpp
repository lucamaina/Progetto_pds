//
// Created by mnalc on 06/05/2019.
//

#include <algorithm>
#include <iostream>
#include "SharedEditor.h"
#include "NetworkServer.h"


SharedEditor::SharedEditor(NetworkServer &netserv) : _server(netserv) {
    _siteId = _server.connect(this);
    _counter = 0;
}

/**
 *  Questo metodo costruisce un symbol che incapsula il carattere value, gli associa un identificativo
 *  univoco ed una posizione frazionaria tale da risultare compresa tra le posizioni frazionarie degli
 *  elementi di _symbols all’indice index-1 e index (se esistenti). A seguito dell’inserimento,
 *  nell’indice index verrà a trovarsi il nuovo simbolo, e tutti quelli successivi scaleranno di una unità.
 *  Le loro posizioni frazionarie, tuttavia, non verranno modificate e si manterrà sempre l’invariante
 *  tale per cui tutti gli elementi adiacenti nel vettore _symbols hanno posizione frazionaria
 *  strettamente minore l’una dell’altra.
 *
 *  A seguito dell’inserimento, prepara un oggetto di tipo Message, all’interno del quale descrive
 *  l’azione compiuta: inserimento da parte di _siteId del
 *  carattere value, con identificativo univoco e posizione frazionaria corrispondente. Tale
 *  messaggio verrà inviato all’oggetto _server che provvederà ad accodarlo, in attesa di inviarlo a
 *  tutte le altre istanze della classe SharedEditor.
 * @param index
 * @param value
 */
void SharedEditor::localInsert(int index, char value) {
    // creo numero frazionario tra index e index-1;
    // c:0(0/1) a:1(1/1) t:2(2/1) -> insert h:1 -> c:0 h:0.5 a:1 t:2 => c:0 h:1(1/2) a:2 t:3
    // insert j:1 -> c:0 j:0(0.25) h:1(1/2) a:2 t3 => c:0(0/1) j:1(1/3) h:2(1/2) a:3(1/1) t4(2/1)

    int tmp_num, tmp_den;

    if (index >= _symbols.size() ) {
        // indice fuori dal vettore
        index = _symbols.size();
        tmp_num = index;
        tmp_den = 1;
    } else if (index >= 1) {
        // indice in vettore != 0
        // num = num_pos + num_pre;
        Symbol post_idx = _symbols.at(index);
        Symbol pre_idx = _symbols.at(index - 1);

        tmp_num = post_idx.num + pre_idx.num;
        tmp_den = pre_idx.den + pre_idx.den;
    } else {
        // indice <= 0
        index = 0;
        tmp_num = 0;
        tmp_den = 1;
    }
    // eseguo modifica al vettore
    Symbol sym = Symbol( this->_siteId, value,tmp_num, tmp_den );
    _symbols.insert( _symbols.begin() + index, sym );

    Message msg = Message(sym, Message::insert);
    this->_server.send(msg);
}

/**
 *  ricostruisce la sequenza dei caratteri contenuta nell’editor
 */
std::string SharedEditor::to_string() {
    std::string testo;
    std::for_each( _symbols.begin(), _symbols.end(),
            [&testo] (auto i) {
        testo = testo + i.getChar();
    } );

    return testo;
}

/**
 *  Questo metodo elimina dal vettore _symbols l’elemento all’indice indicato, prepara un oggetto
 *  di tipo Message in cui descrive l’azione compiuta e lo affida all’oggetto _server affinché lo
 *  consegni agli altri SharedEditor.
 */
void SharedEditor::localErase(int index) {
    if ( index < _symbols.size() ){
        // verifico indice presente nel vettore
        Symbol sym = _symbols.at(index);
        sym.setSymID(_siteId);
        Message msg = Message(sym, Message::erase);
        this->_server.send(msg);
        _symbols.erase( _symbols.begin()+index );
    } else {
        // possibile msg errore
    }
}

/**
 *  esamina il contenuto del messaggio m e provvede a eseguirne le relative azioni:
 *  se si tratta di un messaggio di inserimento provvede ad identificare, a partire dalla posizione
 *  frazionaria contenuta nel messaggio, l’indice nel vettore _symbols in cui inserire il nuovo simbolo;
 *  se, invece, si tratta di una cancellazione, cerca nel vettore _symbols se è presente alla posizione
 *  frazionaria contenuta nel messaggio un simbolo con l’identificatore univoco corrispondente e, nel
 *  caso, lo elimina
 */
void SharedEditor::process(const Message &m) {
    switch (m.get_cmd()) {
        case Message::insert : this->remoteInsert( m.get_sym() );
            break;

        case Message::erase : this->remoteErase( m.get_sym() );
            break;
    }
}

/**
 *  se si tratta di un messaggio di inserimento provvede ad identificare, a partire dalla posizione
 *  frazionaria contenuta nel messaggio, l’indice nel vettore _symbols in cui inserire il nuovo simbolo;
 *
 * @param sym simbolo da aggiungere
 * @return index se successo, -1 se errore
 */
int SharedEditor::remoteInsert(Symbol sym) {
    std::vector< Symbol >::iterator iter;
    iter = std::find_if( _symbols.begin(), _symbols.end(),
            [sym](auto i) {
        double res1 = i.num / i.den, res2 = sym.num / sym.den;
                return res1 > res2;
    });
    _symbols.insert(iter, sym);

    return 0;
}

int SharedEditor::remoteErase(Symbol sym) {
    std::vector< Symbol >::iterator iter;
    iter = std::find_if( _symbols.begin(), _symbols.end(),
                         [sym](auto i) {
                             double res1 = i.num / i.den, res2 = sym.num / sym.den;
                             return res1 == res2;
                         });

    if (iter != _symbols.end()){
        _symbols.erase(iter);
    }

    return 0;
}

