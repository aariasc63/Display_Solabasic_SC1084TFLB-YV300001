De un display propietario a una librería para Arduino Uno: ingeniería inversa aplicada

Comparto uno de mis proyectos recientes: el análisis y control del display LCD de un No Break SOLA BASIC mediante Arduino Uno.

El objetivo fue comprender su funcionamiento, documentar sus conexiones y desarrollar una librería reutilizable que permita integrar este display en nuevos proyectos electrónicos.

El módulo analizado utiliza la placa SC1084TFLB-YV300001 y un controlador LCD HT1621. El principal desafío no consistía solamente en encender el display, sino en identificar la relación entre cada bit de la memoria del controlador y los símbolos físicos del cristal LCD.

 Procedimiento técnico
1. Identificación de conexiones
Se realizó el análisis del conector y se documentaron las siguientes señales:

 Backlight: +5 V
 RDVCC: +5 V
 GND: tierra
 CS: Arduino D2
 RESET: Arduino D3
 WR/CLK: Arduino D4
 DATA: Arduino D5

2. Análisis del protocolo

Una vez identificado el controlador HT1621, se implementaron las señales necesarias para inicializarlo, enviar comandos y escribir directamente en su memoria.

Aunque el protocolo permite comunicarse con el controlador, el fabricante del equipo no proporciona públicamente la correspondencia entre la memoria y los segmentos del LCD. Por ello fue necesario obtener este mapa mediante ingeniería inversa.

3. Mapeo manual del display

Se desarrolló un programa de exploración que permite activar individualmente 128 posiciones lógicas, identificadas desde E000 hasta E127.

Cada posición fue habilitada y observada directamente en el LCD. Después se registró qué número, icono, indicador o segmento físico respondía. También se realizó una prueba acumulativa para detectar segmentos relacionados y comprobar el direccionamiento.

El programa puede controlarse desde el monitor serie a 115200 baudios, facilitando la activación, desactivación y exploración de cada elemento.

4. Desarrollo de la librería

Con la información obtenida se creó la librería `SolabasicHT1621.h` para Arduino Uno, encapsulando las operaciones principales:

htInit()`: inicialización del controlador.
clearAll()`: limpieza completa del display.
setElement()`: activación de un elemento.
getElement()`: consulta de su estado.
toggleAddrBit()`: cambio de estado de una posición.
serviceAuto()`: exploración automática de segmentos.

El resultado es una base reutilizable para controlar el display sin depender de la electrónica original del No Break.

Esta experiencia demuestra que la ingeniería inversa no consiste únicamente en copiar un funcionamiento: implica observar, medir, formular hipótesis, experimentar, documentar y convertir los resultados en una herramienta útil para otros desarrolladores.

 Compartiré el código, el diagrama de conexiones y la documentación del mapeo para que puedan utilizarse en proyectos con Arduino Uno, automatización, instrumentación, sistemas embebidos o recuperación de componentes electrónicos.

Nota de seguridad: cualquier prueba con componentes procedentes de un No Break debe realizarse con el módulo separado de la etapa de potencia y verificando previamente sus niveles eléctricos.

¿En qué proyecto utilizarías este display?

#Arduino #ArduinoUno #IngenieriaInversa #Electronica #SistemasEmbebidos #HT1621 #LCD #Hardware #OpenSource #DesarrolloDeHardware #SolaBasic
