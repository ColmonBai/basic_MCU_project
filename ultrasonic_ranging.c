/*   超声波模块的TRIG管脚接在单片机的P20口，ECHO管脚接在单片机的P21口 其他的电源管脚接在单片机
                开发板上电源口即可。      */
#include"reg52.h"
#include <intrins.h>
sbit LCD1602_RS = P1^5;        //位定义，液晶的数据（1）/命令（0）选择
sbit LCD1602_RW = P1^6;        //位定义，液晶的读写选择
sbit LCD1602_EN = P1^7;        //位定义，液晶使能信号

#define LCDPORT P0          //液晶的数据口

void Delay(unsigned int uiCount);        //延时函数
void LCD1602_CheckBusy(void);        //液晶忙检测
void LCD1602_WriteInformation(unsigned char ucData,bit bComOrData);        //在液晶上写数据或者写命令，0为命令，1为数据
void LCD1602_Init(void);        //液晶初始化
void LCD1602_MoveToPosition(unsigned char x,unsigned char y);        //液晶的坐标移动到指定位置
void LCD1602_DisplayOneCharOnAddr(unsigned char x,unsigned char y,unsigned char ucData);        //在液晶指定位置显示字符
void LCD1602_DisplayString(unsigned char *ucStr);        //在液晶上显示字符串
sbit RX=P2^1;
sbit TX=P2^0;
unsigned int  time=0;
unsigned int  timer=0;
unsigned char posit=0;
unsigned long S=0;
bit flag =0;

unsigned char code DIG_CODE[17]={
'0','1','2','3','4','5','6','7','8','9'};
unsigned char disbuff[4]           ={ 0,0,0,0,};
/********************************************************/
void Conut(void)
{
         time=TH0*256+TL0;
         TH0=0;
         TL0=0;
        
         S= (long)(time*0.17);     //算出来是CM
         if((S>=4000)||flag==1) //超出测量范围显示“ERR0”
         {         
         flag=0;
         disbuff[0]='N';           //“-”
         disbuff[1]='O';           //“-”
         disbuff[2]='N';           //“-”
         disbuff[3]='E';           //“-”
         }
         else
         {
         disbuff[0]=DIG_CODE[S%10000/1000];
         disbuff[1]=DIG_CODE[S%1000/100];
         disbuff[2]=DIG_CODE[S%100/10];
         disbuff[3]=DIG_CODE[S%10/1];
         }
}
/********************************************************/
void zd0() interrupt 1                  //T0中断用来计数器溢出,超过测距范围
{
     flag=1;                                                         //中断溢出标志
}
/********************************************************/
void  zd3()  interrupt 3                  //T1中断用来扫描数码管和计800MS启动模块
{
         TH1=0xf8;
         TL1=0x30;
         //DigDisplay();
         LCD1602_MoveToPosition(1,0);        //显示位置移动到指定位置
                LCD1602_DisplayString(disbuff);        //显示的内容
         timer++;
         if(timer>=100)
         {
         timer=0;
         TX=1;                                        //800MS  启动一次模块
         _nop_(); 
         _nop_(); 
         _nop_(); 
         _nop_(); 
         _nop_(); 
         _nop_(); 
         _nop_(); 
         _nop_(); 
         _nop_(); 
         _nop_(); 
         _nop_(); 
         _nop_(); 
         _nop_(); 
         _nop_(); 
         _nop_(); 
         _nop_(); 
         _nop_();
         _nop_(); 
         _nop_(); 
         _nop_(); 
         _nop_();
         TX=0;
         } 
}
/*********************************************************/

        void  main(  void  )

{  
    TMOD=0x11;                   //设T0为方式1，GATE=1；
        TH0=0;
        TL0=0;          
        TH1=0xf8;                   //2MS定时
        TL1=0x30;
        ET0=1;             //允许T0中断
        ET1=1;                           //允许T1中断
        TR1=1;                           //开启定时器
        EA=1;                           //开启总中断

        LCD1602_Init();          //液晶初始化
        LCD1602_MoveToPosition(0,0);        //显示位置移动到指定位置
        LCD1602_DisplayString("distance");        //显示的内容
        LCD1602_MoveToPosition(1,5);        //显示位置移动到指定位置
        LCD1602_DisplayString("mm");        //显示的内容

        while(1)
        {
        while(!RX);                //当RX为零时等待
        TR0=1;                            //开启计数
        while(RX);                        //当RX为1计数并等待
        TR0=0;                                //关闭计数
        Conut();                        //计算

        }

}
/******************************************************************************
函数名称：LCD1602_CheckBusy
函数功能：忙检测
入口参数：无
返回值：无
备注：忙检测采用了多加一个延时条件的办法，放置程序卡在检测函数中
*******************************************************************************/
void LCD1602_CheckBusy(void)                
{
        unsigned char i = 255;
        LCDPORT = 0xFF;  //读之前先置位，准备读取IO口数据
        LCD1602_RS = 0;
        LCD1602_RW = 1;         //使液晶处于读数据状态
        LCD1602_EN = 1;         //使能液晶，高电平有效
        while((i--) && (LCDPORT & 0x80));        //忙检测
        LCD1602_EN = 0;
}

