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
    ADD, AND, BRn, BRz, BRp, BRnz, BRnp, BRzp, BR, BRnzp, JMP, JSR, JSRR, LDB,
    LDW, LEA, NOT, RET, RTI, LSHF, RSHFL, RSHFA,
    STB, STW, TRAP, XOR, HALT, NOP
};

int readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char ** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4);
int isOpcode(char * word);
int psuedoOp(char* word, char* arg, int* lCount);
int isValidOp(char* word, char * pArg1, char * pArg2, char * pArg3, char * pArg4, int address);
int toNum( char * pStr );
int labelExists(char* label);
int brChecker(char* branch);
#define MAX_LABEL_LEN 20
#define MAX_SYMBOLS 255
typedef struct {
	int address;
	char label[MAX_LABEL_LEN + 1];
} TableEntry;
TableEntry symbolTable[MAX_SYMBOLS];

int main (int argc, char* argv[]){
    //infile = fopen("kolbe.txt", "r");
    //outfile = fopen("output.txt", "w");
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
    int lineCount=0;
    
    int bool=1;
    do
    {
        parseRet=readAndParse(infile,lLine,&lLabel,&lOpcode,&lArg1,&lArg2,&lArg3,&lArg4);
        if(strcmp(lOpcode,".orig")==0){
            bool=0;
            fprintf(outfile,"%#X\n",toNum(lArg1));
            //lineCount=toNum(lArg1);
        }
        if(strcmp(lOpcode,".end")==0)
            return(-1);//error for .end before .orig
        if(parseRet==DONE)
            return(-1);//error for no .orig
    } while (bool);
    //By here we have established a .orig
    int lCount=lineCount;
    bool=1;
    do{
        parseRet = readAndParse( infile, lLine, &lLabel,
                &lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
        if( parseRet != DONE && parseRet != EMPTY_LINE){//do &&
            TableEntry t;
            t.address=lineCount;
            strcpy(t.label,lLabel);
            symbolTable[lineCount]=t;
            lCount+=2;
            //printf("OPCODE:%d\n", isOpcode(lOpcode));
        }
        if(parseRet==DONE)
            return(-1);//no .end
        if(strcmp(lOpcode,".end")==0)
            bool=0;
    } while(bool);
        
    //now we have established a symbol table
    rewind(infile);
    bool=1;
    do
    {
        parseRet=readAndParse(infile,lLine,&lLabel,&lOpcode,&lArg1,&lArg2,&lArg3,&lArg4);
        if(strcmp(lOpcode,".orig")==0){
            bool=0;
        }
        if(strcmp(lOpcode,".end")==0)
            return(-1);//error for .end before .orig
        if(parseRet==DONE)
            return(-1);//error for no .orig
    } while (bool);
    // //time to decode instructions
    rewind(infile);

    lCount=lineCount;
    bool=1;
    do{
        parseRet=readAndParse(infile,lLine,&lLabel,&lOpcode,&lArg1,&lArg2,&lArg3,&lArg4);
        if(parseRet!=DONE && parseRet!=EMPTY_LINE){
            //printf("Line:%d ",lCount);
            int check=isValidOp(lOpcode, lArg1, lArg2, lArg3, lArg4, lCount);
            if(check!=-1)
                fprintf(outfile,"%#X\n",check);
            lCount+=2;
        }
        if(parseRet==DONE)
            return(-1);//no .end
        if(strcmp(lOpcode,".end")==0)
            bool=0;
    } while(bool);
    
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
           *lPtr != '\n' &&' ' )
                lPtr++;

           *lPtr = '\0';
           if( !(lPtr = strtok( pLine, "\t\n ," ) ) )
                return( EMPTY_LINE );

           if( isOpcode( lPtr ) == -1 && lPtr[0] != '.' ) /* found a label */
           {
                *pLabel = lPtr;
                if( !( lPtr = strtok( NULL, "\t\n ," ) ) ) return( OK );
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
    if(strcmp(word,"rti")==0)
        return RTI;
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
    if(word[0]=='b'&&word[1]=='r')
        return brChecker(word);

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

int isValidOp(char* word, char * pArg1, char * pArg2, char * pArg3, char * pArg4, int address){
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
    if(isOpcode(word)>=BRn&&isOpcode(word)<=BRnzp){
        if((strlen(pArg4)==0)&&(strlen(pArg1)>0)&&(strlen(pArg2)==0)&&(strlen(pArg3)==0)){
            if(labelExists(pArg1)!=-1){
                    int distance=(address-symbolTable[labelExists(pArg1)].address);
                    if(distance>=-256&&distance<=255){
                        ans=0x0800+distance;
                        if(isOpcode(word)==BRn)
                            ans+=0x800;
                        if(isOpcode(word)==BRz)
                            ans+=0x400;
                        if(isOpcode(word)==BRp)
                            ans+=0x200;
                        if(isOpcode(word)==BR)
                            ans+=0x000;
                        if(isOpcode(word)==BRzp)
                            ans+=0x600;
                        if(isOpcode(word)==BRnp)
                            ans+=0xA00;
                        if(isOpcode(word)==BRnz)
                            ans+=0xC00;
                        if(isOpcode(word)==BRnzp)
                            ans+=0xE00;
                    }
                }
            }
        }

    if(isOpcode(word)==JSR){
        if((strlen(pArg4)==0)&&(strlen(pArg1)>0)&&(strlen(pArg2)==0)&&(strlen(pArg3)==0)){
            if(labelExists(pArg1)!=-1){
                    int distance=(address-symbolTable[labelExists(pArg1)].address);
                    if(distance>=-256&&distance<=255){
                        ans=0x0800+distance;
                    }
                }
            }
        }
    
    //need to fix
    if(isOpcode(word)==RET||isOpcode(word)==RTI){
        if((strlen(pArg4)==0)&&(strlen(pArg1)==0)&&(strlen(pArg2)==0)&&(strlen(pArg3)==0)){
            if(isOpcode(word)==RET){
                ans=0xC1C0;
            }
            if(isOpcode(word)==RTI){
                ans=0x8000;
            }
        }
    }
    if(isOpcode(word)==JSRR){
        if((strlen(pArg4)==0)&&(strlen(pArg1)>0)&&(strlen(pArg2)==0)&&(strlen(pArg3)==0)){
            if(((pArg1[0]=='r')&&((pArg1[1] - '0')>=0)&&((pArg1[1] - '0')<8))){
                ans=0x4000+((pArg1[1] - '0')*64);
            }
        }
    }
    if(isOpcode(word)==LDB||isOpcode(word)==LDW||isOpcode(word)==STB||isOpcode(word)==STW){
         if((strlen(pArg4)==0)&&(strlen(pArg1)==2)&&(strlen(pArg2)==2)&&(strlen(pArg3)>0))
            {
                if(pArg1[0]=='r'&&pArg2[0]=='r')
                {
                    if(((pArg1[1] - '0')>=0)&&((pArg1[1] - '0')<8)&&((pArg2[1] - '0')>=0)&&((pArg2[1] - '0')<8))
                        if((toNum(pArg3)<32&&toNum(pArg3)>-33)){
                           if(isOpcode(word)==LDB){
                               ans=0x2000;
                           }
                           if(isOpcode(word)==LDW){
                               ans=0x6000;
                           }
                           if(isOpcode(word)==STB){
                               ans=0x3000;
                           }
                           if(isOpcode(word)==STW){
                               ans=0xE000;
                           }
                           int add=pArg1[1] - '0';
                                add*=512;
                                ans+=add;
                            add=pArg2[1] - '0';
                                add*=64;
                                ans+=add;
                            ans+=toNum(pArg3);
                        }
                }
            }
    }
    if(isOpcode(word)==LEA){
        if((strlen(pArg4)==0)&&(strlen(pArg1)==2)&&(strlen(pArg2)>0)&&(strlen(pArg3)==0)){
            if(((pArg1[0]=='r')&&((pArg1[1] - '0')>=0)&&((pArg1[1] - '0')<8))){
                if(labelExists(pArg1)!=-1){
                        int distance=(address-symbolTable[labelExists(pArg1)].address);
                        if(distance>=-256&&distance<=255){
                            ans=0xE000+distance;
                            int add=pArg1[1] - '0';
                                add*=512;
                                ans+=add;
                        }
                    }
                }
            }
        
    }
    if(isOpcode(word)==NOT){
        if((strlen(pArg4)==0)&&(strlen(pArg1)==2)&&(strlen(pArg2)==2)&&(strlen(pArg3)==0)){
            if(((pArg1[0]=='r')&&((pArg1[1] - '0')>=0)&&((pArg1[1] - '0')<8))){
                if(((pArg2[0]=='r')&&((pArg2[1] - '0')>=0)&&((pArg2[1] - '0')<8))){
                    ans=0x9000;
                    int add=pArg1[1] - '0';
                                add*=512;
                                ans+=add;
                            add=pArg2[1] - '0';
                                add*=64;
                                ans+=add;
                    ans+=0x003F;
                }
            }
        }
    }
    if(isOpcode(word)==LSHF||isOpcode(word)==RSHFL||isOpcode(word)==RSHFA){
        if((strlen(pArg4)==0)&&(strlen(pArg1)==2)&&(strlen(pArg2)==2)&&(strlen(pArg3)>0)){
            if(pArg1[0]=='r'&&pArg2[0]=='r'){
                if(((pArg1[1] - '0')>=0)&&((pArg1[1] - '0')<8)&&((pArg2[1] - '0')>=0)&&((pArg2[1] - '0')<8))
                    if(((pArg3[0]=='r')&&((pArg3[1] - '0')>=0)&&((pArg3[1] - '0')<8))||(toNum(pArg3)<16&&toNum(pArg3)>=0)){
                            ans=0x9000;
                            int add=pArg1[1] - '0';
                                add*=512;
                                ans+=add;
                            add=pArg2[1] - '0';
                                add*=64;
                                ans+=add;
                                if(isOpcode(word)==RSHFL){
                                  ans+=0x0020;
                                }
                                if(isOpcode(word)==RSHFA){
                                  ans+=0x0060;
                                }
                                ans+=toNum(pArg3);
                        }
            }
        }
    }
    if(strcmp(word,".fill")==0){
        ans=toNum(pArg1);
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

int labelExists(char* label){
    int i;
    int a=-1;
    for(i=0;i<MAX_SYMBOLS;i++){
        if(strcmp(symbolTable[i].label, label)==0)
        a=i;
    }
    return a;
}

int brChecker(char* branch){
        if(strcmp(branch, "br")==0)
        return BR;
        if(strcmp(branch, "brn")==0)
        return BRn;
        if(strcmp(branch, "brz")==0)
        return BRz;
        if(strcmp(branch, "brp")==0)
        return BRp;
        if(strcmp(branch, "brnp")==0)
        return BRnp;
        if(strcmp(branch, "brnz")==0)
        return BRnz;
        if(strcmp(branch, "bzp")==0)
        return BRzp;
        if(strcmp(branch, "brnzp")==0)
        return BRnzp;
    return -1;
}