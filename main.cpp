#include <iostream>
#include <fstream>
using namespace std;

//declaramos las funciones
char* leerArchivo(const char* nombreArchivo);
unsigned char rotateRight(unsigned char b, int n);
unsigned char applyXOR(unsigned char b, unsigned char key);
char* desencriptar(const char* mensaje, int n, unsigned char key);
char* descomprimirRle(char* mensaje);
char* descomprimirLZ78(char* mensaje);
bool contieneFragmento(const char* texto, const char* fragmento);
char* procesarMensaje(const char* mensajeRaw);
int miStrlen(const char* s);
void miStrcpy(char* dest, const char* src);

/* ----------------- utilidades ----------------- */
int miStrlen(const char* s) {
    if (!s) return 0;
    int i = 0;
    while (s[i] != '\0') i++;
    return i;
}

void miStrcpy(char* dest, const char* src) {
    if (!dest || !src) return;
    int i = 0;
    while (src[i] != '\0') { 
        dest[i] = src[i]; 
        i++; 
    }
    dest[i] = '\0';
}

bool contieneFragmento(const char* texto, const char* fragmento) {
    if (!texto || !fragmento) return false;
    
    int lt = miStrlen(texto);
    int lf = miStrlen(fragmento);
    
    if (lf == 0 || lf > lt) return false;
    
    for (int i = 0; i <= lt - lf; i++) {
        bool coincide = true;
        for (int j = 0; j < lf; j++) {
            if (texto[i + j] != fragmento[j]) { 
                coincide = false; 
                break; 
            }
        }
        if (coincide) return true;
    }
    return false;
}

//procesa el mensaje para quitar separadores ZR si existen
char* procesarMensaje(const char* mensajeRaw) {
    if (!mensajeRaw) return nullptr;
    
    int len = miStrlen(mensajeRaw);
    char* resultado = new char[len + 1];
    int pos = 0;
    
    // Verifica si tiene patron ZR
    bool tieneZR = false;
    for (int i = 0; i < len - 1; i++) {
        if (mensajeRaw[i] == 'Z' && mensajeRaw[i+1] == 'R') {
            tieneZR = true;
            break;
        }
    }
    
    if (tieneZR) {
        cout << "detectado patron ZR, eliminando separadores..." << endl;
        for (int i = 0; i < len; ) {
            if (i + 1 < len && mensajeRaw[i] == 'Z' && mensajeRaw[i+1] == 'R') {
                i += 2;
            } else {
                resultado[pos++] = mensajeRaw[i++];
            }
        }
    } else {
        miStrcpy(resultado, mensajeRaw);
        pos = len;
    }
    
    resultado[pos] = '\0';
    return resultado;
}

/* ----------------- lectura de los archivos.txt ----------------- */
char* leerArchivo(const char* nombreArchivo) {
    ifstream archivo(nombreArchivo, ios::binary);
    if (!archivo) {
        cout << "algo salio mal, no se pudo abrir el archivo " << nombreArchivo << endl;
        return nullptr;
    }
    
    archivo.seekg(0, ios::end);
    int tamaño = (int)archivo.tellg();
    archivo.seekg(0, ios::beg);
    
    if (tamaño <= 0) {
        cout << "error, el archivo esta vacio: " << nombreArchivo << endl;
        archivo.close();
        return nullptr;
    }
    
    char* buffer = new char[tamaño + 1];
    archivo.read(buffer, tamaño);
    buffer[tamaño] = '\0';
    archivo.close();
    
    return buffer;
}

//----------------- operaciones de la rotacion de bits -----------------//
unsigned char rotateRight(unsigned char b, int n) {
    n &= 7;
    return (unsigned char)(((b >> n) | (b << (8 - n))) & 0xFF);
}

unsigned char applyXOR(unsigned char b, unsigned char key) {
    return (unsigned char)(b ^ key);
}

//----------------- desencriptar ----------------//
char* desencriptar(const char* mensaje, int n, unsigned char key) {
    if (!mensaje) return nullptr;
    int len = miStrlen(mensaje);
    if (len == 0) return nullptr;
    
    char* resultado = new char[len + 1];
    for (int i = 0; i < len; i++) {
        unsigned char b = (unsigned char)mensaje[i];
        unsigned char despuesXOR = applyXOR(b, key);
        unsigned char original = rotateRight(despuesXOR, n);
        resultado[i] = (char)original;
    }
    resultado[len] = '\0';
    return resultado;
}

