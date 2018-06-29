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
U8 rx_Buffer1[1500];//定义了一个100字节的接收缓冲

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
* 名    称:DM9000AEP软件复位
* 功    能: 
* 参    数: 
* 返 回 值: 
**************************************************************************************/
void dm9000_reset(void)
{
				DM9000A_STM32();
				DM9000_iow(DM9000_RCR,0x30);
        DM9000_iow(DM9000_GPCR, 0x01);//设置GPCR bit[0]=1,使DM9000的GPIO3为输出
        DM9000_iow(DM9000_GPR, 0x00);//GPR bit[0]=0;使DM9000的GPIO3输出为低以激活内部PHY
        DelayMs(3);//延时2ms以上等待PHY上电
        
        DM9000_iow(DM9000_NCR, 3);//软件复位
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
				DM9000_iow(DM9000_RCR,0x31);//打开接收，但是关闭广播包
}







void phy_write(unsigned char addr,unsigned short reg)
{
    int j;
		DM9000_iow(0x0c, addr|0x40);//前面是address 后面是数据	
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
	DM9000_iow(DM9000_IMR,0x81);//网卡的接收和发送中断使能
}
void dm9000a_disrx(void)
{
	DM9000_iow(DM9000_IMR,0x80);//网卡的接收关闭
}




void dm9000a_disablemultical(void)
{
	 DM9000_iow(DM9000_RCR,0x31);//关闭广播接收
}


/**************************************************************************************
* 名    称:DM9000AEP初始化函数
* 功    能: 
* 参    数: 
* 返 回 值: 
**************************************************************************************/
void DMA9000AE_INIT(void)
{
  U8 i;
	dm9000_reset();
  DelayMs(10);//延时2ms以上等待PHY上电  
	   
  //======= 清除中断设定==========
  DM9000_iow(DM9000_NSR,0x2C);//清除各种状态标识
  DM9000_iow(DM9000_ISR,0x3f);//清除中断标志位
  //=======设定DMA9000相关连接界面 ==========
  DM9000_iow(DM9000_RCR,0x30);//接收控制,关闭接收
  //		  DM9000_iow(DM9000_RSR,0x01);//接收控制

  DM9000_iow(DM9000_TCR,0x00);//发送控制
  DM9000_iow(DM9000_BPTR,0x3f);//Back pressure 条件设置
  DM9000_iow(DM9000_FCTR,0x8f);//Flow Control 条件设置
  DM9000_iow(DM9000_FCR,0xff);//流量控制设置
  DM9000_iow(DM9000_SMCR,0x00);//特殊模式控制
  //=======设定NOTE ADRESS位置 ==========
  

  for(i=0;i<6;i++)
    DM9000_iow(DM9000_PAR+i,MAC[i]);
	
	//=======設定 Multicast 設置==========

	//=======保险起见清除标识==========
	DM9000_iow(DM9000_NSR,0x2C);//清除各种状态标识
	DM9000_iow(DM9000_ISR,0x3f);//清除中断标志位
	//=======開啟中断==========
	DelayMs(20);									//等待一会不然读取寄存器有几个不正常
	DM9000_iow(DM9000_IMR,0x8d); 	//打开接收，但是关闭广播包
}


//void DMA9000AE_FX_INIT(void)
//{
//  U8 i;
//  U16 TMP;
//  //======= 软件复位==========
//	//phy设置将dm9000a设置为fx模式
//	
//// 	
//  phy_write( 00, 0x8000);
//  DM9000_iow(DM9000_GPCR, 0x01);//设置GPCR bit[0]=1,使DM9000的GPIO3为输出
//	DM9000_iow(DM9000_GPR, 0x00);//GPR bit[0]=0;使DM9000的GPIO3输出为低以激活内部PHY
//// 	phy_write( 00, 0xa100);
//  DelayMs(20);//延时2ms以上等待PHY上电

