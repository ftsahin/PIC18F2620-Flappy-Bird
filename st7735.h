#define BPP         16                  // Bits per pixel   
#define ST7735_NOP 0x0
#define ST7735_SWRESET 0x01
#define ST7735_RDDID 0x04
#define ST7735_RDDST 0x09
 
#define ST7735_SLPIN  0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON  0x12
#define ST7735_NORON  0x13
 
#define ST7735_INVOFF 0x20
#define ST7735_INVON 0x21
#define ST7735_DISPOFF 0x28
#define ST7735_DISPON 0x29
#define ST7735_CASET 0x2A
#define ST7735_RASET 0x2B
#define ST7735_RAMWR 0x2C
#define ST7735_RAMRD 0x2E
 
#define ST7735_COLMOD 0x3A
#define ST7735_MADCTL 0x36
 
 
#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR 0xB4
#define ST7735_DISSET5 0xB6
 
#define ST7735_PWCTR1 0xC0
#define ST7735_PWCTR2 0xC1
#define ST7735_PWCTR3 0xC2
#define ST7735_PWCTR4 0xC3
#define ST7735_PWCTR5 0xC4
#define ST7735_VMCTR1 0xC5
 
#define ST7735_RDID1 0xDA
#define ST7735_RDID2 0xDB
#define ST7735_RDID3 0xDC
#define ST7735_RDID4 0xDD
 
#define ST7735_PWCTR6 0xFC
 
#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1


#define MADCTL_MY  0x80

#define MADCTL_MX  0x40

#define MADCTL_MV  0x20

#define MADCTL_ML  0x10

#define MADCTL_RGB 0x00

#define MADCTL_MH  0x04

#define SPI_MODE_0  (SPI_L_TO_H | SPI_XMIT_L_TO_H)   //    low       leading edge
#define SPI_MODE_1  (SPI_L_TO_H)                     //    low       trailing edge
#define SPI_MODE_2  (SPI_H_TO_L)                     //    high      leading edge
#define SPI_MODE_3  (SPI_H_TO_L | SPI_XMIT_L_TO_H)   //    high      trailing edge


#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define purple  0xF81F 
#define lightgray 0xE71C
#define orange  0xFCC0
#define visne   0x80C3
#define krem    0xE717

#define sagtus pin_b0
#define yukaritus pin_b1
#define asagitus pin_b2
#define oktusu pin_b3


#define reset pin_c0 
#define rs pin_c1     
#define cs pin_c7
#define sclk pin_c3 //
#define sda pin_c5  //
#define ss pin_a5
/*
#define gen 127
#define yuk 159
#define gen2 64
#define yuk2 80
*/
unsigned int8 uz;
unsigned int16 skala,renkrgb;
unsigned int16 r1,g1,b1;
unsigned int16 yenirenk;
unsigned int16 gen=127;
unsigned int16 yuk=159;
unsigned int16 gen2=128;
unsigned int16 yuk2=160;
/*unsigned int16 color565(unsigned int8 r_,unsigned int8 g_,unsigned int8 b_)
{
   return ((b_ & 0xF8) << 8) | ((g_ & 0xFC) << 3) | (r_ >> 3);
}
*/
void write_spi_byte(unsigned char c)
 {
   // using software SPI
   char x;
   for(x=0;x<8;x++){       //clock the byte out
      output_low(sclk);
      output_low(sda);
      if(c & 0x80)
      output_high(sda);
      output_high(sclk);
      c <<= 1;
   }
 }

void write_spi_word(unsigned int16 c)
 {
   char x;
   for(x=0;x<16;x++)
   {
      output_low(sclk);
      output_low(sda);
      if(c & 0x8000)
      output_high(sda);
      output_high(sclk);
      c <<= 1;
   }
 }
 
 
void wr_cmd(int cmd) 
{
    output_low(rs); // rs low, cs low for transmitting command
    output_low(cs);
    spi_write(cmd);
    output_high(cs);
}
  
void wr_dat(int dat) 
{
    output_high(rs); // rs high, cs low for transmitting data
    output_low(cs);                         
      spi_write(dat);
    output_high(cs);
}
  
void wr_reg (unsigned char reg, unsigned short val) 
{
    wr_cmd(reg);
    wr_dat(val);
}
 
 void window (unsigned int x, unsigned int y, unsigned int w, unsigned int h) {
  wr_cmd(ST7735_CASET);  // column addr set
  wr_dat(0x00);
  wr_dat(x);   // XSTART 
  wr_dat(0x00);
  wr_dat(x+w+1);   // XEND
 
  wr_cmd(ST7735_RASET);  // row addr set
  wr_dat(0x00);
  wr_dat(y);    // YSTART
  wr_dat(0x00);
  wr_dat(y+h+1);    // YEND
 
  wr_cmd(ST7735_RAMWR);  // write to RAM
}


