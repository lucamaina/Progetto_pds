
/*
 * Classe implementa l'editor condiviso
 */

#include "editor.h"

Editor::Editor(QObject *parent) : QObject(parent)
{

}

Editor::Editor(QString Id, QString fName, QByteArray body, QString username)
{
    this->nomeFile = fName;
    this->username = username;
    this->DocId = Id;
    QByteArray b = QByteArray::fromBase64(body);
    QDataStream out(&b, QIODevice::ReadWrite);
    out >> this->symMap;
}

/**
 * @brief Editor::loadMap
 * @return
 * old code
 */
bool Editor::loadMap()
{
    QByteArray ba = this->file->readLine();
    int idx = 1;
    while (!ba.isNull()) {
        for (int i = 0; i< ba.size(); i++){
            QChar c = ba.at(i);
            Symbol sym = Symbol("Client", c, idx,1);      // username = server
            this->symMap.insert(idx, sym);
            //this->_symbols.push_back(sym);
            idx++;
        }
        ba = this->file->readLine();
    }
    return true;
}

/**
 * @brief Editor::~Editor
 */
Editor::~Editor()
{
    this->file->remove();
}

double Editor::getLocalIndexDelete(int posCursor)
{
    auto lista = symMap.keys();
    if (posCursor > lista.size() + 1 || posCursor < 0){
        return -1;
    }
    double tempmax = lista.at(posCursor);
    return tempmax;
}

double Editor::getLocalIndexInsert(int posCursor)
{
    double index = posCursor;
    auto lista = symMap.keys();

    if(symMap.empty()){  // il primo parte da 1
        index=1;
        return index;
     }

    if(lista.size()<=posCursor){ //prendi l ultimo index float e fai +1
        index = posCursor+1;
        return index;
    } else {
        double tempmin=0,tempmax=0;
        if (posCursor > 0){
            tempmin = lista.at(posCursor-1);
        }
        tempmax = lista.at(posCursor);


        double tmpidx = fmed(tempmin,tempmax);

        if ( fequal(tmpidx, tempmin) || fequal(tmpidx, tempmax)){
            return -1;   // spazio tra i double insufficiente, ricaricare il file e l'editor
        }

       return tmpidx;
    }
}

int Editor::localPosCursor(double index)
{
    auto keyList = this->symMap.keys();

    if (keyList.contains(index)){
        return keyList.indexOf(index);
    }
    return -1;
}





/*********************************************************************************************************
 ************************ metodi di gestione della mappa **************************************************
 *********************************************************************************************************/


/**
 * @brief Editor::fmed
 * @param num1
 * @param num2
 * @return indice medio tra precedente e successivo
 */
double Editor::fmed(double num1, double num2) {
    double mid = (num1 + num2)/static_cast<double>(2);
    qDebug()<< "Num1: "<<num1
            << " mid: "<<mid
            << " Num2: "<< num2;
    return mid;
}

/**
 * @brief Editor::fequal
 * @param a
 * @param b
 * @return verifica l'uguaglianza tra double
 */
bool Editor::fequal(double a, double b)
{    return qFuzzyCompare(a, b);    }

QString Editor::getTesto()
{
    QString testo;
    QList<Symbol> lista = this->symMap.values();
    for(Symbol s : lista){
        testo.append(s.car);
    }
    return testo;
}

/**
 * @brief Editor::insertLocal
 * @param index
 * @param value
 * @param formato
 * @return
 * nuova versione
 */
double Editor::insertLocal(double index,char value, QTextCharFormat formato){
    Symbol s = Symbol(username,value,1,formato);
    double flag = -1;
    if (symMap.contains(index)){
        //cancello carattere in textEdit e lo riscrivo
        flag = index;
    }
    symMap.insert(index, s);

    return flag;
}


int Editor::deleteLocal(double index, char car){
    if( this->symMap.contains(index) ){
        if (symMap.value(index).car == car){
            auto lista = symMap.keys();
            int posCur = lista.lastIndexOf(index);
            symMap.remove(index);
            return posCur;
        }
    }
}

void Editor::updateFormat(double index, QTextCharFormat formato){
    if( fequal(index, 0) ){
        return;
    } else{
        double i = 0;
        for(double d : symMap.keys()){
            i++;
            if( fequal(i, index) ){
                auto t=symMap.find(d);
                t->setFormat(formato);
                Symbol s = symMap.take(d);
                s.setFormat(formato);
                symMap.insert(d,s);
                return;
            }
        }
    }
}
