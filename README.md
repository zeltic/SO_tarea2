# SO_tarea2
Tarea 2 de Sistemas Operativos

El codigo de pregunta 4 combina los codigos de todas las preguntas. Ádemas este esta comentado para entender mejor el codigo.
Adicionalmente se agrega una imagen con el grafo usado.

Para correr los codigos de pregunta 2 y 3 se necesita usar: g++ -pthread "pregunta 2.cpp" -o "pregunta 2"
Para correr el codigo de pregunta 4 se necesita usar: g++ -std=c++20 -pthread "pregunta 4.cpp" -o "pregunta 4"
Esto debido a que la libreria <semaphore> pertenece a C++20 (C++ version 2020).
El codigo es compatible tanto en Linux como en Windows, necesitando tan solo tener un compilador con soporte completo para C++20, como por ejemplo cualquier version de GCC 11 en adelante (GCC 10 no tiene soporte completo), o Microsoft Visual C++ 2020.

Para los grafos se usaron 2 clases, una para representar los nodos y otra para representar las rutas.
Al crear las rutas escojo entre que nodos ira y cual es su peso.
Al crear los threads uso un for loop para crear varios a la vez, mandandolos a la funcion de inicio.
Cuando los threads entran a la funcion de recorrido estos revisan si estan en el destino o no, si no estan en el destino entonces deciden avanzar a un nodo conectado de manera aleatoria, tras decidir a que nodo avanzar bloquean la ruta que usaran y liberan la ruta anterior que usaron, para luego proceder a avanzar al nuevo nodo. El bloqueo se realiza con un semaforo que permite a solo 3 threads usar una ruta a la vez. Cada ruta tiene su propio semaforo, teniendo un vector de semaforos.
Cuando los threads llegan al nodo de destino entonces proceden a revisar si el peso total de la ruta que tomaron es mejor que el guardado en mejor_peso, si es mejor entonces entran a una zona critica protegida por un mutex y guardan su ruta y peso como los mejores, ademas de imprimir aquella ruta y peso en pantalla.
Tras esto los threads proceden a volver al inicio, repitiendo todo el proceso.
Antes de volver a iniciar los threads revisan un while, que de condicion tiene que una variable atomica debe ser 0, si es asi entonces seguiran, pero en caso contrario saldran del while y uno por uno los threads finalizaran.
Si se desea finalizar el programa, la funcion Terminar permite cambiar el valor de esta variable atomica por un valor diferente a 0, finalizando los threads.
Finalmente los threads se unen, se imprime la mejor ruta encontrada en el grafo y se termina el programa.

Para entender mejor el programa se recomienda leer los comentarios de pregunta 4.cpp
