/*

This is a flappy bird clone that has been converted to PIC18 from the project by 
"Arduino - Flappy Bird clone by Themistokle "mrt-prodz" Benetatos"
Thanks to him.

Fatih Þahin
ftsahin@gmail.com


Processor : PIC18F2620
TFT Screen : ST7735 1.8"

Connections :

PIC18F2620             TFT LCD
   C0                   RESET
   C1                   RS/A0
   C3                   SCLK
   C5                   SDA
   C7                   CS
----------------------------------


   B1 -Button

*/


#include <18F2620.h>
#device ADC=1024
#fuses INTRC_IO,NOWDT,PROTECT,BROWNOUT,PUT,NOLVP,NOMCLR
#use delay(clock=32000000)
#include "math.h"
#include "font24.h"
#include "st7735.h"
#include "string.h"
#define RAND_MAX 86
#include <STDLIB.h>


#BYTE OSCTUNE  =  0xF9B
#BYTE OSCCON   =  0xFD3 
#BYTE CONFIG1H =  0X300001    
/* Kart üzerindeki butonlar
Sað : B0
Aþaðý B1
Sol B2
Üst B3
OK B4
*/


#define genislik         128   
#define yukseklik        160     
#define yarigenislik     64     
#define yariyukseklik    80     

#define hiz              1
#define yercekimi        9.8
#define ziplama_hizi     1.5

#define kusgen           8     
#define kusyuk           8     
#define kusgen2          4     
#define kusyuk2          4     

#define borugen          12     
#define boruboslugu      36     

#define zeminyuk         20     

#define cimenyuk         4     


float delta;
unsigned int8 skor,rekor;
unsigned int8 skortextsol,rekortextsol;
unsigned int8 skorsol,rekorsol;

unsigned int16 arkaplan=36702;

unsigned int16 borurengi=26313;
unsigned int16 boruparlakrengi=61405;
unsigned int16 borucizgirengi=4418;
unsigned int16 zeminrengi=63348;
unsigned int16 cimenrengi2=36619;
unsigned int16 cimenrengi1=40811;

const unsigned int16 C0=36702; 
const unsigned int16 C1=50473;
const unsigned int16 C2=65525;
const unsigned int16 C3=white;
const unsigned int16 C4=red;
const unsigned int16 C5=65198;

static unsigned int16 kuspixel[] =
{ C0, C0, C1, C1, C1, C1, C1, C0,
  C0, C1, C2, C2, C2, C1, C3, C1,
  C0, C2, C2, C2, C2, C1, C3, C1,
  C1, C1, C1, C2, C2, C3, C1, C1,
  C1, C2, C2, C2, C2, C2, C4, C4,
  C1, C2, C2, C2, C1, C5, C4, C0,
  C0, C1, C2, C1, C5, C5, C5, C0,
  C0, C0, C1, C5, C5, C5, C0, C0};


static struct KUS {
  unsigned char x, y, old_y;
  unsigned int16 col;
  float vel_y;
} kus;

static struct BORU {
  signed int16 x, bosluk_y;
  unsigned int16 col;
} boru;



byte tmpx,tmpy;
char flappybird[]="FLAPPY BIRD";
char baslamakicin[]="Baslamak icin";
char butonabasin[]="Butona Basiniz";
char skortext[]="Skor :";
char rekortext[]="Rekor :";



void eproma_yaz(unsigned int16 hafizadegeri,unsigned int16 deger)
{
   write_eeprom(hafizadegeri,deger);
   write_eeprom(hafizadegeri+1,deger>>8);
}

unsigned int16 eprom_oku(unsigned int8 hafizadegeri)
{
   unsigned int16 eprom_degisken;
   eprom_degisken=read_eeprom(hafizadegeri+1);
   eprom_degisken=eprom_degisken<<8;
   eprom_degisken=eprom_degisken+read_eeprom(hafizadegeri);
   return eprom_degisken;
}

