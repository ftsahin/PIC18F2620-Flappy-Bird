


#ifdef PI
#undef  PI
#endif
#define PI     3.1415926535897932
#ifdef PI_DIV_BY_TWO
#undef PI_DIV_BY_TWO
#endif
#define PI_DIV_BY_TWO   1.5707963267948966
#ifdef TWOBYPI
#undef TWOBYPI
#define TWOBYPI          0.6366197723675813
#endif

float32 cos(float32 x)
{
   float32 y, t, t2 = 1.0;
   unsigned int8 quad, i;
   float32 frac;
   float32 p[4] = {
      -0.499999993585,
       0.041666636258,
      -0.0013888361399,
       0.00002476016134
   };

   if (x < 0) x = -x;                  // absolute value of input

   quad = (unsigned int8)(x / PI_DIV_BY_TWO);    // quadrant
   frac = (x / PI_DIV_BY_TWO) - quad;  // fractional part of input
   quad = quad % 4;                    // quadrant (0 to 3)

   if (quad == 0 || quad == 2)
      t = frac * PI_DIV_BY_TWO;
   else if (quad == 1)
      t = (1-frac) * PI_DIV_BY_TWO;
   else // should be 3
      t = (frac-1) * PI_DIV_BY_TWO;

   y = 0.999999999781;
   t = t * t;
   for (i = 0; i <= 3; i++)
   {
      t2 = t2 * t;
      y = y + p[i] * t2;
   }

   if (quad == 2 || quad == 1)
      y = -y;  // correct sign

   return (y);
}



float32 sin(float32 x)
{
   return cos(x - PI_DIV_BY_TWO);
}




