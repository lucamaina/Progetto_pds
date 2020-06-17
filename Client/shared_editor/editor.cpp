
/*
 * Classe implementa l'editor condiviso
 */

#include "editor.h"

QString Editor::getNomeFile() const
{
    return nomeFile;
}

QString Editor::getDocId() const
{
    return DocId;
}

Editor::Editor(QObject *parent) : QObject(parent)
{
    
}

Editor::Editor(QString Id, QString fName, QByteArray& body, QString username)
{
    this->nomeFile = fName;
    this->username = username;
    this->DocId = Id;

    QDataStream stream(&body, QIODevice::ReadWrite);
    stream >> this->symVec;
}


/**
 * @brief Editor::~Editor
 */
Editor::~Editor()
{
    this->file->remove();
}

/**
 * @brief Editor::getLocalIndexDelete
 * @param posCursor
 * @return
 *
 */
bool Editor::getLocalIndexDelete(int posCursor, Symbol& sym)
{
    if ( posCursor < 0 || posCursor > symVec.size() ){
        // cursore fuori dalla mappa, errore
        return false;
    }
    sym = symVec.at(posCursor);
    return true;
}

/**
 * @brief Editor::getLocalIndexInsert
 * @param posCur
 * @return
 * crea nuovo indice alla posizione del cursore
 */
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



/**
 * @brief Editor::localPosCursor
 * @param index
 * @return
 * trova posizione in cui possono essere inseriti i nuovi indici
 */
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

/**
 * @brief Editor::findLocalPosCursor
 * @param index
 * @return
 * cerca posizione con idici uguali
 */
int Editor::findLocalPosCursor(QVector<qint32> &index)
{
    for (Symbol s : this->symVec){
        auto idx = s.getIndici();
        if ( idx == index){
            return symVec.indexOf(s);
        }
    }
    return -1; // non trovato
}

void Editor::updateFormat(int pos, QTextCharFormat format)
{
    // update formato

    return;
}