void sayiyaz(unsigned int8 x, unsigned int8 y, unsigned int16 sayidegeri,unsigned int16 renk1, unsigned int16 renk2 )
{
   char yazidegeri[6];
   unsigned int8 w;
   sprintf(yazidegeri ,"\%ld",sayidegeri);
   yazidegeri[4]='\0';
   if (sayidegeri<1000)
   {
      yazidegeri[3]='\0';
       w=32;
   }
   if (sayidegeri<100)
   {
      yazidegeri[2]='\0';
       w=24;
   }
   if (sayidegeri<10)
   {
      yazidegeri[1]='\0';
       w=16;
   }
   glcd_text8(x, y, yazidegeri,renk1,renk2);
}

void ana_dongu() 
{
  unsigned char oyunalani = yukseklik - zeminyuk;
  cls(arkaplan);
  line(0, oyunalani, genislik,oyunalani, black);

  fillRectangle(0, oyunalani+1, yarigenislik, cimenyuk+oyunalani+1, cimenrengi2);

  fillRectangle(yarigenislik, oyunalani+1, genislik, cimenyuk+oyunalani+1, cimenrengi1);

  line(0, oyunalani+cimenyuk, genislik, oyunalani+cimenyuk,black);

  fillRectangle(0, oyunalani+cimenyuk+1, genislik, yukseklik, zeminrengi);
  signed int16 cimenx = genislik;
  int loops;
  int1 boru_gecildi = false;
  unsigned char px;
  skor=0;
  while (1) 
  {
    loops = 0;
    {
      if(! input(yukaritus) )
      {
        if (kus.y > kusyuk2*0.5) kus.vel_y = -ziplama_hizi;
        else kus.vel_y = 0;
        delta=0;
      }

      delta += 0.005;
      kus.vel_y += yercekimi * delta;
      kus.y += kus.vel_y;

      boru.x -= hiz;
      if (boru.x < -borugen) 
      {
        boru.x = genislik;
        boru.bosluk_y = rand()+10;
      }
      loops++;
    }

  
    if (boru.x >= 0 && boru.x < genislik) 
    {

      line(boru.x+3, 0,boru.x+3, boru.bosluk_y-1, borurengi);
      line(boru.x+3, boru.bosluk_y+boruboslugu+1,boru.x+3, oyunalani-1, borurengi);

      line(boru.x, 0,boru.x, boru.bosluk_y-1, boruparlakrengi);
      line(boru.x, boru.bosluk_y+boruboslugu+1, boru.x,oyunalani-1, boruparlakrengi);

      Pixel(boru.x, boru.bosluk_y, borucizgirengi);
      Pixel(boru.x, boru.bosluk_y-6, borucizgirengi);
      Pixel(boru.x, boru.bosluk_y+boruboslugu, borucizgirengi);
      Pixel(boru.x, boru.bosluk_y+boruboslugu+6, borucizgirengi);
      Pixel(boru.x+3, boru.bosluk_y-6, borucizgirengi);
      Pixel(boru.x+3, boru.bosluk_y+boruboslugu+6, borucizgirengi);
    }

    if (boru.x <= genislik) line(boru.x+borugen, 0, boru.x+borugen,oyunalani-1, arkaplan);

    tmpx = kusgen-1;
    do 
    {
          px = kus.x+tmpx+kusgen;
 
          tmpy = kusyuk - 1;
          do 
          {
            Pixel(px, kus.old_y + tmpy, arkaplan);
          } while (tmpy--);
          tmpy = kusyuk - 1;
          do 
          {
            Pixel(px, kus.y + tmpy, kuspixel[tmpx + (tmpy * kusgen)]);
          } while (tmpy--);
    } while (tmpx--);

    kus.old_y = kus.y;

    cimenx -= hiz;
    if (cimenx < 0) cimenx = genislik;
    line( cimenx %genislik, oyunalani+1, cimenx %genislik,oyunalani+cimenyuk-1, cimenrengi2);
    line((cimenx+64)%genislik, oyunalani+1, (cimenx+64)%genislik,oyunalani+cimenyuk-1, cimenrengi1);

    if (kus.y > oyunalani-kusyuk) break;

    if (kus.x+kusgen >= boru.x-kusgen2 && kus.x <= boru.x+borugen-kusgen) 
    {
      if (kus.y < boru.bosluk_y || kus.y+kusyuk > boru.bosluk_y+boruboslugu) break;
      else boru_gecildi = true;
    }
    else if (kus.x > boru.x+borugen-kusgen && boru_gecildi) 
    {
      boru_gecildi = false;
      skor++;
    }
    sayiyaz(yarigenislik,4,skor,black,arkaplan);
  }
  delay_ms(1200);
}

