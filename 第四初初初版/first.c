#include <STC15F2K60S2.h>
sbit beep = P3^4;
//sbit  ADDR3 = P1^3;
//sbit  ENLED = P1^4;
//uchar duanxuan[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};     //��ʾ0-9
unsigned char weixuan[]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07};			 //�����0-7

sbit led_sel=P2^3;	
sbit key1 = P3^2;					//����1����
bit flag;	
//���Ʒ��������صı�־λ
void delay(unsigned int xms)
{
	unsigned char i;
	for(; xms>0; xms--)						
		for(i=114; i>0; i--)
		{
			;
		}
}	 


//�������LED���л�����
//void delay_ms(unsigned int n)		
//{
	//while(n){
	//unsigned char i, j;
	//i = 11;
	//j = 190;
	//do
	//{
		//while (--j);
	//} while (--i);
	//n--;
//}
//}

void Init(){
	P3M1 = 0x00;
	P3M0 = 0x10;
	
	P0M1 = 0x00;
	P0M0 = 0xff;

	P2M1 = 0x00;
	P2M0 = 0x08;
	
	beep = 0;
	flag = 0;
	led_sel=0;	//��ѡ���������
	
	//flag_1 = 0;
	//flag_2 = 0;
}

///////////////////////////////////////////////////////////////////////////
unsigned char code LedChar[] = {  //�������ʾ�ַ�ת����
    //0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    //0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
		0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f
};
unsigned char LedBuff[9] = {  //�����+����LED��ʾ������
    //0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
bit flag1s = 1;
unsigned char T0RH = 0;
unsigned char T0RL = 0;

void ConfigTimer0(unsigned int ms);
void TrafficLight();
void main()
{
		Init();
		led_sel = 0;
    EA = 1;      //�����ж�
    //ENLED = 0;   //ʹ������ܺ�LED
    //ADDR3 = 1;
		ConfigTimer0(1);  //����T0��ʱ1ms
    
    while (1)
    {
        if (flag1s)  //ÿ��ִ��һ�ν�ͨ��ˢ��
        {
            flag1s = 0;
            TrafficLight();
        }
    }
}

void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp;  //��ʱ����
    
    tmp = 11059200 / 12;      //��ʱ������Ƶ��
    tmp = (tmp * ms) / 1000;  //��������ļ���ֵ
    tmp = 65536 - tmp;        //���㶨ʱ������ֵ
    tmp = tmp + 18;           //�����ж���Ӧ��ʱ��ɵ����
    T0RH = (unsigned char)(tmp>>8);  //��ʱ������ֵ���Ϊ�ߵ��ֽ�
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0;   //����T0�Ŀ���λ
    TMOD |= 0x01;   //����T0Ϊģʽ1
    TH0 = T0RH;     //����T0����ֵ
    TL0 = T0RL;
    ET0 = 1;        //ʹ��T0�ж�
    TR0 = 1;        //����T0
}
void TrafficLight()
{
	static unsigned char color = 2;
	static unsigned char timer = 0;
	
	
	
	if(timer == 0)
	{	
		if(key1==0)
		{	
			delay(10);						 //��ʱ����
			if(key1==0)
			{									  
				while(!key1);
				color = 2;					  //���������ر�־λ��ת
				timer = 30;
				LedBuff[8] = 0x01;
			}
		}
		switch(color)
		{
			case 0:
			color = 1;
			timer = 10;
			//LedBuff[8] = 0x03;
			break;

			case 1:
			color = 2;
			timer = 30;
			LedBuff[8] = 0x01;
			break;

			case 2:
			color = 0;
			timer = 8;//�̵�
			LedBuff[8] = 0x07;
			break;

			default:
			break;
		}
	}
	else
	{
		timer--;
		
		if(key1==0)
		{	
			delay(10);						 //��ʱ����
			if(key1==0)
			{									  
				while(!key1);
				color = 2;					  //���������ر�־λ��ת
				timer = 30;
				LedBuff[8] = 0x01;
			}
		}
		
		if (color == 2){
				flag=~flag;
		}else{
			flag=0;
		}
		
		if (color == 1){
			
				if (timer%2==0){
						LedBuff[8] = 0x03;
				}else{
						LedBuff[8] = 0x00;
				}
			
		}
		
	}
	//LedBuff[0] = LedChar[timer%10];
	//LedBuff[1] = LedChar[timer/10];	
		LedBuff[0] = LedChar[timer/10];
		LedBuff[1] = LedChar[timer%10];	
}
void LedScan()
{
    static unsigned char i = 0;  //��̬ɨ������
    led_sel = 0;
    P0 = 0xFF;             //�ر����ж�ѡλ����ʾ����
    
		
		P2 = (P2 & 0x00) | i;  //λѡ����ֵ��ֵ��P2�ڵ�3λ
    
	
		P0 = LedBuff[i];       //������������λ�õ������͵�P0��
    if (i < 8)             //��������ѭ������������������
        i++;
    else
        i = 0;
}

void InterruptTimer0() interrupt 1
{
    static unsigned int tmr1s = 0;  //1�붨ʱ��
		//static unsigned int time1s = 0;
		//static unsigned int time1s_2 = 0;
	
    TH0 = T0RH;  //���¼�������ֵ
    TL0 = T0RL;
    LedScan();   //LEDɨ����ʾ
    tmr1s++;     //1�붨ʱ�Ĵ���
		//time1s++;
	
    if (tmr1s >= 1000)
    {
        tmr1s = 0;
        flag1s = 1;  //�����붨ʱ��־
    }
		if(flag)
		{
				beep=~beep;				   //��������ʹ�÷���������
		}
		else
		{
				beep=0;				      //ֹͣ����������beep�˿����ڵ͵�ƽ
		}
		
		//if(flag_1==1&&flag_2==0){
			//if(time1s<=1000){
					//LedBuff[8] = 0x00;
					//if(time1s==1000){
						//flag_2 = 1;
					//}
			//}
			
			
		//}else if(flag_1==1&&flag_2==1){
			//if(time1s >1000){
					//LedBuff[8] = 0x03;
					//if(time1s==2000){
							//flag_2 = 0;
							//time1s=0;
					//}
			//}
		//}
		
}