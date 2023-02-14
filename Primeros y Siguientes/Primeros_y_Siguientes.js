
//PRIMEROS 

INICIO´ -> {const, var, funcion, identificador, 
    comprobar_color, comprobar_pared_arriba, comprobar_pared_atras,   
    comprobar_pared_derecha, comprobar_pared_izquierda, girar_izquierda,
    avanzar, apagate, parar, retroceder, si, mientras,λ}

//PRIMEROS DE INICIO
INICIO -> {const, var, funcion, identificador, 
    comprobar_color, comprobar_pared_arriba, comprobar_pared_atras,   
    comprobar_pared_derecha, comprobar_pared_izquierda, girar_izquierda,
    avanzar, apagate, parar, retroceder, si, mientras}

//PRIMEROS DE CONST
CONST -> {const}

//PRIMEROS DE VAR
VAR -> {var}

//PRIMEROS DE FUNCIÓN
FUNCION -> {funcion}

//PRIMEROS DE INSTRUCCIÓN
INSTRUCCION -> {identificador, comprobar_color, comprobar_pared_arriba, comprobar_pared_atras,   
    comprobar_pared_derecha, comprobar_pared_izquierda, girar_izquierda,
    avanzar, apagate, parar, retroceder, si, mientras}

//PRIMEROS DE EXPRESIÓN
EXPRESION -> {verdadero, falso, SIMBOLO_NEGACION, EXPRESION_RELACIONAL, EXPRESION_CONJUNCION }

//PRIMEROS DE VALOR
VALOR -> { numero_entero | numero_real | verdadero | falso}

//PRIMEROS DE EXPRESION NUMÉRICA
EXPRESION_NUMERICA ->  {SIMBOLO_NEGACION, EXPRESION_ARITMETICA, EXPRESION_RELACIONAL,EXPRESION_CONJUNCION }


//PRIMEROS DE EXPRESIÓN CONJUNCIÓN
EXPRESION_CONJUNCION -> {SIMBOLO_NEGACION, EXPRESION_RELACIONAL}

//PRIMEROS DE EXPRESIÓN RELACIONAL
EXPRESION_RELACIONAL -> {SIMBOLO_NEGACION}

//PRIMEROS DE SÍMBOLO
SIMBOLO -> {">","<" ,">=" ,"<=","!", "=","=="}

//PRIMEROS DE SÍMBOLO NEGACIÓN
SIMBOLO_NEGACION -> { "!"  } 

//PRIMEROS DE EXPRESIÓN ARITMETICA
EXPRESION_ARITMETICA -> {"+" , "-" , numero_entero, numero_real, comprobar_color, comprobar_pared_arriba , comprobar_pared_atras, comprobar_pared_derecha, comprobar_pared_izquierda, girar_izquierda, avanzar,apagate, parar, retroceder, FACTOR, TERMINO}

//PRIMEROS DE TÉRMINO
TERMINO -> {"+" , "-" , numero_entero, numero_real, comprobar_color, comprobar_pared_arriba , comprobar_pared_atras, comprobar_pared_derecha, comprobar_pared_izquierda, girar_izquierda, avanzar,apagate, parar, retroceder, FACTOR}

//PRIMEROS DE FACTOR
FACTOR -> {"+" , "-" , identificador, numero_real,comprobar_color, comprobar_pared_arriba, comprobar_pared_atras, comprobar_pared_derecha, comprobar_pared_izquierda, girar_izquierda, avanzar, apagate, parar}


//PRIMEROS DE PÁRAMETROS
PARAMETRO -> {identificador}

//PRIMEROS FUNCIONES CARRITO
FUNCIONES_CARRITO -> {comprobar_color, comprobar_pared_arriba, comprobar_pared_atras, comprobar_pared_derecha,comprobar_pared_izquierda, girar_izquierda, avanzar, apagate, parar, retroceder}

//PRIMEROS ARGUMENTOS
ARGUMENTO ->{booleano, numero_entero}

//PRIMEROS TIPO DE DATO
TIPO_DATO -> {booleano, numero_entero}

//----------------------------------------------------------
// SIGUIENTES


INICIO´ -> {$,λ}

//SIGUIENTES DE INICIO
INICIO -> {$}

//SIGUIENTES DE CONST
CONST -> {VAR, const}

//SIGUIENTES DE VAR
VAR -> {FUNCION, var}

//SIGUIENTES DE FUNCIÓN
FUNCION -> {INSTRUCCION, funcion}

//SIGUIENTES DE INSTRUCCIÓN
INSTRUCCION -> {$,"}",identificador}

//SIGUIENTES DE EXPRESIÓN
EXPRESION -> {")" , ";"}

//SIGUIENTES DE VALOR
VALOR -> {";"}

//SIGUIENTES DE EXPRESION NUMÉRICA
*EXPRESION_NUMERICA ->  {λ}

//SIGUIENTES DE EXPRESIÓN CONJUNCIÓN
EXPRESION_CONJUNCION -> {"||", SIMBOLO_NEGACION,EXPRESION_RELACIONAL, EXPRESION_ARITMETICA,EXPRESION_CONJUNCION,verdadero, falso}

//SIGUIENTES DE EXPRESIÓN RELACIONAL
EXPRESION_RELACIONAL -> {"&&",SIMBOLO_NEGACION, EXPRESION_RELACIONAL}

//SIGUIENTES DE SÍMBOLO
*SIMBOLO -> {EXPRESION_ARITMETICA}

//SIGUIENTES DE SÍMBOLO NEGACIÓN
*SIMBOLO_NEGACION -> {EXPRESION_ARITMETICA} 

//SIGUIENTES DE EXPRESIÓN ARITMETICA
EXPRESION_ARITMETICA -> {SIMBOLO,SIMBOLO_NEGACION}

//SIGUIENTES DE TÉRMINO
TERMINO -> {"+" , "-" , numero_entero, numero_real, comprobar_color, comprobar_pared_arriba , comprobar_pared_atras, comprobar_pared_derecha, comprobar_pared_izquierda, girar_izquierda, avanzar,apagate, parar, retroceder, FACTOR, TERMINO}

//SIGUIENTES DE FACTOR
FACTOR -> {"*" , "/" , "%", "+" , "-" , numero_entero, numero_real, comprobar_color, comprobar_pared_arriba , comprobar_pared_atras, comprobar_pared_derecha, comprobar_pared_izquierda, girar_izquierda, avanzar,apagate, parar, retroceder, FACTOR }

//SIGUIENTES DE PÁRAMETROS
PARAMETRO -> {")" , identificador}

//SIGUIENTES FUNCIONES CARRITO
*FUNCIONES_CARRITO -> {"+" , "-" , identificador, numero_real,comprobar_color, comprobar_pared_arriba, comprobar_pared_atras, comprobar_pared_derecha, comprobar_pared_izquierda, girar_izquierda, avanzar, apagate, parar }

//SIGUIENTES ARGUMENTOS
ARGUMENTO ->{")" , booleano, numero_entero}

//SIGUIENTES TIPO DE DATO
TIPO_DATO -> {identificador}