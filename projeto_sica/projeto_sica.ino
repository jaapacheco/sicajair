#include <SPI.h>
#include <Ethernet.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
byte servidor[] = {000,000,000,000};//obs: aqui coloca o IP do local
//char servidor[] = "";//nome do servidor(neilsonsevidor.com.br
#define portaHTTP 80

EthernetClient clienteArduino;
///----------------/ declaração de sensores /----------------------------
///--------------------------------------------

void enviarParaServidor(String nome, String tipo, String dados);
String receberDadosServidor();

void setup() {
  Serial.begin(9600);
  Ethernet.begin(mac);

  if(Ethernet.begin(mac) == 0){
    Serial.println("Falha ao rede conectar a rede");
    Ethernet.begin(mac);
  }
  Serial.print("Conectado a rede, no ip: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  enviarParaServidor("teste_1", "tipo", "1995");
//  receberDadosServidor();
}

void enviarParaServidor(String nome, String tipo, String dados){
  Serial.println("Conectando ao servidor...");
  //  envia dados
  if(clienteArduino.connect(servidor, portaHTTP)){
    //http://000.000.000.000/InterfaceWeb.1.1/config/get_arduino.php?nome_s=dados1&tipo_s=dados2&dados_s=dados3
    clienteArduino.print("GET /InterfaceWeb.1.1/config/get_arduino.php?");
    clienteArduino.print("nome_s=");
    clienteArduino.print(nome);
    clienteArduino.print("&tipo_s=");
    clienteArduino.print(tipo);
    clienteArduino.print("&dados_s=");
    clienteArduino.print(dados);
    clienteArduino.println(" HTTP/1.0");
    clienteArduino.print("Host: 000.000.000.000");//(servidor)
    clienteArduino.println("Connection: close");
    clienteArduino.println();
  }
}
String receberDadosDoServidor(){
  //  int cont =0;
  String texto = "";
  //  resebe dados (teste 1)
  //  if(clienteArduino.available()){
  //    char dadosRetornados = clienteArduino.read();
  //    texto = String(texto + dadosRetornados);
  //    Serial.print(dadosRetornados);
  //  }
  // resebe dados (teste 2)

  do{
    char dadosRetornados = clienteArduino.read();
    texto = String(texto + dadosRetornados);
    Serial.print(dadosRetornados);
  }while(clienteArduino.available());
  
  if(!clienteArduino.connected()){
    clienteArduino.stop();
  }
  return texto;
}
