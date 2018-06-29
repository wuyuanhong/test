#include "def.h"

typedef struct
{
	unsigned short FSMC_INDEX;
	unsigned short FSMC_DATA;
}FSMC_TypeDef;

//stm32会右移动一位，即0x60000010变成实际的0x60000008
#define DM9000A_NE1_BASE        ((unsigned int)(0x61000000|0x7f))//表示a7
#define DM9000A_NE1             ((FSMC_TypeDef*)DM9000A_NE1_BASE)


// #define DM9000INDEX   *(volatile unsigned short *)(0x61000000)
// #define DM9000DATA  *(volatile unsigned short *)(0x61000100)


#define DM9000INDEX1   *(volatile unsigned short *)(0x61000000)
#define DM9000DATA1 *(volatile unsigned short *)(0x61000100)

#define DM9000INDEX2   *(volatile unsigned short *)(0x65000000)
#define DM9000DATA2  *(volatile unsigned short *)(0x65000100)

#define DM9000INDEX3_1   *(volatile unsigned short *)(0x69000002)
#define DM9000DATA3_1  *(volatile unsigned short *)(0x69000102)

#define DM9000INDEX3_2   *(volatile unsigned short *)(0x69000004)
#define DM9000DATA3_2  *(volatile unsigned short *)(0x69000104)

#define DM9000INDEX3_3   *(volatile unsigned short *)(0x69000006)
#define DM9000DATA3_3  *(volatile unsigned short *)(0x69000106)

// #define DM9000INDEX  DM9000A_NE1->FSMC_INDEX
// #define DM9000DATA   DM9000A_NE1->FSMC_DATA


// #define DM9000_DATA DM9000A_NE1->FSMC_RAM
// #define DM9000_IO 	DM9000A_NE1->FSMC_REG

#define USHORT	unsigned short

#define DM9000_DATA 0x61000100
#define DM9000_IO 	0x61000000

#define DM9000_outw(d,r) ( *(volatile unsigned short *)r = d)

#define DM9000_inb(r) (*(volatile unsigned char *)r)
#define DM9000_inw(r) (*(volatile unsigned short *)r)

#define DM9000_VID_L 0x28
#define DM9000_VID_H 0x29
#define DM9000_PID_L 0x2A
#define DM9000_PID_H 0x2B
#define DM9000_ID 0x90000A46
#define DM9000_int32_MII 0x00
#define DM9000_10MHD 0x1
#define DM9000_10MFD 0x2
#define DM9000_100MHD 0x3
#define DM9000_100MFD 0x4
#define LOOP_MAC 0x1
#define LOOP_PHY100M 0x2

#define DM9000_NCR             0x00

#define DM9000_NCR             0x00
#define DM9000_NSR             0x01
#define DM9000_TCR             0x02
#define DM9000_TSR1            0x03
#define DM9000_TSR2            0x04
#define DM9000_RCR             0x05
#define DM9000_RSR             0x06
#define DM9000_ROCR            0x07
#define DM9000_BPTR            0x08
#define DM9000_FCTR            0x09
#define DM9000_FCR             0x0A
#define DM9000_EPCR            0x0B
#define DM9000_EPAR            0x0C
#define DM9000_EPDRL           0x0D
#define DM9000_EPDRH           0x0E
#define DM9000_WCR             0x0F

#define DM9000_PAR             0x10
#define DM9000_MAR             0x16

#define DM9000_GPCR			       0x1e
#define DM9000_GPR			 			 0x1f
#define DM9000_TRPAL           0x22
#define DM9000_TRPAH           0x23
#define DM9000_RWPAL           0x24
#define DM9000_RWPAH           0x25

#define DM9000_VIDL            0x28
#define DM9000_VIDH            0x29
#define DM9000_PIDL            0x2A
#define DM9000_PIDH            0x2B

#define DM9000_CHIPR           0x2C
#define DM9000_SMCR            0x2F

#define DM9000_MRCMDX          0xF0
#define DM9000_MRCMD           0xF2
#define DM9000_MRRL            0xF4
#define DM9000_MRRH            0xF5
#define DM9000_MWCMDX          0xF6
#define DM9000_MWCMD           0xF8
#define DM9000_MWRL            0xFA
#define DM9000_MWRH            0xFB
#define DM9000_TXPLL           0xFC
#define DM9000_TXPLH           0xFD
#define DM9000_ISR             0xFE
#define DM9000_IMR             0xFF

//========== DM9000 ??? =====================================================

