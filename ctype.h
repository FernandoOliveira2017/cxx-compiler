#ifndef _ssaop_h_
#define _ssaop_h_

#define ctype		0x00001
#define function	0x00002
#define pointer		0x00003
#define array		0x00004
#define cvoid		0x00002
#define cchar		0x00004
#define cint		0x00008
#define cfloat		0x00010
#define cdouble		0x00020
#define csigned		0x00040
#define	cunsigned	0x00080
#define cconst		0x00100
#define cvolatile	0x00200
#define	cshort		0x00400
#define clong		0x00800
#define clonglong	0x01000
#define cstatic		0x02000
#define cextern		0x04000
#define cauto		0x08000
#define cregister	0x10000

#define c_void						0x00
#define c_char						0x01
#define c_unsigned_char				0x02
#define c_short_int					0x03
#define c_unsigned_short_int		0x04
#define c_int						0x05
#define c_unsigned_int				0x06
#define c_long_int					0x07
#define c_unsigned_long_int			0x08
#define c_long_long_int				0x09
#define c_unsigned_long_long_int	0x0A
#define c_float						0x0B
#define c_double					0x0C
#define c_long_double				0x0D
#define c_static					0x0E
#define c_extern					0x0F
#define c_auto						0x10
#define c_register					0x11
#define c_const						0x12
#define c_volatile					0x13

#define basic_type(a)				a & 0x3E
#define is_fp(a)					a == c_float || a == c_double || a == c_long_double

#endif
