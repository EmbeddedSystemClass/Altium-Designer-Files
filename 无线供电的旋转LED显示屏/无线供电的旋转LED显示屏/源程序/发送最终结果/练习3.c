#include<STC15F2K.h>
#include <intrins.H>
#include"ziku.h"
#define uint unsigned int
#define uchar unsigned char
uchar code table[]="0123456789abcdef";
uchar ok;
uchar abc=0;			//��Ҫ���ͺ��ӵĸ���
uchar x;				//�յ������źű�־
uchar flag_end=0;		//һ�δ��ڽ��ս���
uint receive_length=6;	//����һ�ν������ݳ��ȶ��壬Ĭ��Ϊ6���� 
uchar menu=1; 
uchar xdata ckou[256];	//����װ8������
sbit lcden=P5^4;
sbit lcdrw=P5^5;
sbit lcdrs=P4^5;


void delayms(uint x)
{
	uint i,j;
	for(i=x;i>0;i--)
		for(j=110;j>0;j--);
}




void yjwrite_com(uchar com)
{
	lcdrs=0;
	P0=com;
	delayms(5);
	lcden=1;
	delayms(5);
	lcden=0;
}
void yjwrite_date(uchar date)
{
	lcdrs=1;
	P0=date;
	delayms(5);
	lcden=1;
	delayms(5);
	lcden=0;
}
void yjinit()
{
	lcden=0;
	lcdrw=0;
	yjwrite_com(0x38);		
	yjwrite_com(0x0c);
	yjwrite_com(0x06);
	yjwrite_com(0x01);		//��ʾ��0��ָ����0
}








#define TX_ADR_WIDTH   5  // 5�ֽڿ�ȵķ���/���յ�ַ
#define TX_PLOAD_WIDTH 32  // ����ͨ����Ч���ݿ��
#define RX_PLOAD_WIDTH 32
sbit CE   = P2^2;  //оƬ��ģʽ�����ߣ���CSNΪ��ʱ��CE��CONFIG�Ĵ�����ͬ����״̬ Chip Enable pin signal (output)
sbit CSN  = P2^3;  //оƬ��Ƭѡ�ߣ�CSNΪ�͵�ƽоƬ���� Slave Select pin, (output to CSN, nRF24L01)  
sbit SCK  = P2^1;  //оƬ���Ƶ�ʱ���ߣ�SPIʱ�ӣ� Master Out, Slave In pin (output)	
sbit MOSI = P2^4;  //оƬ���������ߣ��������� Serial Clock pin, (output)  
sbit MISO = P2^0;  //оƬ���������ߣ�������� Master In, Slave Out pin (input)
sbit IRQ  = P3^2;  // �����ж�дʱ����ӵ�P32��P33��Interrupt signal, from nRF24L01 (input)

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

void init_io(void);
uchar SPI_RW(uchar byte);
uchar SPI_RW_Reg(uchar reg, uchar value);
uchar SPI_Read(uchar reg);
uchar SPI_Read_Buf(uchar reg, uchar * pBuf, uchar bytes);
uchar SPI_Write_Buf(uchar reg, uchar * pBuf, uchar bytes);
void RX_Mode(void);
void TX_Mode(uchar * BUF);
uchar Check_ACK(bit clear);
void CheckButtons();	 

sbit KEY1=P3^4;
sbit KEY2=P3^5;
sbit KEY3=P3^6;
sbit KEY4=P3^7;
sbit key_lock=P3^3;
uchar code table1[]="0123456789abcdef";
uchar length=150;


uchar code TX_ADDRESS[5]={0x34,0x43,0x10,0x10,0x01};//����һ����̬���͵�ַ
uchar TxBuf[TX_PLOAD_WIDTH]={0};	 // 
uchar RxBuf[RX_PLOAD_WIDTH]={0};
uchar sta;
uchar MAX_RT;//�ط���������жϣ�д1����жϣ����MAX_RT�����жϣ����������ϵͳ���ܽ���ͨ
uchar RX_DR;//���������жϣ����յ���Ч���ݺ���1��д1����ж�
uchar  TX_DS;//���ݷ�������жϣ�д1����ж� 


