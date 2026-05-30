# Report

## 1. Runtime complexity analysis of initializing the intersections map in Big-O.

La inicialización del mapa de intersecciones se encuentra implementada en la función `full_table(HashTable* table, Street* street_list)` dentro del archivo `hashmap.c`.

El programa lee la lista enlazada de calles que ya tenemos cargada en memoria y mete cada segmento en la tabla usando los IDs de sus esquinas. (En concreto con el id1)

Llamaremos a S el número total de segmentos de calle que hay en el archivo del mapa.
Recorremos la lista de calles (enlazada) de arriba a abajo una sola vez, por lo tanto nos cuesta un tiempo de **$O(S)$**.

Por cada segmento, calculamos su posición en la tabla con la función hash ($O(1)$) `hash_function(long long id, int table_size)` y lo metemos al principio del cajón (bucket) correspondiente con la función `insert_table(HashTable* table, long long id, Street* segment)`.

**Mejor caso, Peor caso y Caso promedio: $O(S)$**
En este proceso no existen diferencias entre escenarios. El programa está obligado a vaciar la lista de calles entera para construir la tabla, por lo que el bucle principal se ejecutará exactamente $S$ veces siempre.

## 2. Runtime complexity analysis of finding the coordinates of a street or place given the name in Big-O.
(Voy a suponer que cuando en la pregunta pone complexity analysis of finding the coordinates of a street, se refiere a la complejidad de encontrar las coordenadas de la house)

En nuestro programa, cuando buscamos una casa (house) usando su nombre y número, o un lugar (place) usando su nombre, lo que hacemos es recorrer con un bucle la lista enlazada (`house_list` o `place_list`) comparando los nombres uno por uno con `strcasecmp` hasta encontrar el correcto.

Para el caso de house:
Utilizamos `find_house_coordinates`. Como las "houses" están guardadas en una lista enlazada hacemos una búsqueda lineal (`while(actual != NULL)`) comprobando el nombre y el número en cada iteración. (Y como dije con strcasecmp que ignora mayus o minusculas)
En el peor de los casos (que la calle no exista o sea la última), el programa tiene que mirar todos los registros. Por lo tanto tiene que revisar $N$ elementos (houses) por lo que será **$O(N)$**.

Para el caso de place:
Es prácticamente lo mismo usamos la función `find_place_coordinates`. Y recorremos la lista enlazada, pero aqui cambia una pequeña cosa, además de buscar comprobar el nombre también comprobamos el tipo, así que supongo que será más fácil de encontrar, pero igualmente el peor caso será recorrer toda la lista, es decir mirar N elementos (places) por lo que será  **$O(N)$**.

**Mejor caso: $O(1)$ (Constante)**
Ocurre cuando el elemento que busca el usuario (la casa o el lugar) resulta ser el primero de la lista enlazada. El bucle `while` se ejecuta una sola vez, la comparación con `strcasecmp` coincide y la función termina sin tener que mirar el resto de la lista.

**Caso promedio: $O(N)$ (Lineal)**
Por estadística, lo normal es encontrar el elemento hacia la mitad de la lista enlazada (exactamente en la posición $N/2$). Como en la notación Big-O las constantes se descartan (el $/2$ desaparece), la complejidad del caso promedio sigue siendo **$O(N)$**.

## 3. Runtime complexity analysis of your path-finding algorithm in Big-O.

Para buscar la ruta entre dos puntos utilizamos el algoritmo BFS.

Llamaremeos $E$ al numero de total de segmentos del mapa, en nuestro codigo para ver si una calle ha sido ya explorada es con la función `is_street_visited`, donde hay un bucle `for` que recorre un array dinámico (`Street** visited`) de arriba a abajo haciendo comparaciones de IDs y cadenas de texto con `strcmp`.

* En el peor de los casos (cuando el mapa es muy grande o el destino está muy lejos), el array de visitados puede llegar a almacenar casi todos los segmentos de calle del mapa, alcanzando un tamaño de $O(E)$.
* Como este bucle que tarda $O(E)$ se ejecuta cada vez que sacamos una calle de la cola y también cada vez que revisamos los segmentos vecinos en la HashTable, el coste se multiplica.

