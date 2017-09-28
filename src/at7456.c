//*************************************************
//
// AT7456(E) SPI 接口读写程序 --- AT7456_SPI.C
// 杭州中科微电子有限公司
// 2016-12-22
// 0571-28918103 系统部
//
//*************************************************

//头文件
//#include <REG52.H>
#include "AT7456.H"
#include <intrins.h>

// AT7456(E)的 SPI 控制接口
sbit nCS = 		P3 ^ 7; 
sbit SCLK = 	P3 ^ 5; 
sbit SDIN = 	P3 ^ 6;
sbit SDOUT = 	P3 ^ 4;

// 根据需要添加合适的 nop 指令，用作延时，以满足 SPI 时钟数量不大于 10MHz
void delay_spi() 
{
	
}

// 向指定寄存器写 8 位数
void write_at7456_addr_data(unsigned char addr, unsigned char dat)
{ 
	unsigned char i;
	SCLK = 0;
	nCS = 0;
	delay_spi(); 
	
	// 写地址
	for (i=0; i<8; i++) { 
		SDIN = ((addr & 0x80) == 0)? 0: 1;
		addr = addr << 1;
		SCLK = 1;
		delay_spi(); 
		SCLK = 0;
	} 
	
	// 写数据
	for (i=0; i<8; i++) { 
		SDIN = ((dat & 0x80) == 0)? 0: 1;
		dat = dat << 1;
		SCLK = 1;
		delay_spi();
		SCLK = 0;
	} 
		nCS = 1;
		SCLK = 1;
		SDIN =1;
}


// 从指定寄存器读取 8 位数据
unsigned char read_at7456_addr_data(unsigned char addr) 
{
	unsigned char i, r; 
	SCLK = 0; 
	r = 0;
	nCS = 0;
	addr |= 0x80; // D7 = 1 为读寄存器
	
	for (i=0; i<8; i++) { // 写地址
		SDIN = ((addr & 0x80) == 0)? 0: 1;
		addr = addr << 1;
		SCLK = 1;
		delay_spi(); 
		SCLK = 0;
	} 
	for (i=0; i<8; i++) { // 读数据
		r = r << 1; 
		if (SDOUT) 
			r |= 1; 
		SCLK = 1; 
		delay_spi(); 
		SCLK = 0; 
	} 
	
	nCS = 1; 
	SCLK = 1; 
	SDIN = 1; 
	
	return r; 
}


void write_at7456_data(unsigned char dat)
{ 
	unsigned char i;
	SCLK = 0;
	nCS = 0;
	delay_spi();
	for (i=0; i<8; i++) {  // 写地址
		SDIN = ((dat & 0x80) == 0)? 0: 1;
		dat = dat << 1;
		SCLK = 1;
		delay_spi();
		SCLK = 0;
	} 
	
	nCS = 1;
	SCLK = 1;
	SDIN = 1;
} 


unsigned char read_at7456_data(void)
{ 
	unsigned char i, r;
	SCLK = 0;
	r = 0;
	nCS = 0;
	delay_spi();
	
	for (i=0; i<8; i++) { // 读数据
		r = r << 1;
		if (SDOUT)
			r |= 1;
		SCLK = 1;
		delay_spi();
		SCLK = 0;
	}
	
	nCS = 1;
	SCLK = 1;
	SDIN = 1;
	
	return r;
} 

// AT7456(E)复位
void at7456_reset(void)
{
	// 先软件复位，主要是针对没有连接 nRESET 的外部 7456
	write_at7456_addr_data(VM0, 2);
	delay_ms(60); // 复位时间至少为 50ms
	
	GPIO_ResetBits(GPIOC, GPIO_Pin_5); // nRST = 0
	delay_ms(60); // 复位脉冲宽度至少为 50ms
	
	GPIO_SetBits(GPIOC, GPIO_Pin_5); // nRST = 1
	delay_ms(60); // 至少 40ms，否则无法将字符写入芯片
}



VER7456 newAt7456;

// AT7456 显示字符
void DisplayChar(unsigned char row, unsigned char col, unsigned char c)
{
	unsigned int kk;
	kk = row * 30 + col;
	write_at7456_addr_data(DMAH, kk / 256); // address
	write_at7456_addr_data(DMAL, kk % 256);
	write_at7456_addr_data(DMDI, c);
}

