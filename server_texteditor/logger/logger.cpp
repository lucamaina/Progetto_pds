#include "logger.h"
#include <QtDebug>

Logger::Logger(QObject *parent, QString fileName) : QObject(parent) {
     m_showDate = true;
     if (!fileName.isEmpty()) {
          file = new QFile;
          file->setFileName(fileName);
          file->open(QIODevice::Append | QIODevice::Text);
     }
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
