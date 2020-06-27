#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QComboBox>
#include <QGridLayout>
#include <QStringList>
#include <QDebug>
#include <QMessageBox>

class RegisterDialog : public QDialog
{

    Q_OBJECT

private:
    QLabel* labelUsername;
    QLabel* labelPassword1;
    QLabel* labelPassword2;
    QComboBox* comboUsername;
    QLineEdit* editPassword1;
    QLineEdit* editPassword2;
    QDialogButtonBox* buttons;

    void setUpGUI();

public:
    explicit RegisterDialog(QWidget *parent = nullptr);
    void setUsername( QString&& username );
    void setPassword1(QString &password);
    void setPassword2(QString &password);
    void setUsernamesList( const QStringList &usernames );

signals:
    void acceptRegistration( QString& username, QString& password);

public slots:
    void slotAcceptRegistration();
};
