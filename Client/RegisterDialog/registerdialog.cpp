#include <RegisterDialog/registerdialog.h>

RegisterDialog::RegisterDialog(QWidget *parent) : QDialog(parent)
{
    setUpGUI();
    setWindowTitle( tr("User Registration") );
    setModal( true );
}

void RegisterDialog::setUpGUI(){
    // set up the layout
    QGridLayout* formGridLayout = new QGridLayout( this );

    // initialize the username combo box so that it is editable
    comboUsername = new QComboBox( this );
    comboUsername->setEditable( true );
    editPassword1 = new QLineEdit( this );
    editPassword1->setEchoMode( QLineEdit::Password );

    editPassword2 = new QLineEdit( this );
    editPassword2->setEchoMode( QLineEdit::Password );

    // initialize the labels
    labelUsername = new QLabel( this );
    labelPassword1 = new QLabel( this );
    labelPassword2 = new QLabel( this );
    labelUsername->setText( tr( "Username" ) );
    labelUsername->setBuddy( comboUsername );
    labelPassword1->setText( tr( "Password" ) );
    labelPassword1->setBuddy( editPassword1 );
    labelPassword2->setText( tr( "Repeat Password" ) );
    labelPassword2->setBuddy( editPassword2 );
    // initialize buttons
    buttons = new QDialogButtonBox( this );
    buttons->addButton( QDialogButtonBox::Ok );
    buttons->addButton( QDialogButtonBox::Cancel );
    buttons->button( QDialogButtonBox::Ok )->setText( tr("Register") );
    buttons->button( QDialogButtonBox::Cancel )->setText( tr("Abort") );

    // connects slots
    connect( buttons->button( QDialogButtonBox::Cancel ),
        SIGNAL (clicked()),
        this,
        SLOT (close()) );

    connect( buttons->button( QDialogButtonBox::Ok ),
        SIGNAL (clicked()),
        this,
        SLOT (slotAcceptRegistration()) );

    // place components into the dialog
    formGridLayout->addWidget( labelUsername, 0, 0 );
    formGridLayout->addWidget( comboUsername, 0, 1 );
    formGridLayout->addWidget( labelPassword1, 1, 0 );
    formGridLayout->addWidget( editPassword1, 1, 1 );
    formGridLayout->addWidget( labelPassword2, 2, 0 );
    formGridLayout->addWidget( editPassword2, 2, 1 );
    formGridLayout->addWidget( buttons, 3, 0, 1, 3 );

    setLayout( formGridLayout );
}

void RegisterDialog::setUsername(QString&& username){
    bool found = false;
    for( int i = 0; i < comboUsername->count() && ! found ; i++ ){
        if( comboUsername->itemText( i ) == username ){
            comboUsername->setCurrentIndex( i );
            found = true;
        }
    }

    if( ! found ){
        int index = comboUsername->count();
        qDebug() << "Select username " << index;
        comboUsername->addItem( username );
        comboUsername->setCurrentIndex( index );
    }

    // place the focus on the password field
    editPassword1->setFocus();
    editPassword2->setFocus();
}

void RegisterDialog::setPassword1(QString &password){
    editPassword1->setText( password );
}

void RegisterDialog::setPassword2(QString &password){
    editPassword2->setText( password );
}

void RegisterDialog::slotAcceptRegistration(){
    QString username = comboUsername->currentText();
    QString password1 = editPassword1->text();
    QString password2 = editPassword2->text();

    if(password1!=password2){
        QMessageBox q;
        q.critical( nullptr, "Registration Error", "Please, be sure to type the same Password in both boxes!");
        q.setFixedSize(500,200);
        return;
    }

    if(password1.size()==0 || password2.size()==0 || username.size()==0){
        QMessageBox q;
        q.critical( nullptr, "Registration Error", "Please, be sure to fill every box!");
        q.setFixedSize(500,200);
        return;
    }

    emit acceptRegistration( username,      // current username
                            password1 );    // current password
    // close this dialog
    close();
}

void RegisterDialog::setUsernamesList(const QStringList &usernames){
    comboUsername->addItems( usernames );
}
