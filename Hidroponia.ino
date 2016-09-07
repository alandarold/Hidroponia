// Programa : Horta Hidroponica
// Autor : Rafael Moraes
//Data : jan/16

#define bomba A1                             //definindo o pino Analogico 1 com o nome bomba

//*********inicio da programacao dedicada ao sensor de pH******************//

#define pinph A3                             //definindo o pino Analógico 3 com o nome "pinph
#define Offset 0.00                          //offset de correcao para o valor do phmetro

//************Fim da programacao dedicada ao sensor de pH******************//

#define pinconduc A2                        //definindo o pino Analógico 2 com o nome "pinph

#include <Wire.h>
#define DS1307_ADDRESS 0x68                 //definindo o endereco utilizado pelo RTC DS1307

byte zero = 0x00;

//*********inicio da programacao dedicada ao sensor de display 16x2******************//

#include <LiquidCrystal.h>                 //Carrega a biblioteca LCD visor que mostrar' data e temperatura com umidade
LiquidCrystal lcd(12, 11, 7, 8, 9, 10);    //RS - E - d4 - d5 - d6 - d7

//************fim da programacao dedicada ao sensor de display 16x2******************//

//*********inicio da programacao dedicada ao sensor de temperatura e umidade******************//
#include <DHT.h>                          //Carrega a biblioteca DHT sensor de temperatura
#define DHTPIN A0                         //Define a ligação ao pino de dados do sensor
#define DHTTYPE DHT22                     //Define o tipo de sensor DHT utilizado
DHT dht(DHTPIN, DHTTYPE);
//Array simbolo grau para formar o grau celsius no display
byte grau[8] ={ B00001100,
                B00010010,
                B00010010,
                B00001100,
                B00000000,
                B00000000,
                B00000000,
                B00000000,};
//************Fim da programacao dedicada ao sensor de temperatura e umidade******************//                

//*****************inicio da programacao dedicada ao sensor de fluxo***************************//
volatile int  flow_frequency;          // Measures flow meter pulses
 float  l_min;                         // Calculated litres/min                      
unsigned char flowmeter = 2;           // Flow Meter Pin number
unsigned long currentTime;
unsigned long cloopTime;

void flow ()                          // Interruot function
{ 
   flow_frequency++;
} 

//****************fim da programacao dedicada ao sensor de fluxo*******************************//

//*********inicio da programacao dedicada ao cronometro***************************************//

int seconds, minutes, hours, init_seconds, init_minutes, init_hours, elap_seconds, elap_minutes, elap_hours;
boolean memory = 0;
int target = 10;
int colheita_target = 22;
int colheita = 0;

//************Fim da programacao dedicada ao cronometro****************************************//

void setup()
{
Serial.begin(9600);                  //Inicializa a serial

lcd.begin(16,2);                     //Inicializa LCD, modelo 16x2
lcd.clear();                         //Limpa o LCD
lcd.createChar(0, grau);             //Cria o caractere customizado com o simbolo do grau

Wire.begin();
//SelecionaDataeHora();             //parte do programa retirada apos setar data e hora

pinMode(bomba, OUTPUT);

pinMode(pinph, INPUT);              //configuração do pino pinph como entrada

pinMode(pinconduc, INPUT);          //configuração do pino pinph como entrada

pinMode(flowmeter, INPUT);
attachInterrupt(0, flow, RISING);  // Setup Interrupt 
sei();                             // Enable interrupts  
currentTime = millis();
cloopTime = currentTime;
initialTime();
}

void loop()
{

telaabertura();
delay(2000);

stopwatch();

Mostrarelogio();
delay(2000);

stopwatch();

DHT_22();
delay(2000);

stopwatch();

pH_cond();
delay(1000);

stopwatch();

fluxo_med();
delay(1000);

stopwatch();

fluxo_med();
delay(1000);

stopwatch();

fluxo_show();
delay(2000);

stopwatch();

tempo_colheita();
delay(2000);

stopwatch();

}

void telaabertura()
{
 lcd.clear();
 lcd.setCursor(5,0);
 lcd.print("HORTA"); 
 lcd.setCursor(2,1);
 lcd.print("HIDROPONICA");
}  

void DHT_22()
{
float h = dht.readHumidity(); //Le o valor da umidade
float t = dht.readTemperature(); //Le o valor da temperatura
lcd.clear();
lcd.setCursor(0,0);
lcd.print("Temp : ");
lcd.print(" ");
lcd.setCursor(7,0);
lcd.print(t,1);
lcd.setCursor(12,0);
lcd.write((byte)0); //Mostra o simbolo do grau formado pelo array
lcd.write("C");
lcd.setCursor(0,1);
lcd.print("Umid : ");
lcd.print(" ");
lcd.setCursor(7,1);
lcd.print(h,1);
lcd.setCursor(12,1);
lcd.print("%");
Serial.println (t);

}

