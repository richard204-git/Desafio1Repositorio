#include <iostream>
#include <fstream>
using namespace std;

//declaramos las funciones
char* leerArchivo(const char* nombreArchivo);
unsigned char rotateRight(unsigned char b, int n);
unsigned char applyXOR(unsigned char b, unsigned char key);
char* desencriptar(const char* mensaje, int n, unsigned char key);
void descomprimirRle(char* mensaje, char* mensaje2) 
void copiaArreglo(char arreDestino[], char arreOrigen[])
int longitudArr(char cadena[]) 
void lz78_descomprimir(char comprimido[], char descomprimido[])  


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
unsigned char rotateRight(unsigned char b, int n) {
    n &= 7; //asegura n está en rango 0-7
    return (unsigned char)(((b >> n) | (b << (8 - n))) & 0xFF); //hace rotacion a la derecha para invertir la hecha a la izquierda por la encriptacion
}

// desplaza los bits de b a la derecha o desplaza los bit de b 8 posiciones, con el 0xFF se asegura de que el resultado este limitado a 8 bits.

//--------------- claves XOR ---------------------
unsigned char applyXOR(unsigned char b, unsigned char key) { //recibe 2 parametros b y key
    return (unsigned char)(b ^ key); // aplica un XOR bit a bit entre b y key, devulelve 1 los bits son diferentes, y 0 si son iguales 
}

//----------------- desencriptar ----------------//
char* desencriptar(const char* mensaje, int n, unsigned char key) { //mensaje.txt n=bits rotados key=XOR, retorna un puntero en una nueva cadena desencripta
    if (!mensaje) return nullptr; // si mensaje nulo o no hay nada no hace nada
    int len = 0;
    while (mensaje[len] != '\0') len++; //calcula longitud del mensaje hasta encontrar caracter nulo 
    
    if (len == 0) return nullptr; // si es 0 no hay nada q desencriptar
    
    char* resultado = new char[len + 1]; //reserva memoria para cadena + /0
    for (int i = 0; i < len; i++) { //recorre cada caracter el mensaje encriptado
        unsigned char b = (unsigned char)mensaje[i];
        //convierte caracteres unsigned char para q trabajen a nivel de bits (0-255)
        unsigned char despuesXOR = applyXOR(b, key);
        //Rotación derecha (inverso de rotacion izquierda)
        unsigned char original = rotateRight(despuesXOR, n); //aplica la rotacion derecha revirtiendo la izquierda
        resultado[i] = (char)original; //vuelve la cadena desencripta a se otra vez char, no unsigned char
    }
    resultado[len] = '\0'; //para q sea valida
    return resultado;
}

//----------------- DESCOMPRESIÓN LZ78 ----------------//

//Función que copia un char arreglo en otro char arreglo.
void copiaArreglo(char arreDestino[], char arreOrigen[]) {
    int i = 0;
    while (arreOrigen[i] != '\0') {
        arreDestino[i] = arreOrigen[i];
        i++;
    }
    arreDestino[i] = '\0';
}

int longitudArr(char cadena[]) {
    int contador = 0;
    while (cadena[contador] != '\0') {
        contador++;
    }
    return contador;
}

// función de descompresión LZ78
void lz78_descomprimir(char comprimido[], char descomprimido[]) {
    char diccionario[1000][1000];
    int tamDic = 0;
    int posSalida = 0;
    int i = 0;
    while (comprimido[i] != '\0') {
        int indice = 0;

        // Leer números (índice)
        while (comprimido[i] >= '0' && comprimido[i] <= '9') {
            indice = indice * 10 + (comprimido[i] - '0');
            i++;
        }

        char c = comprimido[i];
        i++;

        char nuevo[1000] = "";

        if (indice == 0) {
            nuevo[0] = c;
            nuevo[1] = '\0';
        }
        else {
            copiaArreglo(nuevo, diccionario[indice - 1]);
            int len = longitudArr(nuevo);
            nuevo[len] = c;
            nuevo[len + 1] = '\0';
        }

        copiaArreglo(diccionario[tamDic], nuevo);
        tamDic++;

        int j = 0;
        while (nuevo[j] != '\0') {
            descomprimido[posSalida] = nuevo[j];
            posSalida++;
            j++;
        }
    }

    descomprimido[posSalida] = '\0';
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


