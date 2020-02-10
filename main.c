#include <stdio.h> /* standard input/output library */
#include <stdlib.h> /* Standard C Library */
#include <string.h> /* String operations library */
#include <ctype.h> /* Library for useful character operations */
#include <limits.h> /* Library for definitions of common variable type characteristics */
#include <math.h>
#define MAX_LINE_LENGTH 255

FILE* infile = NULL;
FILE* outfile = NULL;

enum
{
    DONE, OK, EMPTY_LINE, PSEUDO, ORIG, END
};
enum
{
    ADD, AND, BR, JMP, JSR, JSRR, LDB,
    LDW, LEA, NOT, RET, RTI, LSHF, RSHFL, RSHFA,
    STB, STW, TRAP, XOR, HALT, NOP
};

int readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char ** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4);
int isOpcode(char * word);
int psuedoOp(char* word, char* arg, int* lCount);
int isValidOp(char* word, char * pArg1, char * pArg2, char * pArg3, char * pArg4);
int toNum( char * pStr );
#define MAX_LABEL_LEN 20
#define MAX_SYMBOLS 255
typedef struct {
	int address;
	char label[MAX_LABEL_LEN + 1];
} TableEntry;
TableEntry symbolTable[MAX_SYMBOLS];

int main (int argc, char* argv[]){
    // infile = fopen("kolbe.txt", "r");
    // outfile = fopen("output.txt", "w");
    infile = fopen(argv[1], "r");
    outfile = fopen(argv[2], "w");
            
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
    int lineCount=-1;
    do
    {
        parseRet=readAndParse(infile,lLine,&lLabel,&lOpcode,&lArg1,&lArg2,&lArg3,&lArg4);
        if(parseRet==PSEUDO)
            parseRet=psuedoOp(lOpcode,lArg1,&lineCount);
        if(parseRet==DONE)
            return(-1);//some error
    } while (parseRet!=ORIG);
       
    //By here we have established a .orig
    int lCount=lineCount;
    do{
        parseRet=readAndParse(infile,lLine,&lLabel,&lOpcode,&lArg1,&lArg2,&lArg3,&lArg4);
        if(parseRet!=DONE && parseRet!=EMPTY_LINE && parseRet!=PSEUDO){
            TableEntry t;
            t.address=lineCount;
            strcpy(t.label,lLabel);
            symbolTable[lineCount]=t;
            lCount+=2;
            printf("OPCODE:%d\n", isOpcode(lOpcode));//DEBUG
        }
        if(parseRet==PSEUDO)
            parseRet=psuedoOp(lOpcode,lArg1,&lineCount);
        if(parseRet==DONE)
            return(-1);
    }while (parseRet!=END);
    //now we have established a symbol table
    rewind(infile);
    do{
        parseRet=readAndParse(infile,lLine,&lLabel,&lOpcode,&lArg1,&lArg2,&lArg3,&lArg4);
        if(parseRet==PSEUDO)
            parseRet=psuedoOp(lOpcode,lArg1,&lineCount);
        if(parseRet==DONE)
            return(-1);//some error
    } while (parseRet!=ORIG);
    lCount=lineCount;
    //time to decode instructions
    do{
        parseRet=readAndParse(infile,lLine,&lLabel,&lOpcode,&lArg1,&lArg2,&lArg3,&lArg4);
        if(parseRet!=DONE && parseRet!=EMPTY_LINE && parseRet!=PSEUDO){
            isValidOp(lOpcode, lArg1, lArg2, lArg3, lArg4);
        }
        if(parseRet==PSEUDO)
            parseRet=psuedoOp(lOpcode,lArg1,&lineCount);
        if(parseRet==PSEUDO){
            //.fill
        }
    }while (parseRet!=END);
    
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

    while( *lPtr != ';' && *lPtr != '\0' && *lPtr != '\n' )
        lPtr++;

    *lPtr = '\0';
    if( !(lPtr = strtok( pLine, "\t\n ," ) ) )
        return( EMPTY_LINE );
    
    //
    if(lPtr[0]=='.'){
        *pOpcode=lPtr;
        if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) 
            return( PSEUDO );
        *pArg1 = lPtr;
        return(PSEUDO);
    }
    //

    if( isOpcode( lPtr ) == -1 && lPtr[0] != '.' ) /* found a label */
    {
        *pLabel = lPtr;
        if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
    }
    if(pLine[0]=='.'){
        *pOpcode = lPtr;
        if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) 
            return (PSEUDO);
        *pArg1 = lPtr;
        return (PSEUDO);
    }
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
        return ADD;
    if(strcmp(word,"and")==0)
        return AND;
    if(strcmp(word,"halt")==0)
        return HALT;
    if(strcmp(word,"jmp")==0)
        return JMP;
    if(strcmp(word,"jsr")==0)
        return JSR;
    if(strcmp(word,"jsrr")==0)
        return JSRR;
    if(strcmp(word,"ldb")==0)
        return LDB;
    if(strcmp(word,"ldw")==0)
        return LDW;
    if(strcmp(word,"lea")==0)
        return LEA;
    if(strcmp(word,"nop")==0)
        return NOP;
    if(strcmp(word,"not")==0)
        return NOT;
    if(strcmp(word,"ret")==0)
        return RET;
    if(strcmp(word,"lshf")==0)
        return LSHF;
    if(strcmp(word,"rshfl")==0)
        return RSHFL;
    if(strcmp(word,"rshfa")==0)
        return RSHFA;
    if(strcmp(word,"rti")==0)
        return RTI;
    if(strcmp(word,"stb")==0)
        return STB;
    if(strcmp(word,"stw")==0)
        return STW;
    if(strcmp(word,"trap")==0)
        return TRAP;
    if(strcmp(word,"xor")==0)
        return XOR;
    
    return -1;
}

