/**********************************************************/
/*             Interprete para codigo-p.                  */
/*                                                        */
/*                  Febrero de 2009                       */
/*             Uso : execp progobjeto.obp                 */
/* Microsoft Visual C++ Version 6.0 Enterprise Edition    */
/**********************************************************/ 


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAXIC     200  /*Tamaño del array codigo: instrucciones codigo-p*/
#define LONGSTACK 500  /*Longitud del stack de datos                    */

/*    Instrucciones del ensamblador (codigo-p)    */
enum fcn {LIT,OPR,CAR,ALM,LLA,INS,SAL,SAC};
typedef struct {
 enum fcn f; /*Mnemotecnico                       */
 int     ni; /*Nivel 0..MAXNIV                    */
 int     di; /*Direccion o desplazamiento 0..32767*/
} ensamblador;
ensamblador codigo[MAXIC]; /*array con las instrucciones de codigo-p*/
int ic;                    /*indice sobre el array codigo           */ 

FILE *obj;

int p[LONGSTACK] ;  /*        memoria de datos-stack.          */
		            /*el manejo del stack esta desguarnecido y */
		            /*no se usa ningun nivel de abstraccion.   */
		            /*          (tarea...).                    */

/*    Prototipos de funciones   */
void interpretar(void),listar_p(void);
int base(int ni,int b);


main(int argc,char *argv[])
{

  /*Analizar argumentos de main*/
  if (argc!=2)
   printf("\nUso : execp progobjeto.obp");
  else { 
   if ( (obj=fopen(argv[1],"r+b"))==NULL ) {
     printf("\nerror al abrir archivo %s",argv[1]);
     exit(1);
   };
   /*Leer el array de codigo*/
   ic=0;
   fread(&codigo[ic],sizeof(ensamblador),1,obj);
   while ( !feof(obj) ) {
    ++ic;
    fread(&codigo[ic],sizeof(ensamblador),1,obj);
   }
   /*listar_p();*/ /*Quita los comentarios para verificar que el array*/
		           /*de codigo ha sido cargado correctamente.         */
   interpretar();  /*Interpreta el codigo-p*/  
   fclose(obj);
  }
 
  return(0);
}

/**************************************************/
/*                interpretar                     */
/*                                                */
/*           interpreta codigo-p                  */
/**************************************************/

void interpretar(void)
{
 char *mnemonico(enum fcn x);
 register int d,b,s; /*d=registro de direccion del programa. apunta  */
		             /*a la siguiente instruccion por ejecutar.      */
		             /*b=registro de direccion base.                 */
		             /*s=apuntador al tope de stack.                 */
 
 ensamblador i;      /*registro de instruccion. contiene la siguiente*/
		             /*instruccion a ejecutar.                       */		     
			
 s=-1;b=0;d=0;
 p[0]=p[1]=p[2]=0;   /*ED,EE y DR para el programa principal*/
 
 do {
  
   i=codigo[d++];   
   printf("\n\nejecutando la instruccion %4d: %3s %5d %5d",d-1,mnemonico(i.f),i.ni,i.di); 
   
   switch(i.f) {
    case LIT:
     p[++s]=i.di;
	 printf("\nLIT : cargando la literal %d en la direccion %d (s en %d)",i.di,s,s);
     break;

    case OPR:
     printf("\nOPR : ");
     switch(i.di) {  /*cual operador?*/
      case 0: /*retornar o fin*/
       s=--b;
       d=p[s+3];
       b=p[s+2];
	   printf("retornar a la instruccion %d, base=%d (s en %d)",d,b,s);
       break;

      case 1:
	   printf("- unario para %d (s en %d)",p[s],s);
       p[s]=-p[s];
       break;

      case 2:
       --s;
	   printf("suma de %d + %d (s en %d)",p[s],p[s+1],s);
       p[s]=p[s]+p[s+1];
	   break;

      case 3:
	   --s;
       printf("resta de %d - %d (s en %d)",p[s],p[s+1],s);
       p[s]=p[s]-p[s+1];
       break;

      case 4:
	   --s;
       printf("multiplicacion de %d * %d (s en %d)",p[s],p[s+1],s);
       p[s]=p[s]*p[s+1];
       break;

      case 5:
       --s;
	   printf("division entera de %d / %d (s en %d)",p[s],p[s+1],s);
       if(p[s+1]==0) {
	    printf("\nError a tiempo de ejecucion\nSe intenta dividir entre cero");
	    printf("\nPrograma abortado!!!");
	    exit(1);
       }
       p[s]=p[s]/p[s+1];
       break;

      case 6:
       printf("odd(%d)? (s en %d)",p[s],s);
       p[s]=(p[s]%2!=0);
       
      case 7:
       /*reservado para la funcion primo*/
       break;

      case 8:
	   --s;
       printf("%d=%d? (s en %d)",p[s],p[s+1],s);
       p[s]=(p[s]==p[s+1]);
       break;

      case 9:
       --s;
       printf("%d!=%d? (s en %d)",p[s],p[s+1],s);
       p[s]=(p[s]!=p[s+1]);
       break;

      case 10:
       --s;
       printf("%d<%d? (s en %d)",p[s],p[s+1],s);
       p[s]=(p[s]<p[s+1]);
       break;

      case 11:
       --s;
       printf("%d>=%d? (s en %d)",p[s],p[s+1],s);
       p[s]=(p[s]>=p[s+1]);
       break;

      case 12:
       --s;
       printf("%d>%d? (s en %d)",p[s],p[s+1],s);
       p[s]=(p[s]>p[s+1]);
       break;

      case 13:
	   --s;
       printf("%d<=%d? (s en %d)",p[s],p[s+1],s);
       p[s]=(p[s]<=p[s+1]);
       break;       
     };
     break;
    
    case CAR:
     p[++s]=p[base(i.ni,b)+i.di];
	 printf("\nCAR : cargando %d en la direccion %d (s en %d)",p[base(i.ni,b)+i.di],s,s);
     break;

    case ALM:
     printf("\nALM : almacenando %d en la direccion %d (s en %d)",p[s],base(i.ni,b)+i.di,s-1);
     p[base(i.ni,b)+i.di]=p[s];
     --s;
     break;

    case LLA:
     /*se genera un nuevo bloque*/
     p[s+1]=base(i.ni,b);
     p[s+2]=b;
     p[s+3]=d;
     printf("\nLLA : activando subrutina, enlaces y DR: %d %d %d",p[s+1],p[s+2],p[s+3]);

     b=s+1;d=i.di;
     printf("\n    : nueva base %d, instruccion %d (s en %d)",b,d,s);
     break;

    case INS:
     printf("\nINS : asignando %d espacios en el stack (s en %d)",i.di,s+i.di);
     s+=i.di;
     break;

    case SAL:
     printf("\nSAL : saltando incondicionalmente a la instruccion %d (s en %d)",i.di,s);
     d=i.di;
     break;

    case SAC:
     printf("\nSAC : ");
     if (p[s]==0) {
      d=i.di;
      printf("la condicion es falsa. saltando condicionalmente a la instruccion %d.",d);
     }
     else
      printf("la condicion es verdadera. prosigo en la instruccion %d",d);

     --s;
	 printf("(s en %d)",s);
     break;
   };

 } while (d!=0);
}
 
