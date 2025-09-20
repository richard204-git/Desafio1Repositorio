#include "utils.h"

//mira la longitud de una cadena
int miStrlen(const char* texto) {
    int len = 0;
    while (texto[len] != '\0') {
        len++;
    }
    return len;
}

//copia de src a dest
void miStrcpy(char* dest, const char* src) {
    int i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

//compara cadenas (0 = iguales)
int miStrcmp(const char* a, const char* b) {
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (a[i] != b[i]) return a[i] - b[i];
        i++;
    }
    return a[i] - b[i];
}

//busca si la pista esta en texto
bool contieneFragmento(const char* texto, const char* fragmento) {
    int lenTexto = miStrlen(texto);
    int lenFrag = miStrlen(fragmento);

    if (lenFrag == 0 || lenFrag > lenTexto) return false;

    for (int i = 0; i <= lenTexto - lenFrag; i++) {
        bool match = true;
        for (int j = 0; j < lenFrag; j++) {
            if (texto[i + j] != fragmento[j]) {
                match = false;
                break;
            }
        }
        if (match) return true;
    }
    return false;
}