#define DM9000_NCR             0x00  //Network Control Register
#define NCR_EXT_PHY            (1<<7)
#define NCR_WAKEEN             (1<<6)
#define NCR_FCOL               (1<<4)
#define NCR_FDX                (1<<3)
#define NCR_LBK_1              (1<<2)
#define NCR_LBK_0              (1<<1)
#define NCR_RST                0x01

// 7:  0, RW, PHY???1????PHY,0????PHY,??????????
// 6:  0, RW, ????????0???WCR(0FH)bit0~2,??????????
// 5:  0, RO, ??
// 4:  0, RW, Force Collision Mode, used for testing
// 3:  0, RW, ??????????PHY???,??PHY????
// 2: 00, RW, ?????
// 1:         00: ????(???)?
//            01: DM9000 MAC ??????
//            10: DM9000 ?? PHY 100M ???????
//            11: (Reserved)
// 0:  0, RW, ????,10us??????

#define DM9000_NSR             0x01
#define NSR_SPEED              (1<<7)
#define NSR_LINKST             (1<<6)
#define NSR_WAKEST             (1<<5)
#define NSR_TX2END             (1<<3)
#define NSR_TX1END             (1<<2)
#define NSR_RXOV               (1<<1)

#define DM9000_TCR             0x02
#define TCR_TJDIS              (1<<6)
#define TCR_EXCECM             (1<<5)
#define TCR_PAD_DIS2           (1<<4)
#define TCR_CRC_DIS2           (1<<3)
#define TCR_PAD_DIS1           (1<<2)
#define TCR_CRC_DIS1           (1<<1)
#define TCR_TXREQ              (1<<0)

#define DM9000_TSR1            0x03
#define DM9000_TSR2            0x04
#define TSR_TJTO               (1<<7)
#define TSR_LC                 (1<<6)
#define TSR_NC                 (1<<5)
#define TSR_LCOL               (1<<4)
#define TSR_COL                (1<<3)
#define TSR_EC                 (1<<2)

#define DM9000_RCR             0x05
#define RCR_WTDIS              (1<<6)
#define RCR_DIS_LONG           (1<<5)
#define RCR_DIS_CRC            (1<<4)
#define RCR_ALL                (1<<3)
#define RCR_RUNT               (1<<2)
#define RCR_PRMSC              (1<<1)
#define RCR_RXEN               (1<<0)

#define DM9000_RSR             0x06
#define DM9000_ROCR            0x07
#define DM9000_BPTR            0x08
#define DM9000_FCTR            0x09
#define DM9000_FCR             0x0A
#define DM9000_EPCR            0x0B
#define DM9000_EPAR            0x0C
#define DM9000_EPDRL           0x0D
#define DM9000_EPDRH           0x0E
#define DM9000_WCR             0x0F

#define DM9000_PAR             0x10
#define DM9000_MAR             0x16

#define DM9000_GPCR            0x1e
#define DM9000_GPR             0x1f

#define DM9000_TRPAL           0x22
#define DM9000_TRPAH           0x23
#define DM9000_RWPAL           0x24
#define DM9000_RWPAH           0x25

#define DM9000_VIDL            0x28
#define DM9000_VIDH            0x29
#define DM9000_PIDL            0x2A
#define DM9000_PIDH            0x2B
#define DM9000_CHIPR           0x2C
#define DM9000_SMCR            0x2F

#define DM9000_MRCMDX          0xF0
#define DM9000_MRCMD           0xF2
#define DM9000_MRRL            0xF4
#define DM9000_MRRH            0xF5
#define DM9000_MWCMDX          0xF6
#define DM9000_MWCMD           0xF8
#define DM9000_MWRL            0xFA
#define DM9000_MWRH            0xFB
#define DM9000_TXPLL           0xFC
#define DM9000_TXPLH           0xFD

#define DM9000_ISR             0xFE
#define ISR_ROOS               (1<<3)
#define ISR_ROS                (1<<2)
#define ISR_PTS                (1<<1)
#define ISR_PRS                (1<<0)
#define ISR_CLR_STATUS         (ISR_ROOS | ISR_ROS | ISR_PTS | ISR_PRS)

#define DM9000_IMR             0xFF
#define IMR_PAR                (1<<7)
#define IMR_ROOM               (1<<3)
#define IMR_ROM                (1<<2)
#define IMR_PTM                (1<<1)
#define IMR_PRM                (1<<0)




extern unsigned short * DM9000INDEX;
extern unsigned short * DM9000DATA;


void SendPackage(U8 *datas,U16 length);
void DMA9000AE_INIT(void);
USHORT DM9000_ior(USHORT offset);
void DM9000_iow (USHORT offset, USHORT dat);
void DM9000A_STM32(void);
void DelayMs(int a);
