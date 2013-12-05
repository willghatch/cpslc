#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "symtab.h"
#include "parser.h"
#include "register.h"
#include "mipsout.h"

//extern union yylvalUnion yylval;
extern FILE *yyin;
int yylex();
extern int yydebug;
//void yyparse();


void printHelp(int exitStatus) {
    printf("The incredible, amazing, and stupendous cpslc written by William Hatch.\n");
    printf("Compiles CPSL programs into mips assembly suitable for spim or MARS\n");
    printf("I/O to/from stdout/stdin unless otherwise specified with these flags.\n");
    printf("Options:\n");
    printf("-v --verbose                print symbol table\n");
    printf("-h --help                   print this message\n");
    printf("-f <file> --file <file>     set input file\n");
    printf("-o <file> --output <file>   set output file\n");
    printf("\n");
    exit(exitStatus);
}

int main(int argc, char **argv)
{
    //yydebug = 1;

    FILE *infile = stdin;

    char* inFileName = NULL;
    char* outFileName = "-";
    int printHelpP = 0;


    struct option long_options[] =
        {
            {"verbose", no_argument,        0,   'v'},
            {"file",    required_argument,  0,   'f'},
            {"output",  required_argument,  0,   'o'},
            {"help",    no_argument,        0,   'h'},
            {0,0,0,0}
        };

    while (1) {
        int c;
        int option_index = 0;
        c = getopt_long(argc, argv, "vf:o:h", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch(c) {
            case 'v':
                verbosity = 1;
                break;
            case 'h':
                printHelpP = 1;
                break;
            case 'f':
                inFileName = optarg;
                break;
            case 'o':
                outFileName = optarg;
                break;
            case '?':
                printHelp(1);
                break;
            default:
                printHelp(1);
                break;
        }
    }

    if(printHelpP) {
        printHelp(0);
    }

    if(inFileName != NULL) {
        infile = fopen(inFileName, "r");
    }


    yyin = infile;

    // Initialize stuff
    init_registerState();
    mips_init();
    symtabInit();
    pushScope();


    // Parse stuff!
    yyparse();

    popScope();
    popScope(); // Incidentally, this call makes the scope stack pointer go out of bounds.  But hey, at this point I don't care.

    m_write_file(outFileName);
    

    return 0;
}

