#include "logger.h"
#include <QtDebug>

Logger::Logger(QObject *parent, QString fileName) : QObject(parent) {
 m_showDate = true;
 if (!fileName.isEmpty()) {
  file = new QFile;
  file->setFileName(fileName);
  file->open(QIODevice::Append | QIODevice::Text);
  qDebug() << file->exists("");
 }
}

void Logger::write(const QString &value) {
 QString text = value;// + "";
 if (m_showDate)
  text = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss ") + text;
 QTextStream out(file);
 out.setCodec("UTF-8");
 if (file != nullptr) {
  out << text;
 }
}

void Logger::setShowDateTime(bool value) {
 m_showDate = value;
}

Logger::~Logger() {
 if (file != nullptr)
 file->close();
}
