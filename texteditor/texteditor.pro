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
                  Client/client.h \ \
    Client/cmdstring.h \
    Highlighter/highlighter.h \
    LoginDialog/logindialog.h \
    RegisterDialog/registerdialog.h

SOURCES         = textedit.cpp \
                  Highlighter/highlighter.cpp \
                  LoginDialog/logindialog.cpp \
                  RegisterDialog/registerdialog.cpp \
                  main.cpp \
                  Client/client.cpp

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
