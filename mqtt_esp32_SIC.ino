//Descripción del programa.
/*  Autor: JorgeIsurBalderasRamírez
    Fecha: 19-08-2021
    Descripción:
    Dispositivo Objetivo: ESP32CAM
    Ejercicio de aplicacion. Hacer un programa para encender 4 leds
    en secuencia exclusiva ascendente durante 5 segundos, encender en secuencia aditiva
    durante 5 segundos, enceder en secuencia descendente exclusiva durante 5 segundos,
    encender en secuencia descendente aditiva durante 5 segundos.
    Repetir en este orden durante durante 30 segundo y repetir en orden inverso durante
    90 segundos minutos. Que esto se realice solo 4 veces. Despues todos los leds se apagan.
    Las exclusivas las tienen que hacer con operadores incrementales o decrementales.
    Las aditivas las tienen que hacer con operadores bitwise.
    Mientras se envia por mqtt una suma incremental mas el valor en binario de la secuencia
    de los leds. Cada estado del led dura 0.2 segundos
    LEDS= 0000
    Secuencia ascendente exclusiva: 0000 1000 0100 0010 0001
    Secuencia aditiva esxclusiva: 0000 1000 1100 1110 1111
    Todo debe ser programado haciendo uso de las logicas no bloqueantes.
*/
//Bibliotecas
#include <WiFi.h>  // Biblioteca para el control de WiFi
#include <PubSubClient.h> //Biblioteca para conexion MQTT
//Datos del WiFi.
const char* ssid = "INFINITUM3033_2.4";  // Aquí debes poner el nombre de tu red
const char* password = "yYYmteq554";  // Aquí debes poner la contraseña de tu red
//Datos del broker MQTT
const char* mqtt_server = "18.198.240.106"; // Si estas en una red local, coloca la IP asignada, en caso contrario, coloca la IP publica
IPAddress server(18,198,240,106);
const char* tema = "BinOperation/Sic"
// Objetos
WiFiClient esp32Client; // Este objeto maneja los datos de conexion WiFi
PubSubClient client(esp32Client); // Este objeto maneja los datos de conexion al broker
//VARIABLES
const int repeticiones=4;
const int first_phase=30000;
const int second_phase=90000;
const int LED=4;
const int LED2=2;
const int LED3=14;
const int LED4=15;
bool first_phase_complete=0;
bool second_phase_complete=0;
double tempo_n, tempo_l,tempo_lv1,tempo_secuencia_n,tempo_secuencia_l;
int contador=1;
int indice=0;
int binario = 1;
int resultado=0;
void setup(){
    //inicializa comunicacion serial y el GPIO
    SerialConfig();
    WifiConnection(ssid,password);
    broker_Connect(server);
    // Inicia el control de tiempo
    tempo_l= millis (); 
    tempo_lv1 = millis();
}
void loop(){
    //contador general de tiempo
    tempo_n = millis();
    if (contador<repeticiones)
    {
        if ((tempo_n-tempo_lv1<first_phase)&&(first_phase_complete==0 && second_phase_complete==0))
        {
            //comprobar que los leds esten apagados:
            apagarLeds();
            //Ascendente exclusiva
            actualizaSecuencia(indice);
            //Ascendente aditiva
            actualizaSecuencia(indice);
            //Descendente exclusiva
            actualizaSecuencia(indice);
            //Descendente aditiva            
            actualizaSecuencia(indice);
            //Cuando terminen las secuencias, cambiar valores de bandera
            first_phase_complete=1;
            indice = 0;
            if (first_phase_complete==1)
            {
                resultado=bitOperation(binario);
                Serial.println(resultado);
            }
            
        }
        if ((tempo_n-tempo_lv1 >second_phase)&&(first_phase_complete==1 && second_phase_complete==0))
        {
            //Ascendente exclusiva
            actualizaSecuencia(indice);
            //Ascendente aditiva
            actualizaSecuencia(indice);
            //Descendente exclusiva
            actualizaSecuencia(indice);
            //Descendente aditiva 
            actualizaSecuencia(indice);
            //Incrementar en 1 contador_v
            contador++;
            //Cuando terminen las secuencias, cambiar valores de bandera
            second_phase_complete=1;
            indice = 0;
            //una vez terminado, publicaremos mediante MQTT EL RESULTADO DE BITOPERATION
            if (second_phase_complete==1)
            {
                publicarMqtt(tema,resultado);
            }
            
        }
        
    }
    
}
//funciones del usario:
void SerialConfig(){
    //iniciar comunicacion serial
    Serial.begin(115200);
    //inicializar GPIO
    pinMode(LED,OUTPUT);
    pinMode(LED2,OUTPUT);
    pinMode(LED3,OUTPUT);
    pinMode(LED4,OUTPUT);
}
void WifiConnection(const char* ssid,const char* password){
    Serial.println();
    Serial.println();
    Serial.print("Conectar a ");
    Serial.println(ssid);
 
    WiFi.begin(ssid, password); // Esta es la función que realiz la conexión a WiFi
 
    while (WiFi.status() != WL_CONNECTED) { // Este bucle espera a que se realice la conexión
        digitalWrite (statusLedPin, HIGH);
        delay(500); //dado que es de suma importancia esperar a la conexión, debe usarse espera bloqueante
        digitalWrite (statusLedPin, LOW);
        Serial.print(".");  // Indicador de progreso
        delay (5);
    }
    // Cuando se haya logrado la conexión, el programa avanzará, por lo tanto, puede informarse lo siguiente
    Serial.println();
    Serial.println("WiFi conectado");
    Serial.println("Direccion IP: ");
    Serial.println(WiFi.localIP());
    // Si se logro la conexión, encender led
    if (WiFi.status () > 0){
        digitalWrite (statusLedPin, LOW);
    }
    delay (1000); // Esta espera es solo una formalidad antes de iniciar la comunicación con el broker
}
void broker_Connect(IPAddress server){
    // Conexión con el broker MQTT
    client.setServer(server, 1883); // Conectarse a la IP del broker en el puerto indicado
}
int actualizaSecuencia(int indice){
    tempo_secuencia_l = millis();
    switch (indice)
    {
    case 0:
        tempo_secuencia_n=millis();
        digitalWrite(LED,HIGH);
        if ((tempo_secuencia_n - tempo_secuencia_l) > 0.2 && (tempo_secuencia_n - tempo_secuencia_l)<0.4)
        {
            digitalWrite(LED,LOW);
            digitalWrite(LED2,HIGH);
        }
        if ((tempo_secuencia_n - tempo_secuencia_l)>0.4 && (tempo_secuencia_n - tempo_secuencia_n)<0.6)
        {
            digitalWrite(LED2,LOW);
            digitalWrite(LED3,HIGH);
        }
        if ((tempo_secuencia_n - tempo_secuencia_l)>0.6 && (tempo_secuencia_n - tempo_secuencia_l)<0.8)
        {
            digitalWrite(LED3,LOW);
            digitalWrite(LED4,HIGH);
        }
        apagarLeds();
        return indice++;
        break;
    case 1:
        tempo_secuencia_n=millis();
        digitalWrite(LED,HIGH);
        if ((tempo_secuencia_n - tempo_secuencia_l) > 0.2 && (tempo_secuencia_n - tempo_secuencia_l)<0.4)
        {
            digitalWrite(LED2,HIGH);
        }
        if ((tempo_secuencia_n - tempo_secuencia_l)>0.4 && (tempo_secuencia_n - tempo_secuencia_n)<0.6)
        {
            digitalWrite(LED3,HIGH);
        }
        if ((tempo_secuencia_n - tempo_secuencia_l)>0.6 && (tempo_secuencia_n - tempo_secuencia_l)<0.8)
        {
            digitalWrite(LED4,HIGH);
        }
        apagarLeds();
        return indice++;
        break;
    case 2:
        tempo_secuencia_n=millis();
        digitalWrite(LED4,HIGH);
        if ((tempo_secuencia_n - tempo_secuencia_l) > 0.2 && (tempo_secuencia_n - tempo_secuencia_l)<0.4)
        {
            digitalWrite(LED4,LOW);
            digitalWrite(LED3,HIGH);
        }
        if ((tempo_secuencia_n - tempo_secuencia_l)>0.4 && (tempo_secuencia_n - tempo_secuencia_n)<0.6)
        {
            digitalWrite(LED3,LOW);
            digitalWrite(LED2,HIGH);
        }
        if ((tempo_secuencia_n - tempo_secuencia_l)>0.6 && (tempo_secuencia_n - tempo_secuencia_l)<0.8)
        {
            digitalWrite(LED2,LOW);
            digitalWrite(LED,HIGH);
        }
        apagarLeds();
        return indice++;
        break;
    case 3:
        tempo_secuencia_n=millis();
        digitalWrite(LED4,HIGH);
        if ((tempo_secuencia_n - tempo_secuencia_l) > 0.2 && (tempo_secuencia_n - tempo_secuencia_l)<0.4)
        {
            digitalWrite(LED3,HIGH);
        }
        if ((tempo_secuencia_n - tempo_secuencia_l)>0.4 && (tempo_secuencia_n - tempo_secuencia_n)<0.6)
        {
            digitalWrite(LED2,HIGH);
        }
        if ((tempo_secuencia_n - tempo_secuencia_l)>0.6 && (tempo_secuencia_n - tempo_secuencia_l)<0.8)
        {
            digitalWrite(LED,HIGH);
        }
        apagarLeds();
        return indice++;
        break;
    }
}
void apagarLeds(int LED,int LED2,int LED3,int LED4){
    digitalWrite(LED,LOW);
    digitalWrite(LED2,LOW);
    digitalWrite(LED3,LOW);
    digitalWrite(LED4,LOW);
    Serial.println("STATUS:OFF");
}
int bitOperation(int binario){
    binario = binario <<1;
    return binario;
}
void publicarMqtt(const char* tema, int resultado){
    client.publish(tema,resultado);
}
