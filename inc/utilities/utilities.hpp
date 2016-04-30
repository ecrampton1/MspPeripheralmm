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

//slow but space saving divide
static int32_t divide_num(int32_t num, int divisor)
{
	if(num < divisor) { return 0; }
	return divide_num(num-divisor,divisor) + 1;
}

//TODO more optimization on this? can make int64_t but it doubles the size of a program.
static char *itoa(int32_t num, char *str, Base base)
{
  //long int value;
  int32_t value;
  char *sp = str;
  char *sp2;
  value = num;

  if(base == Base::BASE_HEX) {
	  *sp++ = '0';
	  *sp++ = 'x';
  }
  else if (base == Base::BASE_BIN) {
	  *sp++ = '0';
	  *sp++ = 'b';
  }
  else  {
	  if (0 > num) {
		*sp++ = '-';
		value = -num;
	  }
  }

  sp2 = sp;

  do {
    char rem = value % static_cast<int>(base);
    value = value / static_cast<int>(base);
    //value = divide_num(value,static_cast<int>(base));
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




#if 0
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
#endif

}

#endif //_UTILITIES_HPP
