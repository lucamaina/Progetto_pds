QT -= gui
QT += sql
QT += network
QT += core


CONFIG += c++11 console
CONFIG -= app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        database/db.cpp \
        logger/logger.cpp \
        main.cpp \
        server/s_thread.cpp \
        server/server.cpp \
        server_main.cpp \
        utente/utente.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    database/Nuovo documento di testo.txt \
    database/Nuovo documento di testo.txt

HEADERS += \
    database/db.h \
    logger/logger.h \
    server/s_thread.h \
    server/server.h \
    server_main.h \
    utente/utente.h
