#ifndef AUDIO_INFO_H
#define AUDIO_INFO_H
#include <QObject>
#include <QThread>
#include <QFile>
#include "my_define.h"

#define TITLE_PATH "/rp_test/title_runtime"
#define POSITION_PATH "/rp_test/position_runtime"

class audio_info: public QThread
{
    Q_OBJECT
public:
    audio_info(QString Hwid);
    virtual void run();
    void GetAudioInfo();
    int QprocessCommand(QString program, QString &output, QStringList arguments);
    QString devid ;
    QString readFileContent(const QString &filePath);
    void DisposeTitleFile();
    void extractFields(const QString &input);

    S_Aduio_Meta meta_data;
signals:
    void updateMeta(S_Aduio_Meta *);
};

#endif // AUDIO_INFO_H
