#ifndef DB_H
#define DB_H

#include <QObject>
#include <QtSql>

class db : public QObject
{
    Q_OBJECT
public:
    explicit db(QObject *parent = nullptr);
    bool conn();
    void query(QString querySrc);

private:
    QSqlDatabase myDb;

signals:

public slots:
};

#endif // DB_H