void pH_leitura()
{
  //programação dedicada para ler a entrada analógica do pH
  int valor_ph = analogRead(pinph);         //realiza a leitura do valor do pino "pinph" e armazena em "valor_ph"
  float voltage = valor_ph*5.0/1024;          //multiplica o "valor_ph" por 5 e divide por 1024 armazenando em "voltage"
  float pH = 3.5*voltage-Offset;              //multiplica por 3.5 o valor "voltage" e retira o "offset" (correção) 
}

/* parte do programa retirada apos setar data e hora
void SelecionaDataeHora() // Seta a data e a hora do DS1307
{
 byte segundos = 00; // Valores de 0 a 59
 byte minutos = 07; // Valores de 0 a 59
 byte horas = 19; // Valores de 0 a 23
 byte diadasemana = 1; // Valores de 0 a 6 (0=Domingo, 1 = Segunda...)
 byte diadomes = 1; // Valores de 1 a 31
 byte mesdoano = 1; // Valores de 1 a 12
 byte ano = 16; // Valores de 0 a 99
 Wire.beginTransmission(DS1307_ADDRESS);
 // Stop no CI para que o mesmo possa receber os dados
 Wire.write(zero); 
 
 // As linhas abaixo escrevem no CI os valores de 
 // data e hora que foram colocados nas variaveis acima
 Wire.write(ConverteParaBCD(segundos));
 Wire.write(ConverteParaBCD(minutos));
 Wire.write(ConverteParaBCD(horas));
 Wire.write(ConverteParaBCD(diadasemana));
 Wire.write(ConverteParaBCD(diadomes));
 Wire.write(ConverteParaBCD(mesdoano));
 Wire.write(ConverteParaBCD(ano));
 Wire.write(zero); //Start no CI
 Wire.endTransmission(); 
}
*/

byte ConverteParaBCD(byte val)
{ 
 // Converte o número de decimal para BCD
 return ( (val/10*16) + (val%10) );
}
byte ConverteparaDecimal(byte val) 
{ 
 // Converte de BCD para decimal
 return ( (val/16*10) + (val%16) );
}
 
void Mostrarelogio()
{
 
 Wire.beginTransmission(DS1307_ADDRESS);
 Wire.write(zero);
 Wire.endTransmission();
 Wire.requestFrom(DS1307_ADDRESS, 7);
 int segundos = ConverteparaDecimal(Wire.read());
 int minutos = ConverteparaDecimal(Wire.read());
 int horas = ConverteparaDecimal(Wire.read() & 0b111111); 
 int diadasemana = ConverteparaDecimal(Wire.read()); 
 int diadomes = ConverteparaDecimal(Wire.read());
 int mesdoano = ConverteparaDecimal(Wire.read());
 int ano = ConverteparaDecimal(Wire.read());

 lcd.clear();
 // Mostra a hora atual no display
 lcd.setCursor(4, 1);
 if (horas < 10)
 {
 lcd.print("0");
 }
 lcd.print(horas);
 lcd.print(":");
 if (minutos < 10)
 {
 lcd.print("0");
 }
 lcd.print(minutos);
 lcd.print(":");
 if (segundos < 10)
 {
 lcd.print("0");
 }
 lcd.print(segundos);
 // Mostra a data atual no display
 lcd.setCursor(5,0);
 if (diadomes < 10)
 {
 lcd.print("0");
 }
 lcd.print(diadomes);
 
 lcd.setCursor(8,0);
 switch(mesdoano)
 {
 case 0:lcd.print("JAN");
 break;
 case 1:lcd.print("FEV");
 break;
 case 2:lcd.print("MAR");
 break;
 case 3:lcd.print("ABR");
 break;
 case 4:lcd.print("MAI");
 break;
 case 5:lcd.print("JUN");
 break;
 case 6:lcd.print("JUL");
 break;
 case 7:lcd.print("AGO");
 break;
 case 8:lcd.print("SET");
 break;
 case 9:lcd.print("OUT");
 break;
 case 10:lcd.print("NOV");
 break;
 case 11:lcd.print("DEZ");
 }
 lcd.setCursor(12,0);
 lcd.print(ano);
 //Mostra o dia da semana no display
 lcd.setCursor(0,0);
 switch(diadasemana)
 {
 case 0:lcd.print("DOM");
 break;
 case 1:lcd.print("SEG");
 break;
 case 2:lcd.print("TER");
 break;
 case 3:lcd.print("QUA");
 break;
 case 4:lcd.print("QUI");
 break;
 case 5:lcd.print("SEX");
 break;
 case 6:lcd.print("SAB");
 }
}
 
 void tempo_colheita()
 {
  
   
   lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("TEMPO P/");
  lcd.setCursor(0,1);
  lcd.print("COLHEITA  DIAS");
  lcd.setCursor(11,0);
  lcd.print(colheita);
  
 }
 
 void pH_cond()
 {
  //programação dedicada para ler a entrada analógica do pH
  int valor_ph = analogRead(pinph);         //realiza a leitura do valor do pino "pinph" e armazena em "valor_ph"
  float voltage = valor_ph*5.0/1024;          //multiplica o "valor_ph" por 5 e divide por 1024 armazenando em "voltage"
  float pH = 3.5*voltage-Offset;              //multiplica por 3.5 o valor "voltage" e retira o "offset" (correção) 
  lcd.clear();
  lcd.setCursor(2,0);
  lcd.print("pH : ");
  lcd.print(pH);
  //programa para ler a entrada analogica do condutivimetro
  int CONDUCTIVITY = analogRead(pinconduc);
  lcd.setCursor(0,1);
  lcd.print("COND : ");
  lcd.print(CONDUCTIVITY);
  lcd.print("mS");
 } 

