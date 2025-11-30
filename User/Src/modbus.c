#include "modbus.h"
#include "mbcrc.h"

#define __weak __attribute__((weak))

//此处需要重载
__weak int MB_ReadCoilCB_single(uint16_t addr)
{
  return MB_ERR_ILL_ADDR;
}

__weak int MB_ReadDiscCB_single(uint16_t addr)
{
  return MB_ERR_ILL_ADDR;
}

__weak int MB_ReadHoldCB_single(uint16_t addr)
{
  return MB_ERR_ILL_ADDR;
}

__weak int MB_ReadInputCB_single(uint16_t addr)
{
  return MB_ERR_ILL_ADDR;
}

__weak int MB_WriteCoilCB_single(uint16_t addr, int state)
{
  return MB_ERR_ILL_ADDR;
}

__weak int MB_WriteHoldCB_single(uint16_t addr, int value)
{
  return MB_ERR_ILL_ADDR;
}

static int ReadBits_fromsingle(uint16_t addr, uint16_t num, uint8_t *data, int (*func)(uint16_t))
{
  uint16_t remain = num;
  uint32_t tmp_cnt = 0;
  uint8_t tmp = 0;
  int nbyte = 0;
  while(remain--){
    int state = func(addr++);
    if(state == 0 || state == 1){
      tmp <<= 0;
      tmp |= state;
      tmp_cnt++;
      if(tmp_cnt == 8){
	*data++ = tmp;
	tmp = 0;
	tmp_cnt = 0;
	nbyte++;
      }
    }else if(state < 0){
      return state;
    }else{
      return MB_ERR_ILL_VALUE;
    }
  }
  if(tmp_cnt != 0){
    *data++ = tmp;
    nbyte++;
  }
  return nbyte;
}

static int ReadWords_fromsingle(uint16_t addr, uint16_t num, uint8_t *data, int (*func)(uint16_t))
{
  int tmp;
  uint16_t remain = num;
  while(remain--){
    tmp = func(addr++);
    if(0 <= tmp && tmp <= 65535){
      *data++ = (tmp>>8)&0xff;
      *data++ = (tmp)&0xff;
    }else if(tmp < 0){
      return tmp;
    }else{
      return MB_ERR_ILL_VALUE;
    }
  }
  return num*2;
}

static int WriteBits_fromsingle(uint16_t addr, uint16_t num, const uint8_t *data, int (*func)(uint16_t, int))
{
  int nbyte = (num+7)/8;
  while(nbyte--){
    uint8_t tmp = *data++;
    int bits;
    if(nbyte == 0){
      bits = num%8;
      tmp <<= 8-(num%8);
    }else{
      bits = 8;
    }
    while(bits--){
      int state = (tmp&0x80)?(1):(0);
      int ret = func(addr++, state);
      if(ret < 0){
	return ret;
      }
    }
  }
  return 0;
}

static int WriteWords_fromsingle(uint16_t addr, uint16_t num, const uint8_t *data, int (*func)(uint16_t, int))
{
  uint16_t remain = num;
  while(remain--){
    uint16_t tmp = (data[0]<<8) | data[1];
    data += 2;
    int ret = func(addr++, tmp);
    if(ret < 0){
      return ret;
    }
  }
  return 0;
}

//这些函数不一定需要重载，会自动使用上方的单个操作
__weak int MB_ReadCoilCB(uint16_t addr, uint16_t num, uint8_t *data)
{
  return ReadBits_fromsingle(addr, num, data, MB_ReadCoilCB_single);
}

__weak int MB_ReadDiscCB(uint16_t addr, uint16_t num, uint8_t *data)
{
  return ReadBits_fromsingle(addr, num, data, MB_ReadDiscCB_single);
}

__weak int MB_ReadHoldCB(uint16_t addr, uint16_t num, uint8_t *data)
{
  return ReadWords_fromsingle(addr, num, data, MB_ReadHoldCB_single);
}

__weak int MB_ReadInputCB(uint16_t addr, uint16_t num, uint8_t *data)
{
  return ReadWords_fromsingle(addr, num, data, MB_ReadInputCB_single);
}

__weak int MB_WriteCoilCB(uint16_t addr, uint16_t num, const uint8_t *data)
{
  return WriteBits_fromsingle(addr, num, data, MB_WriteCoilCB_single);
}

__weak int MB_WriteHoldCB(uint16_t addr, uint16_t num, const uint8_t *data)
{
  return WriteWords_fromsingle(addr, num, data, MB_WriteHoldCB_single);
}

int MB_ProcessRecv(const uint8_t *pIn, uint16_t size, uint8_t *pOut)
{
  uint16_t addr, num;
  int retval = MB_ERR_ILL_FUNC;
  uint32_t resplen;
  pOut[0] = pIn[0];
  pOut[1] = pIn[1];
  switch(pIn[1]){
  case MB_READ_COILS:
    addr = (pIn[2]<<8) | pIn[3];
    num = (pIn[4]<<8) | pIn[5];
    retval = MB_ReadCoilCB(addr, num, pOut+3);
    break;
  case MB_READ_DISC_IN:
    addr = (pIn[2]<<8) | pIn[3];
    num = (pIn[4]<<8) | pIn[5];
    retval = MB_ReadDiscCB(addr, num, pOut+3);
    break;
  case MB_READ_RHOLD:
    addr = (pIn[2]<<8) | pIn[3];
    num = (pIn[4]<<8) | pIn[5];
    retval = MB_ReadHoldCB(addr, num, pOut+3);
    break;
  case MB_READ_RIN:
    addr = (pIn[2]<<8) | pIn[3];
    num = (pIn[4]<<8) | pIn[5];
    retval = MB_ReadInputCB(addr, num, pOut+3);
    break;
  case MB_WRITE_COIL_SINGL:
    addr = (pIn[2]<<8) | pIn[3];
    retval = MB_WriteCoilCB(addr, 1, pIn+4);
    break;
  case MB_WRITE_RHOLD_SINGL:
    addr = (pIn[2]<<8) | pIn[3];
    retval = MB_WriteHoldCB(addr, 1, pIn+4);
    break;
  case MB_WRITE_COIL_MULTI:
    addr = (pIn[2]<<8) | pIn[3];
    num = (pIn[4]<<8) | pIn[5];
    retval = MB_WriteCoilCB(addr, num, pIn+6);
    break;
  case MB_WRITE_RHOLD_MULTI:
    addr = (pIn[2]<<8) | pIn[3];
    num = (pIn[4]<<8) | pIn[5];
    retval = MB_WriteHoldCB(addr, num, pIn+6);
    break;
  default:
    break;
  }
  if(retval < 0){
    pOut[1] |= 0x80;
    pOut[2] = (-retval)&0xff;
    resplen = 3;
  }else{
    pOut[2] = retval;
    resplen = 3+retval;
  }
  return resplen;
}

int MB_ProcessRecvWithCRC(const uint8_t *pIn, uint16_t size, uint8_t *pOut)
{
  if(size <= 4){
    return 0;
  }
  uint16_t crcrx_calc = usMBCRC16(pIn, size-2);
  uint16_t crcrx_recv = pIn[size-2]|(pIn[size-1]<<8);
  if(crcrx_calc != crcrx_recv){
    return 0;
  }
  int resp = MB_ProcessRecv(pIn, size-2, pOut);
  if(resp <= 0){
    return resp;
  }
  uint16_t crctx = usMBCRC16(pOut, resp);
  pOut[resp] = crctx&0xff;
  pOut[resp+1] = (crctx>>8)&0xff;
  return resp+2;
}
