#-------------------------------------------------
#
# Project created by QtCreator 2019-05-01T17:54:48
#
#-------------------------------------------------

QT += widgets
QT += network
requires(qtConfig(filedialog))
qtHaveModule(printsupport): QT += printsupport

TEMPLATE        = app
TARGET          = textedit

HEADERS         = textedit.h \
    windows/browsewindow.h \
                  Client/client.h \ \
    Client/cmdstring.h \
    Client/comando.h \
    Client/mysocket.h \
    windows/logindialog.h \
    windows/nuovofileremoto.h \
    windows/usermanager.h \
    windows/registerdialog.h \
    Utils/utils.h \
    shared_editor/editor.h \
    shared_editor/message.h \
    shared_editor/symbol.h

SOURCES         = textedit.cpp \
                  windows/browsewindow.cpp \
                  Client/comando.cpp \
                  Client/mysocket.cpp \
                  windows/logindialog.cpp \
                  windows/nuovofileremoto.cpp \
                  windows/usermanager.cpp \
                  windows/registerdialog.cpp \
                  Utils/utils.cpp \
                  main.cpp \
                  Client/client.cpp \
                  shared_editor/editor.cpp \
                  shared_editor/message.cpp \
                  shared_editor/symbol.cpp

RESOURCES += textedit.qrc
build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release

    CONFIG += c++11
}

EXAMPLE_FILES = textedit.qdoc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/richtext/textedit
INSTALLS += target
