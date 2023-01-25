#ifndef _token_h_
#define _token_h_

typedef enum {
#include "token.def"
} token;

#if !defined _lexer_h_
#define typespec(a)     a == token_void || a == token_char || a == token_int || a == token_float || a == token_double
#define modifier(a)     a == token_short || a == token_long || a == token_signed || a == token_unsigned
#define storage(a)      a == token_static || a == token_extern || a == token_auto || a == token_register
#define qualifier(a)    a == token_volatile || a == token_const
#define declaration(a)  typespec(a) || modifier(a) || storage(a) || qualifier(a)
#define brackets(a)     a == '{'
#define primary_expr(a)	a == integer_literal || a == single_floating || a == double_floating || a == character_constant || a == string_literal
#define post_expr(a)	a == token_inc || a == token_dec || a == '(' || a == '[' || a == token_pointer || a == '.'
#define prod_expr(a)    a == '*' || a == '/' || a == '%'
#define sum_expr(a)     a == '+' || a == '-'
#define shift_expr(a)   a == token_left_shift || a == token_right_shift
#define rel_expr(a)     a == '<' || a == token_less_equal || a == '>' || a == token_greater_equal
#define rel_eq_expr(a)  a == token_equal || a == token_not_equal
#define assign_expr(a)  a == '=' || a == token_add_assign || a == token_sub_assign || a == token_mul_assign || a == token_div_assign ||        \
                        a == token_mod_assign || a == token_left_shift_assign || a == token_right_shift_assign || a == token_bit_and_assign ||  \
                        a == token_xor_assign || a == token_bit_or_assign
#endif

#endif