/*MAX_RT=sta>>4&0x01;//�ط���������жϣ�д1����жϣ����MAX_RT�����жϣ����������ϵͳ���ܽ���ͨ
RX_DR=sta>>6&0x01;//���������жϣ����յ���Ч���ݺ���1��д1����ж�
TX_DS=sta>>5&0x01;//���ݷ�������жϣ�д1����ж� 		   




/**************************************************
����: init_io()

����:
    ��ʼ��IO
/**************************************************/
void init_io(void)
{
	CE  = 0;        // ����
	CSN = 1;        // SPI��ֹ������
	SCK = 0;        // SPIʱ���õ�
	IRQ = 1;        // �жϸ�λ
}
/**************************************************/

/**************************************************/

/**************************************************
������SPI_RW()
�˴�Ϊspi�ĺ��ģ���spiЭ��ı��
������
    ����SPIЭ�飬дһ�ֽ����ݵ�nRF24L01��ͬʱ��nRF24L01
	����һ�ֽ�
/**************************************************/
uchar SPI_RW(uchar byte)
{
	uchar i;
   	for(i=0; i<8; i++)          // ѭ��8��
   	{
   		MOSI = (byte & 0x80);   // byte���λ�����MOSI
   		byte <<= 1;             // ��һλ��λ�����λ
   		SCK = 1;                // ����SCK��nRF24L01��MOSI����1λ���ݣ�ͬʱ��MISO���1λ����
   		byte |= MISO;       	// ��MISO��byte���λ
   		SCK = 0;            	// SCK�õ�
   	}
    return(byte);           	// ���ض�����һ�ֽ�
}
/**************************************************/

/**************************************************
������SPI_RW_Reg()

������
    д����value��reg�Ĵ���
/**************************************************/
uchar SPI_RW_Reg(uchar reg, uchar value)
{
	uchar status;
  	CSN = 0;                   // CSN�õͣ���ʼ��������
  	status = SPI_RW(reg);      // ѡ��Ĵ�����ͬʱ����״̬��
  	SPI_RW(value);             // Ȼ��д���ݵ��üĴ���
  	CSN = 1;                   // CSN���ߣ��������ݴ���
  	return(status);            // ����״̬�Ĵ���
}
/**************************************************/

/**************************************************
������SPI_Read()

������
    ��reg�Ĵ�����һ�ֽ�
/**************************************************/
uchar SPI_Read(uchar reg)
{
	uchar reg_val;
  	CSN = 0;                    // CSN�õͣ���ʼ��������
  	SPI_RW(reg);                // ѡ��Ĵ���
  	reg_val = SPI_RW(0);        // Ȼ��ӸüĴ���������READ_REG    0x00  //���Ĵ���ָ��
  	CSN = 1;                    // CSN���ߣ��������ݴ���
  	return(reg_val);            // ���ؼĴ�������
}
/**************************************************/

/**************************************************
������SPI_Read_Buf()

������
    ��reg�Ĵ�������bytes���ֽڣ�ͨ��������ȡ����ͨ��
	���ݻ����/���͵�ַ
����: ���ڶ����ݣ�reg��Ϊ�Ĵ�����ַ��pBuf��Ϊ���������ݵ�ַ��uchars���������ݵĸ���
/**************************************************/
uchar SPI_Read_Buf(uchar reg, uchar * pBuf, uchar bytes)
{
	uchar status, i;
  	CSN = 0;                    // CSN�õͣ���ʼ��������
  	status = SPI_RW(reg);       // ѡ��Ĵ�����ͬʱ����״̬��
  	for(i=0; i<bytes; i++)
    	pBuf[i] = SPI_RW(0);    // ����ֽڴ�nRF24L01����
  	CSN = 1;                    // CSN���ߣ��������ݴ���
  	return(status);             // ����״̬�Ĵ���
}
/**************************************************/

