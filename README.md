# Desafío 1 - Informática II 2025-2

Este proyecto implementa un sistema de ingeniería inversa para desencripta y descomprime un mensaje: 

1. Lee el mensaje encriptado (mensaje.txt) y una pista (pista.txt).
2. Intenta desencriptar el mensaje probado:
    - todas las rotaciones de bits (1 al 7)
    - todas las claves XOR posibles (0 a 255)
3. Intenta descomprimir el mensaje con el arreglo obtenido de las operaciones anterioes, y probar con ese arreglo los dos metodos de descomprensión:
    - RLE
    - LZ78
4. Verifica si el mensaje descomprimido contiene la algun fragmento de la pista dada (pista.txt), si lo encuentra, muestra el mensaje y que parámetros(cuantas rotaciones, claves XOR usadas y que tipo de descompresión) se ha usado.


## Estructura del desafío
```
[Mensaje encriptado]
        ↓
[Desencriptar]
    - Probar rotación (1..7) 
    - Probar clave XOR (0..255) 
        ↓
[Mensaje comprimido]
        ↓
[Intentar descompresión con RLE] 
        ↓
[Intentar descompresión con LZ78] 
        ↓
[Comparar con fragmento o pista conocida ]
        ↓
[Confirmar método descompresión + parámetros]
        ↓
[Mensaje original completo]
```
## Funciones a implementar
- Desencriptar usando rotación de bits + XOR.
- Detectar que parámetros (rotación y clave XOR).
- Descomprimir mensajes usando RLE o LZ78.
- Devolver el texto original completo y ademas el metodo utilizado y los parametros en la comprension y en la encriptación.

## Restricciones o reglas a seguir
- Sin `std::string` ni STL.
- Uso de punteros y memoria dinamica en el heap y el stack (`new`, `delete`).
- Variables justificadas: `unsigned char` para bytes, `char*` para cadenas.

