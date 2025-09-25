#include <iostream>
#include <fstream>
using namespace std;

//declaramos las funciones
char* leerArchivo(const char* nombreArchivo);
unsigned char rotateLeft(unsigned char b, int n);
unsigned char rotateRight(unsigned char b, int n);
unsigned char applyXOR(unsigned char b, unsigned char key);
char* encriptar(const char* mensaje, int n, unsigned char key);
char* desencriptar(const char* mensaje, int n, unsigned char key);
void descomprimirRLE(char []);
char* descomprimirLZ78(char* mensaje);
bool contieneFragmento(const char* texto, const char* fragmento);
int miStrlen(const char* s);
void miStrcpy(char* dest, const char* src);


/* ----------------- lectura del los archivos.txt ----------------- */
char* leerArchivo(const char* nombreArchivo) {
    ifstream archivo(nombreArchivo, ios::binary); // con el binario para evitar saltos de linea
    if (!archivo) { // si el archivo no exite o hay un error muestra un mensaje y davuleve nullptr q no apunta a nada
        cout << "ERROR: No se pudo abrir el archivo " << nombreArchivo << endl;
        return nullptr;
    }
    
    archivo.seekg(0, ios::end); // con seekg se mueve al final del archivo
    int tamaño = (int)archivo.tellg(); //telling para saber la cantidad de bytes
    archivo.seekg(0, ios::beg);//mueve el puntero para volver a leer    
    if (tamaño <= 0) { //si el tamaño es 0 lo cierra
        cout << "ERROR: Archivo vacío: " << nombreArchivo << endl;
        archivo.close();
        return nullptr;
    }
    
    char* buffer = new char[tamaño + 1]; //asigna espacio para el contenido y para el +1 para el caracternulo q es el fin de la cadena
    archivo.read(buffer, tamaño); //lee el contenido y lo guada en buffer
    buffer[tamaño] = '\0'; //agrega el caracter nulo en el +1 de arriba
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
// ----------------- UTILIDADES DE CADENAS del LZ78 ----------------- //
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
void descomprimirRle(char* mensaje, char* mensaje2) {
    //Ingresa un puntero que contiene la dirección de un arreglo de char.
    int i = 0;
    int k = 0;

    while (mensaje[i] != '\0') { //Solo se entra cuando es distinto del final de la cadena(! caracter vacio).
        int contador = 0;


        while (mensaje[i] >= '0' && mensaje[i] <= '9') { //Verifico que es un número y no una letra, utlizando el intervalo[0,9], en ASCII[48,57]
            contador = contador * 10 + (mensaje[i] - '0'); //Utilizando el valor en código ASCII, convierto los caracter de números en 
            i++;                                           //variables entero para poder operar.( en este caso[n-valor (ASII entre[48,57])-48 posición de '0'
        }        // ejem: '4'-'0' --> 52-48=4


        char caracter = mensaje[i]; //me quedo con el caracter letra, para despues repetirlo n-veces.

        for (int j = 0; j < contador; j++) {  //ciclo encardo de repetir n-veces.
            mensaje2[k++] = caracter; //Indexación con auto incremento.
        }

        i++;
    }

}
