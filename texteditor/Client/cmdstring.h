#ifndef CMDSTRING_H
#define CMDSTRING_H

/****************************************************************************
 * nomi dei comandi definiti ************************************************/
#define INIT "<INIT>"               // token di inizio messaggio
#define CMD "CMD"

#define CONN "CONNECT"
#define DISC "DISCONNETC"
#define LOGIN "LOG-IN"
#define LOGOUT "LOG-OUT"
#define REG "REGISTER"

#define REM_IN "REMOTE-INSERT"
#define REM_DEL "REMOTE-DELETE"

#define FILES "FILE"
#define ADD_FILE "ADD-FILE"
#define OPEN_FILE "OPEN-FILE"
#define REMOVE_FILE "REMOVE-FILE"

#define FBODY "FILE-BODY"
#define BROWS "BROWS"
#define CRS "CURSOR"

#define OK "OK"
#define ERR "ERR"


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



/****************************************************************************
 * messaggi standard ********************************************************/
#define CONN_ERR "error - connection to db not working"
#define CONN_OK "ok - connection to db work"

#define LOGIN_ERR "error - login failed"
#define LOGIN_OK "ok - login success"

#define LOGOUT_ERR "error - logout failed"
#define LOGOUT_OK "ok - logout success"

#define REG_ERR "error - registration failed"
#define REG_OK "ok - registration success"

#define FILE_ERR "error - user cannot open file"
#define FILE_OK "ok - file open"


/****************************************************************************
 * dimensioni utili *********************************************************/
#define INIT_DIM 6
#define LEN_NUM 8


#endif // CMDSTRING_H
