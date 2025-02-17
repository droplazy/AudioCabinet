#ifndef FINGERTHREAD_H
#define FINGERTHREAD_H

#include <QObject>
#include <QThread>
#include <QByteArray>
#include <QDebug>
#include <QFile>
#include <QTextStream>

#define  INPUT_FILE "/proc/rp_gpio/input_finger"

typedef enum{
    FO_NOP = 0,
    FO_MATCH =1,
    FO_ENROLL =2
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
    int GetFingerInputFile();
private:
    int ttyFD;
    QFile Input_file();
    FINGERT_OPTION Fig_Opt =FO_NOP;
};

#endif // FINGERTHREAD_H
