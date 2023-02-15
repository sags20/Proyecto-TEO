/**********************************************************/
/* Compilador para el lenguaje PL-0. Check Syntax Only    */
/* Luego sera ampliado para una mejor deteccion de errores*/
/*                PL-0 por Niklaus Wirth.                 */
/*                  Febrero de 2009                       */
/*               Uso : parser1 progfuente                 */
/*    Microsoft Visual C++ 6.0 Enterprise Edition         */
/**********************************************************/ 


#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


/***************************************/
/*          Globales                   */
/***************************************/

#define MAXLINEA 1000  /*Tamaño maximo de una linea del programa fuente*/
#define MAXPAL     11  /*Numero de palabras reservadas                 */
#define MAXDIGIT    5  /*Maximo numero de digitos en los enteros       */
#define MAXID      10  /*Maxima longitud de los identificadores        */
#define MAXIT     100  /*Maxima longitud de la Tabla de Simbolos       */
#define LONG_FECHA 26  /*Longitud de la fecha a imprimir en el listado */

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

/*Definicion de la tabla de simbolos - Organizacion y acceso lineal*/
enum objeto {CONSTANTE,VARIABLE,PROCEDIMIENTO};
typedef struct {
 char nombre[MAXID+1];
 enum objeto tipo;
} registro ; 

registro tabla[MAXIT+1]; /*Tabla de simbolos (+1 porque tabla[0] esta reservada)*/
int it;                  /*variable global sobre la tabla de simbolos           */

/*             Prototipos de funciones           */
void init_spec(),obtoken(),error(int no);
int obtch(),getline(char s[],int lim);

void bloque(),declaracionconst(),declaracionvar(),poner(enum objeto k);
void instruccion(),expresion(),termino(),factor(),condicion();
int posicion();

/***************************************/
/*              main                   */
/***************************************/

