#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<ctype.h>

const char* input = "lvr %r0, $4\0";

#define NUM_CHAR_SPEC 11
const char char_spec[ NUM_CHAR_SPEC ] = { '%', '$', ',', '[', ']', '(', ')', '+', '-', '*', '/' };
#define NUM_INSTR 2
const char* instr_valid[ NUM_INSTR ] = { "lvr", "add" };

const enum {
    T_NULL,
    T_PERCENT,
    T_DOLLAR,
    T_COMMA,
    T_RBRACKET,
    T_LBRACKET,
    T_RPAREN,
    T_LPAREN,
    T_PLUS,
    T_MINUS,
    T_STAR,
    T_SLASH,
    T_INSTR
};

int g_tok = 0;
char* g_tok_str;

char nextchar( void ) {
    return *input++;
}
int token( char c ) {
    switch( c ) {
        case '%':
            // add register processing
            return T_PERCENT;
        case '$':
            return T_DOLLAR;
        case ',':
            return T_COMMA;
        case '[':
            return T_RBRACKET;
        case ']':
            return T_LBRACKET;
        case '(':
            return T_RPAREN;
        case ')':
            return T_LPAREN;
        case '+':
            return T_PLUS;
        case '-':
            return T_MINUS;
        case '*':
            return T_STAR;
        case '/':
            return T_SLASH;
        default:
            printf( "c = %c\n", c );
            if( isalpha( c ) ) {
                char* tmpc = calloc( 16, sizeof( *tmpc ) );
                if( !tmpc ) {
                    printf( "tmpc failed to allocate.\n" );
                    exit( 1 );
                }
                int pl_tmp = 0;
                
                tmpc[ pl_tmp++ ] = c;
                c = nextchar();
                while( c != ' ' ) {
                    tmpc[ pl_tmp++ ] = c;
                    c = nextchar();
                    printf( "c = %c\n", c );
                }
                tmpc[ pl_tmp ] = 0;
                strcpy( g_tok_str, tmpc );
                free( tmpc );
                return T_INSTR;
            }
            printf( "Unknown character processed: '%c'\n", c );
            exit( 1 );
    }
    return T_NULL;
}
char* scan( void ) {
    static int pl_input = 0;
    int pl_tmp = 0;
    char* tmp = calloc( 128, sizeof( *tmp ) );
    if( !tmp ) {
        printf( "tmp failed to allocate.\n" );
        exit( 1 );
    }
    while( true ) {
        g_tok = token( nextchar() );
        printf( "g_tok = %d\n", g_tok );
        printf( "g_tok_str = '%s'\n", g_tok_str );
    }
    /*
    while( true ) {
        while( *( input + pl_input ) == ' ' || *( input + pl_input ) == '\t' ) {
            pl_input++;
        }
        *( tmp + pl_tmp ) = *( input + pl_input );
        if( *( input + pl_input ) == '\0' ) return "AS_EOF";
        printf( "*( input + %d ) = '%c'\n", pl_input, *( input + pl_input ) );
        pl_tmp++;
        for( int i = 0; i < NUM_CHAR_SPEC; i++ ) {
            if( *( input + pl_input ) == char_spec[ i ] ) {
                pl_input++;
                *( tmp + pl_tmp++ ) = *( input + pl_input++ );
                *( tmp + pl_tmp ) = '\0';
                return tmp;
            }
        }
        pl_input++;
    }
    */
}

int main( void ) {
    g_tok_str = calloc( 128, sizeof( *g_tok_str ) );
    if( !g_tok_str ) {
        printf( "g_tok_str failed to allocate.\n" );
        exit( 1 );
    }
    char* first = scan();
    char* second = scan();
    char* third = scan();
    char* fourth = scan();
    char* fifth = scan();
    char* sixth = scan();
    printf( "first = '%s'\n", first );
    printf( "second = '%s'\n", second );
    printf( "third = '%s'\n", third );
    printf( "fourth = '%s'\n", fourth );
    printf( "fifth = '%s'\n", fifth );
    printf( "sixth = '%s'\n", sixth );
    free( g_tok_str );
    return 0;
}