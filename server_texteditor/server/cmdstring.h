#ifndef CMDSTRING_H
#define CMDSTRING_H

/****************************************************************************
 * nomi dei comandi definiti ************************************************/
#define INIT "<INIT>"               // token di inizio messaggio
#define CONN "CONNECT"
#define LOGIN "LOG-IN"
#define LOGOFF "LOG-OFF"
#define REG "REGISTER"

#define REM_IN "REMOTE-INSERT"
#define REM_DEL "REMOTE-DELETE"
#define DISC "DISCONNETC"
#define FILES "FILE"
#define ADD_FILE "ADD-FILE"
#define OPEN_FILE "OPEN-FILE"
#define REMOVE_FILE "REMOVE-FILE"
#define FBODY "FILE-BODY"
#define CMD "CMD"
#define BROWS "BROWS"
#define CURSOR "CURSOR"

#define OK "OK"
#define ERR "ERR"

#define PATH "files/"

/****************************************************************************
 * dimensioni messaggi definiti *********************************************/
#define INIT_DIM 6
#define LEN_NUM 8

/****************************************************************************
 * identificatori di utenti e files definiti ********************************/
#define UNAME "username"
#define NICK "nickname"
#define PASS "password"
#define FNAME "filename"
#define DOCID "docid"

#define CAR "char"
#define IDX "index"
#define DIMTOT "dimtot"
#define BODY "body"
#define MEX "msg"
#define CUR "cursor"


/****************************************************************************
 * query prepared definite **************************************************/
#define queryLOGIN "SELECT connesso, NickName FROM utenti WHERE UserName = ? AND Password = ?;"
#define queryUpdateLOGIN "UPDATE utenti SET connesso = '1' WHERE UserName = :UserName;"

#define queryLOGOUT "UPDATE utenti SET connesso = '0' WHERE UserName = :UserName;"
#define queryREG "INSERT INTO utenti(UserName, NickName, Password) VALUE (:UserName, :NickName, :Password);"

#define queryPreReg "SELECT Count(*) FROM utenti WHERE UserName = :UserName AND Password = :Password FOR UPDATE;"

#define queryFILEexist "SELECT Count(*) FROM documenti WHERE FileSysPath = :FileSysPath;"
#define queryFILEadd "INSERT INTO documenti(FileSysPath, LinkPath) VALUE (:FileSysPath, :LinkPath);"

#define queryBROWS "SELECT * FROM documenti WHERE DocId IN (SELECT DocId FROM relazioni WHERE UserName = :UserName);"
#define queryOPEN "SELECT Count(*) FROM relazioni WHERE UserName = :UserName AND DocId = :DocId;"

#define querySetUpUtenti "UPDATE utenti SET connesso = '0';"

#define queryPROVA "SELECT connesso FROM utenti where UserName = 'asd' AND Password = '1' FOR UPDATE;"
#define queryP "SELECT connesso FROM utenti where UserName = ?;"


#endif // CMDSTRING_H
