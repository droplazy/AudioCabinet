#include "fingerthread.h"
#include "fingerpackage.h"
#include "SetupSerialPort.h"


FingerThread::FingerThread()
{
    ttyFD= OpenComPort(0, 57600, 8, "1", 'N');
    if(ttyFD == -1)
    {
        qDebug() << "Error , Open comport faild !";
    }
    else
    {
        qDebug(" Open comport successly [%d] !",ttyFD);
    }


}

void FingerThread::run()
{

    int ret=0 ;
    unsigned char message[128]={0};
    HandShakeCheck();

    sleep(1);
    AutoEnroll();



    while(1)
    {
       // if()

        ret = GetFingerInputFile();
       // qDebug() << "FINGER INPUT : " <<ret <<"FINGER TYPe : " << Fig_Opt;
        if(ret == 1 && Fig_Opt!= FO_ENROLL)
        {
            AutoIdentify();
        }
     //   usleep(100*1000);
#if 1
        int ret = -1;

        ret =  ReadComPort (ttyFD, (void *)message, 128,100*1000);

        if(ret >0)
        {
            printHex((uint8_t* )message,(size_t)ret);
            uint8_t returnParams[256];  // 用于存储返回的参数
            size_t returnParamsLength = 0;
           uint8_t result =  parseResponsePackage(message, (size_t) ret, returnParams, &returnParamsLength);
            if (result != 0xff) {
                printf("解析成功！len = %d confirmcode %02x 返回参数：",returnParamsLength,result);
                for (size_t i = 0; i < returnParamsLength; i++) {
                    printf("0x%02x ", returnParams[i]);
                }
                printf("\n");
                if(Fig_Opt== FO_ENROLL  )
                {
                    if(returnParams[0] == 0x06 && returnParams[1] == 0xf2 && result == 0x00)
                    {
                    qDebug() <<"指纹录入成功";
                    Fig_Opt =FO_NOP;
                    }
                    else if(result == 0x0b || result == 0x25 ||result == 0x1f ||result == 0x22)
                    {
                        qDebug() <<"指纹录入失败";
                        Fig_Opt =FO_NOP;
                    }
                }
                if(Fig_Opt== FO_MATCH  )
                {
                    uint16_t score = (returnParams[3] <<8) |returnParams[4];

                    if(result == 0x00)
                    {
                    qDebug() <<"指纹匹配成功 score = "  <<(int)score;
                    Fig_Opt =FO_NOP;
                    emit upanddownlock();
                    usleep(1500*1000);//TODO
                    }
                    else
                    {
                        qDebug() <<"指纹匹配失败 score = "<<(int)score;
                        Fig_Opt =FO_NOP;
                    }
                }


            } else {
                printf("解析失败，错误码：%02x\n", result);
            }
        }
#endif
    }
}

bool FingerThread::HandShakeCheck()
{
    unsigned char message[128]={0};
    int lenth = 0;

    lenth = HandShakeGeneralPackage(message);
    printHex((uint8_t* )message,(size_t)lenth);
//    QByteArray byteArray((char *)message);  // 将 char* 转换为 QByteArray
//    qDebug() << "Hexadecimal representation:" << byteArray.toHex();  // 打印十六进制
//    qDebug() << "lenth :" << lenth;  // 打印十六进制

    WriteComPort(ttyFD,message,lenth);




    int ret = -1;

    ret =  ReadComPort (ttyFD, (void *)message, 128,500000);

    if(ret >0)
    {
        printHex((uint8_t* )message,(size_t)ret);
        uint8_t returnParams[256];  // 用于存储返回的参数
        size_t returnParamsLength = 0;
       int result =  parseResponsePackage(message, (size_t) ret, returnParams, &returnParamsLength);
        if (result >= 0) {
            printf("解析成功！len = %d 返回参数：",returnParamsLength);
            for (size_t i = 0; i < returnParamsLength; i++) {
                printf("0x%02X ", returnParams[i]);
            }
            printf("\n");
        } else {
            printf("解析失败，错误码：%d\n", result);
        }
    }
    else
    {
         qDebug() << "Error ,Read comport faild !\n";
    }

}
/*
 *     1) bit0：采图背光灯控制位，0-LED 长亮，1-LED 获取图像成
    功后灭；
    2) bit1：保留
    3) bit2：注册过程中，要求模组返回关键步骤，0-要求返回，1-不要求返回
    4) bit3：是否允许覆盖ID 号，0-不允许，1-允许；
    5) bit4：允许指纹重复注册控制位，0-允许，1-不允许；
    6) bit5：注册时，多次指纹采集过程中，是否要求手指离开才能进入下一
    次指纹图像采集， 0-要求离开；1-不要求离开；
 * */
void FingerThread::AutoEnroll()
{
    unsigned char message[128]={0};
    int lenth = 0;
    uint16_t param = 0x0B;//       00 1011


    lenth = AutoEnrollGeneralPackage(message,0x0001,0x0003,param);

    printHex((uint8_t* )message,(size_t)lenth);

    WriteComPort(ttyFD,message,lenth);
    Fig_Opt =FO_ENROLL;
}
/*
ID 号：2byte，大端模式。比如录入1 号指纹，则是0001H。ID 号为0xFFFF，则进行
1：N 搜索；否进行1:1 匹配参数：最低位为bit0。
1) bit0：采图背光灯控制位，0-LED 长亮，1-LED 获取图像成功后灭；
2) bit1：保留
3) bit2：验证过程中，是否要求模组返回关键步骤，0-要求返回，1-不要求返回
2) bit3~bit15：预留。
*/
void FingerThread::AutoIdentify()
{
    unsigned char message[128]={0};
    int lenth = 0;
    uint16_t param = 0x05;//       0000   0 1 0 1


    lenth = AutoIdentifyuGeneralPackage(message,0x02,0xffff,param);

    printHex((uint8_t* )message,(size_t)lenth);

    WriteComPort(ttyFD,message,lenth);
    Fig_Opt =FO_MATCH;

}

int FingerThread::GetFingerInputFile()
{
        QString filePath =INPUT_FILE;

        // 创建 QFile 对象并传入路径
        QFile file(filePath);

        // 尝试打开文件
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug() << "无法打开文件:" << file.errorString();
            return -1;
        }

        QString get_data = file.readAll();
        file.close();

        return get_data.toInt(NULL,10);
}