void pixel(int x, int y, unsigned int16 color) 
{
  //if ((x >= 128) || (y >= 128)) return;
  window(x,y,x,y);
  output_high(rs); 
  output_low(cs);
  //write_spi_word(color); 
  spi_write(color>>8);
  spi_write(color);
  output_high(cs);
}

void Line(unsigned int16 x1, unsigned int16 y1, unsigned int16 x2, unsigned int16 y2, unsigned int16 color)
{
   unsigned int16        dy, dx;
   signed int8  addx=1, addy=1;
   signed int16 P, diff;

   unsigned int16 i=0;
   dx = abs((signed int16)(x2 - x1));
   dy = abs((signed int16)(y2 - y1));

   if(x1 > x2)
      addx = -1;
   if(y1 > y2)
      addy = -1;

   if(dx >= dy)
   {
      dy *= 2;
      P = dy - dx;
      diff = P - dx;

      for(; i<=dx; ++i)
      {
         pixel(x1, y1, color);

         if(P < 0)
         {
            P  += dy;
            x1 += addx;
         }
         else
         {
            P  += diff;
            x1 += addx;
            y1 += addy;
         }
      }
   }
   else
   {
      dx *= 2;
      P = dx - dy;
      diff = P - dy;

      for(; i<=dy; ++i)
      {
         pixel(x1, y1, color);

         if(P < 0)
         {
            P  += dx;
            y1 += addy;
         }
         else
         {
            P  += diff;
            x1 += addx;
            y1 += addy;
         }
      }
   }
}

void Rectangle(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, unsigned int16 colour)
{
    line(x1,y1,x1,y2,colour);
    line(x1,y1,x2,y1,colour);
    line(x1,y2,x2,y2,colour);
    line(x2,y1,x2,y2,colour);
}

void FillRectangle(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, unsigned int16 color)
{
    int x, y;
    window(x1,y1,x2-x1,y2-y1);
    //wr_cmd(0x2C);
    output_high(rs);
    output_low(cs);
    for (x=x1; x<x2; x++)
    {
        for (y=y1; y<y2; y++)
        {
            spi_write(color>>8);
            spi_write(color);
        }
    }
    output_high(cs);
}


void cls(unsigned int16 colour)
{
    int x, y;

    window(0,0,gen+1,yuk+1);
    wr_cmd(0x2C);
    output_high(rs);
    output_low(cs);
    for (x=0; x<gen2; x++)
        for (y=0; y<yuk2; y++)
        {
            //write_spi_word(colour);
            spi_write(colour>>8);
            spi_write(colour);
        }
    output_high(cs);
}

void LCD_Init(void)
{
    output_low(reset);
    delay_ms(500);
    output_high(reset);
    delay_ms(500);
   
    wr_cmd(0x11);//Sleep out
   delay_ms(120);
   //ST7735R Frame Rate
   wr_cmd(0xB1);
   wr_dat(0x01);
   wr_dat(0x2C);
   wr_dat(0x2D);
   wr_cmd(0xB2);
   wr_dat(0x01);
   wr_dat(0x2C);
   wr_dat(0x2D);
   wr_cmd(0xB3);
   wr_dat(0x01);
   wr_dat(0x2C);
   wr_dat(0x2D);
   wr_dat(0x01);
   wr_dat(0x2C);
   wr_dat(0x2D);
   //------------------------------------End ST7735R Frame Rate-----------------------------------------//
   wr_cmd(0xB4);//Column inversion
   wr_dat(0x07);
   //------------------------------------ST7735R Power Sequence-----------------------------------------//
   wr_cmd(0xC0);
   wr_dat(0xA2);
   wr_dat(0x02);
   wr_dat(0x84);
   wr_cmd(0xC1);
   wr_dat(0xC5);
   wr_cmd(0xC2);
   wr_dat(0x0A);
   wr_dat(0x00);
   wr_cmd(0xC3);
   wr_dat(0x8A);
   wr_dat(0x2A);
   wr_cmd(0xC4);
   wr_dat(0x8A);
   wr_dat(0xEE);
   //---------------------------------End ST7735R Power Sequence-------------------------------------//
   wr_cmd(0xC5);//VCOM
   wr_dat(0x0E);
   wr_cmd(0x36);//MX, MY, RGB mode
   wr_dat(0x00);
   //------------------------------------ST7735R Gamma Sequence-----------------------------------------//
   wr_cmd(0xe0);
   wr_dat(0x02);
   wr_dat(0x1c);
   wr_dat(0x07);
   wr_dat(0x12);
   wr_dat(0x37);
   wr_dat(0x32);
   wr_dat(0x29);
   wr_dat(0x2d);
   wr_dat(0x29);
   wr_dat(0x25);
   wr_dat(0x2b);
   wr_dat(0x39);
   wr_dat(0x00);
   wr_dat(0x01);
   wr_dat(0x03);
   wr_dat(0x10);
   wr_cmd(0xe1);
   wr_dat(0x03);
   wr_dat(0x1d);
   wr_dat(0x07);
   wr_dat(0x06);
   wr_dat(0x2e);
   wr_dat(0x2c);
   wr_dat(0x29);
   wr_dat(0x2d);
   wr_dat(0x2e);
   wr_dat(0x2e);
   wr_dat(0x37);
   wr_dat(0x3f);
   wr_dat(0x00);
   wr_dat(0x00);
   wr_dat(0x02);
   wr_dat(0x10);
   wr_cmd(0x2A);
   wr_dat(0x00);
   wr_dat(0x02);
   wr_dat(0x00);
   wr_dat(0x81);

   wr_cmd(0x2B);
   wr_dat(0x00);
   wr_dat(0x01);
   wr_dat(0x00);
   wr_dat(0xA0);
   //------------------------------------End ST7735R Gamma Sequence-----------------------------------------//

    wr_cmd(0x3A);
    wr_dat(0x05);
    wr_cmd(0x29);//Display on
}

