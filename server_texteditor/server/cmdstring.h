#ifndef CMDSTRING_H
#define CMDSTRING_H

#define INIT "<INIT>"
#define CONN "CONNECT"
#define LOGIN "LOG-IN"
#define REG "REGISTER"
#define REM_IN "REMOTE_INSERT"
#define REM_DEL "REMOTE_DELETE"
#define DISC "DISCONNETC"
#define FILES "FILE"
#define ADD_FILE "ADD_FILE"
#define CMD "CMD"

#define INIT_DIM 6
#define LEN_NUM 8

#define UNAME "username"
#define NICK "nickname"
#define PASS "password"

#define queryLOGIN "SELECT connesso, NickName FROM utenti WHERE UserName = ? AND Password = ?;"
#define queryUpdateLOGIN "UPDATE utenti SET connesso = '1' WHERE UserName = :UserName;"
#define queryLOGOUT "UPDATE utenti SET connesso = '0' WHERE UserName = :UserName;"
#define queryREG "INSERT INTO utenti(UserName, NickName, Password) VALUE (:UserName, :NickName, :Password);"
#define queryPreReg "SELECT Count(*) FROM utenti WHERE UserName = :UserName AND Password = :Password FOR UPDATE;"

#define queryPROVA "SELECT connesso FROM utenti where UserName = 'asd' AND Password = '1' FOR UPDATE;"
#define queryP "SELECT connesso FROM utenti where UserName = ?;"


#endif // CMDSTRING_H
