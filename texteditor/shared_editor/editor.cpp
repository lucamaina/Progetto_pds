/*
 * Classe implementa l'editor condiviso
 * 
 * prova di git
 */

#include "editor.h"

Editor::Editor(QObject *parent) : QObject(parent)
{

}

Editor::Editor(QString Id, QString fName,QString body,QString username)
{
    this->nomeFile = fName;
    this->username=username;
    this->DocId = Id;
    this->file = new QFile(this->nomeFile);
    file->open(QIODevice::ReadWrite | QIODevice::Text);

    if (this->file->isOpen()){
        QTextStream stream(this->file);
        stream << body<<endl;
        this->loadMap();


    }
    file->close();
}

bool Editor::loadMap()
{
    QByteArray ba = this->file->readLine();
    int idx = 1;
    while (!ba.isNull()) {
        for (int i = 0; i< ba.size(); i++){
            QChar c = ba.at(i);
            Symbol sym = Symbol("Client", c, idx,1);      // username = server
           // this->symMap.insert(idx, sym);
            this->_symbols.push_back(sym);
            idx++;
        }
        ba = this->file->readLine();
    }
    return true;
}





Editor::~Editor()
{
    this->file->remove();
}





/*********************************************************************************************************
 ************************ metodi di gestione della mappa **************************************************
 *********************************************************************************************************/



void Editor::localDelete(int index) {
    if ( index < _symbols.size() ){
        // verifico indice presente nel vettore
        Symbol sym = _symbols.at(index);
        _symbols.erase( _symbols.begin()+index );
    } else {
        // possibile msg errore
    }
}

QVector<int> Editor::fsum(int num1, int den1, int num2, int den2) {

    int den = den1*den2;
    int num = num1*den2 + num2*den1;

    QVector<int> v;
    v.push_back(num);
    v.push_back(den);
    return v;
}

QVector<int> Editor::fmed(int num1, int den1, int num2, int den2) {

    int den = den1*den2*2;
    int num = num1*den2 + num2*den1;

    QVector<int> v;
    v.push_back(num);
    v.push_back(den);
    return v;
}


int Editor::insertLocal(int index,char value){
    Symbol s;

     if(_symbols.empty()){
          s=Symbol(username,value,1,1);
         _symbols.push_back(s);
         return 0;
     }

     if(_symbols.size()<index+1){
        QVector<int> tmpvec = fsum(1,1,_symbols.back().getNum(),_symbols.back().getDen());
         s=Symbol(username,value,tmpvec.at(0),tmpvec.at(1));
        _symbols.push_back(s);
        return _symbols.size()-1;
     }

     else if(index==0){
         QVector<int> tmpvec = fmed(index,1,_symbols.back().getNum(),_symbols.back().getDen());
          s=Symbol(username,value,tmpvec.at(0),tmpvec.at(1));
        _symbols.insert(_symbols.begin()+index,s);
        return index;

     }
     else{
         QVector<int> tmpvec = fmed(_symbols.at(index).getNum(),_symbols.at(index).getDen(),_symbols.at(index).getNum(),_symbols.at(index).getNum());
         auto pos = _symbols.begin()+index;
          s=Symbol(username,value,tmpvec.at(0),tmpvec.at(1));
         _symbols.insert(pos,s);
         return std::distance(_symbols.begin(),pos);
     }

//    if (index >= _symbols.size() ) {
//            // indice fuori dal vettore
//            index = _symbols.size();
//            tmp_num = index;
//            tmp_den = 1;
//        } else if (index >= 1) {
//            // indice in vettore != 0
//            // num = num_pos + num_pre;
//            Symbol post_idx = _symbols.at(index);
//            Symbol pre_idx = _symbols.at(index - 1);

//            tmp_num = post_idx.getNum() + pre_idx.getNum();
//            tmp_den = pre_idx.getDen() + pre_idx.getDen();
//        } else {
//            // indice <= 0
//            index = 0;
//            tmp_num = 0;
//            tmp_den = 1;
//        }

//    Symbol sym = Symbol( this->username, value,tmp_num, tmp_den );
//        _symbols.insert( _symbols.begin() + index, sym );

    return index;
};



