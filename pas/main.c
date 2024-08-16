#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<stdbool.h>
#include<ctype.h>
#include"pas.h"

static kwds_t kwds[] = {
	{ I_NOP, "nop" },
	{ I_MOV, "mov" },
	{ I_LDM, "ldm" },
	{ I_STR, "str" },
	{ I_ADD, "add" },
	{ I_SUB, "sub" },
	{ I_MUL, "mul" },
	{ I_DIV, "div" },
	{ I_SHL, "shl" },
	{ I_SHR, "shr" },
	{ I_ROR, "ror" },
	{ I_AND, "and" },
	{ I_OR, "or" },
	{ I_NOR, "nor" },
	{ I_XOR, "xor" },
	{ I_NOT, "not" },
	{ I_PUSH, "push" },
	{ I_POP, "pop" },
	{ I_PUSHA, "pusha" },
	{ I_POPA, "popa" },
	{ I_IN, "in" },
	{ I_OUT, "out" },
	{ I_B, "b" },
	{ I_BR, "br" },
	{ I_RTB, "rtb" }
};

tnode_t* expr( void );
int tnode_interp( tnode_t* node );
int math_op( int op );
tnode_t* int_lit( void );
char skip( void );
int ps_atoi( char c );
tnode_t* mknode( op_t op, tnode_t* lvalue, tnode_t* rvalue, long long value );
void delnode( tnode_t* node );
int next( void );
int scan( token_t* tok );
int scanint( char c );

char* g_buffer = 0;
token_t g_token = { 0 };
int g_line = 0;
tnode_t* g_ast = { 0 };
char g_putback = 0;

const char debug_tokentypes[ 6 ] = {
	'E', 'L', '+', '-', '*', '/'
};

int main( int argc, char** argv ) {
	if( argc != 2 ) exit( 1 );

	FILE* fp = fopen( argv[ 1 ], "r" );
	fseek( fp, 0, SEEK_END );
	long fpsz = ftell( fp );
	fseek( fp, 0, SEEK_SET );
	
	g_buffer = calloc( fpsz+1, sizeof( *g_buffer ) );
	fread( g_buffer, fpsz, 1, fp );
	fclose( fp );
	g_buffer[ fpsz ] = 0;

	/*
	int scanret = scan( &g_token );
	while( scanret ) {
		printf( "Token data:\n" );
		printf( "\tval = %d\n", g_token.val );
		printf( "\ttype = %d (%c)\n", g_token.type, debug_tokentypes[ g_token.type ] );
		scanret = scan( &g_token );
	}
	*/
	
	g_ast = expr();

	printf( "Final result = %d.\n", tnode_interp( g_ast ) );

	free( g_buffer );
	return 0;
}

int tnode_interp( tnode_t* node ) {
	int left, right;
	if( node->lvalue ) {
		left = tnode_interp( node->lvalue );
	}
	if( node->rvalue ) {
		right = tnode_interp( node->rvalue );
	}
	switch( node->op ) {
		case OP_ADD:
			return left + right;
		case OP_SUB:
			return left - right;
		case OP_MUL:
			return left * right;
		case OP_DIV:
			return left / right;
		default:
			printf( "Unknown operator: %d\n", node->op );
			exit( 1 );
	}
	return -1;
}
tnode_t* expr( void ) {
	tnode_t *left, *right, *retval;
	scan( &g_token );
	printf( "type = %d\n", g_token.type );
	left = int_lit();
	if( g_token.type == T_EOF ) {
		return left;
	}
	int node_op = math_op( g_token.type );
	scan( &g_token );
	printf( "type = %d\n", g_token.type );
	right = expr();
	retval = mknode( node_op, left, right, 0 );
	return retval;
}
tnode_t* int_lit( void ) {
	tnode_t* retval;
	if( g_token.type == T_LIT_INT ) {
		retval = mknode( OP_LIT_INT, NULL, NULL, g_token.val );
		scan( &g_token );
		return retval;
	} else {
		printf( "Syntax error on line %d: no further information (int_lit())\n", g_line );
		exit( 1 );
	}
}
int math_op( int op ) {
	switch( op ) {
		case T_PLUS:
			return OP_ADD;
		case T_MINUS:
			return OP_SUB;
		case T_STAR:
			return OP_MUL;
		case T_SLASH:
			return OP_DIV;
		default:
			printf( "Syntax error on line %d: unrecognized arithmetic operation.\n", g_line );
			exit( 1 );
			return -1;
	}
	return -1;
}

int ps_atoi( char c ) {
	for( int i = 48; i < 58; i++ ) {
		if( c == i ) return i - 48;
	}
	return -1;
}
char skip( void ) {
	char c = next();
	while( c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' ) {
		if( c == '\n' ) g_line++;
		c = next();
	}
	return c;
}
tnode_t* mknode( op_t op, tnode_t* lvalue, tnode_t* rvalue, long long value ) {
	tnode_t* retnode = calloc( 1, sizeof( *retnode ) );
	if( !retnode ) {
		printf( "mknode: failed to allocate memory for AST node.\n" );
		exit( 1 );
	}
	retnode->op = op;
	retnode->lvalue = lvalue;
	retnode->rvalue = rvalue;
	retnode->value = value;
	return retnode;
}
void delnode( tnode_t* node ) {
	free( node );
}
int next( void ) {
	if( g_putback != 0 ) {
		char c = g_putback;
		g_putback = 0;
		return c;
	}
	return *g_buffer++;
}
int scan( token_t* tok ) {
	char c = skip();
	switch( c ) {
	case '\0':
		tok->type = T_EOF;
		return 0;
	case '+':
		tok->type = T_PLUS;
		return 1;
	case '-':
		tok->type = T_MINUS;
		return 1;
	case '*':
		tok->type = T_STAR;
		return 1;
	case '/':
		tok->type = T_SLASH;
		return 1;
	default:
		if( isdigit( c ) ) {
			tok->val = scanint( c );
			tok->type = T_LIT_INT;
			return 1;
		}
		printf( "Syntax error: unknown token '%c' on line %d.\n", c, g_line );
		return 0;
	}
	printf( "Unknown error occurred: invalid point in function scan() reached.\n" );
	return 0;
}
int scanint( char c ) {
	int value = 0;
	while( isdigit( c ) ) {
		value = value * 10 + ps_atoi( c );
		c = next();
	}
	g_putback = c;
	return value;
}
