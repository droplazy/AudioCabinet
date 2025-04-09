
#include "fingerpackage.h"

#define HEADER 0xEF01
#define DEFAULT_DEVICE_ADDRESS 0xFFFFFFFF

// 校验和计算函数
uint16_t calculateChecksum(uint8_t* data, size_t length) {
    uint16_t sum = 0;
   // printf("\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX %d \n",length);

    for (size_t i = 6; i < (length); i++) {
        sum += data[i];
      //  printf("%02X ", data[i]);

    }
   // printf("\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");

    return sum & 0xFFFF;  // 保留低16位
}

// 打印十六进制数据
void printHex(uint8_t* data, size_t length) {
#ifdef PRINT_HEX
    for (size_t i = 0; i < length; i++) {
        printf("%02X ", data[i]);
    }
    printf("\n");
#endif
}

// 生成指令包，返回包的整体长度
size_t generateCommandPackage(uint32_t deviceAddress, uint8_t command, uint8_t* params, size_t paramLength, uint8_t* package) {
    size_t offset = 0;

    // 包头 (2 bytes)
    package[offset++] = 0xEF;
    package[offset++] = 0x01;

    // 设备地址 (4 bytes)
    package[offset++] = (deviceAddress >> 24) & 0xFF;
    package[offset++] = (deviceAddress >> 16) & 0xFF;
    package[offset++] = (deviceAddress >> 8) & 0xFF;
    package[offset++] = deviceAddress & 0xFF;

    // 包标识 (1 byte)
    package[offset++] = 0x01;

    // 包长度 (2 bytes)
    uint16_t packetLength = 1  + 2 + paramLength;  // 包标识 + 指令/ 参数 + + 校验和 (2 bytes)
    package[offset++] = (packetLength >> 8) & 0xFF;
    package[offset++] = packetLength & 0xFF;

    // 指令 (1 byte)
    package[offset++] = command;

    // 参数 (如果存在的话)
    if (params != NULL && paramLength > 0) {
        memcpy(&package[offset], params, paramLength);
        offset += paramLength;
    }

    // 校验和 (2 bytes)
    uint16_t checksum = calculateChecksum(package, offset);
    package[offset++] = (checksum >> 8) & 0xFF;
    package[offset++] = checksum & 0xFF;

    return offset; // 返回包整体长度
}

// 生成数据包，返回包的整体长度
size_t generateDataPackage(uint32_t deviceAddress, uint8_t* data, size_t dataLength, uint8_t* package) {
    size_t offset = 0;

    // 包头 (2 bytes)
    package[offset++] = 0xEF;
    package[offset++] = 0x01;

    // 设备地址 (4 bytes)
    package[offset++] = (deviceAddress >> 24) & 0xFF;
    package[offset++] = (deviceAddress >> 16) & 0xFF;
    package[offset++] = (deviceAddress >> 8) & 0xFF;
    package[offset++] = deviceAddress & 0xFF;

    // 包标识 (1 byte)
    package[offset++] = 0x02;

    // 包长度 (2 bytes)
    uint16_t packetLength = 2 + dataLength;  // 包标识 + 数据 + 校验和 (2 bytes)
    package[offset++] = (packetLength >> 8) & 0xFF;
    package[offset++] = packetLength & 0xFF;

    // 数据 (N bytes)
    memcpy(&package[offset], data, dataLength);
    offset += dataLength;

    // 校验和 (2 bytes)
    uint16_t checksum = calculateChecksum(package, offset);
    package[offset++] = (checksum >> 8) & 0xFF;
    package[offset++] = checksum & 0xFF;

    return offset; // 返回包整体长度
}

// 生成结束包，返回包的整体长度
size_t generateEndPackage(uint32_t deviceAddress, uint8_t* data, size_t dataLength, uint8_t* package) {
    size_t offset = 0;

    // 包头 (2 bytes)
    package[offset++] = 0xEF;
    package[offset++] = 0x01;

    // 设备地址 (4 bytes)
    package[offset++] = (deviceAddress >> 24) & 0xFF;
    package[offset++] = (deviceAddress >> 16) & 0xFF;
    package[offset++] = (deviceAddress >> 8) & 0xFF;
    package[offset++] = deviceAddress & 0xFF;

    // 包标识 (1 byte)
    package[offset++] = 0x08;

    // 包长度 (2 bytes)
    uint16_t packetLength = 2 + dataLength;  // 包标识 + 数据 + 校验和 (2 bytes)
    package[offset++] = (packetLength >> 8) & 0xFF;
    package[offset++] = packetLength & 0xFF;

    // 数据 (N bytes)
    memcpy(&package[offset], data, dataLength);
    offset += dataLength;

    // 校验和 (2 bytes)
    uint16_t checksum = calculateChecksum(package, offset);
    package[offset++] = (checksum >> 8) & 0xFF;
    package[offset++] = checksum & 0xFF;

    return offset; // 返回包整体长度
}
int   HandShakeGeneralPackage(unsigned char* package)
{
    uint32_t deviceAddress = DEFAULT_DEVICE_ADDRESS;  // 示例设备地址
    uint8_t command = 0x35;               // 示例指令
    uint8_t params[1] = {0};      // 示例参数

   uint8_t commandPackage[256];

                size_t commandLength = generateCommandPackage(deviceAddress, command, params, 0, commandPackage);
                //printf("Command Package: ");
                //printHex(commandPackage, commandLength);
                //printf("Command Package Length: %zu\n", commandLength);
    memcpy(package,commandPackage,commandLength);
                return commandLength;
}

