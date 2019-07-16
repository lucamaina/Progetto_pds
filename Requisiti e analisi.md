# Requisiti di progetto e analisi

## Sicurezza 
Protocollo (TCP o UDP?) criptato con OpenSSL?
Messaggi in formato prorpietario?


## Server
Deve essere in grado di gestire identificativi di:
- utenti: (username, password) info standard per autentucazione e accesso ai file, 
- documenti: (pathToFileSystem, hashToLink) per trovare il file corretto su File System del server e per dare collegamento esterno,
- relazioni tra documenti  e utenti (con vari permessi?)

### Database
Dati e relazioni tra utenti e documenti gestiti con database MYSQL con tabelle:
 - Utenti (username, encripted password, nickname)
 - Documenti (idDoc, pathToFileSystem, hashToLink)
 - Relazioni (username, idDoc, altro)

### File System manager
File System gestito come cartella all'inteno del server con sottocartella con username dell'utente (creata alla registrazione) e file creati dallo stesso definiti da nome univoco (possible per user vedere directory?).
Salvataggio file automatico (ogni tot secondi?).


Programma multiprocesso con padre in attesa di nuove connessioni e 1 figlio per ogni documento.


aqwesdawuierhauiwea
weraw
raeasefas.
aerasdfea

asfdasfasd


## Client 
All'avvio richiede autenticazione dell'utente per accesso alla sua cartella (e/o disponibili).
(Viene mostrata Directory proprietaria e docuemnti recenti?) 