void yeniden_baslat() 
{
  skor = 0;
  kus.x = 20;
  kus.y = kus.old_y = yariyukseklik - kusyuk;
  kus.vel_y = -ziplama_hizi;
  tmpx = tmpy = 0;
  boru.x = genislik;
  boru.bosluk_y = (rand()*40/43)+20;
  rekor=read_eeprom(0);
  if(rekor==255) rekor=0;
}


void oyun_baslat() {
  cls(krem);
  rectangle(0,0,genislik-1,yukseklik-1,red);
  fillRectangle(10, yariyukseklik - 20, genislik-20, 1, white);
  fillRectangle(10, yariyukseklik + 32, genislik-20, 1, white);
  glcd_text_RGB(yarigenislik-(11*4),yariyukseklik - 16,flappybird,krem,1);
  glcd_text_RGB(yarigenislik - (14*4),yariyukseklik +34,butonabasin,krem,2);
  while (1) 
  {
    if (! input(yukaritus) ) break;
  }
  yeniden_baslat();
}


void oyun_sonu() 
{
   unsigned int8 skorsol,rekorsol;
  cls(orange);
  if(skor>rekor)
  {
      rekor=skor;
      write_eeprom(0,rekor);
  }
  if(skor>10)
  {
      skortextsol=28;
      skorsol=84;
  }
  else
  {
      skortextsol=32;
      skorsol=88;
  }
  if(rekor>10)
  {
      rekortextsol=28;
      rekorsol=84;
  }
  else
  {
      rekortextsol=28;
      rekorsol=92;
  }
  rectangle(0,0,genislik-1,yukseklik-1,blue);
  glcd_text_RGB(skortextsol,20,skortext,orange,2);
  glcd_text_RGB(rekortextsol,40,rekortext,orange,3);
  sayiyaz(skorsol, 20 ,skor,black,orange);
  sayiyaz(rekorsol, 40 ,rekor,black,orange);
  glcd_text_RGB(yarigenislik - (13*4),yariyukseklik + 12,baslamakicin,orange,1);
  glcd_text_RGB(yarigenislik - (14*4),yariyukseklik + 32,butonabasin,orange,3);
  while (1) 
  {
    if (! input(yukaritus) ) break;
  }
}


//**********************************************************************************************************************************************************************
 void main()
{
   set_tris_a(0b00111);
   output_high(ss);
  
   port_b_pullups(TRUE); 
   SET_TRIS_B(255);
   set_tris_c(0);
   output_c(0b00000000);
   OSCTUNE = 0x40;
   OSCCON = 0x7C;
   CONFIG1H = 0x00 ;
   setup_spi(SPI_MASTER | SPI_MODE_0 | SPI_SAMPLE_AT_END | SPI_SCK_IDLE_LOW | SPI_CLK_DIV_4);
   lcd_init();
   cls(arkaplan);
 //  unsigned int16 rxx=60;
 //  rxx=renkhesapla(rxx);
 //  sayiyaz(10,10,rxx,red,arkaplan);
//   delay_ms(5000);
   oyun_baslat();
   while(1)
   {
      ana_dongu();
      oyun_sonu();
      yeniden_baslat();
   }
 
}


