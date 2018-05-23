#include "2450addr.h"
#include "my_lib.h"
#include "pink.h"
#include "start.h"
#include "blue.h"
#include "option.h"
#include "blue_c.h"
#include "pink_c.h"
#include"lose.h"
#include"victory.h"
#include"draw.h"
#include"restart.h"

#define BLACK   0x0000
#define WHITE   0xFFFF
#define A1      880     // La

extern unsigned int HandleTIMER0;
extern unsigned int HandleUART1;
int rxbuf=0;
int tick=0;
int x1,y1;
int user = 2;
int sit_x[]={10,75,140,205,270,335};
int sit_y[]={5,70,135,200};
int sit_state[] = {0,1,0,1,0,1,0,0,1,0,1,0,1,1,0,1,0,1,0,0,1,0,1,0,1};
int i,j;
int sit_xnum=0;
int sit_ynum=0;
int sit_tnum=0;
int flag=0;
int select=0;

volatile  int ADC_x, ADC_y;
volatile float temp;
volatile int cd_x = 0,cd_y = 0;
volatile  int Touch_Pressed=0;


void Touch_ISR(void) __attribute__ ((interrupt ("IRQ")));


 __attribute__((interrupt("IRQ"))) void  Timer0_ISR(void)
{   
   rSUBSRCPND = (1<<3);    
   rSRCPND1 = (1<<23)|(1<<10);
   rINTPND1 = (1<<23)|(1<<10);
   tick++;  
   rxbuf= Uart_GetKey();
}


/* 방석 출력 */
void init_Draw_sit(){

  int i,j;
  int toggle=0;
  
  for(i=0;i<4;i++)
  {
    for(j=0;j<6;j++)
    {
      if(toggle==0)
      {
        Lcd_Draw_BMP(sit_x[j],sit_y[i],blue_bmp);
        toggle=1;
      }
      else if(toggle==1)
      {
        Lcd_Draw_BMP(sit_x[j],sit_y[i],pink_bmp);
        toggle=0;
      }
    }
    if(toggle==1)
      toggle=0;
    else if(toggle==0)
      toggle=1;
  }
  
}

void count_sit(void)
{
   int count =0;
      for(i=1; i<25;i++)
      {
            count += sit_state[i];   
         }

       if(user ==1)
       {
        Buzzer_Beep(1000,10);
          if(count > 12){
             Lcd_Draw_BMP(0,0,victory);
            // Uart_Printf("blue victory\n");
          } 
          else if (count==12)
          {
             Lcd_Draw_BMP(0,0,draw_bmp);            
          } 
          else
          {
             Lcd_Draw_BMP(0,0,lose);
          }
       }
       else if(user ==0)
       {
        Buzzer_Beep(1000,10);
          if(count <12){
             Lcd_Draw_BMP(0,0,victory);
          }
          else if (count==12)
          {
             Lcd_Draw_BMP(0,0,draw_bmp);            
          } 

          else{
             Lcd_Draw_BMP(0,0,lose);
          }
       }
       Lcd_Draw_BMP(335,200, restart_bmp);
       cd_x =0;cd_y =0;
       while(1)
       {   
          if(Touch_Pressed)
          {                  
             temp=((850-ADC_x)*480)/670;
             cd_x=(int)temp;
             temp=((650-ADC_y)*270)/310;
             cd_y=(int)temp;      

             while(!(Touch_Pressed==0));                  
          }
          if((cd_x >= 335 && cd_y >= 200) &&(cd_x <= 400 && cd_y <= 265)){
            Uart_Printf("%d",122);
          Uart_Printf("%d",122);
          Uart_Printf("%d",122);
          game_play();
          }
            
          
     }
  }

void game_play(void)
{
  Lcd_Clr_Screen(WHITE);
  init_Draw_sit();

   HandleTIMER0 = (unsigned int)Timer0_ISR;

   int msec = 10;
   Timer_Delay(msec);
   tick=0;
   int gameTime = 30;
   while(1)
   {   
      if(rxbuf)
            { 
              Uart_Printf("%d", rxbuf);
               
               if(rxbuf==102|rxbuf==108|rxbuf==114|rxbuf==120)
                {
                  y1=((rxbuf-96)/6);
                  x1=(rxbuf-96)%6+6;                         
                }
               else
               {
                y1=((rxbuf-96)/6)+1;
                x1=(rxbuf-96)%6;
               }
                if(sit_x[x1-1]>0&&sit_y[y1-1]>0)
               Draw_sit(sit_x[x1-1],sit_y[y1-1],rxbuf-96);               
               rxbuf= 0;
            }

      if((tick%100)==0)
      {
         Lcd_Printf(400,90,BLACK,WHITE,1,1,"Time : %2d",gameTime);
         gameTime--;
         if(tick>=3001)
         {
            break;
         }
      } 
      if(Touch_Pressed)
      {         
         sit_xnum=0;
         sit_ynum=0;
         sit_tnum=0;
         temp=((850-ADC_x)*480)/670;
         cd_x=(int)temp;
         temp=((650-ADC_y)*270)/310;
         cd_y=(int)temp;

         for(i=0;i<6;i++)
         {
              if(sit_x[i]<cd_x)
              {
                 sit_xnum++;               
              }
         }
           for(j=0;j<4;j++)
           {
                if(sit_y[j]<cd_y)
                {
                   sit_ynum++;               
                }
           }
         sit_tnum=sit_xnum+6*(sit_ynum-1); 
         if(sit_x[sit_xnum-1]>0&&sit_y[sit_ynum-1]>0)
         Draw_sit(sit_x[sit_xnum-1],sit_y[sit_ynum-1],sit_tnum);
         //Uart_Printf("%d \n",sit_tnum);
         Uart_Printf("%c",sit_tnum+96);              
         while(!(Touch_Pressed==0));                
      }
   } 
   count_sit();
}

