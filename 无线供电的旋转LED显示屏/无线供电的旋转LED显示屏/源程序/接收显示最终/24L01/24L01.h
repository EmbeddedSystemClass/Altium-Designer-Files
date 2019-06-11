#ifndef __24L01_H
#define __24L01_H
#include"reg52.h"
#include"system.h"






 

#define TX_ADR_WIDTH   5  // 5�ֽڿ�ȵķ���/���յ�ַ
#define TX_PLOAD_WIDTH 32  // ����ͨ����Ч���ݿ��













sbit CE   = P2^0;  //оƬ��ģʽ�����ߣ���CSNΪ��ʱ��CE��CONFIG�Ĵ�����ͬ����״̬ Chip Enable pin signal (output)
sbit CSN  = P3^5;  //оƬ��Ƭѡ�ߣ�CSNΪ�͵�ƽоƬ���� Slave Select pin, (output to CSN, nRF24L01)  
sbit SCK  = P2^1;  //оƬ���Ƶ�ʱ���ߣ�SPIʱ�ӣ� Master Out, Slave In pin (output)	
sbit MOSI = P3^4;  //оƬ���������ߣ��������� Serial Clock pin, (output)  
sbit MISO = P2^2;  //оƬ���������ߣ�������� Master In, Slave Out pin (input)
sbit IRQ  = P3^3;  // �����ж�дʱ����ӵ�P32��P33��Interrupt signal, from nRF24L01 (input)

// SPI(nRF24L01) commands		24L01��������
#define READ_REG    0x00  //���Ĵ���ָ�� Define read command to register
#define WRITE_REG   0x20  //д�Ĵ���ָ�� Define write command to register
#define RD_RX_PLOAD 0x61  //��ȡ��������ָ�� Define RX payload register address
#define WR_TX_PLOAD 0xA0  //д��������ָ�� Define TX payload register address
#define FLUSH_TX    0xE1  //��ϴ����TX FIFOָ�� Define flush TX register command
#define FLUSH_RX    0xE2  //��ϴ���� FIFOָ�� Define flush RX register command
#define REUSE_TX_PL 0xE3  //�����ظ�װ������ָ�� Define reuse TX payload register command
						  //����ʹ����һ���������Ч���ݣ���CE=1ʱ�����ݲ��ϵ����·���
#define NOP         0xFF  //�ղ���������������״̬�Ĵ��� Define No Operation, might be used to read status register

// SPI(nRF24L01) registers(addresses)	  24L01�Ĵ�����ַ
#define CONFIG      0x00  //�����շ�״̬��CRCУ��ģʽ�Լ��շ�״̬��Ӧ��ʽ 'Config' register address
#define EN_AA       0x01  //�Զ�Ӧ�������� 'Enable Auto Acknowledgment' register address
#define EN_RXADDR   0x02  //�����ŵ����� 'Enabled RX addresses' register address
#define SETUP_AW    0x03  //�շ���ַ������� 'Setup address width' register address
#define SETUP_RETR  0x04  //�Զ��ط��������� 'Setup Auto. Retrans' register address
#define RF_CH       0x05  //����Ƶ������ 'RF channel' register address
#define RF_SETUP    0x06  // �������ʡ����Ĺ�������'RF setup' register address
#define STATUS      0x07  //״̬�Ĵ��� 'Status' register address
#define OBSERVE_TX  0x08  //���ͼ�⹦�� 'Observe TX' register address
#define CD          0x09  //��ַ��� 'Carrier Detect' register address
#define RX_ADDR_P0  0x0A  //Ƶ��0�������ݵ�ַ����󳤶�Ϊ���ֽ� 'RX address pipe0' register address
#define RX_ADDR_P1  0x0B  // 'RX address pipe1' register address
#define RX_ADDR_P2  0x0C  // 'RX address pipe2' register address
#define RX_ADDR_P3  0x0D  // 'RX address pipe3' register address
#define RX_ADDR_P4  0x0E  // 'RX address pipe4' register address
#define RX_ADDR_P5  0x0F  //Ƶ��5�������ݵ�ַ 'RX address pipe5' register address
#define TX_ADDR     0x10  //���͵�ַ�Ĵ��� 'TX address' register address
#define RX_PW_P0    0x11  //����Ƶ��0�������ݳ��ȣ���Ч���ݳ���1~32 'RX payload width, pipe0' register address
#define RX_PW_P1    0x12  // 'RX payload width, pipe1' register address
#define RX_PW_P2    0x13  // 'RX payload width, pipe2' register address
#define RX_PW_P3    0x14  // 'RX payload width, pipe3' register address
#define RX_PW_P4    0x15  // 'RX payload width, pipe4' register address
#define RX_PW_P5    0x16  // ����Ƶ��5�������ݳ��� 'RX payload width, pipe5' register address
#define FIFO_STATUS 0x17  //FIFOջ��ջ��״̬�Ĵ������� 'FIFO Status Register' register address

void init_24L01(void);
uchar SPI_RW(uchar byte);
uchar SPI_RW_Reg(uchar reg, uchar value);
uchar SPI_Read(uchar reg);
uchar SPI_Read_Buf(uchar reg, uchar * pBuf, uchar bytes);
uchar SPI_Write_Buf(uchar reg, uchar * pBuf, uchar bytes);
void RX_Mode(void);
void TX_Mode(uchar * BUF);
uchar Check_ACK(bit clear);	 

#endif

