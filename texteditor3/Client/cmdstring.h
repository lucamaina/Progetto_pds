#ifndef CMDSTRING_H
#define CMDSTRING_H

#define INIT "<INIT>"               // token di inizio messaggio
#define CONN "CONNECT"
#define LOGIN "LOG-IN"
#define LOGOUT "LOG-OUT"
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
#define CRS "CURSOR"
#define ULIST "USER-LIST"
#define ADD_USER "ADD-USER"
#define REM_USER "REM-USER"
#define UP_CRS "UP-CRS"

#define OK "OK"
#define ERR "ERR"

#define MAX_THREAD 8

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
#define FORMAT "format"


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

#define ADD_USER_ERR "error - Utenti non aggiunti"
#define REM_USER_ERR "error - Utenti non rimossi"

#endif // CMDSTRING_H