/**************************************************
������SPI_Write_Buf()

������
    ��pBuf�����е�����д�뵽nRF24L01��ͨ������д�뷢
	��ͨ�����ݻ����/���͵�ַ
����: ����д���ݣ�Ϊ�Ĵ�����ַ��pBuf��Ϊ��д�����ݵ�ַ��uchars��д�����ݵĸ���
/**************************************************/
uchar SPI_Write_Buf(uchar reg, uchar * pBuf, uchar bytes)
{
	uchar status, i;
  	CSN = 0;                    // CSN�õͣ���ʼ��������
  	status = SPI_RW(reg);       // ѡ��Ĵ�����ͬʱ����״̬��
  	for(i=0; i<bytes; i++)
    	SPI_RW(pBuf[i]);        // ����ֽ�д��nRF24L01
  	CSN = 1;                    // CSN���ߣ��������ݴ���
  	return(status);             // ����״̬�Ĵ���
}
/**************************************************/

/**************************************************
������RX_Mode()

������
    �����������nRF24L01Ϊ����ģʽ���ȴ����շ����豸�����ݰ�
/**************************************************/
void RX_Mode(void)
{
	CE = 0;
  	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); 
	 // �����豸����ͨ��0ʹ�úͷ����豸��ͬ�ķ��͵�ַ
  	SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);               
	// ʹ�ܽ���ͨ��0�Զ�Ӧ��ACKӦ������
  	SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);           
	// ʹ�ܽ���ͨ��0
  	SPI_RW_Reg(WRITE_REG + RF_CH, 40);                 
	// ѡ����Ƶͨ��0x40
  	SPI_RW_Reg(WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); 
	 // ����ͨ��0ѡ��ͷ���ͨ����ͬ��Ч���ݿ��
  	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x07);            
	// ���ݴ�����1Mbps�����书��0dBm���������Ŵ�������
  	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0f);              
	// CRCʹ�ܣ�16λCRCУ�飬�ϵ磬����ģʽ
  	CE = 1;        // ����CE���������豸
}
/**************************************************/

/**************************************************
������TX_Mode()

������
    �����������nRF24L01Ϊ����ģʽ����CE=1��������10us����
	130us���������䣬���ݷ��ͽ����󣬷���ģ���Զ�ת�����
	ģʽ�ȴ�Ӧ���źš�
/**************************************************/
void TX_Mode(uchar * BUF)
{
	CE = 0;
  	SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);     
	// д�뷢�͵�ַ
  	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH);  
	// Ϊ��Ӧ������豸������ͨ��0��ַ�ͷ��͵�ַ��ͬ
  	SPI_Write_Buf(WR_TX_PLOAD, BUF, TX_PLOAD_WIDTH);                  
	// д���ݰ���TX FIFO
  	SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);       // ʹ�ܽ���ͨ��0�Զ�Ӧ��
  	SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);   // ʹ�ܽ���ͨ��0
  	SPI_RW_Reg(WRITE_REG + SETUP_RETR, 0x0a);  // �Զ��ط���ʱ�ȴ�250us+86us���Զ��ط�10��
  	SPI_RW_Reg(WRITE_REG + RF_CH, 40);         // ѡ����Ƶͨ��0x40
  	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x07);    // ���ݴ�����1Mbps�����书��0dBm���������Ŵ�������
  	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);      // CRCʹ�ܣ�16λCRCУ�飬�ϵ�
	CE = 1;
}
/**************************************************/

