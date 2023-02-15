/**********************************************************/
/* Scanner-Analizador Lexicografico para el lenguaje PL-0 */
/*                  Febrero de 2023                       */
/*         Uso : scanner puro trabajo honesto             */
/**********************************************************/

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

/***************************************/
/*          Globales                   */
/***************************************/

#define MAXLINEA 1000 /*Tamaño maximo de una linea del programa fuente*/
#define MAXPAL 16     /*Numero de palabras reservadas                 */
#define MAXDIGIT 5    /*Maximo numero de digitos en los enteros       */
#define MAXID 20      /*Maxima longitud de los identificadores        */

/*Lexemes de las palabras reservadas*/
char *lexpal[MAXPAL] = {"CONST", "VAR", "FUNCION", "MIENTRAS", "SI", "SINO", "GIRARIZQUIERDA", "PARAR", "RETROCEDER", "APAGATE", "COMPROBARCOLOR", "COMPROBARPAREDARRIBA", "COMPROBARPAREDABAJO", "COMPROBARPAREDDERECHA", "COMPROBARPAREDIZQUIERDA"};

char linea[MAXLINEA]; /*Buffer de lineas                    */
int offset, ll;       /*Corrimiento y contador de caracteres*/
int fin_de_archivo;   /*bandera de fin de archivo (obtch)   */
int ch;

/*Lexeme del ultimo identificador leido*/
char lex[MAXID + 1]; /* +1 para colocar \0 */

/*Valor numerico del ultimo numero leido*/
long int valor;

FILE *fp;

/*Lista de Tokens de pl-0*/

enum simbolo
{
  identificador,
  numero,
  mas,
  menos,
  por,
  barra,
  igual,
  negacion,
  negacionigual,
  menor,
  mayor,
  mayoroigual,
  menoroigual,
  parentesisabireto,
  parentesiscerrado,
  coma,
  puntoycoma,
  asignacion,
  llaveabierto,
  llavecerrado,
  si,
  sino,
  mientras,
  const,
  var,
  funcion,
  girarizquierda,
  parar,
  retroceder,
  apagate,
  comprobarcolor,
  comprobarparedarriba,
  comprobarparedabajo,
  comprobarparedderecha,
  comprobarparedizquierda
};

enum simbolo token;

/*Tabla de tokens de palabras reservadas*/

enum simbolo tokpal[MAXPAL] = {si, sino, mientras, const, var, funcion, girarizquierda, parar, retroceder, apagate, comprobarcolor, comprobarparedarriba, comprobarparedabajo, comprobarparedderecha, comprobarparedizquierda};

/*Tabla de tokens de operadores y simbolos especiales*/
enum simbolo espec[255];

/*             Prototipos de funciones                ????????????????????????????????????? */
void init_spec(), obtoken(), imprime_token(), error(int no);
int obtch(), getline(char s[], int lim);

/***************************************/
/*              main                   */
/***************************************/

