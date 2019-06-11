#include<reg52.h>
#include"24L01.h"
#include"ziku.h"
#include"24c64.h"
extern uchar TxBuf[5];	  
extern uchar RxBuf[TX_PLOAD_WIDTH];
extern uchar sta;
extern uchar MAX_RT;//�ط���������жϣ�д1����жϣ����MAX_RT�����жϣ����������ϵͳ���ܽ���ͨ
extern uchar RX_DR;//���������жϣ����յ���Ч���ݺ���1��д1����ж�
extern uchar  TX_DS;//���ݷ�������жϣ�д1����ж� 
uchar a;
uchar xdata xiancun1[96];
uchar xdata xiancun2[96];
uint num2;
uchar b;		//�����жϱ�־���ж�1��
uchar stop=1;	//��ʾ�ƶ�������ͣ�ƶ���־λ
uchar wide=4;	//�����ȱ�־
uchar speed=2;	//�����ƶ��ٶ�
uchar up=50;	//��̬ɨ������
uchar down=0;	//��̬ɨ������
void yanshi(uint x)
{
	uint i,j;
	for(i=x;i>0;i--)
		for(j=3;j>0;j--);
}  
void main()
{
	uchar i,j=0,k=0,num=6;
	uchar cycle;	//��ת��Ȧ����־
	uchar c;		//���յ����������ͱ�־
	uchar d=0;		//��������ʾʱ����ʼ��ʾ���ֵ�λ��
	init_24L01();		//24L01init
	AT24C64_init();

	EA=1;
	EX0=0;
	EX1=1;
	IT0=1;
	IT1=1;
	P0=0xff;				 
	P1=0xff;
/*	for(i=0;i<32;i++)
	{
		RxBuf[i]=0xaa;
	}
	for(i=0;i<255;i++)
	{
		page_write(i*32,RxBuf);
		delayms(5);
		P0=~P0;
	}  	
	P0=0xff;  */
	for(i=0;i<32;i++)
	{
		RxBuf[i]=dian[i];
	} 
	for(i=0;i<5;i++)
	{
		TxBuf[i]=0x55;
	}
//	RX_Mode(); 
	TX_Mode(TxBuf);		
	Check_ACK(1);	
	delayms(1);
//	sta = SPI_Read(STATUS);	  // ��״̬�Ĵ���
//	SPI_RW_Reg(WRITE_REG + STATUS, sta|0x70);	   	//���״̬�Ĵ���
	for(i=0;i<6;i++)
	{
		sequential_read(32*i,16,xiancun1+16*i);
		sequential_read(32*i+16,16,xiancun2+16*i);
	}	   
	while(1)
	{ 
		RX_Mode();
		if(b)
		{
			EX0=0;
			SPI_Read_Buf(RD_RX_PLOAD, RxBuf, TX_PLOAD_WIDTH);
			if(RxBuf[0]==0xaa&&RxBuf[1]==0x55)	//����յ�����0xaa,0x55����ʾ�յ��������
			{
				EA=0;
				c=RxBuf[2];
				switch(c)
				{
					case 1:	   		//�����ʾ����
						for(i=0;i<32;i++)
						RxBuf[i]=0;
						for(i=0;i<255;i++)
						{
							page_write(i*32,RxBuf);
							delayms(5);
							P0=~P0;
						}	
						break;
					case 2:stop=!stop;break;
					case 3:					 //����������
						wide++;
						if(wide==10)wide=4;
						break;
					case 4:					  //�����ȼ�С
						wide--;
						if(wide==0)wide=4;
						break;
					case 5:				  		//�����ƶ��ٶ�����
						speed--;
						if(speed==-1)speed=2;
						break;				  //�����ƶ��ٶȱ���
					case 6:
						speed++;
						if(speed==10)speed=2;
						break;	
					case 7:				   	//��������ʾʱ��ʾ��ʼλ��ȷ��
						d+=6;
						if(d>250)d=0;
						num=d+7;
						for(i=0;i<6;i++)
						{
							sequential_read(32*(i+d),16,xiancun1+16*i);
							sequential_read(32*(i+d)+16,16,xiancun2+16*i);
						}
						break;		
					case 8:					  //��������ʾʱ��ʾ��ʼλ��ȷ��
						d-=6;
						if(d<0)d=250;
						num=d+7;
						for(i=0;i<6;i++)
						{
							sequential_read(32*(i+d),16,xiancun1+16*i);
							sequential_read(32*(i+d)+16,16,xiancun2+16*i);
						}
						break;	
					case 9:				 //���յ���������24C64�д洢�Ŀ�ʼλ��
						num2=RxBuf[3];
						break;
					case 10:			 //��̬ɨ�跶Χ����
						up=RxBuf[3];
						break;
					case 11:			 //��̬ɨ�跶Χ����
						down=RxBuf[3];
						break;						
				}
				EA=1;
			}									//����յ��Ĳ���������ʾ�յ���������
			else 
			{								//�����24c64
				page_write(num2,RxBuf);
				num2+=32;
				TxBuf[0]=num2/32;
				TxBuf[1]=num2/32;
				TxBuf[2]=0x55;		  	//����0x55��ʾ���յ��ˣ���֪ͨ����Ҫ����һ֡����
				SPI_RW_Reg(WRITE_REG + STATUS, sta|0x70);
				TX_Mode(TxBuf);
				Check_ACK(1);
			}	
		}	
		EX0=1;
		a=1;	
		if(b)a=0;
		b=0;
		while(a);
		for(i=0;i<96;i++)
		{
			P0=~xiancun1[i];
			P1=~xiancun2[i];
			yanshi(wide);
		}
		P0=0xff;
		P1=0xff; 	
		cycle++;
		if(cycle>=speed&&stop)
		{
			cycle=0;
			for(i=0;i<95;i++)
			{
				xiancun1[i]=xiancun1[i+1];
				xiancun2[i]=xiancun2[i+1];
			}
			xiancun1[95]=read_add(32*num+k);				//���µ��������ֵĵ�16��
			xiancun2[95]=read_add(32*num+k+16);
			k++;
			if(k==16)
			{
				k=0;
				num++;
				if(num>=up)
				num=down;
			}
		}
				  
	}
}
void int0() interrupt 0
{
	a=0;
	EX0=0;
}
void int1() interrupt 2
{
	sta = SPI_Read(STATUS);	  // ��״̬�Ĵ���
	RX_DR=sta>>6&0x01;
	if(RX_DR)
	{
		EX0=0;
		a=0;				//��ֹ���յ��ź��жϺ�a=1��ʹ����ֹͣ
		b=1;				//��ʾ���յ�����
	}
	SPI_RW_Reg(WRITE_REG + STATUS, sta|0x70);		
}