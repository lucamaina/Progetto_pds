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
            this->symMap.insert(idx, sym);
            //this->_symbols.push_back(sym);
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



//void Editor::localDelete(int index) {
//    if ( index < _symbols.size() ){
//        // verifico indice presente nel vettore
//        Symbol sym = _symbols.at(index);
//        _symbols.erase( _symbols.begin()+index );
//    } else {
//        // possibile msg errore
//    }
//}



double Editor::fmed(double num1, double num2) {
    double division=2.0;
    num1 = (num1+num2)/division;
    qDebug()<<num1<<num2;

    return num1;
}


double Editor::insertLocal(double index,char value){
    Symbol s;
    auto lista=symMap.keys();

     if(symMap.empty()){  // il primo parte da 1
          s=Symbol(username,value,1);
          symMap.insert(1,s);
         return 0;
     }

     if(lista.size()<=index){ //prendi l ultimo index float e fai +1
        qDebug()<<"seh negro"<<index;
        s=Symbol(username,value,index+1);
        symMap.insert(index+1,s);
        return index;
     }

     else if(index==0){ //tra 0 e il primo
         double tmpidx = fmed(0,symMap.firstKey());
         s=Symbol(username,value,tmpidx);
         symMap.insert(tmpidx,s);
         return 0;

     }
     else{  // (prima+dopo) / 2
         double tempmin=0,tempmax=0;

             qDebug()<<lista.front()<<"lalalal"<<index;
             while(index>1){
                 index--;
                 lista.pop_front();
             }
             tempmin=lista.front();
             lista.pop_front();
             qDebug()<<tempmin;
             tempmax=lista.front();

         double tmpidx = fmed(tempmin,tempmax);
          s=Symbol(username,value,tmpidx);
         symMap.insert(tmpidx,s);
         return tmpidx;
     }

};



