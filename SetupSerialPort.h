#ifndef _SETUPSERIALPORT_H_
#define _SETUPSERIALPORT_H_

#ifdef __cplusplus
extern "C" {
#endif

void SetBaudrate (int);
int  GetBaudrate ();
void SetDataBit (int databit);
int  BAUDRATE (int baudrate);
int _BAUDRATE (int baudrate);
int  SetPortAttr (int fd, int baudrate, int databit, const char *stopbit, char parity);
void SetStopBit (const char *stopbit);
void SetParityCheck (char parity);

//OpenComPort返回值是打开的串口文件描述符fd
int  OpenComPort (int ComPort, int baudrate, int databit, const char *stopbit, char parity);
void CloseComPort (int fd);
int ReadComPort (int fd, void *data, int datalength,int overTime);
int  WriteComPort (int fd, unsigned char *data, int datalength);

//extern int fd;

#ifdef __cplusplus
}
#endif


#endif /* _SETUPSERIALPORT_H_ */
