
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
            Symbol sym = Symbol("Client", c, idx,"");      // username = server
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


double Editor::insertLocal(double index,char value, QTextCharFormat formato){
    Symbol s;
    auto lista=symMap.keys();
     if(symMap.empty()){  // il primo parte da 1
          s=Symbol(username,value,1,formato);
          symMap.insert(1,s);
         return 0;
     }

     if(lista.size()<=index){ //prendi l ultimo index float e fai +1
        s=Symbol(username,value,index+1,formato);
        symMap.insert(index+1,s);
        return index;
     }

     else if(index==0){ //tra 0 e il primo
         double tmpidx = fmed(0,symMap.firstKey());
         s=Symbol(username,value,tmpidx,formato);
         symMap.insert(tmpidx,s);
         return 0;

     }
     else{  // (prima+dopo) / 2
         double tempmin=0,tempmax=0;

             while(index>1){
                 index--;
                 lista.pop_front();
             }
             tempmin=lista.front();
             lista.pop_front();
             //qDebug()<<tempmin;   DEBUG
             tempmax=lista.front();

         double tmpidx = fmed(tempmin,tempmax);
          s=Symbol(username,value,tmpidx,formato);
         symMap.insert(tmpidx,s);
         return tmpidx;
     }

};


void Editor::deleteLocal(double index){
    if(index==0){return;}
    else{
        double i=0;
        for(double d : symMap.keys()){
            i++;
            qDebug()<<"indice: "<<i<<"  "<<d;
            if(i==index){symMap.remove(d);
                         break;}
        }

    }
    return;
}

void Editor::updateFormat(double index, QTextCharFormat formato){
    if(index==0){return;}
    else{
        double i=0;
        for(double d : symMap.keys()){
            i++;
            if(i==index){auto t=symMap.find(d);
                         t->setFormat(formato);
                         Symbol s=symMap.take(d);
                         s.setFormat(formato);
                         symMap.insert(d,s);
                         break;}
        }
    }
    //for(double d : symMap.keys()){qDebug()<<symMap.find(d)->getFormat().fontFamily();} //DEBUG
}
