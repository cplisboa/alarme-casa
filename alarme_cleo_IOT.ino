/*******************************************************************************
 * J. A. Lisbôa 17/11/2020
 * Referencia:https://www.hackster.io/34909/wifi-security-system-using-wemos-d1mini-esp8266-and-blynk-56c703
 */
#define Luzes D6
#define Sirene D7
#define SensorExterno D1
#define SensorInterno D2
#define Controle D5
#define BLYNK_PRINT Serial    // Comentar para desabilitar saidas para a serial e poupar espaço
#include <BlynkSimpleEsp8266.h> 
#include <ESP8266WiFi.h> 
//#include <SimpleTimer.h> 
#include <ArduinoOTA.h>
// You should get Auth Token in the Blynk App. 
// Go to the Project Settings (nut icon). 


//Token fornecido pelo Blynk para acesso ao projeto LedBlynkOta
char auth[] = "xpj9_mhxzvmoJQHD_s4H3PKMsvha8hO6"; //auth cavera

BlynkTimer timer;
 
char ssid[] = "IOT";
char pass[] = "morandonaselva";
char hostOTA[] = "AlarmeCleo";
char passOTA[] = "";


int FlagAtivado, FlagSirene,FlagLuzes; 
long int T_LUZES,TL,T_SIRENE,TS;
int state; 
int contador=0; 
int flag=1; 
WidgetLED led1(V1);
WidgetLED ledmovimiento(V2);
WidgetBridge bridge1(V1);

void setupOTA()
{
  ArduinoOTA.setHostname(hostOTA);
  //ArduinoOTA.setPassword(passOTA);
  ArduinoOTA.onStart([]() {
    Serial.println("OTA: Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nOTA: End");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("OTA: Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("OTA: Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("OTA: Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("OTA: Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("OTA: End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("OTA: Ready");  
}

void buzzer() {
  digitalWrite(Sirene, HIGH); 
  delay(500);
  digitalWrite(Sirene, LOW);  
}

void MudaEstado() {
    FlagAtivado=!FlagAtivado;                         //Muda estado
    if(FlagAtivado==1) {
        Serial.println("Alarme ativado");
        Blynk.notify("Alarme ativado");
        buzzer();
    } else {
        Serial.println("Alarme desativado");
        Blynk.notify("Alarme desativado");
        digitalWrite(Luzes,LOW); 
        digitalWrite(Sirene,LOW); 
        buzzer();
        delay(200);
        buzzer();
        delay(200);
        buzzer();     
    }
}


void myTimerEvent() 
{
   //Se recebe sinal do controle remoto, muda o estado do alarme
 if (!digitalRead(Controle))
  MudaEstado(); 

  //Testa sensor interno      
 if (digitalRead(SensorInterno)& FlagAtivado==1)     // Se sensor interno=1  
   {     
       Serial.println("Detectado movimento interno"); 
       Blynk.notify("Detectado movimento interno!");
       digitalWrite(Sirene,HIGH);                   //Dispara sirene
       FlagSirene=1;                                //Sinaliza sirene ligada
       TS=0;                                        //Zera temporizador da sirene
       T_SIRENE=600;                                 //Tempo da sirene ligada
       if(analogRead(A0)>800)                       //Se escuro
        {
          digitalWrite(Luzes,HIGH);                 //Acende luzes
          FlagLuzes=1;                              //Sinaliza luzes ligadas
          TL=0;                                     //Zera temporizador das luzes  
          T_LUZES=600;               
        }
        //Aciona discador  
         Serial.println("Discando");
         bridge1.digitalWrite(0, HIGH);  // Aciona pino 0 do discador    
   } 

 //Testa sensor externo. Se detetar movimento e for escuro aciona luzes por 10 min. 
 //Se alarme ativado, sirene por 20s.   
if (digitalRead(SensorExterno)& FlagAtivado==1)     // Se sensor externo=1  
    {
    Serial.println("Detectado movimento externo");
    Blynk.notify("Detectado movimento externo");   
    digitalWrite(Sirene,HIGH);                   //Dispara sirene
    FlagSirene=1;                                //Sinaliza sirene ligada
    TS=0;                                        //Zera temporizador da sirene
    T_SIRENE=20;                                 //Tempo da sirene ligada
    if(analogRead(A0)>800)                       //Se escuro 
        { 
         digitalWrite(Luzes,HIGH);                 //Acende luzes
         FlagLuzes=1;                              //Sinaliza luzes ligadas
         TL=0;                                     //Zera temporizador das luzes   
         T_LUZES=600;                              //Tempo luzes ligadas
         }          
    }
     
  //Verifica se luzes e sirene estão ligadas para incrementar contadores de tempo
 if(FlagLuzes==1) 
  {
    if (TL<T_LUZES)             //se ainda não completou o tempo ligadas
               TL++;                     //incremente contador
            else                        //Se completou
               {                        //o tempo
                 digitalWrite(Luzes,LOW);        //Desliga luzes
               TL=0;                     //Zera contador de tempo
               FlagLuzes=0;                     //Sinaliza luzes desligadas
               }
   
  }

  if(FlagSirene==1) 
  {
    if (TS<T_SIRENE)             //se ainda não completou o tempo ligadas
               TS++;                     //incremente contador
            else                        //Se completou
               {                        //o tempo
                 digitalWrite(Sirene,LOW);        //Desliga luzes
               TS=0;                     //Zera contador de tempo
               FlagSirene=0;                     //Sinaliza luzes desligadas
               }
   
  }    
}


BLYNK_CONNECTED() {
  bridge1.setAuthToken("942084a7d4ce4de18f0cb7646f107ac5"); // Token de autorização do discador
}


 void setup() 
{ 
  Serial.begin(115200); 
  Serial.println();
  Serial.println("Alarme_Orieta");   
  FlagAtivado=0;
  FlagSirene=0;
  FlagLuzes=0;
  T_LUZES=600;                     //Tempo luzes ligadas (em  segundo)
  TL=0;                              //Contador tempo luzes ligadas
  T_SIRENE=600;                    //Tempo sirene ligada (em  segundo)
  TS=0;
  pinMode(Luzes, OUTPUT); 
  pinMode(Sirene, OUTPUT);
  pinMode(Controle, INPUT);
  pinMode(SensorExterno, INPUT);
  pinMode(SensorInterno, INPUT);
  digitalWrite(Luzes, LOW); 
  digitalWrite(Sirene, LOW);  
  buzzer();
  Blynk.begin(auth, ssid, pass); 
  timer.setInterval(100L, myTimerEvent);          //Chama a função myTimerEvent cada 100 ms
  Blynk.notify("Alarme inicializado V1");
  setupOTA();
} 

BLYNK_WRITE(V1){
  Blynk.notify("Aqui ligará as luzes");
}

BLYNK_WRITE(V2){
  Blynk.notify("Aqui vai disparar a sirene");
}

BLYNK_WRITE(V0){
  //Altera o estado do alarme cada vez que V0 e pressionado no aplicativo
  MudaEstado();
}

void loop() 
{
 ArduinoOTA.handle(); 
 Blynk.run(); 
 timer.run(); // Initiates SimpleTimer 
} 
