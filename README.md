# SO TP1 2024-2C
El objetivo de este programa es utilizar IPCs para distribuir el cómputo del md5 de multiples archivos entre varios procesos. El resultado se guarda en un archivo md5_result.txt y se puede mostrar por pantalla con la vista

# Integrantes:
- Luca Rossi
- Alex Köhler
- Javier Liu

# Instrucciones de compilación
Descargar el proyecto y utilizar la imagen de docker en la shell
```sh
docker pull agodio/itba-so-multi-platform:3.0
docker run -v "${PWD}:/root" --privileged -ti agodio/itba-so-multi-platform:3.0
cd /root
```
Una vez que estés en el entorno de desarrollo, compila el programa utilizando el siguiente comando:
```sh
make all
```
# Instrucciones de ejecución
El programa `md5` se encarga de procesar archivos y requiere que se le pase una lista de archivos como argumento único al momento de su ejecución. Una vez ejecutado, el programa `md5` imprimirá en la salida estándar (stdout) información necesaria para el posterior procesamiento con el programa `vista`. Esta información incluye:
1. El identificador del proceso (PID) del programa `md5`.
2. La cantidad de archivos que se están procesando.
### Caso 1: pipear la información de md5 a vista
```
./md5 <archivos> | ./vista
```
### Caso 2: ejecutar md5 y escribir manualmente la informacion impresa por md5 a vista
```
./md5 <archivos>
```
```
./vista <pid_md5> <cantidad_archivos>
```
Por ejemplo si `md5` imprime
`42 23`
para ejecutar `vista` abrá que escribir
```sh
./vista 42 23 
```