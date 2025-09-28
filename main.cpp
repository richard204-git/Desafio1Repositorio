#include <iostream>
#include <fstream>
using namespace std;

//declaraciones de funciones
unsigned char rotarDerecha(unsigned char valor, int n);
void copiaArreglo(char destino[], char origen[], int maxTam);
int longitudArr(char cadena[]);
void descomprimirLz78(char comprimido[], char descomprimido[]);
void xor_buffer(unsigned char* entrada, unsigned char* salida, int len, unsigned char K);
void descomprimirRle(char* mensaje, char* mensaje2);
bool contieneFragmento(char* texto, char* fragmento);
char* leerArchivo(const char* nombreArchivo);

//implementación de funciones
unsigned char rotarDerecha(unsigned char valor, int n) {
    n = n % 8;
    if (n == 0) return valor;

    unsigned char derecha = valor >> n;
    unsigned char izquierda = valor << (8 - n);
    return derecha | izquierda;
}

void copiaArreglo(char destino[], char origen[], int maxTam) {
    int i = 0;
    while (origen[i] != '\0' && i < maxTam - 1) {
        destino[i] = origen[i];
        i++;
    }
    destino[i] = '\0';
}

int longitudArr(char cadena[]) {
    int i = 0;
    while (cadena[i] != '\0') i++;
    return i;
}

void descomprimirLz78(char comprimido[], char descomprimido[]) {
    int maxEntrada = 5000;   //cantidad maxima de entradas en diccionario
    int maxLong = 1000;      //longitud maxima de cada entrada  
    int maxSalida = 200000;  //tamaño maximo del descomprimido

    char diccionario[maxEntrada][maxLong];
    int tamDic = 0;
    int posSalida = 0;
    int i = 0;

    while (comprimido[i] != '\0' && posSalida < maxSalida - 1) {
        int indice = 0;

        //leer numeros (índice)
        while (comprimido[i] >= '0' && comprimido[i] <= '9') {
            indice = indice * 10 + (comprimido[i] - '0');
            i++;
        }

        if (comprimido[i] == '\0') break; //no hay mas caracter
        char c = comprimido[i];
        i++;

        char nuevo[maxLong] = "";

        if (indice == 0) {
            nuevo[0] = c;
            nuevo[1] = '\0';
        } else if (indice > 0 && indice <= tamDic) {
            copiaArreglo(nuevo, diccionario[indice - 1], maxLong);
            int len = longitudArr(nuevo);
            if (len < maxLong - 1) {
                nuevo[len] = c;
                nuevo[len + 1] = '\0';
            }
        }

        //guardar en el diccionario si hay espacio
        if (tamDic < maxEntrada) {
            copiaArreglo(diccionario[tamDic], nuevo, maxLong);
            tamDic++;
        }

        //copiar a la salida
        int j = 0;
        while (nuevo[j] != '\0' && posSalida < maxSalida - 1) {
            descomprimido[posSalida++] = nuevo[j++];
        }
    }

    descomprimido[posSalida] = '\0';
}

void xor_buffer(unsigned char* entrada, unsigned char* salida, int len, unsigned char K) {
    for (int i = 0; i < len; ++i) {
        salida[i] = entrada[i] ^ K;
    }
    salida[len] = '\0';
}

void descomprimirRle(char* mensaje, char* mensaje2) {
    //ingresa un puntero que contiene la direccion de un arreglo de char.
    int i = 0;
    int k = 0;

    while (mensaje[i] != '\0') { 
        int contador = 0;

        while (mensaje[i] >= '0' && mensaje[i] <= '9') {
            contador = contador * 10 + (mensaje[i] - '0'); 
            i++;                                          
        }      

        char caracter = mensaje[i]; 

        for (int j = 0; j < contador; j++) {  
            mensaje2[k++] = caracter; 
        }

        i++;
    }
    
    mensaje2[k] = '\0';
}

bool contieneFragmento(char* texto, char* fragmento) {
    int i = 0, j = 0;

    while (texto[i] != '\0') {
        j = 0;
        int k = i;

        //comparar caracter por caracter
        while (texto[k] != '\0' && fragmento[j] != '\0' && texto[k] == fragmento[j]) {
            k++;
            j++;
        }

        //si llegamos al final del fragmento significa que lo encontramos
        if (fragmento[j] == '\0') {
            return true;
        }

        i++;
    }
    return false; //no se encontro
}

