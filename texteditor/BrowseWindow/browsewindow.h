#ifndef BROWSEWINDOW_H
#define BROWSEWINDOW_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QComboBox>
#include <QGridLayout>
#include <QStringList>
#include <QDebug>

class BrowseWindow: public QDialog
{
    Q_OBJECT

public:
    BrowseWindow();
    void addScelta(QString& nome);
private:
    void setUpGUI();

    QComboBox* comboScelta;
    QLabel* labelScelta;
    QDialogButtonBox* buttons;

public slots:
    void slotOpenFile();
    void slotAddFile();


signals:
    void openFileSignal(QString& filename);
    void addFileSignal(QString& filename);

};

#endif // BROWSEWINDOW_H
