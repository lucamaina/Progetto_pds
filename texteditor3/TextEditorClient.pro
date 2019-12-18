QT += widgets
QT += network
requires(qtConfig(filedialog))
qtHaveModule(printsupport): QT += printsupport

TEMPLATE        = app
TARGET          = textedit

HEADERS         = textedit.h \
    BrowseWindow/browsewindow.h \
                  Client/client.h \ \
    Client/cmdstring.h \
    Highlighter/highlighter.h \
    LoginDialog/logindialog.h \
    RegisterDialog/registerdialog.h \
    shared_editor/editor.h \
    shared_editor/message.h \
    shared_editor/symbol.h

SOURCES         = textedit.cpp \
                  BrowseWindow/browsewindow.cpp \
                  Highlighter/highlighter.cpp \
                  LoginDialog/logindialog.cpp \
                  RegisterDialog/registerdialog.cpp \
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