/**************************************************/
/*                     base                       */
/*                                                */
/*encuentra la base ni niveles mas abajo en el    */
/*stack para el calculo actual (CAR,ALM,LLA).     */
/**************************************************/

int base(int ni,int b)
{
 int b1;
 b1=b;
 while (ni>0) {
  b1=p[b1];
  --ni;
 }
 return (b1);
}

/**************************************************/
/*                  listar_p                      */
/*                                                */
/*             lista el codigo-p                  */
/**************************************************/

void listar_p(void)
{
 int i;
 char mnemo[4];
 char *coment;
 
 printf("\n\n Listado de c¢digo-p simplificado\n\n");

 for(i=0;i<ic;++i) {
  switch(codigo[i].f) {
   case LIT:
    strcpy(mnemo,"LIT");
    coment=";cargar una constante";
    break;
   case OPR:
    strcpy(mnemo,"OPR");
    coment=";operacion aritmetica, relacional o retornar";
    break;	
   case CAR:
    strcpy(mnemo,"CAR");
    coment=";cargar una variable";
    break;
   case ALM:
    strcpy(mnemo,"ALM");
    coment=";almacenamiento instruccion de asignacion";
    break;
   case LLA:
    strcpy(mnemo,"LLA");
    coment=";llamada a procedimiento";
    break;
   case INS:
    strcpy(mnemo,"INS");
    coment=";asignacion de espacio de memoria";
    break;
   case SAL:
    strcpy(mnemo,"SAL");
    coment=";salto incondicional";
    break;
   case SAC:
    strcpy(mnemo,"SAC");
    coment=";salto condicional";
    break;   
  };
  printf("\n %4d  %3s %5d %5d %s",i,mnemo,codigo[i].ni,codigo[i].di,coment);
 }
}

char *mnemonico(enum fcn x)
{
  char *mnemo;
  switch(x) {
   case LIT:
    mnemo="LIT";
    break;
   case OPR:
    mnemo="OPR";
    break;	
   case CAR:
    mnemo="CAR";
    break;
   case ALM:
    mnemo="ALM";
    break;
   case LLA:
    mnemo="LLA";
    break;
   case INS:
    mnemo="INS";
    break;
   case SAL:
    mnemo="SAL";
    break;
   case SAC:
    mnemo="SAC";
    break;
  };
  
  return(mnemo);
}
  