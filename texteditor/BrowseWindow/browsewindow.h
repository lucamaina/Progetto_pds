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
    void setFileMap(QMap<QString, QString>& fileMap);

private:
    void setUpGUI();

    QComboBox* comboScelta;
    QLabel* labelScelta;
    QDialogButtonBox* buttons;
    QMap<QString, QString> fileMap;

public slots:
    void slotOpenFile();
    void slotAddFile();


signals:
    void openFileSignal(QString& filename, QString& docId);
    void addFileSignal(QString& filename);

};

#endif // BROWSEWINDOW_H
