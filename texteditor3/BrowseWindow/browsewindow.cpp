#include "browsewindow.h"
#include "textedit.h"
#include "../Client/client.h"

BrowseWindow::BrowseWindow()
{
    setUpGUI();
    setWindowTitle( tr("Browse directory") );
}

void BrowseWindow::setUpGUI(){
 // set up the layout
    QGridLayout* formGridLayout = new QGridLayout( this );

    comboScelta= new QComboBox(this);
    comboScelta->setEditable(true);
    comboScelta->addItem("test_file1");
    // comboScelta->addItem("questo è il numero file1");


    labelScelta= new QLabel(this);
    labelScelta->setText( tr( "Choose a file:" ) );
    labelScelta->setBuddy( comboScelta );

    buttons = new QDialogButtonBox( this );
    buttons->addButton( QDialogButtonBox::Ok );
    buttons->addButton( QDialogButtonBox::Cancel );
    buttons->addButton( QDialogButtonBox::Open);
    buttons->button( QDialogButtonBox::Ok )->setText( tr("Open") );
    buttons->button( QDialogButtonBox::Open )->setText( tr("New File") );
    buttons->button( QDialogButtonBox::Cancel )->setText( tr("Cancel") );

    formGridLayout->addWidget( labelScelta, 0, 0 );
    formGridLayout->addWidget( comboScelta, 1, 0 );
    formGridLayout->addWidget( buttons->button(QDialogButtonBox::Ok), 2, 0);
    formGridLayout->addWidget( buttons->button(QDialogButtonBox::Cancel), 2, 1);
    formGridLayout->addWidget( buttons->button(QDialogButtonBox::Open), 1, 1);


    connect( buttons->button( QDialogButtonBox::Cancel ),
    SIGNAL (clicked()),
    this,
    SLOT (close())
    );

   connect( buttons->button( QDialogButtonBox::Ok ),
    SIGNAL (clicked()),
    this,
    SLOT (slotOpenFile()) );

   connect( buttons->button( QDialogButtonBox::Open ),
    SIGNAL (clicked()),
    this,
    SLOT ( slotAddFile()));

    setLayout( formGridLayout );


};

void BrowseWindow::addScelta(QString& nome)
{
    this->comboScelta->addItem(nome);
}

void BrowseWindow::setFileMap(QMap<QString, QString> &fileMap)
{
    this->fileMap = fileMap;
    foreach (QString k, fileMap.keys()){
        QString val = fileMap.value(k);
        this->addScelta(val);
    }
}

void BrowseWindow::slotOpenFile()
{
    QString filename = comboScelta->currentText();
    QString docId = this->fileMap.key(filename);

    emit openFileSignal(filename, docId);

}

/**
 * @brief BrowseWindow::slotAddFile
 * invia massaggio di apertura del file selezionato
 */
void BrowseWindow::slotAddFile()
{
    QString filename = comboScelta->currentText();
    QString docId = this->fileMap.key(filename);

    emit addFileSignal(filename);

}