/**************************************************
������Check_ACK()

������
    �������豸���޽��յ����ݰ����趨û���յ�Ӧ����
	���Ƿ��ط�
/**************************************************/
uchar Check_ACK(bit clear)
{	
	uchar i=0;
	while(IRQ&&i<50)i++;

	                    // ����״̬�Ĵ���
	MAX_RT=sta>>4&0x01;
	RX_DR=sta>>6&0x01;
	TX_DS=sta>>5&0x01;
	if(MAX_RT)//�ط���������жϣ�д1����ж�
		if(clear)// �Ƿ����TX FIFO��û������ڸ�λMAX_RT�жϱ�־���ط�
			SPI_RW(FLUSH_TX);//TX FIFO�Ĵ�������־1������0����


	SPI_RW_Reg(WRITE_REG + STATUS, sta|0x70); 
	// ���TX_DS��MAX_RT�жϱ�־�����յ����ݺ�RX_DR,TX_DS,MAX_PT���ø�Ϊ1��ͨ��д1������жϱ�־
	IRQ = 1;
	
	if(TX_DS)//���ݷ�������жϣ�д1����ж�
		return(0x00);
	else
		return(0xff);
}
uchar key_scan()
{
	uchar key=0;
	static uchar key_up=1;
	key_lock=0;
	if(KEY1==0&&key_up)
	{
		delayms(10);
		if(KEY1==0)
		{
			key_up=0;
			key=1;
		}
	}
	if(KEY2==0&&key_up)
	{
		delayms(10);
		if(KEY2==0)
		{
			key_up=0;
			key=2;
		}
	}
	if(KEY3==0&&key_up)
	{
		delayms(10);
		if(KEY3==0)
		{
			key_up=0;
			key=3;
		}
	}
	if(KEY4==0&&key_up)
	{
		delayms(10);
		if(KEY4==0)
		{
			key_up=0;
			key=4;
		}
	}
	if(KEY1&&KEY2&&KEY3&&KEY4)key_up=1;
	return key;
}
void CheckButtons()
{
	uchar k;
	static uchar add=0;		//24c64�д洢λ�ñ�־
	static uchar up=20,down=0;
	k=key_scan();	
	if(k==4)
	{
		menu++;
		if(menu==12)menu=0;
	}
	switch(menu)
	{
		case 0:				  //���
			yjwrite_com(0x80+0x40);
			yjwrite_date('C');yjwrite_date('l');yjwrite_date('e');yjwrite_date('a');
			yjwrite_date('r');yjwrite_date('.');yjwrite_date('.');yjwrite_date('.');
			yjwrite_date(' ');yjwrite_date(' ');
			if(k==1)
			{
				TxBuf[0]=0xAA;
				TxBuf[1]=0x55;
				TxBuf[2]=1;
				TxBuf[3]=0;
				TX_Mode(TxBuf);
				delayms(200);
				Check_ACK(1);
			}
			break;
		case 1:									  //��ͣ�ƶ�
			yjwrite_com(0x80+0x40);
			yjwrite_date('S');yjwrite_date('t');yjwrite_date('o');yjwrite_date('p');
			yjwrite_date(' ');yjwrite_date(' ');yjwrite_date(' ');yjwrite_date(' ');
			yjwrite_date(' ');yjwrite_date(' ');
			if(k==2||k==3||k==1)
			{
				TxBuf[0]=0xAA;
				TxBuf[1]=0x55;
				TxBuf[2]=2;
				TxBuf[3]=0;
				TX_Mode(TxBuf);
				delayms(200);
				Check_ACK(1);	
			}
			break;
		case 2:							   	//����������
			yjwrite_com(0x80+0x40);
			yjwrite_date('W');yjwrite_date('i');yjwrite_date('d');yjwrite_date('e');
			yjwrite_date(':');yjwrite_date(' ');yjwrite_date(' ');yjwrite_date(' ');
			yjwrite_date(' ');yjwrite_date(' ');
			TxBuf[0]=0xAA;TxBuf[1]=0x55;TxBuf[3]=0;
			if(k==2)
			{
				TxBuf[2]=3;
				yjwrite_com(0x80+0x40+7);
				yjwrite_date('+');
				TX_Mode(TxBuf);
				delayms(200);
				Check_ACK(1);
			}
			if(k==3)
			{
				TxBuf[2]=4;
				yjwrite_com(0x80+0x40+7);
				yjwrite_date('-');
				TX_Mode(TxBuf);
				delayms(200);
				Check_ACK(1);
			}
			break;						  
		case 3:								   //�ƶ��ٶ�����
			yjwrite_com(0x80+0x40);
			yjwrite_date('S');yjwrite_date('p');yjwrite_date('e');yjwrite_date('e');
			yjwrite_date('d');yjwrite_date(':');yjwrite_date(' ');yjwrite_date(' ');
			yjwrite_date(' ');yjwrite_date(' ');
			TxBuf[0]=0xAA;TxBuf[1]=0x55;TxBuf[3]=0;
			if(k==2)
			{
				TxBuf[2]=5;
				yjwrite_com(0x80+0x40+7);
				yjwrite_date('+');
				TX_Mode(TxBuf);
				delayms(200);
				Check_ACK(1);
			}
			if(k==3)
			{
				TxBuf[2]=6;
				yjwrite_com(0x80+0x40+7);
				yjwrite_date('-');
				TX_Mode(TxBuf);
				delayms(200);
				Check_ACK(1);
			}
			break;
		case 4:		//��ֹ��ʾʱ��ʾ���ݵĿ�ʼλ������
			yjwrite_com(0x80+0x40);
			yjwrite_date('S');yjwrite_date('p');yjwrite_date('o');yjwrite_date('t');
			yjwrite_date('a');yjwrite_date('d');yjwrite_date('d');yjwrite_date(' ');
			yjwrite_date(' ');yjwrite_date(' ');
			TxBuf[0]=0xAA;TxBuf[1]=0x55;TxBuf[3]=0;
			if(k==2)
			{
				TxBuf[2]=7;
				yjwrite_com(0x80+0x40+7);
				yjwrite_date('+');
				TX_Mode(TxBuf);
				delayms(200);
				Check_ACK(1);
			}
			if(k==3)
			{
				TxBuf[2]=8;
				yjwrite_com(0x80+0x40+7);
				yjwrite_date('-');
				TX_Mode(TxBuf);
				delayms(200);
				Check_ACK(1);
			}
			break;
		case 5:			//���յ���������24C64�д洢λ������
			yjwrite_com(0x80+0x40);
			yjwrite_date('C');yjwrite_date('6');yjwrite_date('4');yjwrite_date('a');
			yjwrite_date('d');yjwrite_date('d');yjwrite_date(':');yjwrite_date(' ');
			yjwrite_date(' ');yjwrite_date(' ');
			TxBuf[0]=0xAA;TxBuf[1]=0x55;
			if(k==2)
			{
				add++;
			}
			if(k==3)
			{
				add--;
				if(add==-1)add=0;
			}
			if(k==1)
			{
				TxBuf[2]=9;
				TxBuf[3]=add;
				TX_Mode(TxBuf);delayms(200);Check_ACK(1);	
			}
			yjwrite_com(0x80+0x40+7);
			yjwrite_date('0'+add/100);yjwrite_date('0'+add/10%10);yjwrite_date('0'+add%10);
			break;
		case 7:					//��̬ɨ�跶Χ����ȷ��
			yjwrite_com(0x80+0x40);
			yjwrite_date('U');yjwrite_date('p');yjwrite_date(' ');yjwrite_date(' ');
			yjwrite_date(' ');yjwrite_date(' ');yjwrite_date(' ');yjwrite_date(' ');
			yjwrite_date(' ');yjwrite_date(' ');
			TxBuf[0]=0xAA;TxBuf[1]=0x55;
			if(k==2)
			{
				up++;
				if(up==255)up=255;	
			}
			if(k==3)
			{
				up--;
				if(up<=(down+6))up=down+7;
			}
			if(k==1)
			{
				TxBuf[2]=10;
				TxBuf[3]=up;
				TX_Mode(TxBuf);delayms(200);Check_ACK(1);	
			}
			yjwrite_com(0x80+0x40+7);
			yjwrite_date('0'+up/100);yjwrite_date('0'+up/10%10);yjwrite_date('0'+up%10);
			break;
		case 6:					//��̬ɨ�跶Χ����ȷ��
			yjwrite_com(0x80+0x40);
			yjwrite_date('D');yjwrite_date('o');yjwrite_date('w');yjwrite_date('n');
			yjwrite_date(':');yjwrite_date(' ');yjwrite_date(' ');yjwrite_date(' ');
			yjwrite_date(' ');yjwrite_date(' ');
			TxBuf[0]=0xAA;delayms(2);TxBuf[1]=0x55;
			if(k==2)
			{
				down++;
				if(down>=(up-6))down=up-7;	
			}
			if(k==3)
			{
				down--;
				if(down==-1)down=0;
			}
			if(k==1)
			{
				TxBuf[2]=11;
				TxBuf[3]=down;
				TX_Mode(TxBuf);delayms(200);Check_ACK(1);	
			}
			yjwrite_com(0x80+0x40+7);
			yjwrite_date('0'+down/100);yjwrite_date('0'+down/10%10);yjwrite_date('0'+down%10);
			break;
		case 8:		//���͵Ŀ�ʼλ������
			yjwrite_com(0x80+0x40);
			yjwrite_date('S');yjwrite_date('t');yjwrite_date('a');yjwrite_date('r');
			yjwrite_date('t');yjwrite_date(':');yjwrite_date(' ');yjwrite_date(' ');
			yjwrite_date(' ');yjwrite_date(' ');
			TxBuf[0]=0xAA;TxBuf[1]=0x55;
			if(k==2)
			{
				abc++;
			}
			if(k==3)
			{
				abc--;
				if(abc==-1)abc=255;
			}
			break;
		case 9:  		//���ͺ��ӵĳ�������
			yjwrite_com(0x80+0x40);
			yjwrite_date('L');yjwrite_date('e');yjwrite_date('n');yjwrite_date('g');
			yjwrite_date('t');yjwrite_date('h');yjwrite_date(':');yjwrite_date(' ');
			yjwrite_date(' ');yjwrite_date(' ');
			if(k==2)
			{
				length++;
			}
			if(k==3)
			{
				length--;
				if(length==0)length=1;
			}
			yjwrite_com(0x80+0x40+7);
			yjwrite_date('0'+length/100);yjwrite_date('0'+length/10%10);yjwrite_date('0'+length%10);
			break;
		case 10:
			yjwrite_com(0x80+0x40);
			yjwrite_date('S');yjwrite_date('e');yjwrite_date('n');yjwrite_date('d');
			yjwrite_date(' ');yjwrite_date(' ');yjwrite_date(' ');yjwrite_date(' ');
			yjwrite_date(' ');yjwrite_date(' ');
			if(k==1)
			{
				ok=!ok;
				x=1;
			}
			break;
		case 11:
			yjwrite_com(0x80+0x40);
			yjwrite_date('R');yjwrite_date('_');yjwrite_date('l');yjwrite_date('o');
			yjwrite_date('n');yjwrite_date('g');yjwrite_date(':');yjwrite_date('_');
			yjwrite_date('_');yjwrite_date(' ');
			if(k==2)
			{
				receive_length++;
				if(receive_length==9)receive_length=1;
			}
			if(k==3)
			{
				receive_length--;
				if(receive_length==0)receive_length=8;
			}
			yjwrite_com(0x80+0x40+7);
			yjwrite_date('0'+receive_length/100);
			yjwrite_date('0'+receive_length/10%10);
			yjwrite_date('0'+receive_length%10);
			break;
	}
}