int psuedoOp(char* word, char* arg, int* lCount){
    if(strcmp(word,".orig")==0){
        *lCount=atoi(arg);
        return (ORIG);
    }
    if(strcmp(word,".end")==0){
        return (END);
    }
    return PSEUDO;
}

int isValidOp(char* word, char * pArg1, char * pArg2, char * pArg3, char * pArg4){
    int ans=-1;
    
    if(isOpcode(word)==ADD||isOpcode(word)==AND||isOpcode(word)==XOR){
        if((strlen(pArg4)==0)&&(strlen(pArg1)==2)&&(strlen(pArg2)==2)&&(strlen(pArg3)>0)){
            if(pArg1[0]=='r'&&pArg2[0]=='r'){
                if(((pArg1[1] - '0')>=0)&&((pArg1[1] - '0')<8)&&((pArg2[1] - '0')>=0)&&((pArg2[1] - '0')<8))
                    if(((pArg3[0]=='r')&&((pArg3[1] - '0')>=0)&&((pArg3[1] - '0')<8))||(toNum(pArg3)<16&&toNum(pArg3)>-17)){
                            if(isOpcode(word)==ADD){
                                ans=0x1000;
                            }
                            else if(isOpcode(word)==AND){
                                ans=0x5000;
                            }
                            else if(isOpcode(word)==XOR){
                                ans=0x9000;
                            }
                            int add=pArg1[1] - '0';
                                add*=512;
                                ans+=add;
                            add=pArg2[1] - '0';
                                add*=64;
                                ans+=add;
                            if((pArg3[0]=='r')&&((pArg3[1] - '0')>=0)&&((pArg3[1] - '0')<8)){
                                   add=pArg3[1] - '0';
                                   ans+=add;
                                }
                            else{
                                    ans+=0x0020;
                                    ans+=toNum(pArg3);
                                }
                            
                        }
            }
        }
    }
    if(isOpcode(word)==BR||isOpcode(word)==JSR){
        if((strlen(pArg4)==0)&&(strlen(pArg1)>0)&&(strlen(pArg2)==0)&&(strlen(pArg3)==0)){
            int i;
            for(i=0;i<sizeof(symbolTable);i++){
                if(strcmp(symbolTable[i].label,pArg1)==0)
                    ans=1;
            }
        }
    }
    if(isOpcode(word)==RET||isOpcode(word)==RTI){
        if((strlen(pArg4)==0)&&(strlen(pArg1)==0)&&(strlen(pArg2)==0)&&(strlen(pArg3)==0))
            ans=1;
    }
    if(isOpcode(word)==JSRR){
        if((strlen(pArg4)==0)&&(strlen(pArg1)>0)&&(strlen(pArg2)==0)&&(strlen(pArg3)==0)){
            if(((pArg1[0]=='r')&&((pArg1[1] - '0')>=0)&&((pArg1[1] - '0')<8)))
                ans=1;
        }
    }
    if(isOpcode(word)==LDB||isOpcode(word)==LDW||isOpcode(word)==STB||isOpcode(word)==STW){
         if((strlen(pArg4)==0)&&(strlen(pArg1)==2)&&(strlen(pArg2)==2)&&(strlen(pArg3)>0))
            {
                if(pArg1[0]=='r'&&pArg2[0]=='r')
                {
                    if(((pArg1[1] - '0')>=0)&&((pArg1[1] - '0')<8)&&((pArg2[1] - '0')>=0)&&((pArg2[1] - '0')<8))
                        if((toNum(pArg3)<32&&toNum(pArg3)>-33))
                            ans=1;
                }
            }
    }
    if(isOpcode(word)==LEA){
        if((strlen(pArg4)==0)&&(strlen(pArg1)==2)&&(strlen(pArg2)>0)&&(strlen(pArg3)==0)){
            if(((pArg1[0]=='r')&&((pArg1[1] - '0')>=0)&&((pArg1[1] - '0')<8))){
                int i;
                for(i=0;i<sizeof(symbolTable);i++){
                    if(strcmp(symbolTable[i].label,pArg2)==0)
                        ans=1;
                }
            }
        }
    }
    if(isOpcode(word)==NOT){
        if((strlen(pArg4)==0)&&(strlen(pArg1)==2)&&(strlen(pArg2)==2)&&(strlen(pArg3)==0)){
            if(((pArg1[0]=='r')&&((pArg1[1] - '0')>=0)&&((pArg1[1] - '0')<8))){
                if(((pArg2[0]=='r')&&((pArg2[1] - '0')>=0)&&((pArg2[1] - '0')<8)))
                    ans=1;
            }
        }
    }
    return ans;
   
}

toNum( char * pStr )
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

