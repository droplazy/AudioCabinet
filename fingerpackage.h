#ifndef FINGERPACKAGE_H
#define FINGERPACKAGE_H

#endif // FINGERPACKAGE_H

#include "unistd.h"
#include "string.h"
#include "stdlib.h"
#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


int  HandShakeGeneralPackage(unsigned char *package);
void printHex(uint8_t* data, size_t length);
uint8_t  parseResponsePackage( uint8_t* package, size_t packageLength, uint8_t* returnParams, size_t* returnParamsLength);
int  CancelGeneralPackage(unsigned char* package);
int  AutoEnrollGeneralPackage(unsigned char* package, uint16_t id_count, uint8_t roll_cnt, uint16_t param);
int  AutoIdentifyuGeneralPackage(unsigned char* package,uint8_t level,uint16_t id_type,uint16_t param);


#ifdef __cplusplus
}
#endif
