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

#define INIT_DIM 6
#define LEN_NUM 8

#define UNAME "username"
#define NICK "nickname"
#define PASS "password"

#define queryLOGIN "SELECT Count(*) FROM utenti WHERE UserName = :UserName AND Password = :Password;"
#define queryREG "INSERT INTO utenti(UserName, NickName, Password) VALUE (:UserName, :NickName, :Password);"


#endif // CMDSTRING_H
