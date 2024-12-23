#include "mainwindow.h"
#include <QApplication>
#include <QtDBus/QDBusMetaType>
#include <QtDBus/QtDBus>
typedef QMap<QString, QMap<QString, QVariant> > ConnectionDetails;
Q_DECLARE_METATYPE(ConnectionDetails)

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // qDBusRegisterMetaType<ConnectionDetails>();
    qDBusRegisterMetaType<ConnectionDetails>();
        QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    MainWindow w;
    w.show();

    return a.exec();
}
