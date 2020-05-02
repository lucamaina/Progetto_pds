#include "browsewindow.h"
#include "textedit.h"
#include "../Client/client.h"

#ifdef Q_OS_MAC
const QString srcPath = ":/images/mac";
#else
const QString srcPath = ":/images/win";
#endif


BrowseWindow::BrowseWindow()
{
    setUpGUI();
    setWindowTitle( tr("Sfoglia documenti") );
}

void BrowseWindow::setUpGUI(){
 // set up the layout
    QGridLayout* formGridLayout = new QGridLayout( this );

    listaScelta = new QListWidget(this);
    listaScelta->setMinimumSize(5, 1);

    // comboScelta= new QComboBox(this);
    // comboScelta->setEditable(true);

    labelScelta= new QLabel(this);
    labelScelta->setText( tr( "Choose a file:" ) );
    labelScelta->setBuddy( listaScelta );

    //labelHint = new QLabel("Apri un documento \n"
                         //  "esistente o \n"
//                           "creane uno nuovo", this);

    buttons = new QDialogButtonBox( this );
    buttons->addButton( QDialogButtonBox::Ok );
    buttons->addButton( QDialogButtonBox::Cancel );
    //buttons->addButton( QDialogButtonBox::Open);

    buttons->button( QDialogButtonBox::Ok )->setText( tr("Apri") );
    //buttons->button( QDialogButtonBox::Open )->setText( tr("Nuovo File") );
    buttons->button( QDialogButtonBox::Cancel )->setText( tr("Cancel") );

    formGridLayout->addWidget( labelScelta, 0, 0 );
    formGridLayout->addWidget( listaScelta, 1, 0 ); //comboScelta
    //formGridLayout->addWidget( labelHint, 1, 1);
    formGridLayout->addWidget( buttons->button(QDialogButtonBox::Ok), 2, 0);
    formGridLayout->addWidget( buttons->button(QDialogButtonBox::Cancel), 2, 1);
    //formGridLayout->addWidget( buttons->button(QDialogButtonBox::Open), 0, 1);


    connect( buttons->button( QDialogButtonBox::Cancel ),
             SIGNAL(clicked()),
             this,
             SLOT(close()) );

   connect( buttons->button( QDialogButtonBox::Ok ),
            SIGNAL (clicked()),
            this,
            SLOT(slotOpenFile()) );

   connect( listaScelta,
            SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this,
            SLOT(slotOpenFile()),
            Qt::DirectConnection);

   /*connect( buttons->button( QDialogButtonBox::Open ),
            SIGNAL(clicked()),
            this,
            SLOT(slotAddFile()) );*/

    setLayout( formGridLayout );
}

void BrowseWindow::addScelta(QString& nome)
{
//    this->comboScelta->addItem(nome);
    QIcon icona = QIcon::fromTheme("document-new", QIcon(srcPath + "/filenew.png"));
    QListWidgetItem *item = new QListWidgetItem(icona, nome, listaScelta);
    this->listaScelta->addItem(item);
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
    QString filename = listaScelta->currentItem()->text(); // comboScelta->currentText();
    QString docId = this->fileMap.key(filename);

    emit openFileSignal(filename, docId);
    close();
}

/**
 * @brief BrowseWindow::slotAddFile
 * invia massaggio di apertura del file selezionato
 */
void BrowseWindow::slotAddFile()
{
    /*
    QString filename = comboScelta->currentText();
    QString docId = this->fileMap.key(filename);

    emit addFileSignal(filename);
*/
}

