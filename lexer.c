#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "token.h"
#include "lexer.h"

#define max_identifier 2048

char identifier[max_identifier];
double number;
char * string;
char character;

char * filename = NULL;
FILE * source;
static char lookahead;
unsigned int line_number;

keyword reserved[7][7] =
{
#define _keyword(k, w) {k, w}
#include "keyword.def"
#undef _keyword
};

static bool next_char(char c);

#define next_char_2(a, b)			next_char(a) || next_char(b)

#define bin_base					2
#define oct_base					8
#define dec_base					10
#define hex_base					16
#define integer_suffix(c)			c == 'l' || c == 'L' ||								\
									c == 'u' || c == 'U'
#define space(c)        			c == ' ' || c == '\t' || c == '\n'
#define bindigit(c)					c == '0' || c == '1'
#define decdigit(c)					c >= '0' && c <= '9'
#define octdigit(c) 			    c >= '0' && c <= '7'
#define hexupper(c) 			    c >= 'A' && c <= 'F'
#define hexlower(c)					c >= 'a' && c <= 'f'
#define hexdigit(c)					hexupper(c) || hexlower(c) || decdigit(c)
#define digit(c, b)					b == bin_base ? bindigit(c) :						\
									b == oct_base ? octdigit(c) :						\
									b == dec_base ? decdigit(c) :						\
													hexdigit(c)
#define upper(c)    			    c >= 'A' && c <= 'Z'
#define lower(c)    			    c >= 'a' && c <= 'z'
#define special(c)  			    c == '$' || c == '_'
#define alpha(c)    			    upper(c) || lower(c) || special(c)
#define alphanum(c) 			    alpha(c) || decdigit(c)
#define inline(c)   			    c != EOF && c != '\n'

#define get_num_digit				get_next_char() - '0'
#define get_hex_digit				hexupper(lookahead) ? get_next_char() - 'A' + 10 :	\
									hexlower(lookahead) ? get_next_char() - 'a' + 10 :	\
														  get_num_digit
#define get_digit(b)				(b == hex_base ? get_hex_digit : get_num_digit)

#define operator_1(a, b)            next_char('=') ? a : b
#define operator_2(a, b, c)      next_char(a) ? b : operator_1(c, a)
#define operator_3(a, b, c, d)   next_char(a) ? operator_1(b, c) : operator_1(d, a)

char
get_next_char(void)
{
    char c = lookahead;
    
    if (lookahead != EOF) {
        lookahead = fgetc(source);
        if (c == '\n')
            line_number++;
    }
    
    return c;
}

static bool
next_char(char c)
{
    if (lookahead == c) {
        get_next_char();
        return true;
    } else
        return false;
}

static void
skip_spaces(void)
{
    do get_next_char();
    while (space(lookahead));
}

static void
skip_line_comment(void)
{
    while (inline(lookahead))
        get_next_char();
}

static void
skip_block_comment(void)
{
	char c;
	
	do if (lookahead == EOF)
		error("unterminated comment");
	else
		c = get_next_char();
	while (!(c == '*' && next_char('/')));
}

static token
get_word(int i, size_t size)
{
    for (int j = 0; j < size; j++)
        if (!strcmp(identifier, reserved[i][j].word))
            return reserved[i][j].kind;
    return token_ident;
}

static token
get_keyword(size_t size)
{
    switch (size) {
        case 2:
            return get_word(0, 2);
        case 3:
            return get_word(1, 3);
        case 4:
            return get_word(2, 7);
        case 5:
            return get_word(3, 5);
        case 6:
            return get_word(4, 5);
        case 7:
            return get_word(5, 1);
        case 8:
            return get_word(6, 4);
        default:
            return token_ident;
    }
}

static token
get_identifier(void)
{
    unsigned int i = 0;
    
    bzero(identifier, max_identifier);
    do if (i < max_identifier)
        identifier[i++] = get_next_char();
    else
        error("identifier too long");
    while (alphanum(lookahead));
    
    return get_keyword(i);
}

static unsigned
get_base(void)
{
    if (next_char('0'))
        if (next_char_2('b', 'B'))
			return bin_base;
        else if (octdigit(lookahead))
			return oct_base;
        else if (next_char_2('x', 'X'))
			return hex_base;
	return dec_base;
}

static int
get_number(unsigned base)
{
	int n = 0;
	
	if (base == bin_base && !(bindigit(lookahead)))
        error("`%c' is not bin digit", lookahead);
	else if (base == hex_base && !(hexdigit(lookahead)))
        error("`%c' is not hex digit", lookahead);
	while (digit(lookahead, base))
		n = n * base + get_digit(base);
	
	return n;
}

static double
get_mantissa(unsigned base)
{
	double i;
	double n = 0;
	
	for (i = base; digit(lookahead, base); i *= base)
		n += get_digit(base) / i;
	
	return n;
}

static double
get_exponent(unsigned base)
{
	int e;
	int n = 1;
	bool negate = false;
	
	if (next_char('+'));
	else if (next_char('-'))
		negate = true;
	if (!decdigit(lookahead))
		error("expected exponent digit");
	e = get_number(dec_base);
	if (base == bin_base)
		n <<= e;
	else for (int i = 0; i < e; i++)
		n *= 10;
	if (negate)
		return (double) 1 / n;
	else
		return n;
}

static token
get_integer_suffix(bool floating)
{
	token suffix;
	
	if (floating)
		suffix = double_floating;
	else
		suffix = integer_literal;
	while (integer_suffix(lookahead))
		if (next_char_2('l', 'L'))
			if (suffix & LL)
				error("more than two long suffixes");
			else if (suffix & L)
				if (floating)
					error("invalid long long suffix on floating point");
				else
					suffix |= LL;
			else
				suffix |= L;
		else if (next_char_2('u', 'U'))
			if (floating)
				error("invalid unsigned suffix on floating point");
			else if (suffix & U)
				error("two or more unsigned suffixes");
			else
				suffix |= U;
	
	return suffix;
}

