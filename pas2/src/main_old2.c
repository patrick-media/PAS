#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<stdbool.h>
#include<string.h>

int g_line = 0;

const char* tok_str[ 18 ] = { "NULL", "PLUS", "MINUS", "STAR", "SLASH", "PERCENT",
                        "DOLLAR", "LPAREN", "RPAREN", "COLON", "COMMA", "LITERAL",
                        "SYMBOL", "REGISTER", "DREF_PTR", "LVR", "ADD", "SLV" };
struct {
    enum {
        T_NULL,
        T_PLUS,
        T_MINUS,
        T_STAR,
        T_SLASH,
        T_PERCENT,
        T_DOLLAR,
        T_LPAREN,
        T_RPAREN,
        T_COLON,
        T_COMMA,
        T_LITERAL,
        T_SYMBOL,
        T_REGISTER,
        T_DREF_PTR,
        T_LVR,
        T_ADD,
        T_SLV
    } tok;
    long long lit_value;
    char lit_symbol[ 128 ];
} g_token;
char* g_buffer;
char* g_contents;
char g_putback = 0;

int my_atoi( char c ) {
    for( int i = 48; i < 57; i++ ) {
        if( c == i ) return i - 48;
    }
    return -1;
}

char next( void ) {
    if( g_putback ) {
        char c = g_putback;
        g_putback = 0;
        return c;
    }
    return *g_contents++;
}
char skip( void ) {
    char c = next();
    while( g_buffer == ' ' || g_buffer == '\t' ) {
        c = next();
        printf( "skip\n" );
    }
    return c;
}
void putback( char c ) {
    g_putback = c;
}

int scan_int( char c ) {
    int k, val = 0;
    while( my_atoi( c ) >= 0 ) {
        val = val*10 + k;
        c = next();
    }
    putback( c );
    return val;
}
int scan_symbol( char c, char* buf, int lim ) {
    int i = 0;
    while( isalpha( c ) || isdigit( c ) || c == '_' ) {
        if( i == lim-1 ) {
            printf( "Fatal error: symbol too long.\n" );
            exit( 1 );
        } else if( i < lim-1 ) {
            buf[ i++ ] = c;
        }
        c = next();
    }
    putback( c );
    buf[ i ] = 0;
    return i;
}
int keyword( char* s ) {
    switch( *s ) {
        case 'l':
            if( strcmp( s, "lvr" ) == 0 ) {
                printf( ">found keyword 'lvr'\n" );
                return T_LVR;
            }
            break;
        case 'a':
            if( strcmp( s, "add" ) == 0 ) {
                printf( ">found keyword 'add'\n" );
                return T_ADD;
            }
            break;
        case 's':
            if( strcmp( s, "slv" ) == 0 ) {
                printf( ">found keyword 'slv'\n" );
                return T_SLV;
            }
            break;
    }
    return 0;
}
int scan( void ) {
    int c;
    c = skip();
    printf( "c = %c\n", c );
    switch( c ) {
        case '+':
            g_token.tok = T_PLUS;
            break;
        case '-':
            g_token.tok = T_MINUS;
            break;
        case '*':
            g_token.tok = T_STAR;
            break;
        case '/':
            g_token.tok = T_SLASH;
            break;
        case '%':
            g_token.tok = T_PERCENT;
            break;
        case '$':
            g_token.tok = T_DOLLAR;
            break;
        case '(':
            g_token.tok = T_LPAREN;
            break;
        case ')':
            g_token.tok = T_RPAREN;
            break;
        case ':':
            g_token.tok = T_COLON;
            break;
        case ',':
            g_token.tok = T_COMMA;
            break;
        case '~':
            int c2 = next();
            if( c == c2 ) {
                c = next();
                while( c != '\n' ) {
                    c = next();
                }
            }
            putback( c );
            break;
        default:
            if( isdigit( c ) ) {
                g_token.lit_value = scan_int( c );
                g_token.tok = T_LITERAL;
                break;
            } else if( isalpha( c ) || c == '_' ) {
                char* scan_buf;
                scan_symbol( c, scan_buf, 128 );
                int tok_type = keyword( scan_buf );
                if( tok_type ) {
                    g_token.tok = tok_type;
                    break;
                }
                g_token.tok = T_SYMBOL;
                strcpy( g_token.lit_symbol, scan_buf );
                break;
            }
            printf( "Fatal error: unrecognized character: '%c'\n", c );
            exit( 1 );
    }
    return 1;
}

int main( int argc, char** argv ) {
    if( argc != 2 ) {
        printf( "Fatal error: incorrect number of arguments: %d.\n", argc );
        exit( 1 );
    }
    FILE* input_file = fopen( argv[ 1 ], "r" );
    if( !input_file ) {
        printf( "Fatal error: file '%s' failed to open.\n", argv[ 1 ] );
        exit( 1 );
    }
    g_contents = calloc( 4096, sizeof( *g_contents ) );
    if( !g_contents ) {
        printf( "Fatal error: failed to allocate memory.\n" );
        exit( 1 );
    }
    /*
    char* buf = calloc( 128, sizeof( *buf ) );
    if( !buf ) {
        printf( "Fatal error: failed to allocate memory.\n" );
        exit( 1 );
    }
    printf( "[Pass 1]\n" );
    while( fgets( buf, 128, input_file ) ) {
        const char* p = buf;
        while( isspace( p ) ) p++;
        if( !*p || ( *p == '~' && *( p+1 ) == '~' ) ) continue;
        printf( "Line %d:\t\t%s", g_line, p );
        g_line++;
    }
    free( buf );
    buf = calloc( 128, sizeof( *buf ) );
    if( !buf ) {
        printf( "Fatal error: failed to allocate memory.\n" );
        exit( 1 );
    }
    g_line = 0;
    rewind( input_file );
    
    //printf( "\n[Pass 2]\n" );
    */
    char c = fgetc( input_file );
    int c_i = 0;
    while( c != EOF ) {
        //printf( "c = %c\n", c );
        g_contents[ c_i++ ] = c;
        c = fgetc( input_file );
    }
    printf( "g_contents = %s\n", g_contents );
    int scan_result = scan();
    int tryc = 0;
    while( scan_result ) {
        if( tryc > 25 ) break;
        tryc++;
        printf( "g_token.tok = %s\n", tok_str[ g_token.tok ] );
        printf( "g_token.lit_value = %d\n", g_token.lit_value );
        printf( "g_token.lit_symbol = '%s'\n", g_token.lit_symbol );
        printf( "end token\n\n" );
    }
    /*
    while( fgets( buf, 128, input_file ) ) {
        const char* p = buf;
        while( isspace( p ) ) p++;
        if( !*p || ( *p == '~' && *( p+1 ) == '~' ) ) continue;
        printf( "Line %d:\t\t%s", g_line, p );

        int len = 0;
        char* buf2 = scan( p, &len );
        printf( "len = %d\n", len );
        printf( "buf2 = %s\n", buf2 );

        g_line++;
    }
    free( buf );
    */
    printf( "\n\nExiting\n" );
    return 0;
}