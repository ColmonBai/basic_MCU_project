#include<reg52.h>
sbit s0=P1^0;
sbit s1=P1^1;
sbit s2=P1^2;
sbit s3=P1^3;
sbit rlight=P1^4;
sbit llight=P1^5;
unsigned int  flag =0, j=0;
void delay(int n)
{
    int i,j;
    for(i=0;i<n;i++)
            for(j=0;j<100;j++);
}
void stop()
{
	s0=0;s1=0;s2=0;s3=0;
}
void front()
{
    if(flag){s0=1;s1=0;s2=1;s3=0;}
    	else
            stop();
}
void back()
{
    s0=0;s1=1;s2=0;s3=1;
}
void right()
{
    s0=0;s1=1;s2=1;s3=0;
}
void left()
{
    s0=1;s1=0;s2=0;s3=1;
}

void main()
{
    EA = 1;
    TMOD= 0x01;
    TL0 = 0x18;
    TH0 = 0xFC;
    TF0 = 0;
    TR0 = 1;
    ET0 = 1;
    P2=0xff;        
    {s0=1;s1=0;s2=1;s3=0;}
        delay(50);
    while(1)
        {         
            if(!rlight&&!llight)
            {
                    P2=0x00;
                    front();
            }
            else if(rlight&&!llight)
            {
                    stop();
                    delay(1000);
                    P2=0x0e;
                    right();
                    delay(200);
            }
            else if(llight&&!rlight)
            {
                    stop();
                    delay(1000);
                    P2=0x03;
                    left();
                    delay(200);
            }
                else
                    stop();
        }
}
void InterruptTimer0() interrupt 1
{        
      TL0 = 0x18;
      TH0 = 0xFC;
      j++;
      if(0<=j&&j<4)
            flag=1;
            else
            flag=0;
            if (j>8)
                j=0;
}
