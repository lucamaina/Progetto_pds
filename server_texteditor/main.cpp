#include <QCoreApplication>
#include <server_main.h>
#include <QDebug>

bool asdscriviXML(void);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qDebug() << "Server texteditor distribuito";

    server_main* server = new server_main();

    asdscriviXML();
    return a.exec();
}

bool asdscriviXML(void)  // @TODO scrivere xml
{
    QFile *f = new QFile("cmd.txt");
    f->open(QIODevice::ReadWrite | QIODevice::Text);
    QByteArray xml;
    QXmlStreamWriter stream(&xml);
    stream.setAutoFormatting(false);
    stream.setCodec("UTF-8");
    stream.writeStartDocument();
    stream.writeStartElement("LOG-IN");

    stream.writeTextElement("username", "val1");
    stream.writeTextElement("password", "val2");


    stream.writeEndElement();
    f->close();
    QByteArray str = "00000000";
    QByteArray dim = QByteArray::number(xml.size(), 16);
    str.replace(8-dim.size(), dim.size(), dim);

    xml.prepend(str);
    qDebug() << xml;
    return false;
}