main(int argc, char *argv[])
{
  FILE *fopen();
  /*Analizar argumentos de main*/
  if (argc != 2)
    printf("\nUso : scanner progfuente");
  else
  {
    fp = fopen(argv[1], "r");
    if (fp == NULL)
    {
      printf("\n\n--No se encontro al Programa Fuente--");
      exit(1);
    }
    printf("\n\nCompilador PL-0 ver 1.0 - Solo Scanner -- Febrero 2023\n\n");
    init_spec(); /*inicializacion de simbolos especiales*/
    ch = ' ';
    fin_de_archivo = 0;
    offset = -1;
    ll = 0;
    while (1)
    {
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
  if (fin_de_archivo == 1)
  {
    printf("\nFinal del programa");
    exit(1);
  }

  if (offset == ll - 1)
  {
    ll = getline(linea, MAXLINEA);
    if (ll == 0)
      fin_de_archivo = 1; /*se retrasa en un blanco la deteccion de EOF, porque*/
    /*obtoken lleva un caracter adelantado. si no, en    */
    /*algunos casos, p/ejem, no se reconoceria el punto  */
    /*del programa (...end.)                             */
    /*printf("%s",linea);*/ /*Quitar el comentario para obtener listado del fuente*/
    offset = -1;
  }

  ++offset;
  if ((linea[offset] == '\0') || (fin_de_archivo == 1))
    return (' ');
  else
  {
    return (toupper(linea[offset]));
  }
}

/****************************************/
/*              getline                 */
/* lee la siguiente linea del fuente y  */
/* regresa su tama�o (incluyendo '\n') o*/
/* 0 si EOF. (eje. para VAR regresa 4)  */
/* pag 28 de K&R                        */
/****************************************/

int getline(char s[], int lim)
{
  int c, i;
  for (i = 0; i < lim - 1 && (c = getc(fp)) != EOF && c != '\n'; ++i)
    s[i] = c;
  if (c == '\n')
  {
    s[i] = c;
    ++i;
  }
  s[i] = '\0';
  return (i);
}

/***************************************/
/*              obtoken                */
/* Obtiene el siguiente token del      */
/* programa fuente.                    */
/***************************************/

void obtoken()
{
  char lexid[MAXID + 1]; /* +1 para colocar \0 */
  int i, j;
  int ok = 0;

  while (ch == ' ' || ch == '\n' || ch == '\t')
    ch = obtch(); /*quitar blancos*/
  /*Si comienza con una letra es identificador o palabra reservada*/
  if (isalpha(ch))
  {
    lexid[0] = ch;
    i = 1;
    while (isalpha((ch = obtch())) || isdigit(ch))
      if (i < MAXID)
        lexid[i++] = ch;
    lexid[i] = '\0';

    /*¿es identificador o palabra reservada?*/
    for (j = 0; j < MAXPAL; ++j)
      if (strcmp(lexid, lexpal[j]) == 0)
      {
        ok = 1;
        break;
      }

    if (ok == 1)
    {
      token = tokpal[j];
    }
    else
    {
      token = ident;
    }

    strcpy(lex, lexid);
    /* printf("\nLast Lex %s",lex); */
  }
  else /*si comienza con un digito...*/
  {
    if (isdigit(ch))
    {
      lexid[0] = ch;
      i = j = 1;
      while (isdigit((ch = obtch())))
      {
        if (i < MAXDIGIT)
          lexid[i++] = ch;
        j++;
      }
      lexid[i] = '\0';
      if (j > MAXDIGIT)
        error(30); /*Este numero es demasiado grande*/
      token = numero;
      valor = atol(lexid);
      /*printf("\n numero --> %ld",valor);*/
    }
  }
  else
  { /*simbolos especiales, incluyendo pares de simbolos (Lookahead Symbol) */

    if (ch == '!')
    {
      ch = obtch();
      if (ch == '=')
      {
        token = negacionigual
            ch = obtch();
      }
      else
      {
        token = negacion;
      }
    }
    else if (ch == '<')
    {
      ch = obtch();
      if (ch == '=')
      {
        token = menoroigual;
        ch = obtch();
      }
      else
      {
        token = menor;
      }
    }
    else if (ch == '>')
    {
      ch = obtch();
      if (ch == '=')
      {
        token = mayoroigual;
        ch = obtch();
      }
      else
        token = mayor;
    }
    else if (ch == '=')
    {
      ch = obtch();
    }
    else
    {
      ch = asignacion;
    }
    else
    {
      token = espec[ch];
      ch = obtch();
    }

    /*printf("\nLexeme %s",lexid);*/ /*Si quieres ver la ultima lexeme*/
  }
}

/***************************************/
/*          imprime_token              */
/* utilitario del programador para     */
/* imprimir un token.                  */
/***************************************/

void imprime_token()
{
  switch (token)
  {
  case identificador:
    printf("\ntoken-->identificador");
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
  case igual:
    printf("\ntoken-->igual");
    break;
  case negacion:
    printf("\ntoken-->negacion");
    break;
  case negacionigual:
    printf("\ntoken-->negacionigual");
    break;
  case menor:
    printf("\ntoken-->menor");
    break;
  case menoroigual:
    printf("\ntoken-->menoroigual");
    break;
  case mayor:
    printf("\ntoken-->mayor");
    break;
  case mayoroigual:
    printf("\ntoken-->mayoroigual");
    break;
  case parentesisabireto:
    printf("\ntoken-->parentesisabireto");
    break;
  case parentesiscerrado:
    printf("\ntoken-->parentesiscerrado");
    break;
  case coma:
    printf("\ntoken-->coma");
    break;
  case puntoycoma:
    printf("\ntoken-->puntoycoma");
    break;
  case asignacion:
    printf("\ntoken-->asignacion");
    break;
  case llaveabierto:
    printf("\ntoken-->llaveabierto");
    break;
  case llavecerrado:
    printf("\ntoken-->llavecerrado");
    break;
  case si:
    printf("\ntoken-->si");
    break;
  case sino:
    printf("\ntoken-->sino");
    break;
  case mientras:
    printf("\ntoken-->mientras");
    break;
  case const:
    printf("\ntoken-->const");
    break;
  case var:
    printf("\ntoken-->var");
    break;
  case funcion:
    printf("\ntoken-->funcion");
    break;
  case girarizquierda:
    printf("\ntoken-->girarizquierda");
    break;
  case parar:
    printf("\ntoken-->parar");
    break;
  case retroceder:
    printf("\ntoken-->etroceder");
    break;
  case apagate:
    printf("\ntoken-->apagate");
    break;
  case comprobarcolor:
    printf("\ntoken-->comprobarcolor");
    break;
  case comprobarparedarriba:
    printf("\ntoken-->comprobarparedarriba");
    break;
  case comprobarparedabajo:
    printf("\ntoken-->comprobarparedabajo");
    break;
  case comprobarparedizquierda:
    printf("\ntoken-->comprobarparedizquierda");
    break;
  case comprobarparedderecha:
    printf("\ntoken-->comprobarparedderecha");
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
  printf("\n^ Error %d: Este n�mero es demasiado grande", no);
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
  for (i = 0; i <= 254; ++i) /*inicializacion de lex especiales*/
    espec[i] = nulo;

  espec[43] = mas;
  espec[45] = menos;
  espec[42] = por;
  espec[47] = barra;
  espec[40] = parena;
  espec[41] = parenc;
  espec[61] = igl;
  espec[46] = punto;
  espec[44] = coma;
  espec[59] = puntoycoma;
}
