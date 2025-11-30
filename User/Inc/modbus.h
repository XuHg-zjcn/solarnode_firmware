#ifndef MODBUS_H
#define MODBUS_H

#include <stdint.h>

#define MB_READ_COILS         0x01
#define MB_READ_DISC_IN       0x02
#define MB_READ_RHOLD         0x03
#define MB_READ_RIN           0x04
#define MB_WRITE_COIL_SINGL   0x05
#define MB_WRITE_RHOLD_SINGL  0x06
#define MB_WRITE_COIL_MULTI   0x0F
#define MB_WRITE_RHOLD_MULTI  0x10

#define MB_ERR_ILL_FUNC       -1
#define MB_ERR_ILL_ADDR       -2
#define MB_ERR_ILL_VALUE      -3
#define MB_ERR_DEV_FAIL       -4
#define MB_ERR_BUSY           -5

int MB_ProcessRecv(const uint8_t *pIn, uint16_t size, uint8_t *pOut);
int MB_ProcessRecvWithCRC(const uint8_t *pIn, uint16_t size, uint8_t *pOut);

#ifdef MB_MASTER_ENABLE
void MB_Read_Coils       (MB_Handle *handle, uint16_t addr, uint16_t num, uint8_t *data);
void MB_Read_Disc_Input  (MB_Handle *handle, uint16_t addr, uint16_t num, uint8_t *data);
void MB_Read_Reg_Hold    (MB_Handle *handle, uint16_t addr, uint16_t num, uint16_t *data);
void MB_Read_Reg_Input   (MB_Handle *handle, uint16_t addr, uint16_t num, uint16_t *data);
void MB_Write_Coil_single(MB_Handle *handle, uint16_t addr, uint16_t num, uint8_t data);
void MB_Write_RegHold_single(MB_Handle *handle, uint16_t addr, uint16_t num, uint16_t data);
void MB_Write_Coil_multi(MB_Handle *handle, uint16_t addr, uint16_t num, const uint8_t *data);
void MB_Write_RegHold_multi(MB_Handle *handle, uint16_t addr, uint16_t num, const uint8_t *data);
#endif

#endif