void Draw_sit(int a,int b,int state){

	if(sit_state[state] ==0)
         {
	            sit_state[state] =1;  
	            Lcd_Draw_BMP(a,b, blue_bmp);  
	   }
	else if(sit_state[state] ==1)
	  {
	           sit_state[state] =0 ;   
	           Lcd_Draw_BMP(a,b, pink_bmp); 
         }  
}

void Touch_ISR()
{   
   Buzzer_Beep(A1,10);
   /* ÀÎÅÍ·´Æ® Çã¿ëÇÏÁö ¾ÊÀ½ on Touch */
   rINTSUBMSK |= (0x1<<9);
   rINTMSK1 |= (0x1<<31);
   
   /* TO DO: Pendng Clear on Touch */   
   rSUBSRCPND |= (0x1<<9);
   rSRCPND1 |= (0x1<<31);
   rINTPND1 |= (0x1<<31);
   
   if(rADCTSC & 0x100)
   {
      rADCTSC &= (0xff); 
      Touch_Pressed = 0;
   }   
   else
   {
   rADCTSC =(0<<8)|(1<<7)|(1<<6)|(0<<5)|(1<<4)|(1<<3)|(1<<2)|(0);   

      /* TO DO : ENABLE_START */      
   rADCCON |=(1);
      
      /* wait until End of A/D Conversion */
   while(!(rADCCON & (1<<15)));
      
      /*store X-Position & Y-Position Conversion data value to ADC_x, ADC_y */
   ADC_x = (rADCDAT0 & 0x3ff);
   ADC_y = (rADCDAT1 & 0x3ff);
      
      Touch_Pressed = 1;      
      
   rADCTSC =(1<<8)|(1<<7)|(1<<6)|(1<<4)|(0<<3)|(0<<2)|(3);
   }
   
   /* ÀÎÅÍ·´Æ® ´Ù½Ã Çã¿ë  on Touch */
   rINTSUBMSK &= ~(0x1<<9);
   rINTMSK1 &= ~(0x1<<31);   
}

void Main(void)
{   
   Uart_Init(115200);
      Lcd_Port_Init();
      Lcd_Init();   
      Timer_Init();   
      Touch_Init();
 
   pISR_ADC = (unsigned int)Touch_ISR;   
   rINTSUBMSK &= ~(0x1<<9);
   rINTMSK1 &= ~((0x1<<31)|(1<<10));

   Lcd_Draw_BMP(0,0, start_bmp); 

       /* Interrupt Unmasking */
       rINTMSK1 &= ~(1<<23);
      rINTSUBMSK &= ~(1<<3);   

   while(1){
   
      if(Touch_Pressed)
      {                  
         temp=((850-ADC_x)*480)/670;
         cd_x=(int)temp;
         temp=((650-ADC_y)*270)/310;
         cd_y=(int)temp;      

         while(!(Touch_Pressed==0));
         //Uart_Printf("Released\n\n");         
      }
       /* pink */
        if((cd_x >= 126 && cd_y >= 52) &&(cd_x <= 186 && cd_y <= 112))
        {
	          Lcd_Draw_BMP(126,52, pink_c_bmp);
	          user =0;
	          flag =1;
                select=1;
	          if(flag = 2)
	          {
	             Lcd_Draw_BMP(341,68, blue_bmp);
	          }
       }
       /* blue */
        if((cd_x >= 341 && cd_y >= 68) &&(cd_x <= 401 && cd_y <= 128))
        {   
	          Lcd_Draw_BMP(341,68, blue_c_bmp);
	          user =1;
	          flag =2;
                 select=1;
	          if(flag = 1)
	          {
	            	 Lcd_Draw_BMP(126,52, pink_bmp);
	          }
       }
      /* start!! */
      if(user==1 || user ==0){
          if((cd_x >= 140 && cd_y >= 160) &&(cd_x <= 250 && cd_y <= 200)){
          Uart_Printf("%d",122);
          Uart_Printf("%d",122);
          Uart_Printf("%d",122);
            break;
        }
  }
   }
game_play();      
}