void circle(unsigned int16 x, unsigned int16 y, unsigned int16 radius, int1 fill, unsigned int16 color)
{
   signed int16 a, b, P;

   a = 0;
   b = radius;
   P = 1 - radius;

   do
   {
      if(fill)
      {
         line(x-a, y+b, x+a, y+b, color);
         line(x-a, y-b, x+a, y-b, color);
         line(x-b, y+a, x+b, y+a, color);
         line(x-b, y-a, x+b, y-a, color);
      }
      else
      {
         pixel(a+x, b+y, color);
         pixel(b+x, a+y, color);
         pixel(x-a, b+y, color);
         pixel(x-b, a+y, color);
         pixel(b+x, y-a, color);
         pixel(a+x, y-b, color);
         pixel(x-a, y-b, color);
         pixel(x-b, y-a, color);
      }

      if(P < 0)
         P += 3 + 2 * a++;
      else
         P += 5 + 2 * (a++ - b--);
    } while(a <= b);
}

void fillcircle(unsigned int16 x, unsigned int16 y, unsigned int16 radius, unsigned int16 color)
{
   signed int16 a, b, P;

   a = 0;
   b = radius;
   P = 1 - radius;

   do
   {
         line(x, y+b, x+a, y+b, color);
         line(x, y-b, x+a, y-b, color);
         line(x, y+a, x+b, y+a, color);
         line(x, y-a, x+b, y-a, color);
     
      if(P < 0)
         P += 3 + 2 * a++;
      else
         P += 5 + 2 * (a++ - b--);
    } while(a <= b);
}

void glcd_text(unsigned int16 x_origin, unsigned int16 y_origin, char* textptr, unsigned int16 fgcolour, unsigned int16 bgcolour)
{
   unsigned int16 info_offset;
   int chr_width;
   int chr_bytes;
   unsigned int16 chr_hight;
   unsigned int16 chr_offset;

   int i,j,k;
   unsigned int16 x,y;

   chr_hight = font[6];                                             //the hight is always stored in the 6th bit
   
   while(*textptr != '\0')
      {
         if(*textptr == ' ')                                          //found a space, we will just insert a few lines
            {
               fillrectangle(x_origin,y_origin,x_origin+26,y_origin+24,bgcolour);
               x_origin = x_origin + 5;                              //move to the next chr position   
            }
         else
            {
               info_offset = (((unsigned int16)*textptr - 33) * 4) + 8;      //work out how far into the font array is the info about the current chr skipping the fist 8 byts in the table
               chr_width = font[info_offset];                           //the first byte contanes the width information, this is only how wide the chr is without padding
               chr_offset = make16(font[info_offset+2], font[info_offset+1]);    //The next two bytes contane the offset in the table to the bitmap of the chr

                 chr_bytes = chr_width / 8;                                    //work our how many bytes wide the char is
                  if (chr_width  % 8)
                  {
                     chr_bytes++;
                  }
      
                  x = x_origin;                                                         //save the start x position
                  y = y_origin;                                                         //save the start y position
      
                  for(i=0;i<chr_hight;i++)                                             //loop thought vertical bytes
                     {
                        for(j=0;j<chr_bytes;j++)                                       //loop thought horizontal bytes                              
                           {
                              for(k=0;k<8;k++)                                       //loop though each bit
                                 {
                                    if(bit_test(font[chr_offset],k))         
                                       {
                                          pixel(x,y,fgcolour);                           //bit is set, make the pixel the text colour
                                       }
                                    else
                                       {
                                          pixel(x,y,bgcolour);                           //bit is not set, make it the background colour
                                       }
                                    x++;                                          //move to the next vertical line
                                 }
                              chr_offset++;                                          //move to the next byte
                           }      
                        x = x_origin;                                                //move the x origin to start drawing the next horizontal bytes
                        y++;                                                      //move down to the next row
                     }
      
               x_origin = x_origin + chr_width + 1;                           //move to the next chr position                              
            }
         textptr++;                                                      //move to next char in string
      }  
      uz=x_origin;
}

