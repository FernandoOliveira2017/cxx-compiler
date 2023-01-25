#ifndef _ssaop_h_
#define _ssaop_h_

#define ident			0x01
#define value			0x02
#define csymbol			0x03
#define phi				0x04
#define store			0x05
#define cblock			0x06
#define if_then			0x07
#define cgoto			0x08
#define creturn			0x09
#define cargs			0x0A
#define ccall			0x0B
#define subscript		0x0C
#define pointer			0x0D
#define acess			0x0E
#define neg				0x0F
#define not				0x11
#define bitnot			0x12
#define deref			0x13
#define addressof		0x14
#define csizeof			0x15
#define mul				0x16
#define div				0x17
#define mod				0x18
#define add				0x19
#define sub				0x1A
#define left_shift		0x1B
#define right_shift		0x1C
#define less			0x1D
#define less_equal		0x1E
#define greater			0x1F
#define greater_equal	0x20
#define equal			0x21
#define not_equal		0x22
#define bitand			0x23
#define xor				0x24
#define bitor			0x25
#define and				0x26
#define or				0x27
#define ccond			0x28

#define is_op(a)		a >= ccall && a <= ccond
#define is_un_op(a)		a >= neg && a <= addressof
#define is_bin_op(a)	a >= mul && a <= or

#endif
