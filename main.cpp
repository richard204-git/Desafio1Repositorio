#include <iostream>
#include <fstream>
using namespace std;

//declaramos las funciones
void LRE(char []);
void descomprimirLRE(char []);
char* leerArchivo(const char* nombreArchivo);
unsigned char rotateLeft(unsigned char b, int n);
unsigned char rotateRight(unsigned char b, int n);
unsigned char applyXOR(unsigned char b, unsigned char key);
char* encriptar(const char* mensaje, int n, unsigned char key);
char* desencriptar(const char* mensaje, int n, unsigned char key);

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
    cout << "Mensaje encriptado - Longitud: " << strlen(mensajeEncriptado) << endl;
    cout << "Fragmento de pista: '" << fragmentoPista << "'" << endl;
    cout << endl;
    
    cout << "Iniciando analisis de fuerza bruta:..." << endl;
    cout << "Probando combinaciones (rotacion 1-7, clave XOR 0-255)..." << endl;
    cout << endl;
    
    bool encontrado = false;
    
    //intenta todas las combinaciones
    for (int n = 1; n <= 7 && !encontrado; n++) {
        cout << "Probando rotacion " << n << " bits..." << endl;
        
        for (int key = 0; key <= 255 && !encontrado; key++) {
            //desencripta
            char* desencriptado = desencriptar(mensajeEncriptado, n, (unsigned char)key);
            if (!desencriptado) continue;
            
            //mira si el resultado desencriptado(suponiendo que este en formato RLE) contiene la pista cuando se descomprime con RLE
            
            //crear buffer en el heap para descompresion
            char* buffer = new char[strlen(desencriptado) * 10 + 1];
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
                while (desencriptado[i] >= '0' && desencriptado[i] <= '9') {
                    contador = contador * 10 + (desencriptado[i] - '0');
                    i++;
                }
                
                if (desencriptado[i] == '\0') {
                    formatoValido = false;
                    break;
                }
                
                char caracter = desencriptado[i++];
                for (int j = 0; j < contador && pos < strlen(desencriptado) * 10; j++) {
                    buffer[pos++] = caracter;
                }
            }
            buffer[pos] = '\0'; // el \ es pare terminar la cadena o es un caracter nulo
            
            //verifica si contiene el fragmento de la pista
            if (formatoValido && strstr(buffer, fragmentoPista) != nullptr) {
                cout << endl;
                cout << "¡INFOMRACION ENCONTRADA!" << endl;
                cout << "Método: RLE" << endl;
                cout << "Rotacion (n): " << n << " bits" << endl;
                cout << "Clave XOR (K): " << key << endl;
                cout << "El MENSAJE ORIGINAL COMPLETO ES:" << endl; 
                encontrado = true;
            }
            
            delete[] buffer;
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
void LRE(char* mensaje) {
    int i = 0;
    while (mensaje[i] != '\0') {
        char actual = mensaje[i];
        int contador = 1;
        while (mensaje[i] == mensaje[i + 1]) {
            contador++;
            i++;
        }
        cout << contador << actual;
        i++;
    }
    cout << endl;
}

void descomprimirLRE(char* mensaje) {
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
