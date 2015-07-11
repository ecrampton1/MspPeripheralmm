#ifndef _UTILITIES_HPP
#define _UTILITIES_HPP


namespace McuPeripheral {

enum class Base : int
{
	BASE_BIN = 2,
	BASE_DEC = 10,
	BASE_HEX = 16

};

static void itoa(long unsigned int value, char* result, Base base)
{
  // check that the base if valid
  //if (base < 2 || base > 36) { *result = '\0';}
	int b = static_cast<int>(base);

  char* ptr = result, *ptr1 = result, tmp_char;
  int tmp_value;

  do {
	tmp_value = value;
	value /= b;
	*ptr++ = "fedcba9876543210123456789abcdef" [15 + (tmp_value - value * b)];
  } while ( value );

  // Apply negative sign
  if (tmp_value < 0) *ptr++ = '-';
  *ptr-- = '\0';
  while(ptr1 < ptr) {
	tmp_char = *ptr;
	*ptr--= *ptr1;
	*ptr1++ = tmp_char;
  }

}


void
ftoa(float f,char *buf,int dplaces)
{
    int pos=0,ix,dp,num,loop;

    if (f<0)
    {
        buf[pos++]='-';
        f = -f;
    }
    dp=0;

    loop = dplaces + 2;

    while (f>=10.0)
    {
        f=f/10.0;
        dp++;
        loop++;
    }
//    for (ix=1;ix<8;ix++)
    for (ix=1;ix<loop;ix++)
    {
            num = f;
            f=f-num;
            if (num>9)
                buf[pos++]='#';
            else
                buf[pos++]='0'+num;
            if (dp==0) buf[pos++]='.';
            f=f*10.0;
            dp--;
    }

    buf[pos] = '\0'; // null term
}


}

#endif //_UTILITIES_HPP