static token
get_suffix(bool floating)
{
	if (next_char_2('f', 'F'))
		if (floating)
			return single_floating;
		else
			error("invalid float suffix on integer constant");
	else if (next_char_2('d', 'D'))
		if (floating)
			return double_floating;
		else
			error("invalid double suffix on integer constant");
	else
		return get_integer_suffix(floating);
}

static token
get_number_literal(bool floating)
{
	unsigned base = dec_base;
	
	if (floating)
		goto mantissa;
	base = get_base();
	number = get_number(base);
	if (base == bin_base)
		goto done;
    if (next_char('.')) {
mantissa:
		number += get_mantissa(base);
		floating = true;
	}
	if (base == hex_base) {
		if (next_char_2('p', 'P'))
			number *= get_exponent(bin_base);
		else if (floating)
			error("exponent required");
	} else if (next_char_2('e', 'E'))
		number *= get_exponent(dec_base);
done:
	return get_suffix(floating);
}

static char
get_hex_char(void)
{
	char c;
	
    if (!(hexdigit(lookahead)))
        error("`%c' is not hex digit", lookahead);
	c = get_hex_digit;
    if (hexdigit(lookahead))
        c = c * hex_base + (get_hex_digit);
    
    return c;
}

static char
get_oct_char(void)
{
    char c;
	
	c = get_num_digit;
    if (octdigit(lookahead))
        c = c * oct_base + get_num_digit;
    if (octdigit(lookahead))
        c = c * oct_base + get_num_digit;
    
    return c;
}

static char
get_character(void)
{
    if (next_char('\\'))
        if (next_char('a')) return '\a';
        else if (next_char('b')) return '\b';
        else if (next_char('f')) return '\f';
        else if (next_char('n')) return '\n';
        else if (next_char('r')) return '\r';
        else if (next_char('t')) return '\t';
        else if (next_char('v')) return '\v';
        else if (next_char('\\')) return '\\';
        else if (next_char('\'')) return '\'';
        else if (next_char('\"')) return '\"';
        else if (next_char('x')) return get_hex_char();
        else if (octdigit(lookahead)) return get_oct_char();
        else if (next_char('e')) return '\e';
        else error("unknown escape sequence: `\\%c'", lookahead);
    else
        return get_next_char();
}

static token
get_character_constant(void)
{
	if (!next_char('\'')) {
        if (inline(lookahead))
			character = get_character();
        if (!(inline(lookahead)))
            error("unterminated character constant");
        if (!next_char('\''))
            error("multi-character character constant");
    } else
        error("empty character constant");
    
    return character_constant;
}

static token
get_string_literal(void)
{
	int i;
	
	string = cc_alloc(sizeof(char));
	for (i = 0; !next_char('\"'); i++)
        if (inline(lookahead)) {
			string = cc_realloc(string, i+2);
			string[i] = get_character();
		} else
            error("unterminated string");
	string[i] = '\0';
	
	return string_literal;
}

token
lexan(void)
{
loop:
    if (space(lookahead)) {
        skip_spaces();
        goto loop;
    } else if (alpha(lookahead))
		return get_identifier();
    else if (decdigit(lookahead))
        return get_number_literal(false);
    else if (next_char('\''))
        return get_character_constant();
    else if (next_char('\"'))
        return get_string_literal();
    else if (next_char('('))
        return '(';
    else if (next_char(')'))
        return ')';
    else if (next_char('['))
        return '[';
    else if (next_char(']'))
        return ']';
    else if (next_char('{'))
        return '{';
    else if (next_char('}'))
        return '}';
    else if (next_char('.'))
        if (decdigit(lookahead))
			return get_number_literal(true);
		else if (next_char('.'))
			if (next_char('.'))
				return token_etc;
			else
				error("'..'");
		else
			return '.';
	else if (next_char('+'))
        return operator_2('+', token_inc, token_add_assign);
    else if (next_char('-'))
        if (next_char('>'))
            return token_pointer;
        else
            return operator_2('-', token_dec, token_sub_assign);
    else if (next_char('!'))
        return operator_1(token_not_equal, '!');
    else if (next_char('~'))
        return '~';
    else if (next_char('*'))
        return operator_1(token_mul_assign, '*');
    else if (next_char('&'))
        return operator_2('&', token_and, token_bit_and_assign);
    else if (next_char('/'))
        if (next_char('/')) {
            skip_line_comment();
            goto loop;
        } else if (next_char('*')) {
            skip_block_comment();
            goto loop;
        } else
            return operator_1(token_div_assign, '/');
    else if (next_char('%'))
        return operator_1(token_mod_assign, '%');
    else if (next_char('<'))
        return operator_3('<', token_left_shift_assign, token_left_shift, token_less_equal);
    else if (next_char('>'))
        return operator_3('>', token_right_shift_assign, token_right_shift, token_greater_equal);
    else if (next_char('^'))
        return operator_1(token_xor_assign, '^');
    else if (next_char('|'))
        return operator_2('|', token_or, token_bit_or_assign);
    else if (next_char('?'))
        return '?';
    else if (next_char(':'))
        return ':';
    else if (next_char('='))
        return operator_1(token_equal, '=');
    else if (next_char(','))
        return ',';
    else if (next_char(';'))
        return ';';
    else if (next_char(EOF))
        return eof;
    else
        error("unrecognized character `\\x%x'", lookahead);
}
