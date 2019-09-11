//
// Created by mnalc on 06/05/2019.
//

#ifndef LAB_3_MESSAGE_H
#define LAB_3_MESSAGE_H

#include "Symbol.h"

class Message {
public:
    enum type {insert, erase};

private:
    Symbol _sym;
    type _cmd;

public:
    Message (Symbol sym, type comando ) : _sym(sym), _cmd(comando) {};

    const Symbol &get_sym() const {
        return _sym;
    }

    void set_sym(const Symbol &_sym) {
        Message::_sym = _sym;
    }

    type get_cmd() const {
        return _cmd;
    }

    void set_cmd(type _cmd) {
        Message::_cmd = _cmd;
    }
};


#endif //LAB_3_MESSAGE_H