Por lo tanto, la complejidad real de nuestro algoritmo de búsqueda de caminos actual es de 
**$O(E^2)$**

---
* **Mejor caso: $O(1)$ (Constante)**
Se da cuando la calle de origen y la calle de destino son exactamente la misma.

* **Peor caso: $O(E^2)$ (Cuadrático)**
Ocurre cuando el destino está en la otra punta del mapa, o no hay ningún camino posible que conecte el origen con el destino. Entonces el BFS se ve obligado a explorar prácticamente todos los segmentos de la ciudad, por lo que el array dinámico `visited` crece hasta almacenar casi todas las calles del mapa, alcanzando un tamaño de $O(E)$.
Al tener que usar la función `is_street_visited` (que recorre el array de visitados con un bucle de coste $O(E)$) tanto para la calle que sale de la cola como para cada una de las vecinas que nos devuelve la HashTable, las operaciones se multiplican. Por lo tanto su complejidad será 
**$O(E^2)$**.


## 4. Latency to find connected streets: Sequential list vs Intersections map

### Datos del experimento
Los siguientes datos experimentales reflejan el tiempo de ejecución requerido para consultar y listar todos los tramos de calle conectados a una intersección concreta, comparando ambos métodos en diferentes escalas de mapas:

| Escala del Mapa | Calles | Latencia Lista Secuencial (Lab 4) | Latencia Mapa Intersecciones (Lab 5) |
| :--- | :--- | :--- | :--- |
| `xs_1` | 13 | 0.093425 ms | 0.007832 ms |
| `md_1` | 1318 | 0.017074 ms | 0.007131 ms |
| `lg_1` | 3797 | 0.156874 ms | 0.036295 ms |
| `xl_1` | 18944 | 1.096728 ms | 0.024585 ms |
| `2xl_1` | 50623 | 6.395531 ms | 0.835473 ms|

### Explicación de los Resultados
En esta tabla se ve muy clara la diferencia entre tener una complejidad lineal o una constante.

En la versión del Lab 4, `print_connected_streets` tenía que buscar exhaustivamente por toda la lista de calles. A medida que cargamos mapas más grandes, el tiempo crece de forma directamente proporcional ($\mathcal{O}(S)$), porque tiene que ir comprobando ID por ID. Además, como insertábamos por la cabeza, parar antes de tiempo con un `break` nos generaba fallos, obligando a recorrer casi todo.

Al añadir la `HashTable` en el Lab 5, dimensionada con números primos adaptados a cada mapa (`19`, `101`, `1861`...), ir directamente a una intersección pasa a costar en promedio $\mathcal{O}(1)$. La función mapea el ID de la esquina con su posición en memoria al instante. Por eso la latencia se hunde a fracciones de milisegundo, sin importar lo gigante que sea el mapa.

---

---

## 5. Latency to find a path: Sequential list vs Intersections map

### Datos del experimento
La siguiente tabla registra el tiempo total empleado por el algoritmo BFS para calcular una ruta completa entre dos extremos del mapa, contrastando el impacto de la estructura de búsqueda de adyacencias:

| Escala del Mapa | Calles | Latencia Ruta con Lista Secuencial | Latencia Ruta con Mapa Intersecciones |
| :--- | :--- | :--- | :--- |
| `xs_1` | 13 |  | 0.108140 ms |
| `md_1` | 1318 |  | 0.121196 ms |
| `lg_1` | 3797|  | 6.689064 ms |
| `xl_1` | 18944 |  | 15.122044 ms |
| `2xl_1` | 50623 |  | 48.487469 ms |

### Explicación de los Resultados
Algoritmos como el BFS se pasan todo el tiempo sacando nodos de la cola y buscando sus calles conectadas.

Si para encontrar los vecinos usamos la búsqueda secuencial del Lab 4, cada pequeño paso le cuesta al ordenador $\mathcal{O}(S)$. Como una ruta de GPS hace esto miles de veces, el coste temporal se multiplica enormemente. Es por esto que intentar calcular rutas en los mapas grandes (`xl_1` o `2xl_1`) con listas secuenciales es inasumible, ya que tardaría segundos o incluso minutos.