char* leerArchivo(const char* nombreArchivo) {
    ifstream archivo(nombreArchivo, ios::binary);
    if (!archivo) {
        cout << "Error: no se pudo abrir " << nombreArchivo << endl;
        return nullptr;
    }

    //obtener tamaño del archivo
    archivo.seekg(0, ios::end);
    int tam = archivo.tellg();
    archivo.seekg(0, ios::beg);

    //reservar memoria en el heap
    char* buffer = new char[tam + 1];

    //leer contenido
    archivo.read(buffer, tam);

    //agregar terminador de cadena
    buffer[tam] = '\0';

    archivo.close();
    return buffer;  //devuelve el puntero dinamico
}

//--------------main principal
int main() {
    //cargar dos archivos
    char* texto1 = leerArchivo("mensaje.txt");
    char* texto2 = leerArchivo("pista.txt");
    int lenIni = longitudArr(texto1);
    
    //agregar procesamiento para quitar separadores ZR
    char* mensajeProcesado = new char[lenIni + 1];
    int pos = 0;
    cout << "Procesando mensaje para quitar separadores ZR..." << endl;
    for (int i = 0; i < lenIni; ) {
        if (i + 1 < lenIni && texto1[i] == 'Z' && texto1[i+1] == 'R') {
            i += 2; // Saltar ZR
        } else {
            mensajeProcesado[pos++] = texto1[i++];
        }
    }
    mensajeProcesado[pos] = '\0';
    cout << "Longitud original: " << lenIni << ", después de procesar: " << pos << endl;
    
    //usar la nueva longitud procesada
    char* usoVolatil = new char[pos + 1000];
    char* usoVolatil2 = new char[pos + 1000];
    char* usoVolatil3 = new char[pos * 1000 + 50000];  
    char* usoVolatil4 = new char[pos * 1000 + 50000];  
    
    bool metodoLz78 = false;
    bool metodoRle = false;
    int parametroK (0);
    int parametroN (0);
    
    for (int n = 1; n < 8; ++n) {                      // n = rotación (1-7)
        cout << "Probando rotación " << n << " bits..." << endl;
        for (int k = 0; k < 256; ++k) {                // k = clave XOR (0-255)
            
            xor_buffer((unsigned char*)mensajeProcesado, (unsigned char*)usoVolatil, pos, (unsigned char)k);
            
            for (int j = 0; j < pos; ++j) {
                unsigned char caracter = usoVolatil[j];
                usoVolatil2[j] = rotarDerecha(caracter, n);
            }
            usoVolatil2[pos] = '\0';  
            
            descomprimirLz78(usoVolatil2, usoVolatil3);
            descomprimirRle(usoVolatil2, usoVolatil4);
            
            if (contieneFragmento(usoVolatil3, texto2)){
                metodoLz78 = true;
                parametroK = k;
                parametroN = n;
                cout << "Encontrado LZ78: n=" << n << ", k=" << k << endl;
                break;
            }

            if (contieneFragmento(usoVolatil4, texto2)){
                metodoRle = true;
                parametroK = k;
                parametroN = n;
                cout << "Encontrado RLE: n=" << n << ", k=" << k << endl;
                break;
            }
        }
        if (metodoLz78 == true || metodoRle == true){
            break;
        }
    }
    
    if(metodoLz78 == true || metodoRle == true){
        cout << "Se encontro coincidencia" << endl;
        if (metodoRle) {
            cout << "Metodo: RLE" << endl;
            cout << "Mensaje original: " << usoVolatil4 << endl;
        }
        if (metodoLz78) {
            cout << "Metodo: LZ78" << endl;
            cout << "Mensaje original: " << usoVolatil3 << endl;
        }
        cout << "Parámetros - Rotación: " << parametroN << ", Clave XOR: " << parametroK << endl;
    }
    else {
        cout << "No se encontro coincidencia" << endl;
    }

    // Liberar la nueva memoria asignada
    delete[] mensajeProcesado;
    delete[] usoVolatil;
    delete[] usoVolatil2;
    delete[] usoVolatil3;
    delete[] usoVolatil4;
    delete[] texto1;
    delete[] texto2;

    return 0;
}
