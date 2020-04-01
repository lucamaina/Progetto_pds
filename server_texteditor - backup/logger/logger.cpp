/*
 * Classe che implementa file di log del server
 * Usa pattern Singleton
 *
 */

#include "logger.h"
#include <QtDebug>

/**
 * @brief Logger::Logger
 * @param parent
 * @param fileName  nome del file di log
 * se la creazione del file non va a buon fine non posso continuare esecuzione del programma
 */
Logger::Logger(QString fileName){
     m_showDate = true;
     if (fileName.isEmpty()) {
         fileName = "log_default.log";
     }
     file = new QFile;
     file->setFileName(fileName);
     bool open = file->open(QIODevice::Append | QIODevice::Text);
     if (open == false){
         qDebug() << "Errore apertura file di log";
     }
}

Logger & Logger::getLog(){
    static std::unique_ptr<Logger> p (new Logger("logger.log"));
    return *p;
}

void Logger::write(const QString &value) {
    std::lock_guard<std::mutex> l(this->lock);
    QString text = value;// + "";
    if (m_showDate)
       text = QDateTime::currentDateTime().toString("hh:mm:ss dd.MM.yyyy ") + text;
    QTextStream out(file);
    out.setCodec("UTF-8");
    if (file != nullptr) {
       out << text << endl;
    }
}

void Logger::write(logType tipo, const QString &value) {
    std::lock_guard<std::mutex> l(this->lock);
    QString text = value;// + "";
    if (tipo == logType::Info)
        text = "[Info] " + text;
    else if (tipo == logType::Error) {
        text = "[Err ] " + text;
    }
    if (m_showDate)
       text = QDateTime::currentDateTime().toString("hh:mm:ss dd.MM.yyyy ") + text;
    QTextStream out(file);
    out.setCodec("UTF-8");
    if (file != nullptr) {
       out <<  text << endl;
    }
}

void Logger::setShowDateTime(bool value) {
    std::lock_guard<std::mutex> l(this->lock);
    m_showDate = value;
}

Logger::~Logger() {
    if (file != nullptr)
        file->close();
}
