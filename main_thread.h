#ifndef MAIN_THREAD_H
#define MAIN_THREAD_H

#include <QObject>
#include <QThread>
#include <QDebug>
class main_thread :public QThread
{
    Q_OBJECT
public:
    main_thread();
    virtual void run();
};

#endif // MAIN_THREAD_H
