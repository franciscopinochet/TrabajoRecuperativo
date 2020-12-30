#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <cstdio>
#include <fstream>
#include <unistd.h>

/* cabecera de las llamadas a MPI */
#include <mpi.h> 

using namespace std;

/**
 * Manda un mensaje desde todos los procesos con
 * rango distinto de 0 al proceso 0. El proceso 0
 * los saca por pantalla.
 * @param argc Cantidad de Argumentos
 * @param argv matriz asociativa de argumentos
 * @return contenido de los mensajes recibidos por 
 * el proceso 0, que los imprime
 */

 
//----------------------------------
//--------------FUNCIONES-----------
//----------------------------------

int obtener_year(string line){ //OBTENER EL AÑO DE UNA FILA DEL CSV
    string year;
    int i;
    for(i=1; i<=4 ;i++){
        year = year + line[i];
    }
    return atoi(year.c_str());
}

float obtener_dolar(string line){//OBTENER EL AÑVALOR DEL DOLAR DE UNA FILA DEL CSV
    string dolar;
    int i;
    for(i=14; i<=19 ;i++){
            dolar = dolar + line[i];
        }
    return atof(dolar.c_str());
}

float obtener_salario(string line){//OBTENER EL SALARIO DE UNA FILA DEL CSV
    string salario;
    int i;
    for(i=8; i<=20 ;i++){
        if(line[i]=='"'){
            break;
        }
        else{
            salario = salario + line[i];
        }
    }
    return atof(salario.c_str());
}


//OBTENER EL SALARIO EN DOLARES DESDE EL CSV
void obtener_salario_dolar(char *archivo, float n[], float salarios_dolar[]){
    ifstream file(archivo);
    string line;
    int i=5;
    //descarta cabecera csv
    getline(file,line);
    while(!file.eof()){
        getline(file,line);
        salarios_dolar[i-5] = obtener_salario(line)/n[i];
        i++;
    }
}

//OBTENER EL PROMEDIO DEL DOLAR POR AÑO
void obtener_prom_dolar(char *archivo, float n[]){
    ifstream file(archivo);
    string line;
    int year_comp = 1990, cont = 0, cont_prom=0;
    float dolar=0;
    //descarta cabecera csv
    getline(file,line);
    while(!file.eof()){
        getline(file,line);
        if(obtener_year(line) != year_comp){
            n[cont] = dolar/cont_prom;
            // cout<<"prom dolar año "<<year_comp<<"es: "<<dolar/cont_prom<<endl;
            dolar = 0;
            cont_prom = 0;
            year_comp++;
            cont++;

        }
        dolar = dolar + obtener_dolar(line);
        cont_prom++;
    }
}

//FUNCION QUE GENERA LA FORMULA DE LA ECUACION LINEAL
void regresion_lineal(float salarios_dolar[]){
    int i, n = 25, year = 1995;
    float m, b, sumax=0, sumay=0, sumaxy=0, sumax2=0, d;

    for(i = 0; i < n; i++){
        //suma valores variable x
        sumax += (year + i);
        //suma valores variable y
        sumay += salarios_dolar[i];
        //suma productos
        sumaxy += (year + i) * salarios_dolar[i];
        //suma valores variable x**2
        sumax2 += (year + i) * (year + i);
    }

    //Calculo pendiente(m) e intersepto(b)
    d = n*sumax2 - sumax*sumax;
    m = (n * sumaxy - sumax*sumay) / d;
    b = (sumax2*sumay - sumax*sumaxy) / d;

    cout<<endl<<"=== Resultado ===\n"<<endl;
    cout<<"   y = "<<m<<"x "<<b<<endl;
    cout<<"\n=== Integrantes ===\n"<<endl;
    cout<<"Francisco Javier Pinochet Morales\n"<<endl;
}



//----------------------------------
//--------------MAIN-----------
//----------------------------------

int main(int argc, char** argv) {

    int mi_rango; // rango del proceso    
    int p; // numero de procesos   
    int dest; // rango del que recibe 
    int tag = 0; // etiqueta del mensaje 

    float prom_dolar[31];
    float salarios_dolar[25];


    MPI_Status estado; // devuelve estado al recibir

    //  Comienza las llamadas a MPI 
    MPI_Init(&argc, &argv);

    //  Averiguamos el rango de nuestro proceso 
    MPI_Comm_rank(MPI_COMM_WORLD, &mi_rango);

    // /Averiguamos el número de procesos que estan 
    //  ejecutando nuestro programa 
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    
    if (mi_rango != 0) {
        
        obtener_prom_dolar(argv[1], prom_dolar);
        obtener_salario_dolar(argv[2],prom_dolar,salarios_dolar);

        dest = 0;
        MPI_Send(salarios_dolar, 25, MPI_FLOAT,
                dest, tag, MPI_COMM_WORLD);
    } else { // mi_rango == 0 
        MPI_Recv(salarios_dolar, 25, MPI_FLOAT, 1 || 2,
                tag, MPI_COMM_WORLD, &estado);

        regresion_lineal(salarios_dolar);
    }


    MPI_Finalize();

    return EXIT_SUCCESS;
}



