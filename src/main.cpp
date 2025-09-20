#include <iostream>
#include "utils.h"
#include "rle.h"
#include "lz78.h"
#include "bitwise.h"
using namespace std;

//detecta si es RLE o LZ78
void detectarParametros(const char* mensajeEncriptado, const char* fragmento) {
    for (int n = 1; n < 8; n++) {                // probar rotaciones
        for (int key = 0; key < 256; key++) {    // probar claves
            char* desencriptado = decrypt(mensajeEncriptado, n, (unsigned char)key);
            if (!desencriptado) continue;

            // Intentar RLE
            char* rle = decompressRLE(desencriptado);
            if (rle && contieneFragmento(rle, fragmento)) {
                cout << "Método: RLE, Rotación: " << n << ", Clave: " << key << endl;
                cout << "Texto original: " << rle << endl;
                delete[] desencriptado;
                delete[] rle;
                return;
            }

            // Intentar LZ78
            char* lz78 = decompressLZ78(desencriptado);
            if (lz78 && contieneFragmento(lz78, fragmento)) {
                cout << "Método: LZ78, Rotación: " << n << ", Clave: " << key << endl;
                cout << "Texto original: " << lz78 << endl;
                delete[] desencriptado;
                delete[] lz78;
                return;
            }

            delete[] desencriptado;
            if (rle) delete[] rle;
            if (lz78) delete[] lz78;
        }
    }
    cout << "No se encontraron parámetros correctos." << endl;
}

int main() {
    const char* mensajeEncriptado = "ABC...";   // aquí va el mensaje encriptado real
    const char* fragmentoConocido = "TEST";     // aquí va el fragmento conocido

    detectarParametros(mensajeEncriptado, fragmentoConocido);

    return 0;
}
