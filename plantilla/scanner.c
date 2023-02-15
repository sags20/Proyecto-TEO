/**********************************************************/
/* Scanner-Analizador Lexicografico para el lenguaje PL-0 */
/*                  Febrero de 2007                       */
/*             Uso : scanner progfuente                   */
/* Microsoft Visual C++ versión 6.0 Enterprise Edition    */
/**********************************************************/ 

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

/***************************************/
/*          Globales                   */
/***************************************/

#define MAXLINEA 1000 /*Tamaño maximo de una linea del programa fuente*/
#define MAXPAL     11 /*Numero de palabras reservadas                 */
#define MAXDIGIT    5 /*Maximo numero de digitos en los enteros       */
#define MAXID      10 /*Maxima longitud de los identificadores        */

/*Lexemes de las palabras reservadas*/
char *lexpal[MAXPAL]={"BEGIN","CALL","CONST","DO","END","IF","ODD",
		      "PROCEDURE","THEN","VAR","WHILE"};

char linea[MAXLINEA]; /*Buffer de lineas                    */
int offset,ll;        /*Corrimiento y contador de caracteres*/
int fin_de_archivo;   /*bandera de fin de archivo (obtch)   */
int ch;

/*Lexeme del ultimo identificador leido*/
char lex[MAXID+1]; /* +1 para colocar \0 */

/*Valor numerico del ultimo numero leido*/
long int valor ;

FILE *fp;


/*Lista de Tokens de pl-0*/

enum simbolo {nulo,ident,numero,mas,menos,por,barra,oddtok,
	     igl,nig,mnr,mei,myr,mai,parena,parenc,coma,puntoycoma,
	     punto,asignacion,begintok,endtok,iftok,thentok,
	     whiletok,dotok,calltok,consttok,vartok,proctok} ;

enum simbolo token;

/*Tabla de tokens de palabras reservadas*/
enum simbolo tokpal [MAXPAL]={begintok,calltok,consttok,dotok,endtok,
			 iftok,oddtok,proctok,thentok,vartok,whiletok};

/*Tabla de tokens de operadores y simbolos especiales*/
enum simbolo espec[255] ;


/*             Prototipos de funciones               */
void init_spec(),obtoken(),imprime_token(),error(int no);
int  obtch(),getline(char s[],int lim);

/***************************************/
/*              main                   */
/***************************************/

main (int argc,char *argv[])
{ 
 FILE *fopen();
  /*Analizar argumentos de main*/
  if (argc!=2)
   printf("\nUso : scanner progfuente");
  else { 
    fp=fopen(argv[1],"r");
    if (fp==NULL) {
     printf("\n\n--No se encontró al Programa Fuente--");
     exit(1);
    }
    printf("\n\nCompilador PL-0 ver 1.0 - Solo Scanner -- Febrero 2007\n\n");
    init_spec() ; /*inicializacion de simbolos especiales*/
    ch=' ';
    fin_de_archivo=0;
    offset=-1;ll=0;
    while (1) {
     obtoken();
     imprime_token();
    }
  }
 return (0);
}

/***************************************/
/*              obtch                  */
/* Obtiene el siguiente caracter del   */
/* programa fuente.                    */
/***************************************/

int obtch()
{ 
 if (fin_de_archivo==1) {
   printf("\nFinal del programa");
   exit(1);
 }
  
 if (offset==ll-1) {
  ll=getline(linea,MAXLINEA);
  if (ll==0) 
   fin_de_archivo=1; /*se retrasa en un blanco la deteccion de EOF, porque*/
		             /*obtoken lleva un caracter adelantado. si no, en    */
		             /*algunos casos, p/ejem, no se reconoceria el punto  */
		             /*del programa (...end.)                             */
   /*printf("%s",linea);*/ /*Quitar el comentario para obtener listado del fuente*/
  offset=-1;
 }

 ++offset;
 if ( (linea[offset]=='\0') || (fin_de_archivo==1) )   
  return(' '); 
 else  
  return(toupper(linea[offset]));

}

/****************************************/
/*              getline                 */
/* lee la siguiente linea del fuente y  */
/* regresa su tamaño (incluyendo '\n') o*/
/* 0 si EOF. (eje. para VAR regresa 4)  */
/* pag 28 de K&R                        */
/****************************************/

int getline(char s[],int lim)
{
 int c,i;
 for (i=0;i<lim-1 && (c=getc(fp))!=EOF && c!='\n';++i)
  s[i]=c;
 if (c=='\n') {
  s[i]=c;
  ++i;
 }
 s[i]='\0';
 return (i);
}

/***************************************/
/*              obtoken                */
/* Obtiene el siguiente token del      */
/* programa fuente.                    */
/***************************************/

