#include <iostream>
#include <cstdio>
#include <cstring>
#include <sstream>
#include <vector>
#include <map>
#include <queue>

#define MAX_SIZE 1000
#define MAX_LINEA 1000

using namespace std;

map<string, map<string, string> > procesos;
map<string, map<string, string> > copiaProcesos;


// map<string, int>atendidos;

queue<string> nivel1;
queue<string> nivel1Espera;
queue<string> nivel2;
queue<string> nivel3;



//lista  de valores de RR
vector<int> listaRR;


int contador = 1;

int vecesRR;




int timeCpu = 0;
// int quantum = 3;
int cantProcesos = 0;

void imprimirTiemposDeEspera(string tipoDato) {
    int suma = 0;
    int promedio;
    cout << endl << "_______TIEMPOS PROMEDIO_______" << endl << endl;
    
    // Iterador explícito para el mapa de procesos
    map<string, map<string, string> >::iterator it;
    
    for (it = procesos.begin(); it != procesos.end(); ++it) {
        const string& proceso = it->first;
        const map<string, string>& datos = it->second;
        
        int wt = stoi(datos.at(tipoDato)); 
        suma+=wt;
        promedio = suma/cantProcesos;
        // cout << wt <<endl;
        
        printf("Proceso %s tiene un tiempo de espera de %d. \n", proceso.c_str(), wt);
    }
    printf("Promedio de %s es : %d\n", tipoDato.c_str(), promedio);
}

void imprimirTurnAround() {

    cout << endl << "_______TIEMPOS TAT_______" << endl << endl;
    
    map<string, map<string, string> >::iterator it;
    
    for (it = procesos.begin(); it != procesos.end(); ++it) {
        const string& proceso = it->first;
        const map<string, string>& datos = it->second;
        
        int rt = stoi(datos.at("tat")); 
        
        printf("Proceso %s tiene un tiempo de respuesta de %d. \n", proceso.c_str(),rt);
    }
}


void imprimirTiemposRespuesta() {

    cout << endl << "_______TIEMPOS CT_______" << endl << endl;
    
    map<string, map<string, string> >::iterator it;
    
    for (it = procesos.begin(); it != procesos.end(); ++it) {
        const string& proceso = it->first;
        const map<string, string>& datos = it->second;
        
        int rt = stoi(datos.at("ct")); 
        
        printf("Proceso %s tiene un tiempo de respuesta de %d. \n", proceso.c_str(),rt);
    }
}

void roundRobin(queue<string>& cola) {
    cout << endl << "_____EN EJECUCION ROUND ROBIN_____" << endl << endl;

    while (!cola.empty()) 
    {
        string proceso = cola.front();
        int bursTime = stoi(procesos[proceso]["BursTime"]);
        int arrivalTime = stoi(procesos[proceso]["ArrivalTime"]);
        int cont = stoi(procesos[proceso]["procesado"]) + 1;

        procesos[proceso]["procesado"] = to_string(cont);
        if (bursTime > listaRR[cont-1])
        {
            procesos[proceso]["BursTime"] = to_string(bursTime - listaRR[cont-1]);
            timeCpu += listaRR[cont-1];
            printf("Proceso %s no terminó en cuántum %d y en tiempo %d\n", proceso.c_str(), listaRR[cont-1],timeCpu);
            nivel2.push(proceso);
            while (!nivel1Espera.empty())
            {
                string procesoEspera = nivel1Espera.front();
                int arrivalTimeE = stoi(copiaProcesos[procesoEspera]["ArrivalTime"]);

                if (arrivalTimeE <= timeCpu+listaRR[cont-1]) {
                    nivel1Espera.pop();
                    cola.push(procesoEspera);
                    cout << "Se agregó proceso " << procesoEspera << " a la cola en tiempo " << arrivalTimeE << endl;
                } else {
                    break;
                }
            }
            if (cont < vecesRR){
                cola.push(proceso);
                nivel2.pop();
            }
        } else {
            timeCpu += bursTime;
            procesos[proceso]["BursTime"] = "0";
            procesos[proceso]["ct"] = to_string(timeCpu);
            int bt = stoi(copiaProcesos[proceso]["BursTime"]);
            int ct = stoi(procesos[proceso]["ct"]);
            int turnAround = ct - arrivalTime;
            int waiting = turnAround - bt;
            procesos[proceso]["tat"] = to_string(turnAround);
            procesos[proceso]["wt"] = to_string(waiting);
            printf("Proceso %s terminó en cuántum %d y en tiempo %d\n", proceso.c_str(), listaRR[cont-1] ,timeCpu);
        }
        cola.pop();
    }
    // cout << nivel2.size() << endl;

    while(!nivel2.empty())
    {
        string proceso = nivel2.front();
        nivel1.push(proceso);
        nivel2.pop();
    }

}





