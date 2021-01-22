

#ifndef __TYPE_H__
#define __TYPE_H__

#ifdef UNIT_TEST

typedef 	unsigned char	u8;
typedef 	         char	s8;
typedef 	unsigned short  u16;
typedef 	         short	s16;
typedef 	unsigned int	u32;
typedef 			 int	s32;
typedef     unsigned char	bool;

#define NULL ((void *)0)

#else

typedef 	unsigned char	u8;
typedef 	         char	s8;
typedef 	unsigned int	u16;
typedef 	         int	s16;
typedef 	unsigned long	u32;
typedef 			 long	s32;
typedef     bit				bool;

#define NULL ((void *)0)

#endif

#endif

