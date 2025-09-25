#include <iostream>
#include <fstream>
using namespace std;

//declaramos las funciones
void descomprimirRLE(char []);
char* descomprimirLZ78(char* mensaje);
char* leerArchivo(const char* nombreArchivo);
unsigned char rotateLeft(unsigned char b, int n);
unsigned char rotateRight(unsigned char b, int n);
unsigned char applyXOR(unsigned char b, unsigned char key);
char* encriptar(const char* mensaje, int n, unsigned char key);
char* desencriptar(const char* mensaje, int n, unsigned char key);
bool contieneFragmento(const char* texto, const char* fragmento);
int miStrlen(const char* s);
void miStrcpy(char* dest, const char* src);

// ----------------- UTILIDADES DE CADENAS ----------------- //
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
/* ----------------- lectura del los archivos.txt ----------------- */
char* leerArchivo(const char* nombreArchivo) {
    ifstream archivo(nombreArchivo, ios::binary);
    if (!archivo) {
        cout << "ERROR: No se pudo abrir el archivo " << nombreArchivo << endl;
        return nullptr;
    }
    
    archivo.seekg(0, ios::end);
    int tamaño = (int)archivo.tellg();
    archivo.seekg(0, ios::beg);
    
    if (tamaño <= 0) {
        cout << "ERROR: Archivo vacío: " << nombreArchivo << endl;
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
unsigned char rotateLeft(unsigned char b, int n) {
    n &= 7; // asegurar n está en rango 0-7
    return (unsigned char)(((b << n) | (b >> (8 - n))) & 0xFF);
}

unsigned char rotateRight(unsigned char b, int n) {
    n &= 7; // asegurar n está en rango 0-7
    return (unsigned char)(((b >> n) | (b << (8 - n))) & 0xFF);
}

unsigned char applyXOR(unsigned char b, unsigned char key) {
    return (unsigned char)(b ^ key);
}

//----------------- encriptar ----------------//
char* encriptar(const char* mensaje, int n, unsigned char key) {
    if (!mensaje) return nullptr; // el nullptr es para devolver un puntero nulo
    int len = 0;
    while (mensaje[len] != '\0') len++; // calcular longitud manualmente
    
    if (len == 0) return nullptr;
    
    char* resultado = new char[len + 1];
    for (int i = 0; i < len; i++) {
        unsigned char b = (unsigned char)mensaje[i];
        // Paso 1: Rotación izquierda
        unsigned char rotado = rotateLeft(b, n);
        // Paso 2: XOR
        unsigned char encriptado = applyXOR(rotado, key);
        resultado[i] = (char)encriptado;
    }
    resultado[len] = '\0';
    return resultado;
}

//----------------- desencriptar ----------------//
char* desencriptar(const char* mensaje, int n, unsigned char key) {
    if (!mensaje) return nullptr;
    int len = 0;
    while (mensaje[len] != '\0') len++; // calcular longitud manualmente
    
    if (len == 0) return nullptr;
    
    char* resultado = new char[len + 1];
    for (int i = 0; i < len; i++) {
        unsigned char b = (unsigned char)mensaje[i];
        //1: XOR inverso (XOR es su propio inverso)
        unsigned char despuesXOR = applyXOR(b, key);
        //2: Rotación derecha (inverso de rotación izquierda)
        unsigned char original = rotateRight(despuesXOR, n);
        resultado[i] = (char)original;
    }
    resultado[len] = '\0';
    return resultado;
}
//----------------- DESCOMPRESIÓN LZ78 ----------------//
char* descomprimirLZ78(char* mensaje) {
    if (!mensaje) return nullptr;
    int len = miStrlen(mensaje);
    if (len == 0) return nullptr;
    
    // Buffer de salida (más grande por seguridad)
    char* salida = new char[len * 20 + 1];
    int posSalida = 0;
    
    // Diccionario dinámico
    int tamDic = 0;
    int capacidadDic = 256;
    char** diccionario = new char*[capacidadDic];
    
    int i = 0;
    while (i < len && posSalida < len * 20 - 10) {
        // Leer índice (puede ser de varios dígitos)
        if (!(mensaje[i] >= '0' && mensaje[i] <= '9')) {
            // No es formato LZ78 válido
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
        char letra = mensaje[i++];
        
        // Crear nueva cadena
        int longPrefijo = 0;
        char* prefijo = nullptr;
        
        if (indice > 0 && indice <= tamDic) {
            prefijo = diccionario[indice - 1];
            longPrefijo = miStrlen(prefijo);
        }
        
        // Construir nueva cadena: prefijo + letra
        char* nuevaCadena = new char[longPrefijo + 2];
        int pos = 0;
        
        // Copiar prefijo
        for (int j = 0; j < longPrefijo; j++) {
            nuevaCadena[pos++] = prefijo[j];
        }
        // Añadir nueva letra
        nuevaCadena[pos++] = letra;
        nuevaCadena[pos] = '\0';
        
        // Expandir diccionario si es necesario
        if (tamDic >= capacidadDic) {
            capacidadDic *= 2;
            char** nuevoDic = new char*[capacidadDic];
            for (int j = 0; j < tamDic; j++) {
                nuevoDic[j] = diccionario[j];
            }
            delete[] diccionario;
            diccionario = nuevoDic;
        }
        
        // Guardar en diccionario
        diccionario[tamDic++] = nuevaCadena;
        
        // Añadir a la salida
        int longNuevaCadena = miStrlen(nuevaCadena);
        for (int j = 0; j < longNuevaCadena && posSalida < len * 20 - 1; j++) {
            salida[posSalida++] = nuevaCadena[j];
        }
    }
    
    salida[posSalida] = '\0';
    
    // Liberar diccionario
    for (int k = 0; k < tamDic; k++) {
        delete[] diccionario[k];
    }
    delete[] diccionario;
    
    return salida;
}

//----------------- borrador del main ----------------- //
int main() {
    cout << "   DESAFÍO 1: INGENIERIA INVERSA " << endl;
    cout << "se esta cargando los archivos..." << endl;
    
    //lee el mensaje encriptado
    char* mensajeEncriptado = leerArchivo("mensaje.txt");
    if (!mensajeEncriptado) {
        cout << "!!Hubo un error, no se pudo cargar mensaje.txt¡¡" << endl;
        return 1;
    }
    //lee el fragmento de pista
    char* fragmentoPista = leerArchivo("pista.txt");
    if (!fragmentoPista) {
        cout << "!!Hubo un error, no se pudo cargar pista.txt¡¡" << endl;
        delete[] mensajeEncriptado;
        return 1;
    }
    
    cout << "Los archivos cargados han sido cargados correctamente" << endl;
    cout << "Mensaje encriptado - Longitud: " << miStrlen(mensajeEncriptado) << endl;
    cout << "Fragmento de pista: '" << fragmentoPista << "'" << endl;
    cout << endl;
    
    cout << "Iniciando analisis de fuerza bruta:..." << endl;
    cout << "Probando combinaciones (rotacion 1-7, clave XOR 0-255)..." << endl;
    cout << endl;
    
    bool encontrado = false;
    
    //intenta todas las combinaciones con la rotacion de bits
    for (int n = 1; n <= 7 && !encontrado; n++) {
        cout << "Probando rotacion " << n << " bits..." << endl;
        
        for (int key = 0; key <= 255 && !encontrado; key++) {
            //desencripta, provando claves XOR de 0 a 255
            char* desencriptado = desencriptar(mensajeEncriptado, n, (unsigned char)key);
            if (!desencriptado) continue;
            
            //mira si el resultado desencriptado(suponiendo que este en formato RLE) contiene la pista cuando se descomprime con RLE
            
            //crear buffer en el heap para descompresion
            char* buffer = new char[miStrlen(desencriptado) * 10 + 1];
            int pos = 0;
            int i = 0;
            bool formatoValido = true;
            
            //intenta descomprimir RLE
            while (desencriptado[i] != '\0' && formatoValido) {
                if (desencriptado[i] < '0' || desencriptado[i] > '9') {
                    formatoValido = false;
                    break;
                }
                
                int contador = 0;
                while (desencriptado[i] >= '0' && desencriptado[i] <= '9') { // convierte el caracter a entero.
                    contador = contador * 10 + (desencriptado[i] - '0');
                    i++;
                }
                
                if (desencriptado[i] == '\0') {
                    formatoValido = false;
                    break;
                }
                
                char caracter = desencriptado[i++];
                for (int j = 0; j < contador && pos < miStrlen(desencriptado) * 10; j++) {
                    buffer[pos++] = caracter;
                }
            }
            buffer[pos] = '\0'; // el \ es pare terminar la cadena o es un caracter nulo
            
            //verifica si contiene el fragmento de la pista
            if (formatoValido && contieneFragmento(buffer, fragmentoPista)) {
                cout << endl;
                cout << "¡INFOMRACION ENCONTRADA!" << endl;
                cout << "Método: RLE" << endl;
                cout << "Rotacion (n): " << n << " bits" << endl;
                cout << "Clave XOR (K): " << key << endl;
                cout << "El MENSAJE ORIGINAL COMPLETO ES:" << endl; 
                cout << buffer << endl;
                encontrado = true;
            }
            
            delete[] buffer;
            // Si no encontró con RLE, probar LZ78
            if (!encontrado) {
              char* resultadoLZ78 = descomprimirLZ78(desencriptado);
              if (resultadoLZ78 && miStrlen(resultadoLZ78) > 0) {
                if (contieneFragmento(resultadoLZ78, fragmentoPista)) {
                cout << endl;
                cout << "¡INFORMACION ENCONTRADA!" << endl;
                cout << "Método: LZ78" << endl;
                cout << "Rotacion (n): " << n << " bits" << endl;
                cout << "Clave XOR (K): " << key << endl;
                cout << "El MENSAJE ORIGINAL COMPLETO ES:" << endl;
                cout << resultadoLZ78 << endl;
                encontrado = true;
            }
            delete[] resultadoLZ78;
        }
    }
    delete[] desencriptado;
  }
}

//limpiar memoria del heap
delete[] mensajeEncriptado;
delete[] fragmentoPista;
    
    if (!encontrado) {
        cout << "No se ha encontrado la pista con los parametros dados." << endl;
        return 1;
    }
    
    cout << "El analisis ha sido completado exitosamente." << endl;
    return 0;
}


//----------------- funciones del RLE -----------------//
void descomprimirRLE(char* mensaje) {
    int i = 0;
    while (mensaje[i] != '\0') {
        int contador = 0;
        while (mensaje[i] >= '0' && mensaje[i] <= '9') {
            contador = contador * 10 + (mensaje[i] - '0');
            i++;
        }
        char caracter = mensaje[i];
        for (int j = 0; j < contador; j++) {
            cout << caracter;
        }
        i++;
    }
}
