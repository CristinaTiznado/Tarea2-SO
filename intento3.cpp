#include <thread>
#include <string.h>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <sstream>

using namespace std;

//mutex necesario para la sincronización
pthread_mutex_t mutex;

string cant_paquetes; //cantidad de paquetes a enviar a la ip
string nombre_archivo; //nombre del archivo que contiene las ips

int num; //cantidad de ips en el archivo

vector<string> ips; //vector con las ips
vector<string> info; //vector con linea de resultado del ping
vector<string> detalles; //vector con valores numericos de resultado a imprimir

//función que describe las instrucciones que debe ejecutar cada hebra
void *estructura(int i){
    pthread_mutex_lock(&mutex);
    string posicion = to_string(i); //string con la posicion de la hebra en su arreglo
    int posicion2 = i; //int con la posicion 
    
    //llamado a ping de ip y guardado de resultado en txt nuevo
    string linea = "ping -q -c"+cant_paquetes+" "+ips[posicion2]+" > archivo"+posicion+".txt";
    const char * comando = linea.c_str();
    system(comando);

    //--------------------------------------Recolección de información
    
    char temp[100];
    int cont = 0;
    
    //lectura de archivo anteriormente creado con resultado de ping
    FILE *file;
    string archivo = "archivo"+posicion+".txt";
    //cout << archivo;
    const char* a = archivo.c_str();
    file = fopen(a, "r");

    if(file == NULL){
        cerr << "Error al abrir archivo" << endl;
        terminate();
    }
    
    while((!feof(file)) && (cont != 4)){
        fgets(temp, 100, file);
        if(cont == 3 && (!(temp[0] == '\n'))){
            strtok(temp, "\n");
            info.insert(info.begin() + posicion2, temp);
        }
        cont++;
    }

    cont = 0;
    string temp2;
    stringstream input_stringstream(info[posicion2]);

    //separación de información detallada
    while(getline(input_stringstream, temp2, ' ')){
        if(cont == 0){
            detalles[0] = temp2;
        }
        if(cont == 3){
            detalles[1] = temp2;
        }
        if(cont == 5){
            detalles[2] = temp2;
        }
        cont++;
    }

    //--------------------------Impreción de información
    if(detalles[1] != "0"){
        cout << ips[posicion2] << "     " << detalles[0] << "        " << detalles[1] << "     " << detalles[2]+"      UP\n"; 
    }else{
        cout << ips[posicion2] << "     " << detalles[0] << "        " << detalles[1] << "     " << detalles[2]+"      DOWN\n"; 
    }

    pthread_mutex_unlock(&mutex);
    return 0;
}


//función que cuenta la cantidad de ips en el archivo txt
int cant_ips(){
    char temp[50]; 
    int cont = 0;

    FILE *archivo;
    const char* a = nombre_archivo.c_str();
    archivo = fopen(a, "r");

    if(archivo == NULL){
        cerr << "Error al abrir archivo" << endl;
        terminate();
    }

    while(!feof(archivo)){
        fgets(temp, 50, archivo);
        cont++;
    }

    rewind(archivo);
    int indice = cont-1;

    while(!feof(archivo) && indice != 0){
        fgets(temp, 50, archivo);
        if(!(temp[0] == '\n')){
            strtok(temp, "\n");
            ips.push_back(temp);
        }
        indice--;
    }

    return cont-1;
}

//argc: cantidad de variables en el comando de ejecución
//argv: arreglo con las variables utilizadas en el comando de ejecución
//comando de ejecución: ./intento3 archivoips.txt cantidad_paquetes
//argv[0]: ./intento3
//argv[1]: archivo.txt
//argv[2]: cantidad_paquetes
int main(int argc, char *argv[]){
    pthread_mutex_init(&mutex, NULL);
    
    nombre_archivo = argv[1];
    cant_paquetes = argv[2];
    
    //creación de un arreglo con la misma cantidad
    //de hebras que la cantidad de ips en el archivo
    num = cant_ips();

    //cambia el tamaño de los vectores con la cantidad de espacio que se utilizará
    info.resize(num);
    detalles.resize(3);

    //creación de arreglo con la misma cantidad de hebras que ips leídos
    thread threads[num];
    
    //impresión de primera parte de la visualización del resultado
    cout << "IP              Trans.   Rec.   Perd.   Estado\n";
    cout << "-----------------------------------------------\n";


    for(int i = 0; i < num; i++){
        //creación de las hebras y envío de 
        //argumento con su posición en el arreglo de hebras
        
        threads[i] = thread(estructura, i);
        
    }

    //Ciclo for para esperar por el término de todos los hilos
    for(int i = 0; i < num; i++){
        threads[i].join();
    }
    
    return 0;
}