// 解析应答包的函数
uint8_t parseResponsePackage( uint8_t* package, size_t packageLength, uint8_t* returnParams, size_t* returnParamsLength) {
    // 校验包长度是否足够
    if (packageLength < 9) {
        return 0xff;  // 包太短，无法解析
    }

    // 校验包头是否正确
    if ((package[0] != 0xEF) || (package[1] != 0x01)) {
        return 0xff;  // 包头错误
    }

    // 提取设备地址（4字节）
    uint32_t deviceAddress = 0;
    for (int i = 0; i < 4; i++) {
        deviceAddress = (deviceAddress << 8) | package[2 + i];
    }

    // 提取包标识（1字节）
    uint8_t packageFlag = package[6];

    // 提取包长度（2字节）
    uint16_t packetLength = (package[7] << 8) | package[8];

    // 校验包长度是否匹配
    if (packageLength != packetLength + 9) {
        return 0xff;  // 包长度不匹配
    }
   // printf("packetLength = %d  packageLength =%d \n",packetLength,packageLength);
    // 提取确认码（1字节）
    uint8_t confirmCode = package[9];

    // 校验和校验
    uint16_t checksumReceived = (package[packageLength - 2] << 8) | package[packageLength - 1];
    uint16_t checksumCalculated = calculateChecksum(package, packageLength - 2);

    // 如果校验和错误，返回 -1
    if (checksumReceived != checksumCalculated) {
        return 0xff;  // 校验和错误
    }

    // 返回参数（N字节，跟随在确认码后面）
    *returnParamsLength = packetLength - 3;  // 返回参数的长度
    if(*returnParamsLength>0)
    memcpy(returnParams, &package[10], *returnParamsLength);

    // 如果确认码是 00H，表示指令执行完毕或 OK
   // if (confirmCode == 0x00) {
        return confirmCode;  // 成功
//    } else {
//        return -4;  // 其他错误代码
//    }
}

int  CancelGeneralPackage(unsigned char* package)
{
    uint32_t deviceAddress = DEFAULT_DEVICE_ADDRESS;  // 示例设备地址
    uint8_t command = 0x30;               // 示例指令
    uint8_t params[1] = {0};      // 示例参数

   uint8_t commandPackage[256];

                size_t commandLength = generateCommandPackage(deviceAddress, command, params, 0, commandPackage);
                //printf("Command Package: ");
                //printHex(commandPackage, commandLength);
                //printf("Command Package Length: %zu\n", commandLength);
    memcpy(package,commandPackage,commandLength);
                return commandLength;
}
int  AutoEnrollGeneralPackage(unsigned char* package,uint16_t id_count,uint8_t roll_cnt,uint16_t param)
{
    uint32_t deviceAddress = DEFAULT_DEVICE_ADDRESS;  // 示例设备地址
    uint8_t command = 0x31;               // 示例指令
    uint8_t params[5] = {0};      // 示例参数

    params[0] = (id_count >> 8 )&0xff;
    params[1] = id_count&0xff;
    params[2] = roll_cnt;
    params[3] = (param >> 8 )&0xff;
    params[4] = param&0xff;

   // printHex(params,5);

   uint8_t commandPackage[256];

   size_t commandLength = generateCommandPackage(deviceAddress, command, params, 5, commandPackage);

    memcpy(package,commandPackage,commandLength);
                return commandLength;
}
int  AutoIdentifyuGeneralPackage(unsigned char* package,uint8_t level,uint16_t id_type,uint16_t param)
{
    uint32_t deviceAddress = DEFAULT_DEVICE_ADDRESS;  // 示例设备地址
    uint8_t command = 0x32;               // 示例指令
    uint8_t params[5] = {0};      // 示例参数

    params[0] = level&0xff;
    params[1] = (id_type>>8)&0xff;
    params[2] = id_type&0xff;
    params[3] = (param >> 8 )&0xff;
    params[4] = param&0xff;

   // printHex(params,5);

   uint8_t commandPackage[256];

   size_t commandLength = generateCommandPackage(deviceAddress, command, params, 5, commandPackage);

    memcpy(package,commandPackage,commandLength);
                return commandLength;
}
