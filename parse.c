#include "parse.h"
#define MAX_LINE_LENGTH 255
        enum
        {
           DONE, OK, EMPTY_LINE
        };

        int
        readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char
        ** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4
        )
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
