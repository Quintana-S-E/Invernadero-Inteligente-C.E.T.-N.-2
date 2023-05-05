# El Invernadero inteligente: una oportunidad para la evolución del productor patagónico

English clarification: all contents in this repository, including comments, function and variable names, as well as any documentation are (mostly) in Spanish. All units are metric as well.

___

### Introducción
Este es el código completo (con sus posibles/futuras apps asociadas) del Invernadero inteligente, un proyecto institucional grupal del Centro de Educación Técnica N.º 2 "Jorge Newbery" para la mejora de la productividad y calidad de vida de los productores agrícolas regionales.  
Actualmente cuenta con conectividad a internet y control por comandos mediante Telegram, y autonomía de riego y ventilación basado en sensores de temperatura y humedad. Se encuentra en una fase temprana, cambiando sus capacidades desde prototipo hasta diseño funcional, por lo que muchas características fundamentales cambiarán.

### Estructura
En la carpeta [Código/src](https://github.com/Quintana-S-E/Invernadero-inteligente-C.E.T.-N.-2/tree/master/Controlador/src) se encuentra el código del controlador del invernadero (y otro README.md explicándolo), mientras que otros archivos relacionados a otras partes del proyecto (posibles apps) se encuentran en diferentes carpetas del repositorio.

### Próximos cambios (software)
#### Por implementar
+ [x] Definir posible utilización de una app para comandos y/o gráficos.
+ [ ] Definir adaptación del teclado para introducir configuraciones en un menú inicial (teclas estilo nokia 3 toques).
+ [ ] Que los datos del display no cambien solos, sino con un botón (del teclado, si es posible) y un interrupt.
+ [ ] Ajustar los porcentajes de humedad del suelo probando tierra real.
+ [ ] Mejorar el aviso de “No se encuentra red WiFi” (de mantenerse el sistema).
+ [ ] Añadir en /info el tiempo que pasó desde el último riego (todo escrito y comentado en el final de Telegram.h).
  + [ ] En el main loop agregar una función para detectar millis overflow.