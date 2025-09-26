#include "gattthread.h"
#include "btmanager/bt_test.h"
#include <QDateTime>
#include <QProcess>

#include <QJsonObject>


QJsonObject gattthread::generateJson(int resultId, const QString &operationType)
{
    QJsonObject jsonObj;

    // 获取当前时间戳（以秒为单位）
    qint64 timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch() / 1000;  // 转换为秒

    // 填充 JSON 对象
    jsonObj["Timestamp"] = QString::number(timestamp);  // 设置时间戳
    jsonObj["messageId"] = messageId;                    // 使用全局 messageId
    jsonObj["operationType"] = operationType;            // 设置操作类型
    jsonObj["resultId"] = resultId;                      // 设置结果 ID

    return jsonObj;
}

void gattthread::ResponeGatt(char * data )
{
    QDateTime currentTime = QDateTime::currentDateTime();

    // 获取当前时间的 Unix 时间戳（毫秒）
    qint64 timestampMS = currentTime.toMSecsSinceEpoch();

    // 转换为秒级时间戳
    qint64 timestamp = timestampMS / 1000;

    // 将时间戳转换为字符串
    QString timestampStr = QString::number(timestamp);

    bt_test_send_notify(global_gattMsg_recive.attr_handle,data,strlen(data));
}

void gattthread::ResponeGattWIFICFG(bool res)
{
    int resultId = 0;

    if(res)
        resultId = 0;
    else
        resultId = -1;

    QJsonObject obj = generateJson(resultId, "WiFiConfiguration");

    // 将 QJsonObject 转换为 QString
    QJsonDocument doc(obj);
    QString jsonString = doc.toJson(QJsonDocument::Compact);  // 转换为紧凑格式的 JSON 字符串

    // 将 QString 转换为 char*
    QByteArray byteArray = jsonString.toUtf8();
    char* jsonChar = byteArray.data();

    // 调用 ResponeGatt 函数，传递转换后的 char* 数据
    ResponeGatt(jsonChar);
}


void gattthread::run()
{
    //timer_wificfg.start();       // 启动计时器
    //qint64 elapsed = timer_wificfg.elapsed(); // 获取从启动到现在的时间（毫秒）
    while (1)
    {
        // qDebug() << "GATT MESSAGE THREAD LAUCH !" <<  GetMsgFlag;

        if(timer_wificfg.elapsed() > 1000*10 && GATT_STATE == GATT_MASSAGE::GATT_NETWORKCONFIGURE_RES)
        {
            GATT_STATE = GATT_MASSAGE::GATT_IDLE;
            ResponeGattWIFICFG(false);
        }


        if (GetMsgFlag)
        {
#if 1
            qDebug("attr_handle: 0x%04x, tran_id: %d, len: %d", global_gattMsg_recive.attr_handle, global_gattMsg_recive.trans_id, global_gattMsg_recive.value_len);
            // 声明一个缓冲区来保存接收到的字符串，最大256字节
            char value_str[256];

            // 如果接收到的数据长度大于或等于256字节，直接打印错误信息
            if (global_gattMsg_recive.value_len >= 256)
            {
                printf("收到的信息错吴\n");
            }
            else
            {
                // 尝试将接收到的值复制到 value_str 中，确保字符串是以 '\0' 结尾的
                strncpy(value_str, (char *)global_gattMsg_recive.value, global_gattMsg_recive.value_len);

                // 手动在字符串末尾添加 '\0'，以防止溢出
                value_str[global_gattMsg_recive.value_len] = '\0';
            }
#endif
            Respone_handle =global_gattMsg_recive.attr_handle;

            parseJson(value_str);

//            ResponeGatt();
            GetMsgFlag = 0;
        }
        usleep(100 * 1000);
    }
}

void gattthread::handleWiFiConfiguration(const QJsonObject &dataObj)
{
    QString ssid = dataObj["ssid"].toString();
    QString password = dataObj["password"].toString();

    qDebug() << "WiFi Configuration:";
    qDebug() << "SSID:" << ssid;
    qDebug() << "Password:" << password;

    // 构建命令
    QString command = QString("wifi -c %1 %2").arg(ssid).arg(password);

    // 尝试最多三次执行命令
    int attempt = 0;
    bool success = false;

    while (attempt < 3)
    {
        // 使用 QProcess 执行命令
        QProcess process;
        process.start(command);

        // 等待命令执行完成
        if (process.waitForFinished())
        {
            // 判断命令的执行结果
            int exitCode = process.exitCode();
            if (exitCode == 0)
            {
                success = true;
                qDebug() << "Command executed successfully!";
                emit wificonfigureupdate();
                break; // 执行成功，跳出循环
            }
            else
            {
                qDebug() << "Command failed, attempt" << (attempt + 1) << "of 3";
            }
        }
        else
        {
            qDebug() << "Failed to start the process.";
        }

        // 增加尝试次数
        attempt++;
    }

    // 如果三次尝试都失败，可以处理失败逻辑
    if (!success)
    {
        qDebug() << "WiFi configuration failed after 3 attempts.";
    }
}

void gattthread::handleFormat(const QString &data)
{
    qDebug() << "Format Operation:";
    qDebug() << "Data:" << data;
    emit deviceFormat();
}

void gattthread::handleFingerprintEnrollment(const QString &data)
{
    qDebug() << "Fingerprint Enrollment Operation:";
    qDebug() << "Data:" << data;
    emit enrollFinger();
}




void gattthread::parseJson(const QString &jsonString)
{
    // 解析 JSON 字符串为 QJsonDocument
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());

    // 检查 JSON 是否有效
    if (doc.isNull())
    {
        qWarning() << "Invalid JSON data.";
        return;
    }

    // 获取 JSON 对象
    QJsonObject jsonObj = doc.object();

    // 解析 operationType
    QString operationType = jsonObj["operationType"].toString();
    QJsonValue dataValue = jsonObj["data"];

    // 根据 operationType 进入不同的操作
    if (operationType == "WiFiConfiguration")
    {
        if (dataValue.isObject())
        {
            QJsonObject dataObj = dataValue.toObject();
            handleWiFiConfiguration(dataObj);
           GATT_STATE = GATT_MASSAGE::GATT_NETWORKCONFIGURE_RES;
           timer_wificfg.restart();
        }
    }
    else if (operationType == "Format")
    {
        if (dataValue.isString())
        {
            QString data = dataValue.toString();
            handleFormat(data);
        }
    }
    else if (operationType == "FingerprintEnroll")
    {
        // if (dataValue.isString()) {
        //    QString data = dataValue.toString();
        handleFingerprintEnrollment("3");
        //}
    }
    else
    {
        qWarning() << "Unknown operation type:" << operationType;
    }
}

gattthread::gattthread()
{
    qDebug() << "GATT MESSAGE THREAD LAUCH !";
}
