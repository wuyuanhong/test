#include "stm32f4xx.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>
#include "dm9000a.h"
#include "def.h"


unsigned short * DM9000INDEX;
unsigned short * DM9000DATA;

const unsigned char MAC[6]= {0xe0, 0xcb, 0x4e, 0x8e, 0x2c, 0x53};

char RBuffer[100];
U8 rx_Buffer1[1500];//������һ��100�ֽڵĽ��ջ���

char * dm9000a_s;
volatile	U16 tem;
volatile U16 len;
volatile U16 ok_num=0;
volatile U16 num_61850=0;


USHORT DM9000_ior(USHORT offset)
{
	USHORT regist,j;
	*DM9000INDEX=offset;
  for(j=0;j<4;j++);
	regist=*DM9000DATA;
	return regist;
}

void DelayMs(int a)
{
	int k,j;
	for(k=0;k<a;k++)
	{
		for(j=0;j<35000;j++);
	}

}


void DelayUs(int a)
{
	int k,j;
	for(k=0;k<a;k++)
	{
		for(j=0;j<32;j++);
	}

}
void DM9000_iow (USHORT offset, USHORT dat)
{
   USHORT j;
	*DM9000INDEX=offset;
  for(j=0;j<3;j++);
	*DM9000DATA=dat;
}

//void DM9000A_ATZ(void)
//{
//    dm9000a_s="DM9000A_ATZ";  
//    DM9000INDEX=(unsigned short *)(0x6100000A);
//    DM9000DATA=(unsigned short *)(0x6100010A);  
//}

void DM9000A_STM32(void)
{
    dm9000a_s="DM9000A_STM32"; 
    DM9000INDEX=(unsigned short *)(0x69000006);
    DM9000DATA=(unsigned short *)(0x69000106);  
}
/**************************************************************************************
* ��    ��:DM9000AEP�����λ
* ��    ��: 
* ��    ��: 
* �� �� ֵ: 
**************************************************************************************/
void dm9000_reset(void)
{
				DM9000A_STM32();
				DM9000_iow(DM9000_RCR,0x30);
        DM9000_iow(DM9000_GPCR, 0x01);//����GPCR bit[0]=1,ʹDM9000��GPIO3Ϊ���
        DM9000_iow(DM9000_GPR, 0x00);//GPR bit[0]=0;ʹDM9000��GPIO3���Ϊ���Լ����ڲ�PHY
        DelayMs(3);//��ʱ2ms���ϵȴ�PHY�ϵ�
        
        DM9000_iow(DM9000_NCR, 3);//�����λ
           do {
                DelayUs(25); /* Wait at least 20 us */
        } while (DM9000_ior(DM9000_NCR) & 1);
        DM9000_iow(DM9000_NCR, 0);
        
        DM9000_iow(DM9000_NCR, 3); /* Issue a second reset */
        do {
//                Uart1_Printf("resetting the DM9000, 2nd reset\n");
                DelayUs(25); /* Wait at least 20 us */
        } while (DM9000_ior(DM9000_NCR) & 1);
        DM9000_iow(DM9000_NCR, 0);
    
				
        /* Check whether the ethernet controller is present */
        if ((DM9000_ior(DM9000_PIDL) != 0x0) ||(DM9000_ior(DM9000_PIDH) != 0x90))
           printf("Reset DMA9000AEP Failure \n");
        else
           printf("Reset DMA9000AEP OK!\n");
        DM9000_iow(DM9000_NCR, 0);
				DM9000_iow(DM9000_RCR,0x31);//�򿪽��գ����ǹرչ㲥��
}







void phy_write(unsigned char addr,unsigned short reg)
{
    int j;
		DM9000_iow(0x0c, addr|0x40);//ǰ����address ����������	
    for(j=0;j<3;j++);
		DM9000_iow(0xd, (reg & 0xff));
    for(j=0;j<3;j++);
    DM9000_iow(0xe, ( (reg >> 8) & 0xff));
    for(j=0;j<3;j++);
    DM9000_iow(0xb, 0xa);
    for(j=0;j<3;j++);
    DelayUs(1000);
    DM9000_iow(0xb, 0x0);	
    for(j=0;j<3;j++);
}

unsigned short phy_read(int reg)
{
	DM9000_iow( 0xc, 0x40 | reg);
	DM9000_iow( 0xb, 0xc); 	
	DelayUs(1000);		
	DM9000_iow(0xb, 0x0); 
	return ( DM9000_ior( 0xe) << 8 ) | DM9000_ior( 0xd);
}



void dm9000a_enrxtx(void)
{
	DM9000_iow(DM9000_IMR,0x81);//�����Ľ��պͷ����ж�ʹ��
}
void dm9000a_disrx(void)
{
	DM9000_iow(DM9000_IMR,0x80);//�����Ľ��չر�
}