// 显示数字 0..9
void DisplayNum(unsigned char row, unsigned char col, unsigned char c)
{
	unsigned int kk;
	kk = row * 30 + col;
	write_at7456_addr_data(DMAH, kk / 256); // address
	write_at7456_addr_data(DMAL, kk % 256);
	write_at7456_addr_data(DMDI, ((c == 0)? 10: c));
}

// 显示的字符串为字符和数字以及空格
void DisplayString(unsigned char row, unsigned char col, unsigned char *s)
{
	unsigned int kk;
	unsigned char c;
	
	kk = row * 30 + col;
	write_at7456_addr_data(DMAH, kk / 256); // address
	write_at7456_addr_data(DMAL, kk % 256);
	write_at7456_addr_data(DMM, 0x01); // Auto Inc
	
	c = *s++;
	while (c != 0) { 
		if (c >= '0') && (c <= '9')
			write_at7456_data ((c == '0')? 10: c-'1'+1);
		else if ((c >= 'A') && (c <= 'Z'))
			write_at7456_data(c - 'A' + 11);
		else if  ((c >= 'a') &&(c <= 'z'))
			write_at7456_data(c - 'a'+37);
		else if (c == ' ')
			write_at7456_data(0x00);
		else if (c == '(')
			write_at7456_data(0x3f);
		else if (c == ')')
			write_at7456_data(0x40);
		else if (c == '.')
			write_at7456_data(0x41);
		else if (c == '?')
			write_at7456_data(0x42);
		else if (c == ';')
			write_at7456_data(0x43);
		else if (c == ':')
			write_at7456_data(0x44);
		else if (c == ',')
			write_at7456_data(0x45);
		else if (c == '\'')
			write_at7456_data(0x46);
		else if (c == '/')
			write_at7456_data(0x47);
		else if (c == '"')
			write_at7456_data(0x48);
		else if (c == '-')
			write_at7456_data(0x49);
		else if (c == '<')
			write_at7456_data(0x4a);
		else if (c == '>')
			write_at7456_data(0x4b);
		else if (c == '@')
			write_at7456_data(0x4c);
		else
			write_at7456_data(c);
		c = *s++;
		kk++;
	}
	write_at7456_data(0xff); // Exit Auto Inc
}


// 关闭 OSD
void turnOff_osd(void)
{
	unsigned char k;
	k = read_at7456_addr_data(VM0);
	write_at7456_addr_data(VM0, k&~(1<<3)); // VM0[3]=0，禁止 OSD
	delay_us(30);
}

// 打开 OSD
void turnOn_osd(void)
{
	unsigned char k;
	k = read_at7456_addr_data(VM0);
	write_at7456_addr_data(VM0, k|(1<<3)); // VM0[3]=1，打开 OSD
	delay_us(30);
}

// 清屏，0 地址为空格
void clear_screen(void)
{
	unsigned int i;
	write_at7456_addr_data(DMAH, 0x00); // address
	write_at7456_addr_data(DMAL, 0);
	write_at7456_addr_data(DMM, 0x01); // 地址自动递增
	for (i=0; i <(16*30); i++) 
		write_at7456_data(0);
	
	write_at7456_data(0xff); // 退出自动递增模式
}



// 通过写入/读出 VM0.7 来判断 AT7456 的版本问题，新版可以读写 VM0.7 位
// 防止 SPI 接口开路或短路，需要有 0、1 两种状态
VER7456 checkAT7456(void)
{
	unsigned char r1, r2;
	r1 = read_at7456_addr_data(VM0);
	r2 = (r1&~(1<<1))|0x88; 		// VM0.1(Software Reset Bit) = 0，同时将 VM0.3 置位 1
	write_at7456_addr_data(VM0, r2); // 写 VM0
	delay_us(30);
	r2 = read_at7456_addr_data(VM0) & 0x88;
	write_at7456_addr_data(VM0, r1); // 恢复 VM0
	delay_us(30);
	
	if (r2 == 0x88)
		return NEW7456; // 新版 7456
	else if (r2 == 0x08)
		return OLD7456; // 老版 7456
	else
		return BAD7456; // SPI 接口异常
}