/*----------------------------
    UART1��ʼ��
-----------------------------*/
void InitUART(void)
{
    SCON = 0x50;                //8λ�ɱ䲨����	
	AUXR1= AUXR1 & 0x3F; 
    AUXR = 0x40;                //��ʱ��1Ϊ1Tģʽ
    TMOD = 0x20;                //��ʱ��1Ϊģʽ2(8λ�Զ�����)
    TL1 = 0xDC;   				//���ò�������װֵ
    TH1 = 0xDC;
    TR1 = 1;                    //��ʱ��1��ʼ����
    ES = 1;                     //ʹ�ܴ����ж�
    EA = 1;

}





void main()
{
	init_io();		//24L01init
	sta = SPI_Read(STATUS);	  // ��״̬�Ĵ���
	SPI_RW_Reg(WRITE_REG + STATUS, sta|0x70);
	yjinit();  //1602init
	InitUART();
	EA=1;
	EX0=1;	 	//�ⲿ�ж�0����
	IT0=1;	   	//�½��ش���

	RX_Mode();
	while(1)
	{
		uchar i,j;
		CheckButtons();
		yjwrite_com(0x80+5);
		yjwrite_date(table[abc/100]);
		yjwrite_date(table[abc/10%10]);
		yjwrite_date(table[abc%10]);
		if(ok)
		{
			for(i=0;i<32;i++)
			{
				TxBuf[i]=ziku1[abc*32+i];	
			} 
			abc++; 
			TX_Mode(TxBuf);	delayms(1000);
		    Check_ACK(1);
			delayms(1000);
			if(abc==length)
			ok=0;
			x=0;
		}
		RX_Mode();
		yjwrite_com(0x80);
		yjwrite_date(table[menu/100]);  
		yjwrite_date(table[menu/10%10]);
		yjwrite_date(table[menu%10]);
		if(flag_end)
		{
			for(i=0;i<receive_length;i++)
			{
				for(j=0;j<32;j++)
				{
					TxBuf[j]=ckou[j+32*i];
				}
				TX_Mode(TxBuf);	delayms(1000);
			    Check_ACK(1);
				delayms(1000);
			}
			yjwrite_com(0x80+0x40+13);
			yjwrite_date(' ');yjwrite_date(' ');yjwrite_date(' ');yjwrite_date(' ');
			flag_end=0;	
			yjwrite_com(0x80+9);
			yjwrite_date('0'+TxBuf[0]/16);	
			yjwrite_date('0'+TxBuf[0]%16);
			yjwrite_date('0'+TxBuf[1]/16);
			yjwrite_date('0'+TxBuf[1]%16);
			yjwrite_date('0'+TxBuf[2]/16);
			yjwrite_date('0'+TxBuf[2]%16);
		}
	}	
}
void int0()interrupt 0
{
	EX0=0;
	sta = SPI_Read(STATUS);	  // ��״̬�Ĵ���
	SPI_RW_Reg(WRITE_REG + STATUS, sta|0x70);
	RX_DR=sta>>6&0x01;
	if(RX_DR)
	{ 
		SPI_Read_Buf(RD_RX_PLOAD, RxBuf, TX_PLOAD_WIDTH);
		SPI_RW_Reg(WRITE_REG + STATUS, sta);
		if(RxBuf[2]==0x55)x=1;
	}
	EX0=1;
}
/*----------------------------
UART �жϷ������
-----------------------------*/
void Uart() interrupt 4 using 1
{
	static i=0;
    if (RI)
    {
        RI = 0;                 //���RIλ
        ckou[i] = SBUF;              //P0��ʾ��������
		i++;
		if(i==receive_length*32)
		{
			yjwrite_com(0x80+0x40+13);
			yjwrite_date('0'+receive_length/100);	
			yjwrite_date('0'+receive_length/10%10);
			yjwrite_date('0'+receive_length%10);
			flag_end=1;
			i=0;
		}
    }
}