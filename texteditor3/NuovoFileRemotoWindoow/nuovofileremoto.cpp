#include "nuovofileremoto.h"

nuovoFileRemoto::nuovoFileRemoto()
{
    setUpGUI();
    setWindowTitle( tr("Nuovo File Remoto") );
}

void nuovoFileRemoto::setUpGUI()
{
    formGridLayout = new QGridLayout( this );

    insertName = new QLineEdit(this);
    insertName->setText("");


    labelScelta = new QLabel(this);
    labelScelta->setText( tr( "Nome file:" ) );
    labelScelta->setBuddy( insertName );

    buttons = new QDialogButtonBox(this);
    buttons->addButton( QDialogButtonBox::Open);
    buttons->addButton( QDialogButtonBox::Close);

    buttons->button( QDialogButtonBox::Open )->setText( tr("Crea File") );


    formGridLayout->addWidget(labelScelta, 0, 0);
    formGridLayout->addWidget(insertName, 0, 1);
    formGridLayout->addWidget(buttons, 1, 1);

    connect( buttons->button( QDialogButtonBox::Close ),
        SIGNAL (clicked()), this, SLOT (close()) );

    connect( buttons->button( QDialogButtonBox::Open ),
        SIGNAL (clicked()), this, SLOT (addNewFile()) );

}

void nuovoFileRemoto::addNewFile()
{
    QString str = this->insertName->displayText();
    qDebug() << "sono in nuovoFileRemoto::addNewFile + " << str;
    emit s_addNewFile(str);
    this->close();
}