/******************************************************************************
函数名称：LCD1602_WriteInformation
函数功能：向LCD1602液晶写入数据或者命令
入口参数：ucData-要写入的数据或者命令参数
                  bComOrData-为0时写入的时命令，为1时写入的时数据
返回值：无
备注：无
*******************************************************************************/
void LCD1602_WriteInformation(unsigned char ucData,bit bComOrData)         
{
        LCD1602_CheckBusy();         //在写入数据或者命令前先进行忙检测
        LCDPORT = ucData;                 //先将数据或者命令送至IO
        LCD1602_RS = bComOrData;        //确定是写入数据还是写命令
        LCD1602_RW = 0;                //使液晶处于写入信息状态
        LCD1602_EN = 1;                //使能液晶，高电平有效
        LCD1602_EN = 0;
}

/******************************************************************************
函数名称：LCD1602_Init
函数功能：液晶初始化函数
入口参数：无
返回值：无
备注：无
*******************************************************************************/
void LCD1602_Init(void)                 
{
        LCD1602_WriteInformation(0x38,0);
        Delay(300);
        LCD1602_WriteInformation(0x38,0);
        Delay(100);
        LCD1602_WriteInformation(0x38,0);
        Delay(100);
        LCD1602_WriteInformation(0x38,0);         //写入命令，5x7点阵工作方式，8位数据接口
        Delay(100);
        LCD1602_WriteInformation(0x0c,0);        //显示设置，开显示，光标不显示，不闪烁
        Delay(20);
        LCD1602_WriteInformation(0x01,0);        //清屏指令
        Delay(20);
}

/******************************************************************************
函数名称：LCD1602_MoveToPosition
函数功能：将液晶的光标移动到指定的位置
入口参数：x-液晶显示的行数，范围0-1
                        x = 0:在液晶的第一行
                        x = 1:在液晶的第二行
                  y-液晶显示的列数，范围0-15
                    y = 0:在液晶的第一列
                        y = 1:在液晶的第二列
                        ......
                        y = 15:在液晶的第十六列
返回值：无
备注：通过指定x,y的值可以将液晶的光标移动到指定的位置
*******************************************************************************/
void LCD1602_MoveToPosition(unsigned char x,unsigned char y)        
{
        if(0 == x)
                LCD1602_WriteInformation((0x80 | y),0);           //光标定位到第一行的y列
        if(1 == x)
                LCD1602_WriteInformation((0xC0 | y),0);           //光标定义到第二行的y列
}

/******************************************************************************
函数名称：LCD1602_DisplayOneCharOnAddr
函数功能：在指定的位置上显示指定的字符
入口参数：x-液晶显示的行数，范围0-1
                        x = 0:在液晶的第一行
                        x = 1:在液晶的第二行
                  y-液晶显示的列数，范围0-15
                    y = 0:在液晶的第一列
                        y = 1:在液晶的第二列
                        ......
                        y = 15:在液晶的第十六列
                  ucData-要显示的字符数据
返回值：无
备注：确保x,y的取值要在指定的范围内
*******************************************************************************/
void LCD1602_DisplayOneCharOnAddr(unsigned char x,unsigned char y,unsigned char ucData)
{
        LCD1602_MoveToPosition(x,y);   //光标位置
        LCD1602_WriteInformation(ucData,1);          //写入数据
}

/******************************************************************************
函数名称：LCD1602_DisplayString
函数功能：显示字符串
入口参数：ucStr-字符串的首地址
返回值：无
备注：无
*******************************************************************************/
void LCD1602_DisplayString(unsigned char *ucStr)        
{
        while(*ucStr != '\0')           //字符串结束之前，循环显示
        {
                 LCD1602_WriteInformation(*ucStr,1);         //依次写入每一个字符
                 ucStr++;                                                                 //指针增加
        }
}

/******************************************************************************
函数名称：Delay
函数功能：延时函数
入口参数：uiCount-延时参数，每加1增加0.5ms
返回值：无
备注：无
*******************************************************************************/
void Delay(unsigned int uiCount)
{
        unsigned char j = 244;
        for(;uiCount > 0;uiCount--) while(--j);        
}