//----------------- descompresion rle con memoria dinamica ----------------//
char* descomprimirRle(char* mensaje) {
    if (!mensaje) return nullptr;
    int len = miStrlen(mensaje);
    if (len == 0) return nullptr;
    
    //buffer dinamico mas grande
    int capacidad = len * 100 + 1;
    char* resultado = new char[capacidad];
    int pos = 0;
    int i = 0;

    while (mensaje[i] != '\0' && pos < capacidad - 1) {
        if (mensaje[i] < '0' || mensaje[i] > '9') {
            delete[] resultado;
            return nullptr;
        }
        
        int contador = 0;
        while (mensaje[i] >= '0' && mensaje[i] <= '9') {
            contador = contador * 10 + (mensaje[i] - '0');
            i++;
        }
        
        if (mensaje[i] == '\0') {
            delete[] resultado;
            return nullptr;
        }

        char caracter = mensaje[i++];
        
        // Proteger contra desbordamiento
        for (int j = 0; j < contador && pos < capacidad - 1; j++) {
            resultado[pos++] = caracter;
        }
    }
    
    resultado[pos] = '\0';
    return resultado;
}

//----------------- descompresion lz78 con memoria dinamica ----------------//
char* descomprimirLZ78(char* mensaje) {
    if (!mensaje) return nullptr;
    int len = miStrlen(mensaje);
    if (len == 0) return nullptr;
    
    //diccionario dinamico
    int tamDic = 0;
    int capacidadDic = 512;
    char** diccionario = new char*[capacidadDic];
    
    //buffer de salida dinamico
    int capacidadSalida = len * 100 + 1;
    char* salida = new char[capacidadSalida];
    int posSalida = 0;
    
    int i = 0;
    while (i < len && posSalida < capacidadSalida - 10) {
        if (!(mensaje[i] >= '0' && mensaje[i] <= '9')) {
            delete[] salida;
            for (int k = 0; k < tamDic; k++) delete[] diccionario[k];
            delete[] diccionario;
            return nullptr;
        }

        int indice = 0;
        while (i < len && mensaje[i] >= '0' && mensaje[i] <= '9') {
            indice = indice * 10 + (mensaje[i] - '0');
            i++;
        }
        
        if (i >= len) break;
        char c = mensaje[i++];

        //obtene prefijo
        int longPrefijo = 0;
        char* prefijo = nullptr;
        if (indice > 0) {
            if (indice > tamDic) {
                delete[] salida;
                for (int k = 0; k < tamDic; k++) delete[] diccionario[k];
                delete[] diccionario;
                return nullptr;
            }
            prefijo = diccionario[indice - 1];
            longPrefijo = miStrlen(prefijo);
        }

        //crea nueva entrada
        char* nuevaEntrada = new char[longPrefijo + 2];
        int pos = 0;
        
        if (prefijo) {
            for (int j = 0; j < longPrefijo; j++) {
                nuevaEntrada[pos++] = prefijo[j];
            }
        }
        nuevaEntrada[pos++] = c;
        nuevaEntrada[pos] = '\0';

        //expande diccionario si es necesario
        if (tamDic >= capacidadDic) {
            capacidadDic *= 2;
            char** nuevoDic = new char*[capacidadDic];
            for (int j = 0; j < tamDic; j++) {
                nuevoDic[j] = diccionario[j];
            }
            delete[] diccionario;
            diccionario = nuevoDic;
        }

        diccionario[tamDic++] = nuevaEntrada;

        //Añade a salida
        int longNueva = miStrlen(nuevaEntrada);
        for (int j = 0; j < longNueva && posSalida < capacidadSalida - 1; j++) {
            salida[posSalida++] = nuevaEntrada[j];
        }
    }

    salida[posSalida] = '\0';
    
    //libera diccionario
    for (int k = 0; k < tamDic; k++) {
        delete[] diccionario[k];
    }
    delete[] diccionario;
    
    return salida;
}