main (int argc,char *argv[])
{ 
  FILE *fopen();time_t timer;char fecha[LONG_FECHA];
  /*Analizar argumentos de main*/
  if (argc!=2)
   printf("\nUso : parser1 progfuente");
  else { 
    fp=fopen(argv[1],"r");
    if (fp==NULL) {
     printf("\n\n--No se encontró al Programa Fuente--");
     exit(1);
    }
    timer=time(NULL);
    strcpy(fecha,asctime(localtime(&timer)));
    printf("\n\nCompilador PL-0 ver 1.0 - Parser Check Syntax Only -- Febrero 2009\n");
    printf("%s - %s\n",argv[1],fecha);
    init_spec() ; /*inicializacion de simbolos especiales*/
    ch=' ';
    fin_de_archivo=0;
    offset=-1;ll=0;
    
    obtoken(); /*arrancar al scanner  */

    it=0;      /*inicializamos el indice sobre la TDS*/
    bloque() ; /*activacion del parser*/
    if (token!=punto)
     error(9); /* Error 9 : Se esperaba un punto */

     printf("\n\n***   Estadisticas globales  ***\n");
     printf("*** No se detectaron errores ***");
	 fclose(fp);
      
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
   printf("\nEl programa fuente esta incompleto");
   exit(1);
 }
  
 if (offset==ll-1) {
  ll=getline(linea,MAXLINEA);
  if (ll==0) 
   fin_de_archivo=1; /*se retrasa en un blanco la deteccion de EOF, porque*/
		             /*obtoken lleva un caracter adelantado. si no, en    */
		             /*algunos casos, p/ejem, no se reconoceria el punto  */
		             /*del programa (...end.)                             */
   printf("%s",linea);
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
    /*printf("\nLex en el obtoken = %s",lex);*/
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
/* printf("\nLexeme %s",lexid);*/
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
 printf ("\n^ Error %d:",no);
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

/*****************************************/
/*                bloque                 */
/*                                       */
/*    Inicia el analisis sintactico      */
/*                                	 */
/*****************************************/

void bloque ()
{
 int temp;

 if (token==consttok) {
  obtoken();
  declaracionconst(); /*Llamada a funcion que checa que la constante este*/
		              /*bien definida.                                   */
		              /*Si lo esta, se coloca el identificador en la TDS */
  while (token==coma) {
   obtoken();
   declaracionconst();
  }
  if (token==puntoycoma) 
   obtoken();
  else
   error(5); /* Error 5 :  Falta una coma o un punto y coma */
 }

 if (token==vartok) {
  obtoken();
  declaracionvar(); /*Llamada a funcion que checa que la variable este*/
		            /*bien declarada.                                 */
		            /*Si lo esta, se coloca el identificador en la TDS*/
  while (token==coma) {
   obtoken();
   declaracionvar();
  }
 if (token==puntoycoma)
  obtoken();
 else
  error(5); /* Error 5 :  Falta una coma o un punto y coma */
 }
  
 while (token==proctok) {
  obtoken();
  if (token==ident) {
   poner(PROCEDIMIENTO);
   obtoken();
  }
  else
   error(4); /* Error 4: Const, Var y Procedure deben ir seguidos */
	         /* de un identificador                               */
  if (token==puntoycoma)
   obtoken();
  else
   error(5); /* Error 5 :  Falta una coma o un punto y coma */

  temp=it;
  bloque();
  it=temp;

  if (token==puntoycoma)
   obtoken();
  else 
  error(5);

 }
 instruccion();
}

/***********************************************************************/
/*                       declaracionconst                              */
/*                                                                     */
/* se checa que se tenga una definicion de constante. si esta bien     */
/* el identificador se pone en la tabla de simbolos, otro caso         */
/* el usuario cometio un error al construir la definicion de constante */
/***********************************************************************/

void declaracionconst()
{
 if (token==ident) {
  obtoken();
  if (token==igl) {
   obtoken();
   if (token==numero) {
    poner(CONSTANTE);
    obtoken();
   }
   else
    error(2) ; /* Error 2 : Debe ir seguido de un numero */
  }
  else
   error (3) ; /* Error 3: El identificador debe ir seguido de "=" */
 }
 else
  error(4) ; /* Error 4: Const, Var y Procedure deben ir seguidos */
	         /* de un identificador                               */
}

/***********************************************************************/
/*                          declaracionvar                             */
/*                                                                     */
/* se checa que se tenga una declaracion de variable. si esta bien     */
/* el identificador se pone en la tabla de simbolos, otro caso         */
/* el usuario cometio un error al construir la declaracion de variable */
/***********************************************************************/

void declaracionvar()
{
 if (token==ident) {
  poner(VARIABLE);
  obtoken();
 }
 else
  error(4) ; /* Error 4: Const, Var y Procedure deben ir seguidos */
	         /* de un identificador                               */
}

/****************************************************************/
/*                          poner                               */
/*                                                              */
/*poner un objeto: CONSTANTE, VARIABLE o PROCEDIMIENTO en la TDS*/
/****************************************************************/

void poner(enum objeto k)
{
 ++it;
 if (it>MAXIT) {
  error(31) ; /* Error 31 : Limitacion del compilador        */
	          /* Se rebaso el tamaño de la Tabla de Simbolos */
 }
 else {
  strcpy(tabla[it].nombre,lex); /*poner la lexeme*/
  tabla[it].tipo  =k;           /*poner el objeto*/
 }
 /*printf("\nTDS: Poniendo en it=%d al Lex %s",it,lex);*/
}

/*************************************************************/
/*                         posicion                          */
/*                                                           */
/*encontrar en la tabla de simbolos al identificador para    */
/*ver si ya fue declarado y si su uso es semanticamente legal*/
/*************************************************************/

int posicion()
{
 int i;
   
 strcpy(tabla[0].nombre,lex);
 i=it;
 while (  (strcmp(tabla[i].nombre,lex)) !=0)
  --i;
 /*printf("\nTDS: Buscando al Lex %s -- Exito=%d",lex,i);*/
 return(i);
 
}

/**************************************/
/*       instruccion                  */
/**************************************/
 
void instruccion()
{
 int i;
 if (token==ident) {
  /*ve a buscarlo a la tabla de simbolos*/
  i=posicion() ;
  if (i==0)
   error(11); /* Error 11: Identificador no declarado */
   else 
    if (tabla[i].tipo != VARIABLE)
     error(12); /* Error 12: No estan permitidas las asignaciones a */
		        /* constantes o a procedimientos.                   */
    obtoken();
    if (token==asignacion)
     obtoken();
    else
     error(13) ; /* Error 13: Se esperaba el operador de asignacion */
    expresion();
 } else 
    if (token==calltok) {
     obtoken();
     if (token!=ident)
      error(14); /* Error 14: "CALL" debe ir seguido de un identificador */
     else {
       /*Buscar el nombre del procedimiento en la tabla de simbolos*/
       i=posicion();
       if (i==0)
	error(11); /* Error 11: Identificador no declarado */
       else
	if (tabla[i].tipo!=PROCEDIMIENTO)
	 error(15); /* Error 15 : No tiene sentido llamar a una */
		        /* constante o a una variable               */
       obtoken();
      }
    } else
       if (token==iftok) {
	obtoken();
	condicion();
	if (token==thentok)
	 obtoken();
	else
	 error(16);/* Error 16: Se esperaba un "THEN" */
	instruccion();
       } else
	  if (token==begintok) {
	   obtoken();
	   instruccion();
	   while (token==puntoycoma) {
	    obtoken();
	    instruccion();
	   }
	   if (token==endtok)
	    obtoken();
	   else
	    error(17); /* Error 17: Se esperaba un "END" o un punto y coma */
	  } else
	     if (token==whiletok) {
	      obtoken();
	      condicion();
	      if (token==dotok) 
	       obtoken();
	      else
	       error(18); /* Error 18: Se esperaba un "DO" */
	      instruccion();
	     }
}	      

/****************************************/
/*              expresion               */
/****************************************/ 

void expresion()
{
 if (token==mas || token==menos) {
  obtoken();
  termino();
 }
 else 
  termino();

 while (token==mas || token==menos) {
  obtoken();
  termino();
 }
}

/*****************************************/
/*              termino                  */
/*****************************************/

void termino()
{
 factor();
 while (token==por || token==barra) {
  obtoken();
  factor();
 }
}

/*****************************************/
/*              factor                   */
/*****************************************/

void factor()
{
 int i;
 if (token==ident) {
  i=posicion();
  if (i==0) 
   error(11); /* Error 11: Identificador no declarado */
  else
   if (tabla[i].tipo==PROCEDIMIENTO)
    error(21); /* Error 21: Una expresion no debe contener un */
	           /* identificador de procedimiento              */
  obtoken();
 }
 else 
  if (token==numero)
   obtoken();
  else
   if (token==parena) {
    obtoken();
    expresion();
    if (token==parenc)
     obtoken();
    else
     error(22); /* Error 22: Falta un parentesis de cierre */  
   }
   else 
    error(23); /* Error 23: El factor anterior no puede ir seguido */
	           /* de este simbolo                                  */
}

/*****************************************/
/*              condicion                */
/*****************************************/
	  
void condicion()
{
 if (token==oddtok) {
  obtoken();
  expresion();
 }
 else {
  expresion();
  if ( (token!=igl) && (token!=nig) && (token!=mnr) &&
       (token!=mei) && (token!=myr) && (token!=mai) )
   error(20); /* Error 20: Se esperaba un operador relacional */
  else {
   obtoken();
   expresion();
  }
 }
}

    
