#include <mega328p.h>
#include "RFID.h"

unsigned char RFID_isCard()
{
    unsigned char status;
    unsigned char str[MAX_LEN];
    status = RFID_MFRC522Request(PICC_REQIDL, str);
    if (status == MI_OK)
        return 1;
    else
        return 0;
}

unsigned char RFID_readCardSerial(unsigned char *serNum){
  unsigned char i = 0;
  unsigned char status;
  unsigned char str[MAX_LEN];

  status = RFID_anticoll(str);
  for (i = 0; i < 5; i++)
  {
    serNum[i] = str[i];
  }

  if (status == MI_OK)
    return 1;
  else
    return 0;
}

void RFID_init()
{
    PORTB |= (1 << RSPIN);
    RFID_reset();
    RFID_writeMFRC522(TModeReg, 0x8D);
    RFID_writeMFRC522(TPrescalerReg, 0x3E);
    RFID_writeMFRC522(TReloadRegL, 30);
    RFID_writeMFRC522(TReloadRegH, 0);
    RFID_writeMFRC522(TxAutoReg, 0x40);
    RFID_writeMFRC522(ModeReg, 0x3D);
    RFID_antennaOn();
}

void RFID_reset()
{
  RFID_writeMFRC522(CommandReg, PCD_RESETPHASE);
}

void RFID_writeMFRC522(unsigned char addr, unsigned char val)
{
  PORTB &= ~(1 << SSPIN);
  SPI_Transfer((addr<<1)&0x7E);
  SPI_Transfer(val);
  PORTB |= (1 << SSPIN);
}

unsigned char RFID_readMFRC522(unsigned char addr)
{
    unsigned char val;
    PORTB &= ~(1 << SSPIN);
    SPI_Transfer(((addr<<1)&0x7E) | 0x80);
    val = SPI_Transfer(0x00);
    PORTB |= (1 << SSPIN);
    return val;
}

void RFID_setBitMask(unsigned char reg, unsigned char mask)
{
    unsigned char tmp;
    tmp = RFID_readMFRC522(reg);
    RFID_writeMFRC522(reg, tmp | mask);  // set bit mask
}

void RFID_clearBitMask(unsigned char reg, unsigned char mask)
{
  unsigned char tmp;
  tmp = RFID_readMFRC522(reg);
  RFID_writeMFRC522(reg, tmp & (~mask));  // clear bit mask
}

void RFID_antennaOn(void)
{
    unsigned char temp;
    temp = RFID_readMFRC522(TxControlReg);
    if (!(temp & 0x03))
    {
        RFID_setBitMask(TxControlReg, 0x03);
    }
}

void RFID_antennaOff(void)
{
    unsigned char temp;
    temp = RFID_readMFRC522(TxControlReg);
    if (!(temp & 0x03))
    {
        RFID_clearBitMask(TxControlReg, 0x03);
    }
}

void RFID_calculateCRC(unsigned char *pIndata, unsigned char len, unsigned char *pOutData)
{
  unsigned char i, n;
  RFID_clearBitMask(DivIrqReg, 0x04);
  RFID_setBitMask(FIFOLevelReg, 0x80);
  for (i=0; i<len; i++)
    RFID_writeMFRC522(FIFODataReg, *(pIndata+i));
  RFID_writeMFRC522(CommandReg, PCD_CALCCRC);
  i = 0xFF;
  do
  {
    n = RFID_readMFRC522(DivIrqReg);
    i--;
  }
  while ((i!=0) && !(n&0x04));      //CRCIrq = 1

  pOutData[0] = RFID_readMFRC522(CRCResultRegL);
  pOutData[1] = RFID_readMFRC522(CRCResultRegM);
}

unsigned char RFID_MFRC522ToCard(unsigned char command, unsigned char *sendData, unsigned char sendLen, unsigned char *backData, unsigned int *backLen)
{
  unsigned char status = MI_ERR;
  unsigned char irqEn = 0x00;
  unsigned char waitIRq = 0x00;
  unsigned char lastBits;
  unsigned char n;
  unsigned int i;

  switch (command)
  {
    case PCD_AUTHENT:   //????
    {
      irqEn = 0x12;
      waitIRq = 0x10;
      break;
    }
    case PCD_TRANSCEIVE:  //??FIFO???
    {
      irqEn = 0x77;
      waitIRq = 0x30;
      break;
    }
    default:
      break;
  }
  RFID_writeMFRC522(CommIEnReg, irqEn|0x80);
  RFID_clearBitMask(CommIrqReg, 0x80);
  RFID_setBitMask(FIFOLevelReg, 0x80);
  RFID_writeMFRC522(CommandReg, PCD_IDLE);
  for (i=0; i<sendLen; i++)
    RFID_writeMFRC522(FIFODataReg, sendData[i]);
  RFID_writeMFRC522(CommandReg, command);
  if (command == PCD_TRANSCEIVE)
    RFID_setBitMask(BitFramingReg, 0x80);
  i = 2000;
  do
  {
    n = RFID_readMFRC522(CommIrqReg);
    i--;
  }
  while ((i!=0) && !(n&0x01) && !(n&waitIRq));
  RFID_clearBitMask(BitFramingReg, 0x80);
  if (i != 0)
  {
    if(!(RFID_readMFRC522(ErrorReg) & 0x1B))
    {
      status = MI_OK;
      if (n & irqEn & 0x01)
        status = MI_NOTAGERR;
      if (command == PCD_TRANSCEIVE)
      {
        n = RFID_readMFRC522(FIFOLevelReg);
        lastBits = RFID_readMFRC522(ControlReg) & 0x07;
        if (lastBits)
          *backLen = (n-1)*8 + lastBits;
        else
          *backLen = n*8;
        if (n == 0)
          n = 1;
        if (n > MAX_LEN)
          n = MAX_LEN;
        for (i=0; i<n; i++)
          backData[i] = RFID_readMFRC522(FIFODataReg);
      }
    }
    else
      status = MI_ERR;
  }
  return status;
}

