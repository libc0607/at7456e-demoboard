//*************************************************
//
// AT7456(E) ��д���Ƴ��� --- AT7456.H
// �����п�΢�������޹�˾
// 2017-03-08
// 0571-28918103 ϵͳ��
//
//*************************************************

#ifndef _AT7456_H_
#define _AT7456_H_

// at7456
#define VM0 0x00 // Video Mode0
#define VM1 0x01 // Video Mode1
#define HOS 0x02 // Horizontal Offset
#define VOS 0x03 // Vertical Offset
#define DMM 0x04 // Display Memory Mode
#define DMAH 0x05 // Display Memory Address High
#define DMAL 0x06 // Display Memory Address Low
#define DMDI 0x07 // Display Memory Data In
#define CMM 0x08 // Character Memory Mode
#define CMAH 0x09 // Character Memory Address High
#define CMAL 0x0a // Character Memory Address Low
#define CMDI 0x0b // Character Memory Data In
#define OSDM 0x0c // OSD Insertion Mux
#define RB0 0x10 // Row 0 Brightness
#define RB1 0x11 // Row 1 Brightness
#define RB2 0x12 // Row 2 Brightness
#define RB3 0x13 // Row 3 Brightness
#define RB4 0x14 // Row 4 Brightness
#define RB5 0x15 // Row 5 Brightness
#define RB6 0x16 // Row 6 Brightness
#define RB7 0x17 // Row 7 Brightness
#define RB8 0x18 // Row 8 Brightness
#define RB9 0x19 // Row 9 Brightness
#define RB10 0x1a // Row 10 Brightness
#define RB11 0x1b // Row 11 Brightness
#define RB12 0x1c // Row 12 Brightness
#define RB13 0x1d // Row 13 Brightness
#define RB14 0x1e // Row 14 Brightness
#define RB15 0x1f // Row 15 Brightness
#define OSDBL 0x6c // OSD Black Level
#define STAT 0x20 // Status
#define DMDO 0x30 // Display Memory Data Out
#define CMDO 0x40 // Character Memory Data Out
#define NVM_RAM 0x50 // �� NVM �е��ֿ��ȡ������ RAM ��
#define RAM_NVM 0xa0 // ������ RAM �е��ֿ�����д�� NVM ��

// VM0
#define NTSC (0 << 6) // D6 --- Vidoe Standard Select
#define PAL (1 << 6)
#define SYNC_AUTO (0 << 4) // D5,D4 --- Sync Select Mode
#define SYNC_EXTERNAL (2 << 4)
#define SYNC_INTERNAL (3 << 4)
#define OSD_ENABLE (1 << 3) // D3 --- Enable Display of OSD image
#define OSD_DISABLE (0 << 3)
#define SOFT_RESET (1 << 1) // D1 --- Software Reset
#define VOUT_ENABLE (0 << 0)
#define VOUT_DISABLE (1 << 0)

// VM1
#define BACKGND_0 (0 << 4) // ������ƽ WHT%
#define BACKGND_7 (1 << 4)
#define BACKGND_14 (2 << 4)
#define BACKGND_21 (3 << 4)
#define BACKGND_28 (4 << 4)
#define BACKGND_35 (5 << 4)
#define BACKGND_42 (6 << 4)
#define BACKGND_49 (7 << 4)
#define BLINK_TIME40 (0 << 2) // ��˸����, ms
#define BLINK_TIME80 (1 << 2)
#define BLINK_TIME120 (2 << 2)
#define BLINK_TIME160 (3 << 2)

// ��˸ռ�ձ�(ON : OFF)
#define BLINK_DUTY_1_1 0 // BT : BT
#define BLINK_DUTY_1_2 1 // BT : 2BT
#define BLINK_DUTY_1_3 2 // BT : 3BT
#define BLINK_DUTY_3_1 3 // 3BT : BT

// DMM
#define SPI_BIT16 (0 << 6) // д�ַ�ʱ���� 16bit ��ʽ���ַ��������� DMM[5:3]
#define SPI_BIT8 (1 << 6) // д�ַ�ʱ�� 8bit ��ʽ���ַ����Ե���д��
#define CHAR_LBC (1 << 5) // ���ر���
#define CHAR_BLK (1 << 4) // ��˸��ʾ
#define CHAR_INV (1 << 3) // ������ʾ
#define CLEAR_SRAM (1 << 2) // ������20us
#define VETICAL_SYNC (1 << 1) // �����ڳ�ͬ����������
#define AUTO_INC (1 << 0) // �ַ���ַ�Զ�����

// RBi
#define BLACK_LEVEL_0 (0 << 2) // 0% �׵�ƽ
#define BLACK_LEVEL_10 (1 << 2) // 10% �׵�ƽ
#define BLACK_LEVEL_20 (2 << 2) // 20% �׵�ƽ
#define BLACK_LEVEL_30 (3 << 2) // 30% �׵�ƽ
#define WHITE_LEVEL_120 (0 << 0) // 120% �׵�ƽ
#define WHITE_LEVEL_100 (1 << 0) // 110% �׵�ƽ
#define WHITE_LEVEL_90 (2 << 0) // 90% �׵�ƽ
#define WHITE_LEVEL_80 (3 << 0) // 80% �׵�ƽ

// STAT
#define PAL_DETECT (1 << 0) // ��⵽ PAL �ź�
#define NTSC_DETECT (1 << 1) // ��⵽ NTSC �ź�
#define LOS_DETECT (1 << 2) // ��⵽ LOS �ź�
#define VSYNC (1 << 4) // ��ͬ��

typedef enum
{
	OLD7456,
	NEW7456,
	BAD7456
}VER7456;

typedef enum
{
	MODE_PAL,
	MODE_NTSC,
	MODE_ERR
}VIDEO_MODE;

typedef struct
{
	unsigned short index; // �ֿ�������AT7456 = {0..255}��AT7456E = {0..511}
	unsigned char attrib; // ��ʾ���ԣ�D2 - LBC��D1 - BLK��D0 - INV
}CHAR_ATTRIB;


void DisplayChar(unsigned char row, unsigned char col, unsigned char c);
void DisplayNum(unsigned char row, unsigned char col, unsigned char c);
void DisplayString(unsigned char row, unsigned char col, unsigned char *s);


void at7456_reset(void);
void turnOff_osd(void);
void turnOn_osd(void);
void clear_screen(void);
VER7456 checkAT7456(void);
void configOSDBL(unsigned char autoLevelCtrl);
void write_at7456_font(unsigned char addr, unsigned char *dt);
void read_at7456_font(unsigned char addr, unsigned char *dt);
void wait_memory_wr_ok(void);
void write_at7456E_font(unsigned short addr, unsigned char *dt);
void read_at7456E_font(unsigned short addr, unsigned char *dt);
void at7456_read_char(unsigned char row, unsigned char col, CHAR_ATTRIB *ca);
void at7456_write_char(unsigned char row, unsigned char col, CHAR_ATTRIB *ca);
void at7456E_read_char(unsigned char row, unsigned char col, CHAR_ATTRIB *ca);
void at7456E_write_char(unsigned char row, unsigned char col, CHAR_ATTRIB *ca);
void write_at7456_addr_data(unsigned char addr, unsigned char dat);
unsigned char read_at7456_addr_data(unsigned char addr);
void write_at7456_data(unsigned char dat);
unsigned char read_at7456_data(void);

#endif