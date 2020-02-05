#include <stdio.h> /* standard input/output library */
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */

#define MAX_LINE_LENGTH 255

FILE* infile = NULL;
FILE* outfile = NULL;

enum
{
    DONE, OK, EMPTY_LINE, PSEUDO, ORIG, END
};

int readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char ** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4);
int isOpcode(char * word);
int psuedoOp(char* word, char* arg, int* lCount);

#define MAX_LABEL_LEN 20
#define MAX_SYMBOLS 255
typedef struct {
	int address;
	char label[MAX_LABEL_LEN + 1];
} TableEntry;
TableEntry symbolTable[MAX_SYMBOLS];

int main (int argc, char* argv[]){
    infile = fopen("kolbe.txt", "r");
    outfile = fopen("output.txt", "w");
    //infile = fopen(argv[1], "r");
    //outfile = fopen(argv[2], "w");
                
    if (!infile) {
      printf("Error: Cannot open file %s\n", argv[1]);
      exit(4);
    }
    if (!outfile) {
      printf("Error: Cannot open file %s\n", argv[2]);
      exit(4);
    }

    /* Do stuff with files */
    char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1, *lArg2, *lArg3, *lArg4;
    int parseRet;
    int lCount=1;
    do
    {
        parseRet=readAndParse(infile,lLine,&lLabel,&lOpcode,&lArg1,&lArg2,&lArg3,&lArg4);
        if(parseRet==PSEUDO)
            parseRet=psuedoOp(lOpcode,lArg1,&lCount);
    } while (parseRet!=ORIG);
    printf("%s \n",lLine);
    do{
        parseRet=readAndParse(infile,lLine,&lLabel,&lOpcode,&lArg1,&lArg2,&lArg3,&lArg4);
        if(parseRet!=DONE && parseRet!=EMPTY_LINE && parseRet!=PSEUDO){
            TableEntry t;
            t.address=lCount;
            strcpy(t.label,lLabel);
            symbolTable[lCount]=t;
            lCount++;
        }
        printf("op code: %s ",lOpcode);
        printf("arg1: %s \n",lArg1);
    }while (parseRet!=PSEUDO);
    //printf("\n%s \n",lLine);

    int i;
    for(i=0;i<lCount;i++){
        if(strlen(symbolTable[i].label)!=0){
            printf("%d",symbolTable[i].address);
            printf(symbolTable[i].label);
            printf("\n");
        }
    }

    fclose(infile);
    fclose(outfile);
}

int readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char ** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4)
{
    char * lRet, * lPtr;
    int i;
    if( !fgets( pLine, MAX_LINE_LENGTH, pInfile ) )
        return( DONE );
    for( i = 0; i < strlen( pLine ); i++ )
        pLine[i] = tolower( pLine[i] );
    /* convert entire line to lowercase */
    *pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);

    /* ignore the comments */
    lPtr = pLine;

    while( *lPtr != ';' && *lPtr != '\0' &&
    *lPtr != '\n' )
        lPtr++;

    *lPtr = '\0';
    if( !(lPtr = strtok( pLine, "\t\n ," ) ) )
        return( EMPTY_LINE );
    
    //
    if(lPtr[0]=='.'){
        *pOpcode=lPtr;
        if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( PSEUDO );
        *pArg1 = lPtr;
        return(PSEUDO);
    }
    //

    if( isOpcode( lPtr ) == -1 && lPtr[0] != '.' ) /* found a label */
    {
        *pLabel = lPtr;
        if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
    }
    if(pLine[0]!='.')
    *pOpcode = lPtr;

    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
    
    *pArg1 = lPtr;
    
    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

    *pArg2 = lPtr;
    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

    *pArg3 = lPtr;

    if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );

    *pArg4 = lPtr;

    return( OK );
}
int isOpcode(char * word){
    if(strcmp(word,"add")==0)
        return 1;
    if(strcmp(word,"and")==0)
        return 1;
    if(strcmp(word,"halt")==0)
        return 1;
    if(strcmp(word,"jmp")==0)
        return 1;
    if(strcmp(word,"jsr")==0)
        return 1;
    if(strcmp(word,"jsrr")==0)
        return 1;
    if(strcmp(word,"ldb")==0)
        return 1;
    if(strcmp(word,"ldb")==0)
        return 1;
    if(strcmp(word,"ldw")==0)
        return 1;
    if(strcmp(word,"lea")==0)
        return 1;
    if(strcmp(word,"nop")==0)
        return 1;
    if(strcmp(word,"not")==0)
        return 1;
    if(strcmp(word,"ret")==0)
        return 1;
    if(strcmp(word,"lshf")==0)
        return 1;
    if(strcmp(word,"rshfl")==0)
        return 1;
    if(strcmp(word,"rshfa")==0)
        return 1;
    if(strcmp(word,"rti")==0)
        return 1;
    if(strcmp(word,"stb")==0)
        return 1;
    if(strcmp(word,"stw")==0)
        return 1;
    if(strcmp(word,"trap")==0)
        return 1;
    if(strcmp(word,"xor")==0)
        return 1;
    
    return -1;
}

int psuedoOp(char* word, char* arg, int* lCount){
    if(strcmp(word,".orig")==0){
        *lCount=atoi(arg);
        return ORIG;
    }
    return PSEUDO;
}