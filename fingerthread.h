#ifndef FINGERTHREAD_H
#define FINGERTHREAD_H

#include <QObject>
#include <QThread>
#include <QByteArray>
#include <QDebug>
#include <QTime>
#include <QTimer>
#include <QFile>
#include <QTextStream>

#define  INPUT_FILE "/proc/rp_gpio/input_finger"


typedef enum{
    FO_NOP = 0,
    FO_MATCH =1,
    FO_ENROLL =2,
    FO_CLEAR  =3,
    FO_MATCH_WAITING =4,
    FO_CANCEL =5
}FINGERT_OPTION;


class FingerThread:public QThread
{
    Q_OBJECT
public:
    FingerThread();
    virtual void run();

    bool HandShakeCheck();
    void AutoEnroll();
    void AutoIdentify();
    void PS_Cancel();
    int GetFingerInputFile();
    void clearfinger();
private:
    int ttyFD;
    QFile Input_file();
    FINGERT_OPTION Fig_Opt =FO_NOP;
    QDateTime locktime;
signals:
    void upanddownlock();
};

#endif // FINGERTHREAD_H