unsigned int8 font24uzunlugu(char* textptr)
{
   unsigned int16 x_origin=0;
   unsigned int16 y_origin=0;
   unsigned int16 info_offset;
   int chr_width;
   int chr_bytes;
   unsigned int16 chr_hight;
   unsigned int16 chr_offset;

   int i,j,k;
   unsigned int16 x,y;

   chr_hight = font[6];                                             //the hight is always stored in the 6th bit
   
   while(*textptr != '\0')
      {
         if(*textptr == ' ')                                          //found a space, we will just insert a few lines
            {
               //fillrectangle(x_origin,y_origin,x_origin+26,y_origin+24,bgcolour);
               x_origin = x_origin + 5;                              //move to the next chr position   
            }
         else
            {
               info_offset = (((unsigned int16)*textptr - 33) * 4) + 8;      //work out how far into the font array is the info about the current chr skipping the fist 8 byts in the table
               chr_width = font[info_offset];                           //the first byte contanes the width information, this is only how wide the chr is without padding
               chr_offset = make16(font[info_offset+2], font[info_offset+1]);    //The next two bytes contane the offset in the table to the bitmap of the chr

                 chr_bytes = chr_width / 8;                                    //work our how many bytes wide the char is
                  if (chr_width  % 8)
                  {
                     chr_bytes++;
                  }
      
                  x = x_origin;                                                         //save the start x position
                  y = y_origin;                                                         //save the start y position
      
                  for(i=0;i<chr_hight;i++)                                             //loop thought vertical bytes
                     {
                        for(j=0;j<chr_bytes;j++)                                       //loop thought horizontal bytes                              
                           {
                              for(k=0;k<8;k++)                                       //loop though each bit
                                 {
                                    if(bit_test(font[chr_offset],k))         
                                       {
                                          //pixel(x,y,fgcolour);                           //bit is set, make the pixel the text colour
                                       }
                                    else
                                       {
                                          //pixel(x,y,bgcolour);                           //bit is not set, make it the background colour
                                       }
                                    x++;                                          //move to the next vertical line
                                 }
                              chr_offset++;                                          //move to the next byte
                           }      
                        x = x_origin;                                                //move the x origin to start drawing the next horizontal bytes
                        y++;                                                      //move down to the next row
                     }
      
               x_origin = x_origin + chr_width + 1;                           //move to the next chr position                              
            }
         textptr++;                                                      //move to next char in string
      }  
      return x_origin;
}


void glcd_text8(unsigned int x0, unsigned int y0,unsigned char *s, unsigned int16 color, unsigned int16 bgcolor)
{
    int i,j,k,x,y,xx;
    unsigned char qm;
    long int ulOffset;
    char  ywbuf[32],temp[2];
    char *c;
    c=s;
    int slength;
    slength=0;
    while(*c != '\0')
      {
         c++;
         slength++;
      }
    for(i = 0; i<slength;i++)
    {
        if(((unsigned char)(*(s+i))) >= 161)
        {
            temp[0] = *(s+i);
            temp[1] = '\0';
            return;
        }
        else
        {
            qm = *(s+i);
            ulOffset = (long int)(qm) * 16;
            for (j = 0; j < 16; j ++)
            {
                ywbuf[j]=Font2[ulOffset+j];
            }
            for(y = 0;y < 16;y++)
            {
                for(x=0;x<8;x++)
                {
                    k=x % 8;
                    if(ywbuf[y]&(0x80 >> k))
                    {
                        xx=x0+x+i*8;
                        Pixel(xx, y+y0, color);
                    }
                    else
                    {
                        xx=x0+x+i*8;
                        Pixel(xx, y+y0, bgcolor);
                    }
                }
           }

        }
    }
}

