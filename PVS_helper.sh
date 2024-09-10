#!/bin/bash

# Verifica si se está ejecutando en el directorio correcto
read -p "¿Estás seguro de que estás en el directorio correcto para agregar encabezados a TODOS los archivos .c? (y/n): " confirm
if [[ $confirm != "y" ]]; then
    echo "Cancelado."
    exit 1
fi

# Agregar encabezado a los archivos .c
# echo "Agregando encabezados a los archivos .c..."
# find . -name "*.c" | while read line; do 
#     sed -i '1s/^\(.*\)$/\/\/ This is a personal academic project. Dear PVS-Studio, please check it.\n\1/' "$line"
#     sed -i '2s/^\(.*\)$/\/\/ PVS-Studio Static Code Analyzer for C, C++ and C#: http:\/\/www.viva64.com\n\1/' "$line"
# done
# echo "Encabezados agregados."
echo "Encabezados agregados previamente."

# Activar la licencia gratuita
echo "Activando la licencia gratuita de PVS-Studio..."
pvs-studio-analyzer credentials "PVS-Studio Free" "FREE-FREE-FREE-FREE"

# Realizar el análisis
echo "Iniciando el análisis con PVS-Studio..."
pvs-studio-analyzer trace -- make
pvs-studio-analyzer analyze

# Convertir el log a formato tasklist
echo "Generando el reporte..."
plog-converter -a '64:1,2,3;GA:1,2,3;OP:1,2,3' -t tasklist -o report.tasks PVS-Studio.log

echo "Análisis completo. El reporte se encuentra en 'report.tasks'."
