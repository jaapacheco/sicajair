//#include <FloatToString.h>

//#include <DHT.h>
//#include <DHT_U.h>

#include <WiFi.h> // Importa a Biblioteca ESP8266WiFi
//#include <PubSubClient.h> // Importa a Biblioteca PubSubClient
 
//defines:
//defines de id mqtt e tópicos para publicação e subscribe
#define TOPICO_SUBSCRIBE "MQTT_IFPI_CAANG_SUB"    //Tópico de envio de informações
#define TOPICO_PUBLISH   "MQTT_IFPI_CAANG_PUB"    //Tópico de recepcao de informações
//IMPORTANTE: recomendamos fortemente alterar os nomes
//            desses tópicos. Caso contrário, há grandes
//            chances de você controlar e monitorar o NodeMCU
//            de outra pessoa.
#define ID_MQTT  "IFPI_CAANG_0001"     //id mqtt (para identificação de sessão)
 //IMPORTANTE: este deve ser único no broker (ou seja, 
 //            se um client MQTT tentar entrar com o mesmo 
 //            id de outro já conectado ao broker, o broker 
 //            irá fechar a conexão de um deles).
 
// WIFI
const char* SSID = "brad"; // SSID 
const char* PASSWORD = "bradbrad"; // Senha da rede WI-FI que deseja se conectar
  
// MQTT
const char* BROKER_MQTT = "iot.eclipse.org"; //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883; // Porta do Broker MQTT
 
 
//Variáveis e objetos globais
WiFiClient espClient; // Cria o objeto espClient
PubSubClient MQTT(espClient); // Instancia o Cliente MQTT passando o objeto espClient
char EstadoSaida = '0';  //variável que armazena o estado atual da saída
  
//Prototypes
void initSerial();
void initWiFi();
void initMQTT();
void reconectWiFi(); 
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void VerificaConexoesWiFIEMQTT(void);
void InitOutput(void);
 
/* 
 *  Implementações das funções
 */

//------------------------------SENSOR DE UMIDADE SOLO
#define pinSensorUmSolo1 32
#define pinSensorUmSolo2 33//andruino uno
//------------------------------RELAY
#define relay01 15//arduino uno
//#define relay02 7//arduino uno
//--------------------------------SENSOR DE UMIDADE E TEMPERATURA DO AR
//#define UmTem A1
//#define DHTTYPE DHT11//INDENTIFICA O SENSOR
//
//DHT dht(UmTem, DHTTYPE);

void re(boolean estado){
  if(estado){
    digitalWrite(relay01, LOW);
  }else{
    digitalWrite(relay01, HIGH);
  }
}
void setup() {
  //inicializações:
  InitOutput();
  initSerial();
  initWiFi();
  initMQTT();
  //
//  Serial.begin(112500);//PARA O ESP32
//  Serial.begin(9600);//PARA O ARDUINO UNO
  
  pinMode(relay01, OUTPUT);
}


//Função: inicializa comunicação serial com baudrate 115200 (para fins de monitorar no terminal serial 
//        o que está acontecendo.
//Parâmetros: nenhum
//Retorno: nenhum
void initSerial() 
{
    Serial.begin(115200);
}
 
//Função: inicializa e conecta-se na rede WI-FI desejada
//Parâmetros: nenhum
//Retorno: nenhum
void initWiFi() 
{
    delay(10);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID);
    Serial.println("Aguarde");
     
    reconectWiFi();
}
  
//Função: inicializa parâmetros de conexão MQTT(endereço do 
//        broker, porta e seta função de callback)
//Parâmetros: nenhum
//Retorno: nenhum
void initMQTT() 
{
    MQTT.setServer(BROKER_MQTT, BROKER_PORT);   //informa qual broker e porta deve ser conectado
    MQTT.setCallback(mqtt_callback);            //atribui função de callback (função chamada quando qualquer informação de um dos tópicos subescritos chega)
}
  
//Função: função de callback 
//        esta função é chamada toda vez que uma informação de 
//        um dos tópicos subescritos chega)
//Parâmetros: nenhum
//Retorno: nenhum
void mqtt_callback(char* topic, byte* payload, unsigned int length) 
{
    String msg;
 
    //obtem a string do payload recebido
    for(int i = 0; i < length; i++) 
    {
       char c = (char)payload[i];
       msg += c;
    }
   
    //toma ação dependendo da string recebida:
    //verifica se deve colocar nivel alto de tensão na saída D0:
    //IMPORTANTE: o Led já contido na placa é acionado com lógica invertida (ou seja,
    //enviar HIGH para o output faz o Led apagar / enviar LOW faz o Led acender)
    if (msg.equals("L"))
    {
//        digitalWrite(LED_PIN, HIGH);
        re(true);
        EstadoSaida = '1';
    }
 
    //verifica se deve colocar nivel alto de tensão na saída D0:
    if (msg.equals("D"))
    {
//        digitalWrite(LED_PIN, LOW);
        re(false);
        EstadoSaida = '0';
    }
     
}
  