void configOSDBL(unsigned char autoLevelCtrl)
{
	unsigned char k;
	k = read_at7456_addr_data(OSDBL);
	
	if (autoLevelCtrl)
		k &= ~(1<<4);		// 复位后将 OSDBL.4 强制清零 --- 强制打开自动电平控制
	else
		k |= (1<<4);		// 复位后将 OSDBL.4 强制置 1 --- 强制关闭自动电平控制
	
	write_at7456_addr_data(OSDBL, k);
}



// 更新字库
// addr : 字库地址，0 .. 255
// dt : 存放 54 字节字库数据的地址
void write_at7456_font(unsigned char addr, unsigned char *dt)
{
	unsigned char n;
	write_at7456_addr_data(CMAH, addr); 		// 字库地址
	for (n=0; n<54; n++) {
		write_at7456_addr_data(CMAL, n);
		write_at7456_addr_data(CMDI, *dt++); 	// 写内部镜像 RAM
	}
	write_at7456_addr_data(CMM, RAM_NVM); 		// 将内部镜像 RAM 中的字库数据写到 NVM
												// AT7456 = 2.92ms
	wait_memory_wr_ok(); 						// STAT[5] = 0, 表示写操作完成, MAX7456 约 10ms
}



// 读字库数据
// addr : 字库地址，0 .. 255
// dt : 存放 54 字节字库数据的地址
void read_at7456_font(unsigned char addr, unsigned char *dt)
{
	unsigned char n;
	write_at7456_addr_data(CMAH, addr);
												// 一次性读取 54 字节字库数据
	write_at7456_addr_data(CMM, NVM_RAM); 		// 从 NVM 读取字库数据到内部镜像 RAM
	delay_us(20); 								// AT7456 需要 20us 以上时间
	for (n=0; n<54; n++) {
		write_at7456_addr_data(CMAL, n);
		*dt++ = read_at7456_addr_data(CMDO);
	}
}



/*
	AT7456E 字库读写，512 个字库
*/

#define AUTO_INC_DIS 0 // 启用地址自动递增，速度更快
//#define AUTO_INC_DIS 1 // 禁止地址自动递增

// 更新字库
// addr : 字库地址，0 .. 511
// dt : 存放 54 字节字库数据的地址
void write_at7456E_font(unsigned short addr, unsigned char *dt)
{
	unsigned char addr_h, n;
	addr_h = (addr >> 8);
	write_at7456_addr_data(CMAH, addr); 					// 写字库低 8 位地址
	if (AUTO_INC_DIS)
		write_at7456_addr_data(DMM, 0); 					// 不自动递增
	else {
		write_at7456_addr_data(CMAL, (addr_h<<6)); 			// D6 --- 字库页面
		write_at7456_addr_data(DMM, 0x80); 					// CMAL 自动递增
	}
	
	for (n = 0; n < 54; n++) {
		if (AUTO_INC_DIS) {
			write_at7456_addr_data(CMAL, n|(addr_h<<6)); 	// D6 --- 字库页面
			write_at7456_addr_data(CMDI, *dt++); 			// 写内部镜像 RAM
		}
		else
			write_at7456_data (*dt++);
	}
	write_at7456_addr_data(CMM, RAM_NVM); 					// 将内部镜像 RAM 中的字库数据写到 NVM
															// AT7456 = 2.92ms
	wait_memory_wr_ok (); 									// STAT[5] = 0, 表示写操作完成, MAX7456 约 10ms
}



// 读字库数据
// addr : 字库地址，0 .. 511
// dt : 存放 54 字节字库数据的地址
void read_at7456E_font(unsigned short addr, unsigned char *dt)
{
	unsigned char addr_h, n;
	addr_h = (addr >> 8);
	write_at7456_addr_data(CMAH, addr);
	write_at7456_addr_data(CMAL, (addr_h<<6)); 				// D6 --- 字库页面
															// 一次性读取 54 字节字库数据
	write_at7456_addr_data(CMM, NVM_RAM); 					// 从 NVM 读取字库数据到内部镜像 RAM，0.5us
	delay_us(30); 											// AT7456E 需要 15.5us 以上时间
	for (n = 0; n < 54; n++) {
		write_at7456_addr_data(CMAL, n);
		*dt++ = read_at7456_addr_data(CMDO);
	}
}



