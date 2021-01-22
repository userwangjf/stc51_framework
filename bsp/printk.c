

#include "bsp/config.h"

#include "bsp/printk.h"


#if 1

//输出一个字符到串口,并等待完成
static u8 _putc(u8 ch)
{
	SBUF = ch;
	while(TI==0);   
	TI=0;
	return ch;
}

static u8 _strlen(const u8 *s)
{
	u8 len = 0;
	while (s[len] != '\0') len++;
	return len;
}

static u8 _itoa(u32 value, u8 radix, u8 uppercase, u8 unsig,
	 u8 *buffer, u8 zero_pad)
{
	u8 *pbuffer = buffer;
	u8 negative = 0;
	u8 i, len;

	/* No support for unusual radixes. */
	if (radix > 16)
		return 0;

	if (value < 0 && !unsig) {
		negative = 1;
		value = -value;
	}

	/* This builds the string back to front ... */
	do {
		u8 digit = value % radix;
		*(pbuffer++) = (digit < 10 ? '0' + digit : (uppercase ? 'A' : 'a') + digit - 10);
		value /= radix;
	} while (value > 0);

	for (i = (pbuffer - buffer); i < zero_pad; i++)
		*(pbuffer++) = '0';

	if (negative)
		*(pbuffer++) = '-';

	*(pbuffer) = '\0';

	/* ... now we reverse it (could do it recursively but will
	 * conserve the stack space) */
	len = (pbuffer - buffer);
	for (i = 0; i < len / 2; i++) {
		char j = buffer[i];
		buffer[i] = buffer[len-i-1];
		buffer[len-i-1] = j;
	}

	return len;
}


//输出一个字符串
static u8 _puts(u8 *s, u8 len)
{
	u8 i;

	for (i = 0; i < len; i++) {
		_putc(*s);
		s++;
	}

	return i;
}

u8 _vsprintk(const u8 *fmt, va_list va)
{
	u8 bf[24];
	u8 ch;

	while ((ch=*(fmt++))) {
		if (ch!='%')
			_putc(ch);
		else {
			u8 zero_pad = 0;
			u8 *ptr;
			u8 len;

			ch=*(fmt++);

			/* Zero padding requested */
			if (ch=='0') {
				ch=*(fmt++);
				if (ch == '\0')
					goto end;
				if (ch >= '0' && ch <= '9')
					zero_pad = ch - '0';
				ch=*(fmt++);
			}

			switch (ch) {
				case 0:
					goto end;

				case 'u':
				case 'd':
					len = _itoa(va_arg(va, u32), 10, 0, (ch=='u'), bf, zero_pad);
					_puts(bf, len);
					break;

				case 'x':
				case 'X':
					len = _itoa(va_arg(va, u32), 16, (ch=='X'), 1, bf, zero_pad);
					_puts(bf, len);
					break;

				case 'c' :
					_putc((char)(va_arg(va, u8)));
					break;

				case 's' :
					ptr = (u8*)va_arg(va, u8*);
					_puts(ptr, _strlen(ptr));
					break;

				default:
					_putc(ch);
					break;
			}
		}
	}
end:
	return 0;
}

//非u32的整数，需要转换为u32，才能正常显示
u8 printk(const u8 *fmt, ...)
{
	u8 ret;
	va_list va;

	va_start(va, fmt);
	ret = _vsprintk(fmt, va);
	va_end(va);

	return ret;
}

#endif

