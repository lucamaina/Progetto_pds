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

    labelScelta= new QLabel(this);
    labelScelta->setText( tr( "Choose a file:" ) );
    labelScelta->setBuddy( listaScelta );

    buttons = new QDialogButtonBox( this );
    buttons->addButton( QDialogButtonBox::Ok );
    buttons->addButton( QDialogButtonBox::Cancel );

    buttons->button( QDialogButtonBox::Ok )->setText( tr("Apri") );
    buttons->button( QDialogButtonBox::Cancel )->setText( tr("Cancel") );

    formGridLayout->addWidget( labelScelta, 0, 0 );
    formGridLayout->addWidget( listaScelta, 1, 0 ); //comboScelta
    formGridLayout->addWidget( buttons->button(QDialogButtonBox::Ok), 2, 0);
    formGridLayout->addWidget( buttons->button(QDialogButtonBox::Cancel), 2, 1);

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

    setLayout( formGridLayout );
}

void BrowseWindow::addScelta(QString& nome)
{
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
