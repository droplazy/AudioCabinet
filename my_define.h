#ifndef MY_DEFINE_H
#define MY_DEFINE_H
#include <QString>
#include <QDebug>



struct S_Aduio_Meta {
    QString Title;
    QString Album;
    QString Artist;
    QString Duration;
    QString Position;
    QString Status;

    void print() const {
        qDebug() << "Title:" << Title;
        qDebug() << "Album:" << Album;
        qDebug() << "Artist:" << Artist;
        qDebug() << "Duration:" << Duration;
        qDebug() << "Position:" << Position;
        qDebug() << "Status:" << Status;

    }
};
#endif // MY_DEFINE_H