void wait_memory_wr_ok(void)
{
	// 实测为 38.8ms
	while ((read_at7456_addr_data(STAT) & (1<<5)) != 0); // STAT[5] = 0 --- Available to Operate
	while ((read_at7456_addr_data(DMM)  & (1<<2)) != 0); // DMM[2]  = 0 --- Available to Operate
}



/*
	字符读写，仅适用于采用前 256 个字库的字符
*/
// row：显示字符行号（0..15）；col：显示字符的列号（0..29）
// ca：读取到的字符参数
void at7456_read_char(unsigned char row, unsigned char col, CHAR_ATTRIB *ca)
{
	unsigned short k;
	k = row * 30 + col;
	write_at7456_addr_data(DMM, (0<<6)); 				// 16bit 模式
	write_at7456_addr_data(DMAH, k>>8); 				// 显示位置，高 1 位，读字符，D1=0
	write_at7456_addr_data(DMAL, k); 					// 显示位置，低 8 位
	ca->index = read_at7456_addr_data(DMDO); 			// 字符在字库中的地址
	ca->attrib = read_at7456_data() >> 4; 				// 字符属性
}



// row：显示字符行号（0..15）；col：显示字符的列号（0..29）
// ca：待写字符参数
void at7456_write_char(unsigned char row, unsigned char col, CHAR_ATTRIB *ca)
{
	unsigned short k;
	write_at7456_addr_data(DMM, (ca->attrib)<<3); // 16bit 模式, D6 = 0
	k = row * 30 + col;
	write_at7456_addr_data(DMAH, k >> 8); // 显示位置，高 1 位
	write_at7456_addr_data(DMAL, k); // 显示位置，低 8 位
	write_at7456_addr_data(DMDI, ca->index); // 待显示字符
}



/*
	AT7456E 字符读写，也适用于 AT7456
*/
// row：显示字符行号（0..15）；col：显示字符的列号（0..29）
// ca：读取到的字符参数
void at7456E_read_char(unsigned char row, unsigned char col, CHAR_ATTRIB *ca)
{
	unsigned short k;
	unsigned char addr_h, j;
	k = row * 30 + col;
	addr_h = k  >> 8;
															// 先读字符
	write_at7456_addr_data(DMM, (1<<6)); 					// 按 8 位模式读显示字符(读屏)
	write_at7456_addr_data(DMAH, addr_h); 					// D0 - 显示位置，高 1 位; D1=0，读字符
	write_at7456_addr_data(DMAL, k); 						// 显示位置，低 8 位
	ca- -> >index = read_at7456_addr_data(DMDO); 			// 读取到字符的低 8 位字库地址
															// 后读属性（内含字符的高位字库地址）
	write_at7456_addr_data(DMAH, addr_h|2); 				// D0 - 显示位置，高 1 位; D1=1，读属性
	write_at7456_addr_data(DMAL, k); 						// 显示位置，低 8 位
	j = read_at7456_addr_data(DMDO); 						// D7..D5 - 属性; D4 --- CA[8]
	ca->attrib = j >> 5;
	if (j&(1<<4))
		ca->index |= 0x100;
}



// row：显示字符行号（0..15）；col：显示字符的列号（0..29）
// ca：待写字符参数
void at7456E_write_char(unsigned char row, unsigned char col, CHAR_ATTRIB *ca)
{
	unsigned short k;
	unsigned char addr_h, j;
	k = row * 30 + col;
	addr_h = k >> 8;
															// 先写属性
	write_at7456_addr_data(DMM, (1<<6)); 					// 按 8 位模式写显示字符(写屏)
	write_at7456_addr_data(DMAH, addr_h | 0x2); 			// D0 - 显示位置，高 1 位; D1 - 属性
	write_at7456_addr_data(DMAL, k); 						// 显示位置，低 8 位
	j = (ca->attrib)<<5; 									// 属性，DMDI[7..5]
	if  ((ca->index >>8) != 0)
		j |= 0x10; 											// 属性中加入字库页面标志, D4 = CA[8]
	write_at7456_addr_data(DMDI, j);
															// 再写字符
	write_at7456_addr_data(DMAH, addr_h);
	write_at7456_addr_data(DMAL, k);
	write_at7456_addr_data(DMDI, ca->index); 				// 字符低 8 位，高 1 位已经在前面的字符属性中写入
}



