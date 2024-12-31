#ifndef _TOOL_H_
#define _TOOL_H_

#include<stdio.h>

int getStringToken( char *src , char delim , int count , int maxlen , char *out );
int getIntegerToken( char *src , char delim , int count );
// TODO: 好像没啥用.
double getDoubleToken( char *src , char delim , int count );
void chop( char *src );
void shiftIntArray( int *a , int siz , int count );

int a62toi( char * );
int getInteger62Token( char *, char, int );

void jEncode( char *, int, int, char *, int *, int );
void jDecode( char *, int, int, char *, int * );

int strncmpi( char *s1 , char *s2 , int len );
// Important.
char * makeStringFromEscaped( char* src );
char * makeEscapeString( char* src , char* dest, int sizeofdest);

#endif /*_TOOL_H_*/
