# Desafío 1 - Informática II

Este proyecto implementa un sistema para **desencriptar y descomprimir mensajes**.

## estructura del desafio
```
[Mensaje encriptado] (hay q subirlo en forma de arreglo tipo char- cadena de caracters)
        ↓
[Desencriptar]
    - Probar rotación (1..7) (funcion)
    - Probar clave XOR (0..255) (funcion)
        ↓
[Mensaje comprimido]
        ↓
[Intentar descompresión con RLE] (funcion)
        ↓
[Intentar descompresión con LZ78] (funcion)
        ↓
[Comparar con fragmento o pista conocida ]
        ↓
[Confirmar método + parámetros]
        ↓
[Mensaje original completo]
```
## funciones a implementar
- Desencriptar usando rotación de bits + XOR.
- Detectar parámetros (rotación y clave XOR).
- Descomprimir mensajes usando RLE o LZ78.
- Devolver el texto original completo.

## restricciones o reglas a seguir
- Sin `std::string` ni STL.
- Uso de punteros y memoria dinamica en el heap y el stack (`new`, `delete`).
- Variables justificadas: `unsigned char` para bytes, `char*` para cadenas.