void setRotation(int8 m) {
   int8 rotation;
  wr_cmd(ST7735_MADCTL);
  rotation = m % 4; // can't be higher than 3
  switch (rotation) {
   case 0:
    wr_dat(MADCTL_MX | MADCTL_MY | MADCTL_RGB);
    //_width  = ST7735_TFTWIDTH;
    //_height = ST7735_TFTHEIGHT;
    break;
   case 1:
   wr_dat(MADCTL_MY | MADCTL_MV | MADCTL_RGB);
    //_width  = ST7735_TFTHEIGHT;
   // _height = ST7735_TFTWIDTH;
    break;
   case 2:
    wr_dat(MADCTL_RGB);
    //_width  = ST7735_TFTWIDTH;
    //_height = ST7735_TFTHEIGHT;
    break;
   case 3:
    wr_dat(MADCTL_MX | MADCTL_MV | MADCTL_RGB);
    //_width  = ST7735_TFTHEIGHT;
   // _height = ST7735_TFTWIDTH;
    break;
  }
}



void sayiyaz2(unsigned int8 x, unsigned int8 y, unsigned int16 sayidegeri,unsigned int16 renk1, unsigned int16 renk2 )
{
   char yazidegeri[6];
   unsigned int8 w;
   sprintf(yazidegeri ,"\%ld",sayidegeri);
   //yazidegeri[6]='\0';
   if (sayidegeri<100000)
   {
      yazidegeri[5]='\0';
       w=32;
   }
   if (sayidegeri<10000)
   {
      yazidegeri[4]='\0';
       w=32;
   }
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
unsigned int16 color565(unsigned int16 r_,unsigned int16 g_,unsigned int16 b_)
{
   
   r1=r_*31/255;
   r1*=2048;
  // if(r1>60000) 
  // {
   //cls(white);
   //delay_ms(2000);
  // }
   g1=g_*63/255;
   g1=g1*32;
   b1=b_*31/255;

   //cls(black);
   //sayiyaz2(0,0,r1,white,black);
   //delay_ms(1000);
   return (unsigned int16) (r1+g1+b1);
}

unsigned int16 renkhesapla(unsigned int16 rx,renkskala)
{
   
   
   if(rx<=gen2/2)
   {
      skala=rx;
   }
   else
   {
      skala=gen-rx;
   }
   renkrgb=skala*215/(gen2/2);
   if(renkskala==1)
   {
      yenirenk=color565(renkrgb,20,20);
   }
   else if(renkskala==2)
   {
      yenirenk=color565(20,renkrgb,20);
   } 
   else if(renkskala==3)
   {
      yenirenk=color565(20,20,renkrgb);
   }
   //yenirenk=color565(20,renkrgb,20);
   return (unsigned int16) yenirenk;
}



void glcd_text_RGB(unsigned int x0, unsigned int y0,unsigned char *s, unsigned int16 bgcolor, unsigned int renkskala)
{
    int i,j,k,x,y;
    unsigned int16 xx;
    unsigned char qm;
    long int ulOffset;
    char  ywbuf[32],temp[2];
    char *c;
    c=s;
    int slength;
    slength=0;
    while(*c != '\0')
      {
         c++;
         slength++;
      }
    for(i = 0; i<slength;i++)
    {
        if(((unsigned char)(*(s+i))) >= 161)
        {
            temp[0] = *(s+i);
            temp[1] = '\0';
            return;
        }
        else
        {
            qm = *(s+i);
            ulOffset = (long int)(qm) * 16;
            for (j = 0; j < 16; j ++)
            {
                ywbuf[j]=Font2[ulOffset+j];
            }
            for(y = 0;y < 16;y++)
            {
                for(x=0;x<8;x++)
                {
                    k=x % 8;
                    if(ywbuf[y]&(0x80 >> k))
                    {
                        xx=x0+x+i*8;
                        Pixel(xx, y+y0, renkhesapla(xx,renkskala));
                    }
                    else
                    {
                        xx=x0+x+i*8;
                        Pixel(xx, y+y0, bgcolor);
                    }
                }
           }

        }
    }
}



/*
unsigned int16 color565x(unsigned int8 r_,unsigned int8 g_,unsigned int8 b_)
{
   unsigned int16 r1,g1,b1;
   r1=r_*31/255;
   r1=r1<<11;
   g1=g_*31/255;
   g1=1<<11;
   b1=b_*31/255;
   b1=b1<<11;
   return r1+g1+b1;
}

 //inline uint16_t Color565(uint8_t r, uint8_t g, uint8_t b) {
 //          return ((b & 0xF8) << 8) | ((g & 0xFC) << 3) | (r >> 3);
 // }
 */
