#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <mutex>

class Logger : public QObject
{
    Q_OBJECT
    Q_ENUMS(logType)

public:
    enum logType {Info, Error};
    explicit Logger(QObject *parent, QString fileName);
    ~Logger();
    void setShowDateTime(bool value);

private:
    QFile *file;
    bool m_showDate;
    std::mutex lock;


signals:

public slots:
    void write(const QString &value);
    void write(logType log, const QString &text);

};

#endif // LOGGER_H
