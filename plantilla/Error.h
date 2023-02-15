char *mensaje_de_error[]={"",
/*Error 1 */ "Usar ' = ' en vez de ' := ' ",
/*Error 2 */ "Debe ir seguido de un numero",
/*Error 3 */ "El identificador debe ir seguido de ' = ' ",
/*Error 4 */ "CONST, VAR y PROCEDURE deben ir seguidos de un identificador",
/*Error 5 */ "Falta una coma o un punto y coma",
/*Error 6 */ "Simbolo incorrecto despues de una declaracion de procedimiento",
/*Error 7 */ "Se esperaba una instruccion",
/*Error 8 */ "Simbolo incorrecto detras de las instrucciones de un bloque",
/*Error 9 */ "Se espera un punto",
/*Error 10*/ "Falta un punto y coma entre instrucciones",
/*Error 11*/ "Identificador no declarado",
/*Error 12*/ "No estan permitidas las asignaciones a constantes o procedimientos",
/*Error 13*/ "Se esperaba el operador de asignacion",
/*Error 14*/ "CALL debe de ir seguido de un identificador",
/*Error 15*/ "No tiene sentido llamar a una constante o a una variable",
/*Error 16*/ "Se espera un THEN",
/*Error 17*/ "Se espera un END o un punto y coma",
/*Error 18*/ "Se espera un DO",
/*Error 19*/ "Un simbolo incorrecto sigue a una instruccion",
/*Error 20*/ "Se esperaba un operador relacional",
/*Error 21*/ "Una expresion no debe de contener un identificador de procedimiento",
/*Error 22*/ "Falta un parentesis de cierre",
/*Error 23*/ "El factor anterior no puede ir seguido de este simbolo o falta un punto y coma",
/*Error 24*/ "Una expresion no puede empezar con este simbolo",
/*Error 25*/ "",
/*Error 26*/ "",
/*Error 27*/ "",
/*Error 28*/ "",
/*Error 29*/ "",
/*Error 30*/ "Este numero es demasiado grande - trunc(5)",
/*Error 31*/ "Se rebaso el tamaño de la Tabla de Simbolos",
/*Error 32*/ "El programa fuente esta incompleto",
/*Error 33*/ "El programa fuente es demasiado largo",
/*Error 34*/ "Anidamiento demasiado profundo para los procedures",
/*Error 35*/ "Se rebasa el maximo entero de PL-0. Se asume cero"};

/*Errores sintacticos : 1,2,3,4,5,6,7,8,9,10,13,16,17,18,19,20,22,23,24,32 */

/*Errores semanticos  : 11,12,14,15,35*/

/*Limitaciones especificas del compilador : 30,31,33,34*/



    