//----------------- main principal ----------------- //
int main() {
    cout << " #--DESAFIO 1: INGENIERIA INVERSA--#" << endl;
    cout << "Cargando archivos (mensaje.txt y pista.txt)..." << endl;
    
    char* mensajeRaw = leerArchivo("mensaje.txt");
    if (!mensajeRaw) {
        cout << "Error, No se pudo cargar mensaje.txt" << endl;
        return 1;
    }
    
    char* fragmentoPista = leerArchivo("pista.txt");
    if (!fragmentoPista) {
        cout << "Error, No se pudo cargar pista.txt" << endl;
        delete[] mensajeRaw;
        return 1;
    }
    
    char* mensajeEncriptado = procesarMensaje(mensajeRaw);
    delete[] mensajeRaw;
    
    if (!mensajeEncriptado) {
        cout << "ERROR: No se pudo procesar el mensaje" << endl;
        delete[] fragmentoPista;
        return 1;
    }
    
    int lenMensaje = miStrlen(mensajeEncriptado);
    
    cout << "Los archivos (mensaje.txt y pista.txt) han sido cargados correctamente" << endl;
    cout << "Mensaje procesado - Longitud: " << lenMensaje << endl;
    cout << "Fragmento de pista: '" << fragmentoPista << "'" << endl;
    cout << "Iniciando la fuerza bruta..." << endl;
    cout << "Probando " << (7 * 256) << " combinaciones..." << endl << endl;
    
    bool encontrado = false;
    int combinacionesIntentadas = 0;
    
    for (int n = 1; n <= 7 && !encontrado; n++) {
        cout << "Probando rotacion " << n << " bits..." << endl;
        
        for (int key = 0; key <= 255 && !encontrado; key++) {
            combinacionesIntentadas++;
            
            char* desencriptado = desencriptar(mensajeEncriptado, n, (unsigned char)key);
            if (!desencriptado) continue;
            
            //probar rle
            char* resultadoRLE = descomprimirRle(desencriptado);
            if (resultadoRLE && contieneFragmento(resultadoRLE, fragmentoPista)) {
                cout << endl << "_____________________________________" << endl;
                cout << "     SOLUCION ENCONTRADA!" << endl;
                cout << "_____________________________________" << endl;
                cout << "Metodo de compresion: RLE" << endl;
                cout << "Rotacion (n): " << n << " bits" << endl;
                cout << "Clave XOR (K): " << key << endl;
                cout << "Combinaciones intentadas: " << combinacionesIntentadas << endl;
                cout << "_____________________________________" << endl;
                cout << "MENSAJE ORIGINAL COMPLETO:" << endl;
                cout << resultadoRLE << endl;
                cout << "_____________________________________" << endl;
                encontrado = true;
            }
            
            if (resultadoRLE) delete[] resultadoRLE;
            
            //probar LZ78 si RLE fallo
            if (!encontrado) {
                char* resultadoLZ78 = descomprimirLZ78(desencriptado);
                if (resultadoLZ78 && contieneFragmento(resultadoLZ78, fragmentoPista)) {
                    cout << endl << "_____________________________________" << endl;
                    cout << "     SOLUCION ENCONTRADA!" << endl;
                    cout << "_____________________________________" << endl;
                    cout << "Metodo de compresion: LZ78" << endl;
                    cout << "Rotacion (n): " << n << " bits" << endl;
                    cout << "Clave XOR (K): " << key << endl;
                    cout << "Combinaciones intentadas: " << combinacionesIntentadas << endl;
                    cout << "_____________________________________" << endl;
                    cout << "MENSAJE ORIGINAL COMPLETO:" << endl;
                    cout << resultadoLZ78 << endl;
                    cout << "_____________________________________" << endl;
                    encontrado = true;
                }
                
                if (resultadoLZ78) delete[] resultadoLZ78;
            }
            
            delete[] desencriptado;
            
            if (combinacionesIntentadas % 200 == 0) {
                int porcentaje = (combinacionesIntentadas * 100) / (7 * 256);
                cout << "  Progreso: " << combinacionesIntentadas << "/1792 (" << porcentaje << "%)" << endl;
            }
        }
    }
    
    delete[] mensajeEncriptado;
    delete[] fragmentoPista;
    
    if (!encontrado) {
        cout << endl << "No se encontro solucion" << endl;
        return 1;
    }
    
    cout << endl << "Analisis completado exitosamente." << endl;
    return 0;
}
