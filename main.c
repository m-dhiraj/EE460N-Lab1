#include <stdio.h> /* standard input/output library */
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */
#include "parse.h"
FILE* infile = NULL;
FILE* outfile = NULL;
int main (int argc, char* argv[]){
    //char *prgName   = argv[0];
    // char *iFileName = argv[1];
    // char *oFileName = argv[2];

    //printf("program name = '%s'\n", prgName);
    //printf("input file name = '%s'\n", iFileName);
    //printf("output file name = '%s'\n", oFileName);
    infile = fopen("kolbe.txt", "r");
    //infile = fopen("argv[1]", "r");
    //outfile = fopen(argv[2], "w");
                
    if (!infile) {
      printf("Error: Cannot open file %s\n", argv[1]);
      exit(4);
                 }
    // if (!outfile) {
    //   printf("Error: Cannot open file %s\n", argv[2]);
    //   exit(4);
    // }

    /* Do stuff with files */
    char * pLine;
    char ** pLabel;
    char ** pOpcode;
    char ** pArg1;
    char ** pArg2;
    char ** pArg3;
    char ** pArg4;
    if(readAndParse(infile, pLine, pLabel, pOpcode, pArg1, pArg2, pArg3, pArg4)==0)
    printf("reading:\n");
    

    fclose(infile);
    //fclose(outfile);
}

int toNum( char * pStr )
    {
    char * t_ptr;
    char * orig_pStr;
    int t_length,k;
    int lNum, lNeg = 0;
    long int lNumLong;

    orig_pStr = pStr;
    if( *pStr == '#' )                                /* decimal */
    {
        pStr++;
        if( *pStr == '-' )                                /* dec is negative */
        {
        lNeg = 1;
        pStr++;
        }
        t_ptr = pStr;
        t_length = strlen(t_ptr);
        for(k=0;k < t_length;k++)
        {
        if (!isdigit(*t_ptr))
        {
            printf("Error: invalid decimal operand, %s\n",orig_pStr);
            exit(4);
        }
        t_ptr++;
        }
        lNum = atoi(pStr);
        if (lNeg)
        lNum = -lNum;

        return lNum;
    }
    else if( *pStr == 'x' )        /* hex     */
    {
        pStr++;
        if( *pStr == '-' )                                /* hex is negative */
        {
        lNeg = 1;
        pStr++;
        }
        t_ptr = pStr;
        t_length = strlen(t_ptr);
        for(k=0;k < t_length;k++)
        {
        if (!isxdigit(*t_ptr))
        {
            printf("Error: invalid hex operand, %s\n",orig_pStr);
            exit(4);
        }
        t_ptr++;
        }
        lNumLong = strtol(pStr, NULL, 16);    /* convert hex string into integer */
        lNum = (lNumLong > INT_MAX)? INT_MAX : lNumLong;
        if( lNeg )
        lNum = -lNum;
        return lNum;
    }
    else
    {
            printf( "Error: invalid operand, %s\n", orig_pStr);
            exit(4);  /* This has been changed from error code 3 to error code 4, see clarification 12 */
    }
    }