void obtoken()
{
 char lexid[MAXID+1]; /* +1 para colocar \0 */
 int i,j;
 int ok=0;

 while (ch==' ' || ch=='\n' || ch=='\t') ch=obtch() ;/*quitar blancos*/
 /*Si comienza con una letra es identificador o palabra reservada*/
 if (isalpha(ch)) {
    lexid[0]=ch;
    i=1;
    while ( isalpha( (ch=obtch()) ) ||  isdigit(ch)   ) 
      if (i<MAXID) lexid[i++]=ch;
    lexid[i]='\0';
  
    /*¨es identificador o palabra reservada?*/
    for (j=0;j<MAXPAL;++j) 
       if (strcmp(lexid,lexpal[j])==0) {
	  ok=1;
	  break;
       }

    if (ok==1) 
     token=tokpal[j];
    else
     token=ident;
 
    strcpy(lex,lexid);
    /* printf("\nLast Lex %s",lex); */
 }
 else /*si comienza con un digito...*/
  if (isdigit(ch)) {
     lexid[0]=ch;
     i=j=1;
     while ( isdigit( (ch=obtch()))) {
	 if (i<MAXDIGIT) lexid[i++]=ch;
	 j++;
     }
     lexid[i]='\0';
     if (j>MAXDIGIT)
      error(30); /*Este numero es demasiado grande*/
     token=numero;
     valor=atol(lexid);
     /*printf("\n numero --> %ld",valor);*/
  }
  else /*simbolos especiales, incluyendo pares de simbolos (Lookahead Symbol) */
   if (ch=='<') {
     ch=obtch();
     if (ch=='=') {
      token=mei;
      ch=obtch();
     }
     else
      if (ch=='>') {
       token=nig;
       ch=obtch();
      }
      else
       token=mnr;
   }
   else
    if (ch=='>') {
      ch=obtch();
      if (ch=='=') {
       token=mai;
       ch=obtch();
      }
      else 
       token=myr;
    }
    else 
     if (ch==':') {
       ch=obtch();
       if (ch=='=') {
	token=asignacion;
	ch=obtch();
       }
       else
	token=nulo;
     }
     else {
      token=espec[ch];
      ch=obtch();
     }

 /*printf("\nLexeme %s",lexid);*/ /*Si quieres ver la ultima lexeme*/
}

/***************************************/
/*          imprime_token              */
/* utilitario del programador para     */
/* imprimir un token.                  */
/***************************************/

void imprime_token()
{
 switch(token) {
  case nulo:
   printf("\ntoken-->nulo");
   break;
  case ident:
   printf("\ntoken-->ident");
   break;
  case numero:
   printf("\ntoken-->numero");
   break;
  case mas:
   printf("\ntoken-->mas");
   break;
  case menos:
   printf("\ntoken-->menos");
   break;
  case por:
   printf("\ntoken-->por");
   break;
  case barra:
   printf("\ntoken-->barra");
   break;
  case oddtok:
   printf("\ntoken-->oddtok");
   break;
  case igl:
   printf("\ntoken-->igl");
   break;
  case nig:
   printf("\ntoken-->nig");
   break;
  case mnr:
   printf("\ntoken-->mnr");
   break;
  case mei:
   printf("\ntoken-->mei");
   break;
  case myr:
   printf("\ntoken-->myr");
   break;
  case mai:
   printf("\ntoken-->mai");
   break;
  case parena:
   printf("\ntoken-->parena");
   break;
  case parenc:
   printf("\ntoken-->parenc");
   break;
  case coma:
   printf("\ntoken-->coma");
   break;
  case puntoycoma:
   printf("\ntoken-->puntoycoma");
   break;
  case punto:
   printf("\ntoken-->punto");
   break;
  case asignacion:
   printf("\ntoken-->asignacion");
   break;
  case begintok:
   printf("\ntoken-->begintok");
   break;
  case endtok:
   printf("\ntoken-->endtok");
   break;
  case iftok:
   printf("\ntoken-->iftok");
   break;
  case thentok:
   printf("\ntoken-->thentok");
   break; 
  case whiletok:
   printf("\ntoken-->whiletok");
   break;
  case dotok:
   printf("\ntoken-->dotok");
   break;  
  case calltok:
   printf("\ntoken-->calltok");
   break;
  case consttok:
   printf("\ntoken-->consttok");
   break;
  case vartok:
   printf("\ntoken-->vartok");
   break;
  case proctok:
   printf("\ntoken-->procedure");
   break;
 }
}

/********************************************************************/
/*                            error                                 */
/*                manejador de errores con panico                   */
/*               para un unico error en el scanner                  */
/*                                                                  */
/*           Se marca el error y salida de panico.                  */
/*En algunos casos la marca de error se "barrera" hasta el siguiente*/
/*token, pues se sabe que se tiene el error hasta que se encuentra  */
/*el siguiente token. Ejemplo :                                     */
/* ..End                                                            */
/*   pi = 3 ...                                                     */
/*     ^ Error 5 : Falta un punto y coma                            */  
/********************************************************************/ 

void error(int no)
{     
 printf ("\n^ Error %d: Este número es demasiado grande",no);
 exit(1);
}

/*****************************************/
/*          init_spec                    */
/* Construccion de la tabla de operadores*/
/* y simboles especiales.                */
/*****************************************/

void init_spec()
{
  int i;
  for (i=0;i<=254;++i) /*inicializacion de lex especiales*/
    espec[i]=nulo;

  espec[43]=mas;
  espec[45]=menos;
  espec[42]=por;
  espec[47]=barra;
  espec[40]=parena;
  espec[41]=parenc;
  espec[61]=igl;
  espec[46]=punto;
  espec[44]=coma;
  espec[59]=puntoycoma;
}