void dm9000a_disablemultical(void)
{
	 DM9000_iow(DM9000_RCR,0x31);//�رչ㲥����
}


/**************************************************************************************
* ��    ��:DM9000AEP��ʼ������
* ��    ��: 
* ��    ��: 
* �� �� ֵ: 
**************************************************************************************/
void DMA9000AE_INIT(void)
{
  U8 i;
	dm9000_reset();
  DelayMs(10);//��ʱ2ms���ϵȴ�PHY�ϵ�  
	   
  //======= ����ж��趨==========
  DM9000_iow(DM9000_NSR,0x2C);//�������״̬��ʶ
  DM9000_iow(DM9000_ISR,0x3f);//����жϱ�־λ
  //=======�趨DMA9000������ӽ��� ==========
  DM9000_iow(DM9000_RCR,0x30);//���տ���,�رս���
  //		  DM9000_iow(DM9000_RSR,0x01);//���տ���

  DM9000_iow(DM9000_TCR,0x00);//���Ϳ���
  DM9000_iow(DM9000_BPTR,0x3f);//Back pressure ��������
  DM9000_iow(DM9000_FCTR,0x8f);//Flow Control ��������
  DM9000_iow(DM9000_FCR,0xff);//������������
  DM9000_iow(DM9000_SMCR,0x00);//����ģʽ����
  //=======�趨NOTE ADRESSλ�� ==========
  

  for(i=0;i<6;i++)
    DM9000_iow(DM9000_PAR+i,MAC[i]);
	
	//=======�O�� Multicast �O��==========

	//=======������������ʶ==========
	DM9000_iow(DM9000_NSR,0x2C);//�������״̬��ʶ
	DM9000_iow(DM9000_ISR,0x3f);//����жϱ�־λ
	//=======�_���ж�==========
	DelayMs(20);									//�ȴ�һ�᲻Ȼ��ȡ�Ĵ����м���������
	DM9000_iow(DM9000_IMR,0x8d); 	//�򿪽��գ����ǹرչ㲥��
}


//void DMA9000AE_FX_INIT(void)
//{
//  U8 i;
//  U16 TMP;
//  //======= �����λ==========
//	//phy���ý�dm9000a����Ϊfxģʽ
//	
//// 	
//  phy_write( 00, 0x8000);
//  DM9000_iow(DM9000_GPCR, 0x01);//����GPCR bit[0]=1,ʹDM9000��GPIO3Ϊ���
//	DM9000_iow(DM9000_GPR, 0x00);//GPR bit[0]=0;ʹDM9000��GPIO3���Ϊ���Լ����ڲ�PHY
//// 	phy_write( 00, 0xa100);
//  DelayMs(20);//��ʱ2ms���ϵȴ�PHY�ϵ�

//	DM9000_iow(DM9000_NCR,3);//�����λ
//	   do {
//	        DelayUs(200); /* Wait at least 20 us */
//	} while (DM9000_ior(DM9000_NCR) & 1);
//	DM9000_iow(DM9000_NCR, 0);
//  
//  TMP=DM9000_ior(DM9000_PIDL);
//  TMP=DM9000_ior(DM9000_PIDH);
//	/* Check whether the ethernet controller is present */
//	if ((DM9000_ior(DM9000_PIDL) != 0x0) ||(DM9000_ior(DM9000_PIDH) != 0x90))
//  {
//	   printf("Reset %s Failure \n",dm9000a_s);
//     printf("data is %x,%x\n",DM9000_ior(DM9000_PIDH),DM9000_ior(DM9000_PIDL));
//  }
//	else
//	    printf("Reset %s OK!\n",dm9000a_s);
//	
//	if(DM9000_ior(0xfe)&0x80)
//	{
//		printf("work on 8 bit_mode\n");
//	}
//	else
//	{
//		printf("work on 16 bit_mode\n");
//	}
//	   
//  phy_write( 16, 0x4004);
//  phy_write( 4, 0x400|0x1e1);  
//  phy_write( 0, 0x2100);
////   phy_write( 16, 0x4004);
////   phy_write( 4, 0x5e1);  
////   phy_write( 0, 0x2100);
////   phy_write(20, 0x4000);
////   TMP=phy_read(20);
////   phy_write( 4, 0x01e1);  
////   phy_write( 0, 0x1000);
//  DelayMs(10);//��ʱ2ms���ϵȴ�PHY�ϵ�  
////   phy_write(0x0a, 0x29);
//  
//  //======= ����ж��趨==========
//  DM9000_iow(DM9000_NSR,0x2C);//�������״̬��ʶ
//  DM9000_iow(DM9000_ISR,0x3f);//����жϱ�־λ
//  //=======�趨DMA9000������ӽ��� ==========
//  DM9000_iow(DM9000_RCR,0x30);//���տ���,�رս���
//  //		  DM9000_iow(DM9000_RSR,0x01);//���տ���

