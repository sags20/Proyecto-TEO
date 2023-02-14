INICIO´ -> INICIO | λ

//GRAMÁTICA DE INICIO
INICIO -> CONST VAR FUNCION INSTRUCCION

//GRAMÁTICA DE CONST
CONST -> const identificador = VALOR ";"  | const identificador = VALOR ";"  CONST 

//GRAMÁTICA DE VAR
VAR -> var identificador = EXPRESION ";"  | var identificador = EXPRESION ";"  VAR 

//GRAMÁTICA DE FUNCIÓN
FUNCION -> funcion identificador ( ARGUMENTO ) { INSTRUCCION } | funcion identificador ( ARGUMENTO ) { INTRUCCION } FUNCION 

//GRAMÁTICA DE INSTRUCCIÓN
INSTRUCCION -> identificador ( PARAMETRO ) ";"  |  comprobar_color | comprobar_pared_arriba | comprobar_pared_atras|comprobar_pared_derecha | comprobar_pared_izquierda | girar_izquierda | avanzar | apagate | parar |retroceder | identificador (  ) ";"   | si ( EXPRESION ) { INSTRUCCION } |  si ( EXPRESION ) { INSTRUCCION } sino { INSTRUCCION } | mientras ( EXPRESION ) { INSTRUCCION } | identificador = EXPRESION ";" 

//GRAMÁTICA DE EXPRESIÓN
EXPRESION -> verdadero | falso | SIMBOLO_NEGACION EXPRESION_ARITMETICA | SIMBOLO_NEGACION EXPRESION_ARITMETICA SIMBOLO EXPRESION_ARITMETICA | EXPRESION_RELACIONAL "&&" EXPRESION_RELACIONAL | EXPRESION_CONJUNCION "||" EXPRESION_CONJUNCION 

//GRAMÁTICA DE VALOR
VALOR -> numero_entero | numero_real | verdadero | falso

//GRAMÁTICA DE EXPRESION NUMÉRICA
EXPRESION_NUMERICA -> SIMBOLO_NEGACION EXPRESION_ARITMETICA | SIMBOLO_NEGACION EXPRESION_ARITMETICA SIMBOLO EXPRESION_ARITMETICA | EXPRESION_RELACIONAL "&&" EXPRESION_RELACIONAL | EXPRESION_CONJUNCION "||" EXPRESION_CONJUNCION 

//GRAMÁTICA DE EXPRESIÓN CONJUNCIÓN
EXPRESION_CONJUNCION -> SIMBOLO_NEGACION EXPRESION_ARITMETICA | SIMBOLO_NEGACION EXPRESION_ARITMETICA SIMBOLO EXPRESION_ARITMETICA | EXPRESION_RELACIONAL "&&" EXPRESION_RELACIONAL

//GRAMÁTICA DE EXPRESIÓN RELACIONAL
EXPRESION_RELACIONAL -> SIMBOLO_NEGACION EXPRESION_ARITMETICA | SIMBOLO_NEGACION EXPRESION_ARITMETICA SIMBOLO EXPRESION_ARITMETICA

//GRAMÁTICA DE SÍMBOLO
SIMBOLO ->  ">" | "<" | " >=" | "<=" | "!=" | "== " 

//GRAMÁTICA DE SÍMBOLO NEGACIÓN
SIMBOLO_NEGACION -> "!"  

//GRAMÁTICA DE EXPRESIÓN ARITMETICA
EXPRESION_ARITMETICA -> "+"  identificador | "-" identificador | identificador | numero_entero | "+"  numero_entero | "-" numero_entero | numero_real | "+"  numero_real | "-" numero_real | comprobar_color | comprobar_pared_arriba | comprobar_pared_atras|comprobar_pared_derecha | comprobar_pared_izquierda | girar_izquierda | avanzar | apagate | parar |retroceder | "+"  FUNCIONES_CARRITO | "-" FUNCIONES_CARRITO | FACTOR * FACTOR | FACTOR / FACTOR | FACTOR % FACTOR  | TERMINO "+"  TERMINO | TERMINO "-" TERMINO

//GRAMÁTICA DE TÉRMINO
TERMINO -> "+"  identificador | "-" identificador | identificador | numero_entero | "+"  numero_entero | "-" numero_entero | numero_real | "+"  numero_real | "-" numero_real | comprobar_color | comprobar_pared_arriba | comprobar_pared_atras|comprobar_pared_derecha | comprobar_pared_izquierda | girar_izquierda | avanzar | apagate | parar |retroceder | "+"  FUNCIONES_CARRITO | "-" FUNCIONES_CARRITO | FACTOR * FACTOR | FACTOR / FACTOR | FACTOR % FACTOR 

//GRAMÁTICA DE FACTOR
FACTOR -> "+"  identificador | "-" identificador | identificador | numero_entero | "+"  numero_entero | "-" numero_entero | numero_real | "+"  numero_real | "-" numero_real | comprobar_color | comprobar_pared_arriba | comprobar_pared_atras|comprobar_pared_derecha | comprobar_pared_izquierda | girar_izquierda | avanzar | apagate | parar |retroceder | "+"  FUNCIONES_CARRITO | "-" FUNCIONES_CARRITO
 
//GRAMÁTICA DE PÁRAMETROS
PARAMETRO -> identificador | identificador , PARAMETRO 

//GRAMÁTICA FUNCIONES CARRITO
FUNCIONES_CARRITO -> comprobar_color | comprobar_pared_arriba | comprobar_pared_atras|comprobar_pared_derecha | comprobar_pared_izquierda | girar_izquierda | avanzar | apagate | parar |retroceder

//GRAMÁTICA ARGUMENTOS
ARGUMENTO -> TIPO_DATO identificador | TIPO_DATO identificador , ARGUMENTO

//GRAMÁTICA TIPO DE DATO
TIPO_DATO -> booleano | numero_entero