Al cambiar ese paso por nuestra `HashTable`, consultar las adyacencias cuesta solo $\mathcal{O}(1)$. Esto recorta drásticamente el tiempo de ejecución y es lo que nos permite resolver una ruta por toda Barcelona en muy pocos milisegundos.

---

## 6. Latency to find a path: Close points vs Far points

### Datos del experimento
En este experimento se evalúan los tiempos de respuesta del buscador de rutas sobre el mapa de escala `xl_1`, variando la distancia topológica (medida en el número mínimo de calles/saltos entre el origen y el destino):

| Escenario | Distancia en el Grafo (Saltos) | Latencia Medida (ms) |
| :--- | :--- | :--- |
| Cercano | 3 saltos | 0.115649 ms |
| Medio | 9 saltos | 0.748122 ms |
| Lejano | 13 saltos | 32.762129 ms |

### Explicación de los Resultados
El algoritmo BFS explora "a ciegas" y expande su búsqueda de manera radial en todas direcciones formando círculos concéntricos.

Esto explica perfectamente los tiempos de la tabla: si el destino está muy cerca, el círculo de búsqueda es pequeño, la cola procesa pocos nodos. En cambio, si el destino está lejos, el área del círculo se hace enorme, metiendo miles de intersecciones "basura" en la cola antes de dar por casualidad con el destino, disparando el tiempo de respuesta.

---

## 7. Improvement to the `visited` data structure in the BFS algorithm to improve latency
 
### ¿Qué cambiaríamos?
Para solucionar el cuello de botella del que hablamos en el apartado 3, proponemos quitar el array secuencial y utilizar un HashSet (o un array de direccionamiento directo booleano) que use el propio ID de la intersección como clave.

### Comparativa de Complejidad Temporal
* **Ahora mismo:** Buscar si hemos pisado una calle cuesta **$\mathcal{O}(V_d)$** (donde $V_d$ son los nodos ya descubiertos), haciendo que el tiempo total de ruta de nuestro GPS caiga a **$\mathcal{O}(V^2)$**.
* **Con la mejora:** Mirar en un HashSet y marcar una calle como visitada tiene un coste constante de **$\mathcal{O}(1)$**. De esta forma, el BFS funcionaría al máximo rendimiento y volveríamos a la complejidad óptima de **$\mathcal{O}(V + E)$**.

### Desventajas
La principal pega es el consumo de memoria. Mantener un HashSet extra solo para el control de visitados requiere bastante memoria dinámica adicional. Además, cada vez que el usuario pide una ruta nueva, perderíamos algo de tiempo inicial en reservar y limpiar esa tabla de Hash para asegurarnos de que está a cero.

---

## 8. Improvement to the algorithm to find the street segment given a latitude and longitude

### ¿Qué cambiaríamos?
Actualmente `get_closest_street` recorre el mapa entero calculando la distancia matemática contra todos los tramos, lo cual es muy poco eficiente. Para optimizarlo, se podría implementar un Grid Espacial (Spatial Hashing).

La idea es dividir el mapa en una cuadrícula lógica. Al cargar el programa, cada segmento de calle se registra solo en la celda del grid que le toca. Así, cuando el usuario hace una petición, traducimos sus coordenadas al índice de la cuadrícula correspondiente y calculamos la distancia *solamente* con las 4 o 5 calles que caen dentro de esa pequeña zona.

### Comparativa de Complejidad Temporal
* **Ahora mismo:** La búsqueda lineal hace el cálculo contra todos los tramos de la ciudad, dando una complejidad fija de **$\mathcal{O}(S)$**.
* **Con la mejora:** Saber la celda es una operación matemática directa **$\mathcal{O}(1)$**. Al reducir la comprobación solo a esa cuadrícula, el comportamiento estadístico sería prácticamente **$\mathcal{O}(1)$**.

### Desventajas
Es mucho más complejo de implementar. Como los tramos de calle no son puntos sino líneas geométricas, muchos cruzarán la frontera entre dos o más celdas, obligándonos a programar lógicas para insertarlos duplicados de forma segura en la matriz. Además, el propio *grid* consumiría una cantidad extra de memoria RAM de forma permanente.