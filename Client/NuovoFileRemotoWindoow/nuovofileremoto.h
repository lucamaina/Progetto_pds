#ifndef NUOVOFILEREMOTO_H
#define NUOVOFILEREMOTO_H

#include <QDebug>
#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QPushButton>


class nuovoFileRemoto: public QDialog
{
    Q_OBJECT

public:
    nuovoFileRemoto();


private:
    void setUpGUI();

    QGridLayout* formGridLayout;
    QLabel* labelScelta;
    QLineEdit* insertName;
    QDialogButtonBox* buttons;

signals:
    void s_addNewFile(QString& str);

public slots:
    void addNewFile();

};

#endif // NUOVOFILEREMOTO_H
