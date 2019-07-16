//
// Created by mnalc on 06/05/2019.
//

#ifndef LAB_3_SYMBOL_H
#define LAB_3_SYMBOL_H


#include <string>
#include <vector>

class Symbol {
private:
    int symID;
    char car;

public:
    int num;
    int den;
    Symbol(int symID, char car, int num, int den) : symID(symID), car(car), num(num), den(den) {};

    int getSymID() const {
        return symID;
    }

    void setSymID(int symID) {
        Symbol::symID = symID;
    }

    char getChar() const {
        return car;
    }



};


#endif //LAB_3_SYMBOL_H