//	DM9000_iow(DM9000_NCR,3);//软件复位
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
//  DelayMs(10);//延时2ms以上等待PHY上电  
////   phy_write(0x0a, 0x29);
//  
//  //======= 清除中断设定==========
//  DM9000_iow(DM9000_NSR,0x2C);//清除各种状态标识
//  DM9000_iow(DM9000_ISR,0x3f);//清除中断标志位
//  //=======设定DMA9000相关连接界面 ==========
//  DM9000_iow(DM9000_RCR,0x30);//接收控制,关闭接收
//  //		  DM9000_iow(DM9000_RSR,0x01);//接收控制

//  DM9000_iow(DM9000_TCR,0x00);//发送控制
//  DM9000_iow(DM9000_BPTR,0x3f);//Back pressure 条件设置
//  DM9000_iow(DM9000_FCTR,0x8f);//Flow Control 条件设置
//// 	printf("bptr is %x ,%x \n" ,DM9000_ior(DM9000_BPTR),DM9000_ior(DM9000_FCTR));//Back pressure 条件设置
//  DM9000_iow(DM9000_FCTR,0x8f);//Flow Control 条件设置
//	
////   DM9000_iow(DM9000_FCR,0xfe);//流量控制设置
//  DM9000_iow(DM9000_FCR,0x29);//流量控制设置
//  DM9000_iow(DM9000_SMCR,0x00);//特殊模式控制
//  //=======设定NOTE ADRESS位置 ==========

// 	for(i=0;i<8;i++)
//		DM9000_iow(DM9000_MAR+i,MAC[i]);
//		
//		
//		
//		
//		
//	//=======設定 Multicast 設置==========

//	//=======保险起见清除标识==========
//	DM9000_iow(DM9000_NSR,0x2C);//清除各种状态标识
//	DM9000_iow(DM9000_ISR,0x3f);//清除中断标志位
//	//=======開啟中断==========
//	DelayMs(50);//等待一会不然读取寄存器有几个不正常
//	DM9000_iow(DM9000_IMR,0x8d);
//  if (DM9000_ior(DM9000_NSR) & 0X40)
//  {
//    if (DM9000_ior(DM9000_NSR) & 0X80)
//      printf("网络已连接，速度10M!\r\n");    
//    else
//      printf("网络已连接，速度100M!\r\n");
//  }
//  else
//    printf("网络未连接!\r\n"); 

//}



//stm32接收网口数据中断程序

void STM32RJ45_IRQ(void)//stm32自己的rj45中断
{
    if(EXTI_GetITStatus(EXTI_Line2) != RESET)
    {
       
			
    }
}
//接收udp数据
U8 ReceivePackage(U8 *datas)
{

	U16 i,status,n,len1,k;
	U16 ready,ready1,read_addr_h,read_addr_l;
  U8 io_mode;
	//===有效数据包的前四个状态字节
	ready=0;//希望读取到0x01
	status=0;//数据包状态

  io_mode=DM9000_ior(0xfe)>>7;
	//=====清除接收中断标志
//  	if(DM9000_ior(DM9000_ISR)&0x01)
// 	{
// 		DM9000_iow(DM9000_ISR,0x01);  
// 	} 
	//================检查是否有数据包======================================= 
	read_addr_h=DM9000_ior(0XF4);
  read_addr_l=DM9000_ior(0XF5);
  
  ready1=DM9000_ior(DM9000_MRCMDX);//读取第一个字节
// 	ready=DM9000_inw(DM9000_DATA);//不知道为什么需要重复读取一次才能读到nnd
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
      status=DM9000_ior(DM9000_MRCMD);//读取第二个字节
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
//     DM9000_iow(DM9000_RCR,0x39);//打开接收，但是关闭广播包   	
//     printf("0\n"); 
//     printf("%x\n",DM9000_ior(DM9000_NSR));   
    return 0;
  }
//   status = DM9000_ior(DM9000_MRCMDX);
  return 1;
}

//接收网络报文数据
U8 ReceivePackage1(U8 *datas)
{


}