//  DM9000_iow(DM9000_TCR,0x00);//���Ϳ���
//  DM9000_iow(DM9000_BPTR,0x3f);//Back pressure ��������
//  DM9000_iow(DM9000_FCTR,0x8f);//Flow Control ��������
//// 	printf("bptr is %x ,%x \n" ,DM9000_ior(DM9000_BPTR),DM9000_ior(DM9000_FCTR));//Back pressure ��������
//  DM9000_iow(DM9000_FCTR,0x8f);//Flow Control ��������
//	
////   DM9000_iow(DM9000_FCR,0xfe);//������������
//  DM9000_iow(DM9000_FCR,0x29);//������������
//  DM9000_iow(DM9000_SMCR,0x00);//����ģʽ����
//  //=======�趨NOTE ADRESSλ�� ==========

// 	for(i=0;i<8;i++)
//		DM9000_iow(DM9000_MAR+i,MAC[i]);
//		
//		
//		
//		
//		
//	//=======�O�� Multicast �O��==========

//	//=======������������ʶ==========
//	DM9000_iow(DM9000_NSR,0x2C);//�������״̬��ʶ
//	DM9000_iow(DM9000_ISR,0x3f);//����жϱ�־λ
//	//=======�_���ж�==========
//	DelayMs(50);//�ȴ�һ�᲻Ȼ��ȡ�Ĵ����м���������
//	DM9000_iow(DM9000_IMR,0x8d);
//  if (DM9000_ior(DM9000_NSR) & 0X40)
//  {
//    if (DM9000_ior(DM9000_NSR) & 0X80)
//      printf("���������ӣ��ٶ�10M!\r\n");    
//    else
//      printf("���������ӣ��ٶ�100M!\r\n");
//  }
//  else
//    printf("����δ����!\r\n"); 

//}



//stm32�������������жϳ���

void STM32RJ45_IRQ(void)//stm32�Լ���rj45�ж�
{
    if(EXTI_GetITStatus(EXTI_Line2) != RESET)
    {
       
			
    }
}
//����udp����
U8 ReceivePackage(U8 *datas)
{

	U16 i,status,n,len1,k;
	U16 ready,ready1,read_addr_h,read_addr_l;
  U8 io_mode;
	//===��Ч���ݰ���ǰ�ĸ�״̬�ֽ�
	ready=0;//ϣ����ȡ��0x01
	status=0;//���ݰ�״̬

  io_mode=DM9000_ior(0xfe)>>7;
	//=====��������жϱ�־
//  	if(DM9000_ior(DM9000_ISR)&0x01)
// 	{
// 		DM9000_iow(DM9000_ISR,0x01);  
// 	} 
	//================����Ƿ������ݰ�======================================= 
	read_addr_h=DM9000_ior(0XF4);
  read_addr_l=DM9000_ior(0XF5);
  
  ready1=DM9000_ior(DM9000_MRCMDX);//��ȡ��һ���ֽ�
// 	ready=DM9000_inw(DM9000_DATA);//��֪��Ϊʲô��Ҫ�ظ���ȡһ�β��ܶ���nnd
 	ready=DM9000_ior(DM9000_MRCMDX);
//    Uart1_Printf("ready is %x\n",ready);

  *DM9000INDEX=DM9000_MRCMD;

  if((ready & 0x0ff) == 0x01)
  {
    if(1==io_mode)//8 BIT
    {
      status=DM9000_inb(DM9000DATA)+(DM9000_inb(DM9000DATA)<<8);
      len1=DM9000_inb(DM9000DATA)+(DM9000_inb(DM9000DATA)<<8);
      if(len1<1500)
      {
          for(i=0;i<len1;i++)//16 bit mode
          {
            tem=*DM9000DATA;
            datas[i]=tem&0xff;
          }   
          datas[i]=0;           
       }
      else
      {
           printf("*\n"); 
      }  
      
    }
    else if(0==io_mode)//16 BIT
    {
      status=DM9000_ior(DM9000_MRCMD);//��ȡ�ڶ����ֽ�
      len1=*DM9000DATA;    
//      rx61850len1_reg=len1;
      if(len1<1500)
      {
          for(i=0;i<len1;i+=2)//16 bit mode
          {
            tem=*DM9000DATA;
            datas[i]=tem&0xff;
            datas[i+1]=(tem/256)&0xff;   
          }   
           datas[i]=0;      
      }
      else
      {
           printf("*\n"); 
      }      
    }
  }
  else
  {
//     DMA9000AE_FX_INIT(); 
//     DM9000_iow(DM9000_RCR,0x39);//�򿪽��գ����ǹرչ㲥��   	
//     printf("0\n"); 
//     printf("%x\n",DM9000_ior(DM9000_NSR));   
    return 0;
  }
//   status = DM9000_ior(DM9000_MRCMDX);
  return 1;
}

//�������籨������
U8 ReceivePackage1(U8 *datas)
{


}