#ifndef _UTILITIES_HPP
#define _UTILITIES_HPP


namespace McuPeripheral {

enum class Base : int
{
	BASE_BIN = 2,
	BASE_DEC = 10,
	BASE_HEX = 16

};

#if 0
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
#endif


//TODO more optimization on this?
//Due to bug in binutils https://www.sourceware.org/ml/binutils/2015-07/msg00319.html
//Can't get itoa working with 64 bit
static char *itoa(long unsigned int num, char *str, Base base)
{
  long unsigned int value;
  char *sp = str;
  char *sp2;

  value = num;
#if ! (UNSIGNED - 0)
  /* Store sign at start of buffer for negative base-10 values */
  if (10 == (int)base && 0 > num) {
    *sp++ = '-';
    value = -num;
  }
#endif
  sp2 = sp;

  do {
    char rem = value % (int)base;
    value /= (int)base;
    if (10 > rem) {
      *sp++ = '0' + rem;
    } else {
      *sp++ = 'A' + rem - 10;
    }
  } while (0 < value);

  /* Mark end of string */
  *sp-- = 0;

  /* Reverse string contents (excluding sign) in place */
  while (sp2 < sp) {
    char tmp = *sp2;
    *sp2++ = *sp;
    *sp-- = tmp;
  }

  return str;
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
