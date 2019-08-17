#include <STC15F2K60S2.h>
sbit beep = P3^4;
//sbit  ADDR3 = P1^3;
//sbit  ENLED = P1^4;
//uchar duanxuan[]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};     //显示0-9
unsigned char weixuan[]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07};			 //数码管0-7

sbit led_sel=P2^3;	
sbit key1 = P3^2;					//按键1引脚
bit flag;	
//控制蜂鸣器开关的标志位
void delay(unsigned int xms)
{
	unsigned char i;
	for(; xms>0; xms--)						
		for(i=114; i>0; i--)
		{
			;
		}
}	 


//数码管与LED灯切换引脚
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
	led_sel=0;	//先选择数码管亮
	
	//flag_1 = 0;
	//flag_2 = 0;
}

///////////////////////////////////////////////////////////////////////////
unsigned char code LedChar[] = {  //数码管显示字符转换表
    //0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    //0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
		0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f
};
unsigned char LedBuff[9] = {  //数码管+独立LED显示缓冲区
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
    EA = 1;      //开总中断
    //ENLED = 0;   //使能数码管和LED
    //ADDR3 = 1;
		ConfigTimer0(1);  //配置T0定时1ms
    
    while (1)
    {
        if (flag1s)  //每秒执行一次交通灯刷新
        {
            flag1s = 0;
            TrafficLight();
        }
    }
}

void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp;  //临时变量
    
    tmp = 11059200 / 12;      //定时器计数频率
    tmp = (tmp * ms) / 1000;  //计算所需的计数值
    tmp = 65536 - tmp;        //计算定时器重载值
    tmp = tmp + 18;           //补偿中断响应延时造成的误差
    T0RH = (unsigned char)(tmp>>8);  //定时器重载值拆分为高低字节
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0;   //清零T0的控制位
    TMOD |= 0x01;   //配置T0为模式1
    TH0 = T0RH;     //加载T0重载值
    TL0 = T0RL;
    ET0 = 1;        //使能T0中断
    TR0 = 1;        //启动T0
}
void TrafficLight()
{
	static unsigned char color = 2;
	static unsigned char timer = 0;
	
	
	
	if(timer == 0)
	{	
		if(key1==0)
		{	
			delay(10);						 //延时消抖
			if(key1==0)
			{									  
				while(!key1);
				color = 2;					  //蜂鸣器开关标志位翻转
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
			timer = 8;//绿灯
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
			delay(10);						 //延时消抖
			if(key1==0)
			{									  
				while(!key1);
				color = 2;					  //蜂鸣器开关标志位翻转
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
    static unsigned char i = 0;  //动态扫描索引
    led_sel = 0;
    P0 = 0xFF;             //关闭所有段选位，显示消隐
    
		
		P2 = (P2 & 0x00) | i;  //位选索引值赋值到P2口低3位
    
	
		P0 = LedBuff[i];       //缓冲区中索引位置的数据送到P0口
    if (i < 8)             //索引递增循环，遍历整个缓冲区
        i++;
    else
        i = 0;
}

void InterruptTimer0() interrupt 1
{
    static unsigned int tmr1s = 0;  //1秒定时器
		//static unsigned int time1s = 0;
		//static unsigned int time1s_2 = 0;
	
    TH0 = T0RH;  //重新加载重载值
    TL0 = T0RL;
    LedScan();   //LED扫描显示
    tmr1s++;     //1秒定时的处理
		//time1s++;
	
    if (tmr1s >= 1000)
    {
        tmr1s = 0;
        flag1s = 1;  //设置秒定时标志
    }
		if(flag)
		{
				beep=~beep;				   //产生方波使得蜂鸣器发声
		}
		else
		{
				beep=0;				      //停止发声，并将beep端口置于低电平
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