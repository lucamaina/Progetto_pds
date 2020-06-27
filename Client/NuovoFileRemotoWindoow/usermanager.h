#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QListWidget>
#include <QListWidgetItem>
#include <QDebug>
#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QFrame>

class UserManager: public QDialog
{
    Q_OBJECT
public:
    UserManager(QString myself);
    void setUpGUI();

private:
    QString myself;
    QGridLayout* formGridLayout;
    QDialogButtonBox* buttons;
    QListWidget* listaUser;
    QLabel* lblAddUser, *lbltitoloAdd, *lblTitoloRemove;
    QLineEdit* addUsers;
    QDialogButtonBox* btnAddUser;
    QList<QListWidgetItem>* listItem;

public slots:
    void caricaUsers(QList<QString>&lista);
    void checkItem(QListWidgetItem *item);
    void addUser();
    void removeUser();

signals:
    void s_addUsers(QStringList &);
    void s_removeUsers(QStringList &);
};

#endif // USERMANAGER_H
