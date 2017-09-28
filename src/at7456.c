//*************************************************
//
// AT7456(E) SPI �ӿڶ�д���� --- AT7456_SPI.C
// �����п�΢�������޹�˾
// 2016-12-22
// 0571-28918103 ϵͳ��
//
//*************************************************

//ͷ�ļ�
//#include <REG52.H>
#include "AT7456.H"
#include <intrins.h>

// AT7456(E)�� SPI ���ƽӿ�
sbit nCS = 		P3 ^ 7; 
sbit SCLK = 	P3 ^ 5; 
sbit SDIN = 	P3 ^ 6;
sbit SDOUT = 	P3 ^ 4;

// ������Ҫ��Ӻ��ʵ� nop ָ�������ʱ�������� SPI ʱ������������ 10MHz
void delay_spi() 
{
	
}

// ��ָ���Ĵ���д 8 λ��
void write_at7456_addr_data(unsigned char addr, unsigned char dat)
{ 
	unsigned char i;
	SCLK = 0;
	nCS = 0;
	delay_spi(); 
	
	// д��ַ
	for (i=0; i<8; i++) { 
		SDIN = ((addr & 0x80) == 0)? 0: 1;
		addr = addr << 1;
		SCLK = 1;
		delay_spi(); 
		SCLK = 0;
	} 
	
	// д����
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


// ��ָ���Ĵ�����ȡ 8 λ����
unsigned char read_at7456_addr_data(unsigned char addr) 
{
	unsigned char i, r; 
	SCLK = 0; 
	r = 0;
	nCS = 0;
	addr |= 0x80; // D7 = 1 Ϊ���Ĵ���
	
	for (i=0; i<8; i++) { // д��ַ
		SDIN = ((addr & 0x80) == 0)? 0: 1;
		addr = addr << 1;
		SCLK = 1;
		delay_spi(); 
		SCLK = 0;
	} 
	for (i=0; i<8; i++) { // ������
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
	for (i=0; i<8; i++) {  // д��ַ
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
	
	for (i=0; i<8; i++) { // ������
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

// AT7456(E)��λ
void at7456_reset(void)
{
	// �������λ����Ҫ�����û������ nRESET ���ⲿ 7456
	write_at7456_addr_data(VM0, 2);
	delay_ms(60); // ��λʱ������Ϊ 50ms
	
	GPIO_ResetBits(GPIOC, GPIO_Pin_5); // nRST = 0
	delay_ms(60); // ��λ����������Ϊ 50ms
	
	GPIO_SetBits(GPIOC, GPIO_Pin_5); // nRST = 1
	delay_ms(60); // ���� 40ms�������޷����ַ�д��оƬ
}



VER7456 newAt7456;

// AT7456 ��ʾ�ַ�
void DisplayChar(unsigned char row, unsigned char col, unsigned char c)
{
	unsigned int kk;
	kk = row * 30 + col;
	write_at7456_addr_data(DMAH, kk / 256); // address
	write_at7456_addr_data(DMAL, kk % 256);
	write_at7456_addr_data(DMDI, c);
}

// ��ʾ���� 0..9
void DisplayNum(unsigned char row, unsigned char col, unsigned char c)
{
	unsigned int kk;
	kk = row * 30 + col;
	write_at7456_addr_data(DMAH, kk / 256); // address
	write_at7456_addr_data(DMAL, kk % 256);
	write_at7456_addr_data(DMDI, ((c == 0)? 10: c));
}

// ��ʾ���ַ���Ϊ�ַ��������Լ��ո�
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


// �ر� OSD
void turnOff_osd(void)
{
	unsigned char k;
	k = read_at7456_addr_data(VM0);
	write_at7456_addr_data(VM0, k&~(1<<3)); // VM0[3]=0����ֹ OSD
	delay_us(30);
}

// �� OSD
void turnOn_osd(void)
{
	unsigned char k;
	k = read_at7456_addr_data(VM0);
	write_at7456_addr_data(VM0, k|(1<<3)); // VM0[3]=1���� OSD
	delay_us(30);
}

// ������0 ��ַΪ�ո�
void clear_screen(void)
{
	unsigned int i;
	write_at7456_addr_data(DMAH, 0x00); // address
	write_at7456_addr_data(DMAL, 0);
	write_at7456_addr_data(DMM, 0x01); // ��ַ�Զ�����
	for (i=0; i <(16*30); i++) 
		write_at7456_data(0);
	
	write_at7456_data(0xff); // �˳��Զ�����ģʽ
}



// ͨ��д��/���� VM0.7 ���ж� AT7456 �İ汾���⣬�°���Զ�д VM0.7 λ
// ��ֹ SPI �ӿڿ�·���·����Ҫ�� 0��1 ����״̬
VER7456 checkAT7456(void)
{
	unsigned char r1, r2;
	r1 = read_at7456_addr_data(VM0);
	r2 = (r1&~(1<<1))|0x88; 		// VM0.1(Software Reset Bit) = 0��ͬʱ�� VM0.3 ��λ 1
	write_at7456_addr_data(VM0, r2); // д VM0
	delay_us(30);
	r2 = read_at7456_addr_data(VM0) & 0x88;
	write_at7456_addr_data(VM0, r1); // �ָ� VM0
	delay_us(30);
	
	if (r2 == 0x88)
		return NEW7456; // �°� 7456
	else if (r2 == 0x08)
		return OLD7456; // �ϰ� 7456
	else
		return BAD7456; // SPI �ӿ��쳣
}



void configOSDBL(unsigned char autoLevelCtrl)
{
	unsigned char k;
	k = read_at7456_addr_data(OSDBL);
	
	if (autoLevelCtrl)
		k &= ~(1<<4);		// ��λ�� OSDBL.4 ǿ������ --- ǿ�ƴ��Զ���ƽ����
	else
		k |= (1<<4);		// ��λ�� OSDBL.4 ǿ���� 1 --- ǿ�ƹر��Զ���ƽ����
	
	write_at7456_addr_data(OSDBL, k);
}



// �����ֿ�
// addr : �ֿ��ַ��0 .. 255
// dt : ��� 54 �ֽ��ֿ����ݵĵ�ַ
void write_at7456_font(unsigned char addr, unsigned char *dt)
{
	unsigned char n;
	write_at7456_addr_data(CMAH, addr); 		// �ֿ��ַ
	for (n=0; n<54; n++) {
		write_at7456_addr_data(CMAL, n);
		write_at7456_addr_data(CMDI, *dt++); 	// д�ڲ����� RAM
	}
	write_at7456_addr_data(CMM, RAM_NVM); 		// ���ڲ����� RAM �е��ֿ�����д�� NVM
												// AT7456 = 2.92ms
	wait_memory_wr_ok(); 						// STAT[5] = 0, ��ʾд�������, MAX7456 Լ 10ms
}



// ���ֿ�����
// addr : �ֿ��ַ��0 .. 255
// dt : ��� 54 �ֽ��ֿ����ݵĵ�ַ
void read_at7456_font(unsigned char addr, unsigned char *dt)
{
	unsigned char n;
	write_at7456_addr_data(CMAH, addr);
												// һ���Զ�ȡ 54 �ֽ��ֿ�����
	write_at7456_addr_data(CMM, NVM_RAM); 		// �� NVM ��ȡ�ֿ����ݵ��ڲ����� RAM
	delay_us(20); 								// AT7456 ��Ҫ 20us ����ʱ��
	for (n=0; n<54; n++) {
		write_at7456_addr_data(CMAL, n);
		*dt++ = read_at7456_addr_data(CMDO);
	}
}



/*
	AT7456E �ֿ��д��512 ���ֿ�
*/

#define AUTO_INC_DIS 0 // ���õ�ַ�Զ��������ٶȸ���
//#define AUTO_INC_DIS 1 // ��ֹ��ַ�Զ�����

// �����ֿ�
// addr : �ֿ��ַ��0 .. 511
// dt : ��� 54 �ֽ��ֿ����ݵĵ�ַ
void write_at7456E_font(unsigned short addr, unsigned char *dt)
{
	unsigned char addr_h, n;
	addr_h = (addr >> 8);
	write_at7456_addr_data(CMAH, addr); 					// д�ֿ�� 8 λ��ַ
	if (AUTO_INC_DIS)
		write_at7456_addr_data(DMM, 0); 					// ���Զ�����
	else {
		write_at7456_addr_data(CMAL, (addr_h<<6)); 			// D6 --- �ֿ�ҳ��
		write_at7456_addr_data(DMM, 0x80); 					// CMAL �Զ�����
	}
	
	for (n = 0; n < 54; n++) {
		if (AUTO_INC_DIS) {
			write_at7456_addr_data(CMAL, n|(addr_h<<6)); 	// D6 --- �ֿ�ҳ��
			write_at7456_addr_data(CMDI, *dt++); 			// д�ڲ����� RAM
		}
		else
			write_at7456_data (*dt++);
	}
	write_at7456_addr_data(CMM, RAM_NVM); 					// ���ڲ����� RAM �е��ֿ�����д�� NVM
															// AT7456 = 2.92ms
	wait_memory_wr_ok (); 									// STAT[5] = 0, ��ʾд�������, MAX7456 Լ 10ms
}



// ���ֿ�����
// addr : �ֿ��ַ��0 .. 511
// dt : ��� 54 �ֽ��ֿ����ݵĵ�ַ
void read_at7456E_font(unsigned short addr, unsigned char *dt)
{
	unsigned char addr_h, n;
	addr_h = (addr >> 8);
	write_at7456_addr_data(CMAH, addr);
	write_at7456_addr_data(CMAL, (addr_h<<6)); 				// D6 --- �ֿ�ҳ��
															// һ���Զ�ȡ 54 �ֽ��ֿ�����
	write_at7456_addr_data(CMM, NVM_RAM); 					// �� NVM ��ȡ�ֿ����ݵ��ڲ����� RAM��0.5us
	delay_us(30); 											// AT7456E ��Ҫ 15.5us ����ʱ��
	for (n = 0; n < 54; n++) {
		write_at7456_addr_data(CMAL, n);
		*dt++ = read_at7456_addr_data(CMDO);
	}
}



void wait_memory_wr_ok(void)
{
	// ʵ��Ϊ 38.8ms
	while ((read_at7456_addr_data(STAT) & (1<<5)) != 0); // STAT[5] = 0 --- Available to Operate
	while ((read_at7456_addr_data(DMM)  & (1<<2)) != 0); // DMM[2]  = 0 --- Available to Operate
}



/*
	�ַ���д���������ڲ���ǰ 256 ���ֿ���ַ�
*/
// row����ʾ�ַ��кţ�0..15����col����ʾ�ַ����кţ�0..29��
// ca����ȡ�����ַ�����
void at7456_read_char(unsigned char row, unsigned char col, CHAR_ATTRIB *ca)
{
	unsigned short k;
	k = row * 30 + col;
	write_at7456_addr_data(DMM, (0<<6)); 				// 16bit ģʽ
	write_at7456_addr_data(DMAH, k>>8); 				// ��ʾλ�ã��� 1 λ�����ַ���D1=0
	write_at7456_addr_data(DMAL, k); 					// ��ʾλ�ã��� 8 λ
	ca->index = read_at7456_addr_data(DMDO); 			// �ַ����ֿ��еĵ�ַ
	ca->attrib = read_at7456_data() >> 4; 				// �ַ�����
}



// row����ʾ�ַ��кţ�0..15����col����ʾ�ַ����кţ�0..29��
// ca����д�ַ�����
void at7456_write_char(unsigned char row, unsigned char col, CHAR_ATTRIB *ca)
{
	unsigned short k;
	write_at7456_addr_data(DMM, (ca->attrib)<<3); // 16bit ģʽ, D6 = 0
	k = row * 30 + col;
	write_at7456_addr_data(DMAH, k >> 8); // ��ʾλ�ã��� 1 λ
	write_at7456_addr_data(DMAL, k); // ��ʾλ�ã��� 8 λ
	write_at7456_addr_data(DMDI, ca->index); // ����ʾ�ַ�
}



/*
	AT7456E �ַ���д��Ҳ������ AT7456
*/
// row����ʾ�ַ��кţ�0..15����col����ʾ�ַ����кţ�0..29��
// ca����ȡ�����ַ�����
void at7456E_read_char(unsigned char row, unsigned char col, CHAR_ATTRIB *ca)
{
	unsigned short k;
	unsigned char addr_h, j;
	k = row * 30 + col;
	addr_h = k  >> 8;
															// �ȶ��ַ�
	write_at7456_addr_data(DMM, (1<<6)); 					// �� 8 λģʽ����ʾ�ַ�(����)
	write_at7456_addr_data(DMAH, addr_h); 					// D0 - ��ʾλ�ã��� 1 λ; D1=0�����ַ�
	write_at7456_addr_data(DMAL, k); 						// ��ʾλ�ã��� 8 λ
	ca- -> >index = read_at7456_addr_data(DMDO); 			// ��ȡ���ַ��ĵ� 8 λ�ֿ��ַ
															// ������ԣ��ں��ַ��ĸ�λ�ֿ��ַ��
	write_at7456_addr_data(DMAH, addr_h|2); 				// D0 - ��ʾλ�ã��� 1 λ; D1=1��������
	write_at7456_addr_data(DMAL, k); 						// ��ʾλ�ã��� 8 λ
	j = read_at7456_addr_data(DMDO); 						// D7..D5 - ����; D4 --- CA[8]
	ca->attrib = j >> 5;
	if (j&(1<<4))
		ca->index |= 0x100;
}



// row����ʾ�ַ��кţ�0..15����col����ʾ�ַ����кţ�0..29��
// ca����д�ַ�����
void at7456E_write_char(unsigned char row, unsigned char col, CHAR_ATTRIB *ca)
{
	unsigned short k;
	unsigned char addr_h, j;
	k = row * 30 + col;
	addr_h = k >> 8;
															// ��д����
	write_at7456_addr_data(DMM, (1<<6)); 					// �� 8 λģʽд��ʾ�ַ�(д��)
	write_at7456_addr_data(DMAH, addr_h | 0x2); 			// D0 - ��ʾλ�ã��� 1 λ; D1 - ����
	write_at7456_addr_data(DMAL, k); 						// ��ʾλ�ã��� 8 λ
	j = (ca->attrib)<<5; 									// ���ԣ�DMDI[7..5]
	if  ((ca->index >>8) != 0)
		j |= 0x10; 											// �����м����ֿ�ҳ���־, D4 = CA[8]
	write_at7456_addr_data(DMDI, j);
															// ��д�ַ�
	write_at7456_addr_data(DMAH, addr_h);
	write_at7456_addr_data(DMAL, k);
	write_at7456_addr_data(DMDI, ca->index); 				// �ַ��� 8 λ���� 1 λ�Ѿ���ǰ����ַ�������д��
}



