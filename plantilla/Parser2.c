/**********************************************************/
/* Compilador para el lenguaje PL-0. Check Syntax Only.   */
/* El compilador ya fue ampliado para tratar los errores  */
/* de una manera mas adecuada y realista con las reglas   */
/* de Wirth:                                              */
/* 1) Regla de las palabras reservadas                    */
/* 2) Don't panic rule                                    */
/* 3) Suposiciones sobre la intencion del programador     */
/*    basadas en la experiencia en el uso del lenguaje    */
/*                                                        */
/*                  Febrero de 2009                       */
/*             Uso : parser2 progfuente                   */
/*  Microsoft Visual C++ Version 6.0 Enterprise Edition   */
/**********************************************************/ 


#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "error.h"


/***************************************/
/*          Globales                   */
/***************************************/

#define MAXLINEA 1000  /*Tamaño maximo de una linea del programa fuente*/
#define MAXPAL     11  /*Numero de palabras reservadas                 */
#define MAXDIGIT    5  /*Maximo numero de digitos en los enteros       */
#define MAXID      10  /*Maxima longitud de los identificadores        */
#define MAXIT     100  /*Maxima longitud de la Tabla de Simbolos       */
#define LONG_FECHA 26  /*Longitud de la fecha a imprimir en el listado */
#define NOTOKENS   30  /*Numero de Tokens en el lenguaje               */

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


/*        Conjuntos de tokens para manejo de errores         */
/*Tokens iniciales de declaracion, de instruccion y de factor*/
int tokinidecl[NOTOKENS],tokiniinst[NOTOKENS],tokinifact[NOTOKENS];
int no_de_errores;

/*             Prototipos de funciones           */
void init_spec(),obtoken();
int obtch(),error(int no),getline(char s[],int lim),estadisticas();

void bloque(int toksig[]),declaracionconst(),declaracionvar(),poner(enum objeto k);
void instruccion(int toksig[]),expresion(int toksig[]),termino(int toksig[]);
void factor(int toksig[]),condicion(int toksig[]);
void test(int conjunto1[],int conjunto2[],int n),init_set(int conjunto[]);
void copia_set(int conjunto1[],int conjunto2[]);
void union_set(int conjunto1[],int conjunto2[],int conjunto3[]);
int posicion();

/***************************************/
/*              main                   */
/***************************************/