void fluxo_med()
 {
   currentTime = millis();
   // Every second, calculate and print litres/min
   if(currentTime >= (cloopTime + 1000))
   {     
      cloopTime = currentTime;              // Updates cloopTime
      // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min. (Results in +/- 3% range)
      l_min = (flow_frequency*60/ 7.5/60); // (Pulse frequency x 60 min) / 7.5Q = flow rate in L/hour 
      flow_frequency = 0;                   // Reset Counter
   }
 }
 
 void fluxo_show()
 {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("FLUXO:");
  lcd.print(l_min);
  lcd.print(" L/min");
 }
 
 int initialTime() {
 if(memory == 0)
 {
 Wire.beginTransmission(DS1307_ADDRESS);
 Wire.write(zero);
 Wire.endTransmission();
 Wire.requestFrom(DS1307_ADDRESS, 7);
 
 while(Wire.available()) {
   init_seconds = Wire.read();
   init_minutes = Wire.read();
   init_hours = Wire.read();
   
   init_seconds = (((init_seconds & 0b11110000)>>4)*10 + (init_seconds & 0b00001111)); // convert BCD to decimal
   init_minutes = (((init_minutes & 0b11110000)>>4)*10 + (init_minutes & 0b00001111)); // convert BCD to decimal
   init_hours = (((init_hours & 0b00100000)>>5)*20 + ((init_hours & 0b00010000)>>4)*10 + (init_hours & 0b00001111)); // convert BCD to decimal (assume 24 hour mode)
   return(init_seconds);
 }
 memory == 1;
 }
 else ;
 }
 
 int elapsedTime() { //tempo decorrido
 Wire.beginTransmission(DS1307_ADDRESS);
 Wire.write(zero);
 Wire.endTransmission();
 Wire.requestFrom(DS1307_ADDRESS, 7);
 
 while(Wire.available()) {
   elap_seconds = Wire.read();
   elap_minutes = Wire.read();
   elap_hours = Wire.read();
   
   elap_seconds = (((elap_seconds & 0b11110000)>>4)*10 + (elap_seconds & 0b00001111)); // convert BCD to decimal
   elap_minutes = (((elap_minutes & 0b11110000)>>4)*10 + (elap_minutes & 0b00001111)); // convert BCD to decimal
   elap_hours = (((elap_hours & 0b00100000)>>5)*20 + ((elap_hours & 0b00010000)>>4)*10 + (elap_hours & 0b00001111)); // convert BCD to decimal (assume 24 hour mode
   return(elap_seconds);
 }
 }
 
 int Timer() {
 elapsedTime();
 seconds = ((60 + elap_seconds) - init_seconds) % 60;
 if(seconds%60==0) {
   minutes = ((60 + elap_minutes) - init_minutes) % 60;
 }
 if(minutes%24==0) {
   hours = ((24 + elap_hours) - init_hours) % 24;
 }
}

void stopwatch()
{
   //initialTime();
   Timer();
   if(elap_hours<10) {
     Serial.print("0");Serial.print(elap_hours);
   } else {
     Serial.print(elap_hours);
   }
   Serial.print(":");
   if(elap_minutes<10) {
     Serial.print("0");Serial.print(elap_minutes);
   } else {
     Serial.print(elap_minutes);
   }
   Serial.print(":");
   if(elap_seconds<10) {
     Serial.print("0");Serial.print(elap_seconds);Serial.print("\t");
   } else {
     Serial.print(elap_seconds);Serial.print("\t");
   }
   if(hours<10) {
     Serial.print("0");Serial.print(hours);
   } else {
     Serial.print(hours);
   }
   Serial.print(":");
   if(minutes<10) {
     Serial.print("0");Serial.print(minutes);
   } else {
     Serial.print(minutes);
   }
   Serial.print(":");
   if(seconds<10) {
     Serial.print("0");Serial.println(seconds);
   } else {
     Serial.println(seconds);
   }
  if(minutes >= target)
  {
    digitalWrite(bomba, !digitalRead(bomba));
    initialTime();
    hours = 0;
    minutes = 0;
    seconds = 0;
  }
  else
  {
  }
 }