void algoritmosJuntos(){
    int quantum;

    cout << endl << "Cantidad de veces a ejecutar Round Robin : ";
    cin >> vecesRR;
    for (int i = 1; i <= vecesRR;i++)
    {
        cout << endl << "Ingrese el valor del quantum para Round Robin # " << i << " : ";
        cin >> quantum;
        listaRR.push_back(quantum);
    }
    roundRobin(nivel1);
    // cout << nivel2.size() << endl;
    cout << endl <<"_____EN EJECUCION FCFS_____" << endl << endl;
    while (!nivel1.empty()) 
    // cout << "_____EN EJECUCION FCFS_____" << endl;
    {
        if (nivel1Espera.empty())
        {   

            string proceso = nivel1.front();
            nivel1.pop();
            int bursTime = stoi(procesos[proceso]["BursTime"]);
            int ArrivalTime = stoi(copiaProcesos[proceso]["ArrivalTime"]);


            // Ejecutar el proceso FCFS
            timeCpu += bursTime;
            procesos[proceso]["ct"] = to_string(timeCpu);
            int ct = stoi(procesos[proceso]["ct"]);
            int turnAround = ct - ArrivalTime;
            int burs = stoi(copiaProcesos[proceso]["BursTime"]);
            int waiting = turnAround - burs;
            // cout << ct << turnAround << endl;
            procesos[proceso]["tat"] = to_string(turnAround);
            procesos[proceso]["wt"] = to_string(waiting);
            printf("Proceso %s finalizó en FCFS en tiempo %d \n", proceso.c_str(), timeCpu);

        }else
        {
            string procesoF = nivel1.front();
            int bursTime = stoi(procesos[procesoF]["BursTime"]);
            timeCpu+=1;
            bursTime-=1;
            if (bursTime == 0){
                nivel1.pop();
                printf("Proceso %s finalizó en FCFS en tiempo %d\n", procesoF.c_str(), timeCpu);
            }else{
                printf("Proceso %s no finalizó en FCFS en tiempo %d\n", procesoF.c_str(), timeCpu);
                // cout << "burstTime " << bursTime << endl;
                procesos[procesoF]["BursTime"] = to_string(bursTime);
                while (!nivel1Espera.empty()) {
                    string procesoEspera = nivel1Espera.front();
                    int ArrivalTimeE = stoi(procesos[procesoEspera]["ArrivalTime"]);

                    if (ArrivalTimeE <= timeCpu) {
                        nivel1Espera.pop();
                        nivel3.push(procesoEspera);
                        cout << "se agrego proceso " << procesoEspera << " a la cola nivel 1 en tiempo " << timeCpu << endl;
                    } else {
                        break;
                    }
                }
                if (!nivel3.empty()){
                    roundRobin(nivel3);
                }
                cout << endl <<"_____EN EJECUCION FCFS_____" << endl << endl;
            }
        }
    } 

    int tamCola2 = nivel1.size();

    cout<< endl << "-------Tiempo de cpu: " << timeCpu << " tamaño cola -> " <<tamCola2<<" -------" <<endl;

}

void procesamientosDatos(const vector<vector<string> >& instrucciones) {
    string proceso;
    int anivel1ivalTime;
    int bursTime;
    int algoritmo;
    int wt = 0;
    int ct = 0;
    int rt = 0;
    int tat = 0;
    int procesado = 0;
    
    for (size_t j = 0; j < instrucciones.size(); ++j) {
        cantProcesos+=1;

        proceso = instrucciones[j][0];
        anivel1ivalTime = stoi(instrucciones[j][1]);
        bursTime = stoi(instrucciones[j][2]);
        algoritmo = stoi(instrucciones[j][3]);

        // Guardar en cola conivel1espondiente
        if (anivel1ivalTime == 0){
            nivel1.push(proceso);
        }else{
            nivel1Espera.push(proceso);
        }
        // Guardar en el mapa
        procesos[proceso]["ArrivalTime"] = to_string(anivel1ivalTime);
        procesos[proceso]["BursTime"] = to_string(bursTime);
        procesos[proceso]["Algoritmo"] = to_string(algoritmo);
        procesos[proceso]["wt"] = to_string(wt);
        procesos[proceso]["ct"] = to_string(ct);
        procesos[proceso]["rt"] = to_string(rt);
        procesos[proceso]["tat"] = to_string(tat);
        procesos[proceso]["procesado"] = to_string(procesado);


    }

    // Ejecutar Round Robin y luego FCFS
    copiaProcesos = procesos;
    algoritmosJuntos();

}

void lecturaArchivo() {
    char lista[MAX_SIZE][MAX_LINEA];
    int iterador = 0;

    FILE* archivo = fopen("entrada2.txt", "r");
    if (archivo == NULL) {
        perror("Fallo el intento de abrir el archivo");
        return;
    }

    char linea[MAX_LINEA];
    while (fgets(linea, sizeof(linea), archivo) && iterador < MAX_SIZE) {
        strncpy(lista[iterador], linea, MAX_LINEA - 1);
        lista[iterador][MAX_LINEA - 1] = '\0'; 
        iterador++;
    }

    fclose(archivo);

    vector<vector<string> > instrucciones;
    
    for (int i = 0; i < iterador; i++) {
        istringstream iss(lista[i]);
        string palabra;
        vector<string> partes;

        while (iss >> palabra) {
            partes.push_back(palabra);
        }

        instrucciones.push_back(partes);
    }

    procesamientosDatos(instrucciones);
}

int main() {
    lecturaArchivo();
    imprimirTiemposDeEspera("wt");
    imprimirTurnAround();
    imprimirTiemposRespuesta();
    // imprimirCopiaProcesos();
    // imprimirTiemposDeEspera("wt");
    // imprimirTiemposRespuesta();
    // imprimirTiemposDeEspera("wt");
    return 0;
}