unsigned char RFID_MFRC522Request(unsigned char reqMode, unsigned char *TagType)
{
  unsigned char status;
  unsigned int backBits;
  RFID_writeMFRC522(BitFramingReg, 0x07);
  TagType[0] = reqMode;
  status = RFID_MFRC522ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);
  if ((status != MI_OK) || (backBits != 0x10))
    status = MI_ERR;
  return status;
}

unsigned char RFID_anticoll(unsigned char *serNum)
{
  unsigned char status;
  unsigned char i;
  unsigned char serNumCheck=0;
  unsigned int unLen;

  RFID_writeMFRC522(BitFramingReg, 0x00);

  serNum[0] = PICC_ANTICOLL;
  serNum[1] = 0x20;
  status = RFID_MFRC522ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);

  if (status == MI_OK)
  {
    for (i=0; i<4; i++)
      serNumCheck ^= serNum[i];
    if (serNumCheck != serNum[i])
      status = MI_ERR;
  }
  return status;
}

unsigned char RFID_auth(unsigned char authMode, unsigned char BlockAddr, unsigned char *Sectorkey, unsigned char *serNum)
{
  unsigned char status;
  unsigned int recvBits;
  unsigned char i;
  unsigned char buff[12];
  buff[0] = authMode;
  buff[1] = BlockAddr;
  for (i=0; i<6; i++)
    buff[i+2] = *(Sectorkey+i);
  for (i=0; i<4; i++)
    buff[i+8] = *(serNum+i);

  status = RFID_MFRC522ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits);
  if ((status != MI_OK) || (!(RFID_readMFRC522(Status2Reg) & 0x08)))
    status = MI_ERR;
  return status;
}

unsigned char RFID_read(unsigned char blockAddr, unsigned char *recvData)
{
  unsigned char status;
  unsigned int unLen;
  recvData[0] = PICC_READ;
  recvData[1] = blockAddr;
  RFID_calculateCRC(recvData,2, &recvData[2]);
  status = RFID_MFRC522ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);
  if ((status != MI_OK) || (unLen != 0x90))
    status = MI_ERR;
  return status;
}

unsigned char RFID_write(unsigned char blockAddr, unsigned char *writeData)
{
  unsigned char status;
  unsigned int recvBits;
  unsigned char i;
  unsigned char buff[18];

  buff[0] = PICC_WRITE;
  buff[1] = blockAddr;
  RFID_calculateCRC(buff, 2, &buff[2]);
  status = RFID_MFRC522ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);

  if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
    status = MI_ERR;

  if (status == MI_OK)
  {
    for (i=0; i<16; i++)
      buff[i] = *(writeData+i);

    RFID_calculateCRC(buff, 16, &buff[16]);
    status = RFID_MFRC522ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);

    if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A))
      status = MI_ERR;
  }

  return status;
}

unsigned char RFID_selectTag(unsigned char *serNum)
{
  unsigned char i;
  unsigned char status;
  unsigned char size;
  unsigned int recvBits;
  unsigned char buffer[9];
  buffer[0] = PICC_SElECTTAG;
  buffer[1] = 0x70;
  for (i=0; i<5; i++)
    buffer[i+2] = *(serNum+i);
  RFID_calculateCRC(buffer, 7, &buffer[7]);
  status = RFID_MFRC522ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);
  if ((status == MI_OK) && (recvBits == 0x18))
    size = buffer[0];
  else
    size = 0;
  return size;
}

void RFID_halt()
{
  unsigned char status;
  unsigned int unLen;
  unsigned char buff[4];
  buff[0] = PICC_HALT;
  buff[1] = 0;
  RFID_calculateCRC(buff, 2, &buff[2]);
  status = RFID_MFRC522ToCard(PCD_TRANSCEIVE, buff, 4, buff,&unLen);
}