//Função: reconecta-se ao broker MQTT (caso ainda não esteja conectado ou em caso de a conexão cair)
//        em caso de sucesso na conexão ou reconexão, o subscribe dos tópicos é refeito.
//Parâmetros: nenhum
//Retorno: nenhum
void reconnectMQTT() 
{
    while (!MQTT.connected()) 
    {
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT)) 
        {
            Serial.println("Conectado com sucesso ao broker MQTT!");
            MQTT.subscribe(TOPICO_SUBSCRIBE); 
        } 
        else
        {
            Serial.println("Falha ao reconectar no broker.");
            Serial.println("Havera nova tentatica de conexao em 2s");
            delay(2000);
        }
    }
}
  
//Função: reconecta-se ao WiFi
//Parâmetros: nenhum
//Retorno: nenhum
void reconectWiFi() 
{
    //se já está conectado a rede WI-FI, nada é feito. 
    //Caso contrário, são efetuadas tentativas de conexão
    if (WiFi.status() == WL_CONNECTED)
        return;
         
    WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI
     
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(100);
        Serial.print(".");
    }
   
    Serial.println();
    Serial.print("Conectado com sucesso na rede ");
    Serial.print(SSID);
    Serial.println("IP obtido: ");
    Serial.println(WiFi.localIP());
}
 
//Função: verifica o estado das conexões WiFI e ao broker MQTT. 
//        Em caso de desconexão (qualquer uma das duas), a conexão
//        é refeita.
//Parâmetros: nenhum
//Retorno: nenhum
void VerificaConexoesWiFIEMQTT(void)
{
    if (!MQTT.connected()) 
        reconnectMQTT(); //se não há conexão com o Broker, a conexão é refeita
     
     reconectWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}
 
//Função: envia ao Broker o estado atual do output 
//Parâmetros: nenhum
//Retorno: nenhum
void EnviaEstadoOutputMQTT(void)
{
    if (EstadoSaida == '0')
    re(true);
//      MQTT.publish(TOPICO_PUBLISH, "D");
// 
    if (EstadoSaida == '1')
    re(false);
//      MQTT.publish(TOPICO_PUBLISH, "L");

//  LER O SENSOR DE UMIDADE DO SOLO
  float valorSolo1 =  analogRead(pinSensorUmSolo1);
  float valorSolo2 = analogRead(pinSensorUmSolo2);
  
  float valor = (valorSolo1+valorSolo2)/2;
  
  float valor1 = (valorSolo1*100)/4095;
  float valor2 = (valorSolo2*100)/4095;
  float valor3 = (valor*100)/4095;
  
  Serial.print("Umidade do solo 01: "); 
  Serial.print(valor1);
  Serial.println("%");
  Serial.print("Umidade do solo 02: "); 
  Serial.print(valor2);
  Serial.println("%");
  Serial.print("Umidade do solo: "); 
  Serial.print(valor3);
  Serial.println("%");

  char reler;
  
  //if(valor<1200){
  if(valor3<34){
       Serial.println("molhado!");
    re(false);
    //EstadoSaida = '0';
    reler='D';
    MQTT.publish(TOPICO_PUBLISH, "D");
    
    }else
        if(valor3<67){
        //if(valor<2400){
    Serial.println("umido moderada!");
       re(true);
    
    re(false);
      }else {//if((valorSolo>=61) || (valorSolo<100.00)){
       Serial.println("seco!");
    // EstadoSaida = '1';
    MQTT.publish(TOPICO_PUBLISH, "L");
       re(true);
        }
//  Serial.print("\n\n");
//   final String sms2 = reler+"-"+String(valor3)+"%_";
char solo[10];
dtostrf(valor3,4,2,solo);

//  for(int ii=0; ii<sms.length(); ii++){
//    char test = sms[ii];
    delay(3000);
//    Serial.println(solo);
    MQTT.publish(TOPICO_PUBLISH, solo);
//  }
    Serial.println("Enviado para broker!");
    
    delay(900);
    Serial.flush();
}
 
//Função: inicializa o output em nível lógico baixo
//Parâmetros: nenhum
//Retorno: nenhum
void InitOutput(void)
{
    //IMPORTANTE: o Led já contido na placa é acionado com lógica invertida (ou seja,
    //enviar HIGH para o output faz o Led apagar / enviar LOW faz o Led acender)
//    pinMode (LED_PIN, OUTPUT);
//    digitalWrite(LED_PIN, LOW);          
}


void loop() {
  //garante funcionamento das conexões WiFi e ao broker MQTT
    VerificaConexoesWiFIEMQTT();
 //----------------------------------------------------------------------------
  
//--------------------------------------------------------------------------------
   //envia o status de todos os outputs para o Broker no protocolo esperado
    EnviaEstadoOutputMQTT();
 
    //keep-alive da comunicação com broker MQTT
    MQTT.loop();
    
  delay(5000);
}
