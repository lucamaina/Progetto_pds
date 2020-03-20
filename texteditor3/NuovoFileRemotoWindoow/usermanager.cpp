#include "usermanager.h"

UserManager::UserManager(QString myself) : myself(myself)
{
    setUpGUI();
    setWindowTitle( tr("Nuovo File Remoto") );
}

void UserManager::setUpGUI()
{
    QVBoxLayout* layoutEsterno = new QVBoxLayout(this);
    QGridLayout* layoutAdd = new QGridLayout();

    // parte addUser
    this->lbltitoloAdd = new QLabel(this);
    lbltitoloAdd->setText("Aggiungi utenti alla condivisione:   (user1; user2; ...)");

    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);

    this->addUsers = new QLineEdit(this);
    this->btnAddUser = new QDialogButtonBox(this);
    btnAddUser->addButton(QDialogButtonBox::Apply)->setText(tr("Aggiungi"));

// parte remove user
    QGridLayout* layoutRem = new QGridLayout();
    this->lblTitoloRemove = new QLabel(this);
    lblTitoloRemove->setText("Rimuovi utenti dalla condivisione:");

    this->listaUser = new QListWidget(this);
    listaUser->setSortingEnabled(true);

    buttons = new QDialogButtonBox(this);
    buttons->addButton( QDialogButtonBox::Ok)->setText("Rimuovi");

    layoutAdd->addWidget(addUsers, 0, 0);
    layoutAdd->addWidget(btnAddUser, 0, 2);

    layoutRem->addWidget(listaUser, 0, 0);
    layoutRem->addWidget(buttons, 0, 2);

    layoutEsterno->addWidget(lbltitoloAdd);
    layoutEsterno->addLayout(layoutAdd);
    layoutEsterno->addWidget(line);
    layoutEsterno->addWidget(lblTitoloRemove);
    layoutEsterno->addLayout(layoutRem);

    this->setLayout(layoutEsterno);
    this->layout()->update();

    connect(this->listaUser, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(checkItem(QListWidgetItem *)));

    connect(this->btnAddUser->button( QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(addUser()));

    this->listItem = new QList<QListWidgetItem>;

}

/**
 * @brief UserManager::caricaUsers
 * @param lista
 * carica lista utenti condivisi quando riceve da server
 */
void UserManager::caricaUsers(QList<QString> & lista)
{
    qDebug() << "sono in UserManager::caricaUsers >> " << lista;
    for(QString s : lista){
        QListWidgetItem item(s, listaUser);
        if ( s == this->myself){
            listItem->append(item);
            this->listaUser->insertItem(0, &listItem->back());
        } else {
            item.setFlags(Qt::ItemFlag::ItemIsUserCheckable);
            item.setFlags(Qt::ItemFlag::ItemIsEnabled);
            item.setCheckState(Qt::CheckState::Unchecked);
            listItem->append(item);
            this->listaUser->addItem(&listItem->back());
        }

    }
    this->listaUser->sortItems(Qt::SortOrder::DescendingOrder);

}

/**
 * @brief UserManager::checkItem
 * @param item
 * modifica stato dell'item in checked / unchecked
 */
void UserManager::checkItem(QListWidgetItem *item)
{
    qDebug() << "sono in UserManager::checkItem";
    if (item->text() == this->myself)
        return;

    if ( item->checkState() == Qt::CheckState::Unchecked ) {
        item->setCheckState(Qt::CheckState::Checked);
    } else {
        item->setCheckState(Qt::CheckState::Unchecked);
    }
}

/**
 * @brief UserManager::addUser
 * aggiunge utenti inseriti
 */
void UserManager::addUser()
{
    qDebug() << "sono in UserManager::addUser";
    QString str = this->addUsers->text();
    str = str.trimmed();
    QStringList listaUser = str.split(',', QString::SkipEmptyParts);

    emit s_addUsers(listaUser);

    this->close();
}