main (int argc,char *argv[])
{ 
  FILE *fopen();time_t timer;char fecha[LONG_FECHA];
  int set_arranque[NOTOKENS]; /*Conjunto de sincronizacion de arranque*/

  /*Analizar argumentos de main*/
  if (argc!=2)
   printf("\nUso : parser2 progfuente");
  else { 
    fp=fopen(argv[1],"r");
    if (fp==NULL) {
     printf("\n\n--No se encontró al Programa Fuente--");
     exit(1);
    }
    timer=time(NULL);
    strcpy(fecha,asctime(localtime(&timer)));
    printf("\n\nCompilador PL-0 ver 1.0 - Parser Front End -- Febrero 2009\n");
    printf("%s - %s\n",argv[1],fecha);
    init_spec() ; /*inicializacion de simbolos especiales*/
    ch=' ';
    fin_de_archivo=0;
    offset=-1;ll=0;
    no_de_errores=0; 

    /*inicializacion de conjuntos de tokens iniciales*/
    init_set(tokinidecl);
    tokinidecl[consttok]=tokinidecl[vartok]=tokinidecl[proctok]=1;
    
    init_set(tokiniinst);
    tokiniinst[calltok]=tokiniinst[begintok]=tokiniinst[iftok]=tokiniinst[whiletok]=1;
   
    init_set(tokinifact);
    tokinifact[ident]=tokinifact[numero]=tokinifact[parena]=1;

    /*inicializacion del set de arranque del parser con los tokens */
    /*iniciales de declaracion+tokens iniciales de instruccion+punto*/
    union_set(set_arranque,tokinidecl,tokiniinst);
    set_arranque[punto]=1;/*set_arranque=punto+tokinidecl+tokiiniinst*/
	 
    obtoken(); /*arrancar al scanner  */

    it=0; /*inicializamos el indice sobre la TDS*/
    bloque(set_arranque) ; /*activacion del parser*/
    if (token!=punto)
     error(9); /* Error 9 : Se esperaba un punto */
    estadisticas();
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
   error(32);/*Error 32: El programa fuente esta incompleto*/
   estadisticas();
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

/***************************************/
/*           estadisticas              */
/***************************************/

int estadisticas()
{
 printf("\n\n***   Estadisticas globales   ***\n");
 if (no_de_errores==0) 
  printf("***  No se detectaron errores ***");
 else {
  printf("*** %d error(es) detectado(s)  ***\n",no_de_errores);
  printf("*** El codigo objeto no sirve ***");
 }
 return(no_de_errores);
}
 
/********************************************************************/
/*                            error                                 */
/*               se marcan errores (sin panico)                     */
/*                manejador de errores con panico                   */
/*                                                                  */
/*En algunos casos la marca de error se "barrera" hasta el siguiente*/
/*token, pues se sabe que se tiene el error hasta que se encuentra  */
/*el siguiente token. Ejemplo :                                     */
/* ..End                                                            */
/*   pi = 3 ...                                                     */
/*     ^ Error 5 : Falta un punto y coma                            */  
/********************************************************************/ 

int error(int no)

{ 
 ++no_de_errores;
 printf("%*s^",offset," ");
 printf (" Error %d: %s\n",no,mensaje_de_error[no]);
 return(no_de_errores);
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
/*          init_set                     */
/* Inicializa un conjunto con cero en    */
/* todas las posiciones.                 */
/* Crea al conjunto vacio                */
/*****************************************/

void init_set(int conjunto[])
{ 
 int i;
 for (i=0;i<NOTOKENS;++i) 
   conjunto[i]=0;
}

/******************************************/
/*              copia_set                 */
/* copia el segundo conjunto en el primero*
/******************************************/

void copia_set(int conjunto1[],int conjunto2[])
{
 int i;
 for (i=0;i<NOTOKENS;++i)
  conjunto1[i]=conjunto2[i];
}

/******************************************/
/*              union_set                 */
/* conjunto1=conjunto2+conjunto3          *
/******************************************/

void union_set(int conjunto1[],int conjunto2[],int conjunto3[])
{
 int i;
 copia_set(conjunto1,conjunto2);
 for (i=0;i<NOTOKENS;++i)
  if (conjunto3[i]==1)
   conjunto1[i]=1;
}

/******************************************/
/*               test                     */
/* rutina de salto test y salto de texto  */
/* sobre el programa fuente.              *
/******************************************/

void test (int conjunto1[],int conjunto2[],int n)
{
 int conj_union[NOTOKENS];

 if (conjunto1[token]==0) { 
  /*el token no esta en el conjunto1*/
  error(n); /*se marca el error*/
  union_set(conj_union,conjunto1,conjunto2);
  /*se salta texto de manera inteligente*/
  while (conj_union[token]==0) 
   obtoken();
 }
}
  
/*****************************************/
/*                bloque                 */
/*                                       */
/*    Inicia el analisis sintactico      */
/*          		                 */
/*****************************************/

void bloque (int toksig[])
{
 int temp;
 int setpaso[NOTOKENS]; /*Conjunto de paso por valor*/
 int vacio[NOTOKENS];   /*Conjunto vacio            */

 init_set(vacio);
   
 do { 
     if (token==consttok) {
       obtoken();
       /*do-while:Equivale a insertar una coma*/
       do {
	   declaracionconst();/*Llamada a funcion que checa que la constante este*/
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
       } while (token==ident); 
     }

     if (token==vartok) {
       obtoken();
       /*analogo al do de arriba*/
       do {
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
       } while (token==ident);
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
      copia_set(setpaso,toksig);
      setpaso[puntoycoma]=1; /*setpaso=puntoycoma+toksig*/

      bloque(setpaso);       /*sucesor+simbolos de cajon*/

      it=temp;

      if (token==puntoycoma) {
	obtoken();
	copia_set(setpaso,tokiniinst);
	setpaso[ident]=setpaso[proctok]=1; /*setpaso=tokiniinst+ident+proctok*/
	test(setpaso,toksig,6); /*simbolo incorrecto despues de un procedimiento*/
      }
      else 
	error(5);

     }

     copia_set(setpaso,tokiniinst);
     setpaso[ident]=1; /*setpaso=tokiniinst+ident*/
     test(setpaso,tokinidecl,7); /*se esperaba una instruccion*/
 } while (tokinidecl[token]==1); /*salir cuando el token ya no sea de declaracion*/

 copia_set(setpaso,toksig);
 setpaso[puntoycoma]=setpaso[endtok]=1; /*setpaso=puntoycoma+end+toksig*/
 instruccion(setpaso);

 /*viene el chequeo explicito de que el token que viene a continuacion */
 /*esta en el conjunto de sucesores correctos (los sucesores de bloque)*/

 copia_set(setpaso,toksig);
 test(setpaso,vacio,8); /*simbolo incorrecto detras de las instrucciones de un bloque*/
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
  if ( (token==igl) || (token==asignacion) ) {
   /*mejora usando "adivinacion" de la intencion del programador"*/
   if (token==asignacion) 
     error(1); /*Error 1: Usar '=" en vez de ":="*/
   /*es la misma tecnica que en C se usa para if (x=) por if (x==)*/
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
	          /* Se rebaso el tama¤o de la Tabla de Simbolos */
  estadisticas();
  exit(1); /*El error es fatal*/
 }
 else {
  strcpy(tabla[it].nombre,lex); /*poner la lexeme*/
  tabla[it].tipo  =k;           /*poner el objeto*/
 }
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
 return(i);
}

/**************************************/
/*       instruccion                  */
/**************************************/
 
void instruccion(int toksig[])
{
 int i;
 int setpaso[NOTOKENS]; /*Conjunto de paso por valor*/
 int vacio[NOTOKENS];   /*Conjunto vacio            */

 init_set(vacio);

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

    copia_set(setpaso,toksig);
    expresion(setpaso);
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

	copia_set(setpaso,toksig);
	setpaso[thentok]=setpaso[dotok]=1; /*setpaso=thentok+dotok+toksig*/
	condicion(setpaso);

	if (token==thentok)
	 obtoken();
	else
	 error(16);/* Error 16: Se esperaba un "THEN" */
	
	copia_set(setpaso,toksig);
	instruccion(toksig);
       } else
	  if (token==begintok) {
	   obtoken();

	   copia_set(setpaso,toksig);
	   setpaso[puntoycoma]=setpaso[endtok]=1; /*setpaso=puntoycoma+endtok+toksig*/
	   instruccion(setpaso);

	   while (token==puntoycoma || token==calltok || token==begintok ||
		  token==iftok      || token==whiletok ) {
	     /*aqui el while 'inserta' el punto y coma para continuar  */
	     /*compilando cuando no lo encuentre. El compilador detecta*/
	     /*la omision clasica del punto y coma.                    */
	    if (token==puntoycoma)
	      obtoken();
	    else error(10); /*Error 10: Falta un punto y coma entre instrucciones*/

	    copia_set(setpaso,toksig);
	    setpaso[puntoycoma]=setpaso[endtok]=1; /*setpaso=puntoycoma+endtok+toksig*/
	    instruccion(setpaso);
	   }
	   if (token==endtok)
	    obtoken();
	   else
	    error(17); /* Error 17: Se esperaba un "END" o un punto y coma */
	  } else
	     if (token==whiletok) {
	      obtoken();

	      copia_set(setpaso,toksig);
	      setpaso[dotok]=1;/*setpaso=dotok+toksig*/
	      condicion(setpaso);

	      if (token==dotok) 
	       obtoken();
	      else
	       error(18); /* Error 18: Se esperaba un "DO" */

              copia_set(setpaso,toksig);
	      instruccion(setpaso);
	     }

 /*viene la comprobacion explicita de que los tokens*/
 /*que viene son sucesores de instruccion           */
 copia_set(setpaso,toksig);
 test(setpaso,vacio,19); /*Un simbolo incorrecto sigue a una instruccion*/
}	      

/****************************************/
/*              expresion               */
/****************************************/ 

void expresion(int toksig[])
{
 int setpaso[NOTOKENS]; /*Conjunto de paso por valor*/

 copia_set(setpaso,toksig);
 setpaso[mas]=setpaso[menos]=1; /*setpaso=mas+menos+toksig*/

 if (token==mas || token==menos) {
  obtoken();
  termino(setpaso);
 }
 else 
  termino(setpaso);

 while (token==mas || token==menos) {
  obtoken();
  copia_set(setpaso,toksig);
  setpaso[mas]=setpaso[menos]=1; /*setpaso=mas+menos+toksig*/
  termino(setpaso);
 }
}

/*****************************************/
/*              termino                  */
/*****************************************/

void termino(int toksig[])
{
 int setpaso[NOTOKENS]; /*Conjunto de paso por valor*/

 copia_set(setpaso,toksig);
 setpaso[por]=setpaso[barra]=1;/*setpaso=por+barra+toksig*/
 factor(setpaso);

 while (token==por || token==barra) {
  obtoken();

  copia_set(setpaso,toksig);
  setpaso[por]=setpaso[barra]=1;/*setpaso=por+barra+toksig*/
  factor(setpaso);
 }
}

/*****************************************/
/*              factor                   */
/*****************************************/

void factor(int toksig[])
{
 int i;
 int setpaso[NOTOKENS]; /*Conjunto de paso por valor*/

 test(tokinifact,toksig,24);/*Una expresion no puede empezar con este simbolo*/

 while (tokinifact[token]==1) {
  /*mientras el token sea un simbolo inicial de factor...*/
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

     copia_set(setpaso,toksig);
     setpaso[parenc]=1;/*setpaso=parenc+toksig*/
     expresion(setpaso);

     if (token==parenc)
       obtoken();
     else
      error(22); /* Error 22: Falta un parentesis de cierre */  
   };

   init_set(setpaso);  /*conjunto vacio*/
   setpaso[parena]=1;  /*setpaso=parena*/
   test(toksig,setpaso,23); /*el factor anterior no puede ir seguido de este simbolo o...*/
 }
}

/*****************************************/
/*              condicion                */
/*****************************************/
	  
void condicion(int toksig[])
{
 int setpaso[NOTOKENS]; /*Conjunto de paso por valor*/

 if (token==oddtok) {
  obtoken();
  copia_set(setpaso,toksig);/*setpaso=toksig*/
  expresion(setpaso);
 }
 else {
  /*en este caso los sucesores de expresion son los relacionales*/

  copia_set(setpaso,toksig);
  setpaso[igl]=setpaso[nig]=setpaso[mnr]=setpaso[myr]=setpaso[mei]=setpaso[mai]=1;
  /*setpaso=igl+nig+mnr+myr+mei+mai+toksig*/

  expresion(setpaso);
  if ( (token!=igl) && (token!=nig) && (token!=mnr) &&
       (token!=mei) && (token!=myr) && (token!=mai) )
   error(20); /* Error 20: Se esperaba un operador relacional */
  else {
   obtoken();

   copia_set(setpaso,toksig); /*setpaso=toksig*/
   expresion(setpaso);
  }
 }
}

