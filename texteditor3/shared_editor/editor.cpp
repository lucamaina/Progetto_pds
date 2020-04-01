
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
    auto lista = symList.keys();
    if (posCursor > lista.size() + 1 || posCursor < 0){
        return -1;
    }
    double tempmax = lista.at(posCursor);
    return tempmax;
}

// cerca symbol in posizione posCursor
QVector<qint32> Editor::getLocalIndexInsert(qint32 posCur) // genera indici di symbol
{
    qDebug() << "sono in " << Q_FUNC_INFO;
    QVector<qint32> ret, tmp, indexPre, indexPost;
    Symbol pre, post;

    if ( symVec.length() == 0){  // caso vector vuoto
        ret.push_back(1);
        return ret;
    }
    if ( posCur == 0){ // aggiungo primo elemento
        post = this->symVec.first();
        tmp = post.indici;
        ret.insert(0, tmp.length(), 0);
        ret.push_back(1);
        return ret;
    }
    if ( posCur >= symVec.size() ){ // aggiungo ultimo elemento
        pre = symVec.last();
        qint32 last = pre.indici.first();
        if (last + bond < maxDim){
            ret.push_back(last + bond);
        } else {
            ret.push_back(last);
            ret.push_back(1);
        }
        return ret;
    }
    if ( posCur < symVec.size() ){ // aggiungo elemnto in mezzo
        pre = symVec.at(posCur - 1);
        post = symVec.at(posCur);

        indexPre = pre.indici;
        indexPost = post.indici;
        int maxL = std::max(indexPre.length(), indexPost.length()), flag = 0;

        for (int i = 0 ; i < maxL && flag == 0; i++){
            qint32 idxPre = indexPre.value(i, 0), idxPost = indexPost.value(i, 0);
            if ( idxPost - idxPre == 0 ){ // inidci uguali
                ret.push_back(idxPre);
            } else {
                if (idxPost - idxPre == 1 || idxPre + 10 >= this->maxDim){
                    // nuovo livello
                    ret.push_back(idxPre);
                    idxPre = indexPre.value(i +1, 0);
                    if (idxPre + 10 >= this->maxDim){
                        ret.push_back(idxPre);
                        ret.push_back(1);
                    } else{
                        ret.push_back(idxPre + 10);
                    }
                } else if ( idxPost - idxPre > bond ) {
                    ret.push_back(idxPre + bond);
                } else if ( idxPost - idxPre > 1 ) {
                    ret.push_back(idxPre + 1);
                }
                flag = 1;
            }
        }
    }
    return ret;
}




int Editor::localPosCursor(QVector<qint32> &index)
{
    int newPos = -1;
    if (symVec.isEmpty()){
        return 0;
    }
    for (int pos = 0; pos < symVec.size() && newPos == -1; pos++){
        Symbol s = this->symVec.at(pos);
        int i = 0;
        for (qint32 val : s.indici){
            if ( val < index.value(i, 0) ){
                break;
            }
            if ( val > index.value(i, 0) ){
                newPos = pos;
                break;
            }
            i++;
        }
    }
    if (newPos == -1){
        newPos = symVec.size();
    }
    return newPos;
}

double Editor::localInsert(int posCur)
{
    qDebug() << "sono in " << Q_FUNC_INFO;

    QVector<qint32> newIndex = this->getLocalIndexInsert(posCur);

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

QList<Symbol>& Editor::getTesto(QList<Symbol>& testo)
{
    for( Symbol s : this->symList.values() ){
        s.read(testo);
    }
    return testo;
}

void Editor::read()
{

}




int Editor::deleteLocal(double index, char car){
    if( this->symList.contains(index) ){
        if (symList.value(index).car == car){
            auto lista = symList.keys();
            int posCur = lista.lastIndexOf(index);
            symList.remove(index);
            return posCur;
        }
    }
}

void Editor::updateFormat(double index, QTextCharFormat formato){
    if( fequal(index, 0) ){
        return;
    } else{
        double i = 0;
        for(double d : symList.keys()){
            i++;
            if( fequal(i, index) ){
                auto t=symList.find(d);
                t->setFormat(formato);
                Symbol s = symList.take(d);
                s.setFormat(formato);
                symList.insert(d,s);
                return;
            }
        }
    }
}
