/* Classe per eseguire il log del programma.
 * Segue pattern Singleton
 */

#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <mutex>
#include <memory>

class Logger : public QObject
{
    Q_OBJECT
    Q_ENUMS(logType)

public:
    enum logType {Info, Error};  
    void setShowDateTime(bool value);
    static Logger &getLog();
    ~Logger();

private:
    QFile *file;
    bool m_showDate;
    std::mutex lock;
    explicit Logger(QString fileName);


    // metodi non definiti
    Logger(const Logger & hold);
    const Logger &operator=(const Logger & hold);

public slots:
    void write(const QString &value);
    void write(logType log, const QString &text);

};

#endif // LOGGER_H
