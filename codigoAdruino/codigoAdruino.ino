#include <WiFi.h>


//------------------Servidor Web en puerto 80---------------------

WiFiServer server(80);
const int EchoPin = 22;
const int TriggerPin = 23;


//---------------------Credenciales de WiFi-----------------------

const char* ssid     = "CasaML_ONT_2.4G";
const char* password = "98402594";

//---------------------VARIABLES GLOBALES-------------------------
int contconexion = 0;


String header; // Variable para guardar el HTTP request

//------------------------CODIGO HTML------------------------------
String paginaInicio = "<!DOCTYPE html>"
"<html>"
"<head>"
"<meta charset='utf-8' />"
"<META HTTP-EQUIV='Refresh' CONTENT='0.7'>"
"<title>Estacionamiento</title>"
"</head>"
"<body>"
"<center>";


String mayor= "<table class=default>"

  "<tr>"

    "<th>A001</th>"

  "</tr>"
  "<tr>"

    "<td>1</td>"

  "</tr>";

String menor= "<table class=default>"
  "<tr>"

    "<th>A001</th>"

  "</tr>"
  "<tr>"

    "<td>0</td>"

  "</tr>";


String paginaFin = "</center>"
"</body>"
"</html>";


//---------------------------SETUP--------------------------------
void setup() {
  Serial.begin(9600);
  pinMode(TriggerPin, OUTPUT);
  pinMode(EchoPin, INPUT);
  
  // Conexión WIFI
  WiFi.begin(ssid, password);
  //Cuenta hasta 50 si no se puede conectar lo cancela
  while (WiFi.status() != WL_CONNECTED and contconexion <50) { 
    ++contconexion;
    delay(500);
    Serial.print(".");
  }
  if (contconexion <50) {
      //para usar con ip fija
      //IPAddress ip(192,168,1,180); 
      //IPAddress gateway(192,168,1,1); 
      //IPAddress subnet(255,255,255,0); 
      //WiFi.config(ip, gateway, subnet); 
      
      Serial.println("");
      Serial.println("WiFi conectado");
      Serial.println(WiFi.localIP());
      server.begin(); // iniciamos el servidor
  }
  else { 
      Serial.println("");
      Serial.println("Error de conexion");
   
}
  }

//----------------------------LOOP----------------------------------

void loop(){
  WiFiClient client = server.available();   // Escucha a los clientes entrantes

  if (client) {                             // Si se conecta un nuevo cliente
    Serial.println("New Client.");          // 
    String currentLine = "";                //
    while (client.connected()) {            // loop mientras el cliente está conectado
      if (client.available()) {             // si hay bytes para leer desde el cliente
        char c = client.read();             // lee un byte
        Serial.write(c);                    // imprime ese byte en el monitor serial
        header += c;
        if (c == '\n') {                    // si el byte es un caracter de salto de linea
          // si la nueva linea está en blanco significa que es el fin del 
          // HTTP request del cliente, entonces respondemos:
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
                     
            // Muestra la página web
           int cm = ping(TriggerPin, EchoPin);

            if (cm > 45) {
    // Encender LED
            client.println(paginaInicio + mayor + paginaFin);
            Serial.print("A001, ");
            Serial.print(1);
            Serial.print(",");
            Serial.println();
          }else{
            // Apagar LED
            client.println(paginaInicio + menor + paginaFin);
            Serial.print("A001, ");
            Serial.print(0);
            Serial.print(",");
            Serial.println();
          }
          
          
          
            // la respuesta HTTP temina con una linea en blanco
            client.println();
            break;
          } else { // si tenemos una nueva linea limpiamos currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // si C es distinto al caracter de retorno de carro
          currentLine += c;      // lo agrega al final de currentLine
        }
      }
    }
    // Limpiamos la variable header
    header = "";
    // Cerramos la conexión
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
    delay(1000);
  }
}

int ping(int TriggerPin, int EchoPin) {
   long duration, distanceCm;
   
   digitalWrite(TriggerPin, LOW);  //para generar un pulso limpio ponemos a LOW 4us
   delayMicroseconds(4);
   digitalWrite(TriggerPin, HIGH);  //generamos Trigger (disparo) de 10us
   delayMicroseconds(10);
   digitalWrite(TriggerPin, LOW);
   
   duration = pulseIn(EchoPin, HIGH);  //medimos el tiempo entre pulsos, en microsegundos
   
   distanceCm = duration * 10 / 292/ 2;   //convertimos a distancia, en cm
   return distanceCm;
   
}
