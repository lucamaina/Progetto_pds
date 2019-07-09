# Requisiti di progetto e analisi

## Server
Deve essere in grado di gestire identificativi di:
- utenti: (username, password) info standard per autentucazione e accesso ai file, 
- documenti: (pathToFileSystem, hashToLink) per trovare il file corretto su File System del server e per dare collegamento esterno,

### File System manager
File System gestito come cartella all'inteno del server con sottocartella con username dell'utente e file creati dallo stesso definiti da nome univoco (possible per user vedere directory?).
Salvataggio file automatico (ogni tot secondi?).

Programma multiprocesso con padre in attesa di nuove connessioni e 1 figlio per ogni documento.


 
