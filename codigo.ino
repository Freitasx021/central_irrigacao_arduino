#include "RTClib.h"
RTC_DS1307 rtc;

const char dia0[] PROGMEM = "Dom";
const char dia1[] PROGMEM = "Seg";
const char dia2[] PROGMEM = "Ter";
const char dia3[] PROGMEM = "Qua";
const char dia4[] PROGMEM = "Qui";
const char dia5[] PROGMEM = "Sex";
const char dia6[] PROGMEM = "Sab";
const char* const diaSemana[] PROGMEM = { dia0, dia1, dia2, dia3, dia4, dia5, dia6 };

#include <SPI.h>
#include <SD.h>
File arquivo;
byte pino_SD = 10;
const char confArquivo[] = "conf.txt";

#include <U8x8lib.h>
#include <Wire.h>
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(U8X8_PIN_NONE);

// Outras variáveis
int minuto=99;
char texto[17];

// configurações
char modo[11];
char semana[4][8];
byte inicio[4];
byte duracao[4];
byte quantidade[4];
byte intervalo[4];
byte ciclos[4];
byte intervaloCiclos[4];

// controle
const byte solenoide[4]={1,2,3,4};
byte cb1=0, cb2=0, cb3=0, cb4=0, cb5=0;
byte sol[4]={1,1,1,1};
int cursor1=0;
int cursor2=7;
int cursor3=6;

// Linha 3
byte pl3[4] = {4, 6, 8, 10}; 

//portas
const byte b1=0, b2=1, b3=2, b4=3, b5=4;
const byte ps[4]={8,7,6,5}, pbomba=9, pchuva=A0;  

//leitura
byte vb1=1, vb2=1, vb3=1, vb4=1, vb5=1;

// mensagens
const char linhaVazia[] PROGMEM = "                ";
const char linha1M[] PROGMEM = "Modo: manual    ";
const char linha1A[] PROGMEM = "Modo: automatico";
const char linha2[] PROGMEM = "C B 1 2 3 4";
const char linha3[] PROGMEM = "N N N N N N";
const char linha4[] PROGMEM = "S  D S T Q Q S S";
const char linha6[] PROGMEM = " IN DR Q IT C IC";
// IN = Inicio em horas
// DR = Duração em minutos
// Q = Quantidade de repetições
// IT = Intervalo em minutos entre as repetições
// C = Quantidade de ciclos por dia
// IC = Intervalo entre ciclos em horas
const char inicializando[] PROGMEM = "Inicializando...";
const char dataPerdida[] PROGMEM = "Data perdida!";
const char confFabrica[] PROGMEM = "Conf. de fabrica";
const char erroCartaoSD[] PROGMEM = "Erro cartao SD!";
const char erroRTC[] PROGMEM = "Erro iniciar RTC";
const char erroEscArq[] PROGMEM = "Erro escr. arq.";
const char erroLeiArq[] PROGMEM = "Erro ler arq.";
const char confModoPadrao[] PROGMEM = "manual";
const char confSemanaPadrao[] PROGMEM = "NNNNNNN";
const char loadIn1[] PROGMEM = "/\\";
const char loadIn2[] PROGMEM = "\\/";
const char loadOut[] PROGMEM = "  ";

void setup() {

  pinMode(b1, INPUT_PULLUP);
  pinMode(b2, INPUT_PULLUP);
  pinMode(b3, INPUT_PULLUP);
  pinMode(b4, INPUT_PULLUP);
  pinMode(b5, INPUT_PULLUP);
  pinMode(ps[0], OUTPUT);
  pinMode(ps[1], OUTPUT);
  pinMode(ps[2], OUTPUT);
  pinMode(ps[3], OUTPUT);
  pinMode(pbomba, OUTPUT);
  pinMode(pchuva, INPUT);
  digitalWrite(pbomba,HIGH);
  digitalWrite(ps[0],HIGH);
  digitalWrite(ps[1],HIGH);
  digitalWrite(ps[2],HIGH);
  digitalWrite(ps[3],HIGH);

  // inicializando LCD
  u8x8.begin();
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.clearDisplay();
  strcpy_P(texto, inicializando);
  u8x8.drawString(0, 0, texto);

  delay(1000);

  // Inicializando RTC
  if (!rtc.begin()) {
    strcpy_P(texto, erroRTC);
    mensagem(texto, true);
  }

  // Ajustando data inicial
  if (!rtc.isrunning()) {
    strcpy_P(texto, dataPerdida);
    mensagem(texto,false);
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // Inicializando SD
  if (!SD.begin(pino_SD)) {
    strcpy_P(texto, erroCartaoSD);
    mensagem(texto, true);
  }

  // Verificando se arquivo existe
  if (!SD.exists(confArquivo)) {
    strcpy_P(texto, confFabrica);
    mensagem(texto, false);
    configuracaoFabrica();
  }

  lerConfiguracao();
  imprimirModo();

}

void mensagem(char* mensagem, bool critico){
  u8x8.clearDisplay();
  u8x8.drawString(0, 0, mensagem);
  if(critico){
    while(1);
  }else{
    delay(2000);
    imprimirModo();
  }
}

void imprimirModo(){
  if(strcmp_P(modo, PSTR("manual")) == 0){
    strcpy_P(texto, linha1M);
    u8x8.drawString(0, 1, texto);
    strcpy_P(texto, linhaVazia);
    u8x8.drawString(0, 4, texto);
    u8x8.drawString(0, 5, texto);
    u8x8.drawString(0, 6, texto);
    u8x8.drawString(0, 7, texto);
  }else{
    strcpy_P(texto, linha1A);
    u8x8.drawString(0, 1, texto);
    strcpy_P(texto, linha4);
    u8x8.drawString(0, 4, texto);
    strcpy_P(texto, linha6);
    u8x8.drawString(0, 6, texto);
    diasSemana();
    programas();
  }
  strcpy_P(texto, linha2);
  u8x8.drawString(0, 2, texto);
  strcpy_P(texto, linha3);
  u8x8.drawString(0, 3, texto);
}

void diasSemana(){
  snprintf(texto, sizeof(texto), "%d  %c %c %c %c %c %c %c", solenoide[cursor1], semana[cursor1][0], semana[cursor1][1], semana[cursor1][2], semana[cursor1][3], semana[cursor1][4], semana[cursor1][5], semana[cursor1][6]);
  if(cursor2==0){
    texto[2] = '>';
  }else if(cursor2==1){
    texto[4] = '>';
  }else if(cursor2==2){
    texto[6] = '>';
  }else if(cursor2==3){
    texto[8] = '>';
  }else if(cursor2==4){
    texto[10] = '>';
  }else if(cursor2==5){
    texto[12] = '>';
  }else if(cursor2==6){
    texto[14] = '>';
  }
  u8x8.drawString(0, 5, texto);
}

void programas(){
  snprintf(texto, sizeof(texto), " %02d %02d %d %02d %d %02d", inicio[cursor1],duracao[cursor1],quantidade[cursor1],intervalo[cursor1],ciclos[cursor1],intervaloCiclos[cursor1]);
  if(cursor3==0){
    texto[0] = '>';
  }else if(cursor3==1){
    texto[3] = '>';
  }else if(cursor3==2){
    texto[6] = '>';
  }else if(cursor3==3){
    texto[8] = '>';
  }else if(cursor3==4){
    texto[11] = '>';
  }else if(cursor3==5){
    texto[13] = '>';
  }
  u8x8.drawString(0, 7, texto);
}

void loop() {
  DateTime now = rtc.now();
  if(minuto!=now.minute()){
    minuto = now.minute();
    char dia_semana[4];
    strcpy_P(dia_semana, (char*)pgm_read_word(&(diaSemana[now.dayOfTheWeek()])));
    snprintf(texto, sizeof(texto), "%02d/%02d %s  %02d:%02d", now.day(), now.month(), dia_semana, now.hour(), now.minute());
    u8x8.drawString(0, 0, texto);
  }

  vb1 = digitalRead(b1);
  vb2 = digitalRead(b2);
  vb3 = digitalRead(b3);
  vb4 = digitalRead(b4);
  vb5 = digitalRead(b5);

  if(digitalRead(pchuva)==LOW){
    digitalWrite(pbomba,HIGH);
    for(int i=0;i<4;i++){
      digitalWrite(ps[i],HIGH);
      sol[i]=1;
    }
    u8x8.drawString(0, 3, "S N N N N N");
    delay(10000);
  }else{
    u8x8.drawString(0, 3, "N");

    if(strcmp_P(modo, PSTR("manual")) == 0){
      if (vb1==0 && cb1==1) {
        cb1=0;
        if(sol[0]==0){
          sol[0]=1;
          u8x8.drawString(4, 3, "N");
        }else{
          sol[0]=0;
          u8x8.drawString(4, 3, "S");
        }
        digitalWrite(ps[0],sol[0]);
      }else if (vb2==0 && cb2==1) {
        cb2=0;
        if(sol[1]==0){
          sol[1]=1;
          u8x8.drawString(6, 3, "N");
        }else{
          sol[1]=0;
          u8x8.drawString(6, 3, "S");
        }
        digitalWrite(ps[1],sol[1]);
      }else if (vb3==0 && cb3==1) {
        cb3=0;
        if(sol[2]==0){
          sol[2]=1;
          u8x8.drawString(8, 3, "N");
        }else{
          sol[2]=0;
          u8x8.drawString(8, 3, "S");
        }
        digitalWrite(ps[2],sol[2]);
      }else if (vb4==0 && cb4==1) {
        cb4=0;
        if(sol[3]==0){
          sol[3]=1;
          u8x8.drawString(10, 3, "N");
        }else{
          sol[3]=0;
          u8x8.drawString(10, 3, "S");
        }
        digitalWrite(ps[3],sol[3]);
      }
    }else if(strcmp_P(modo, PSTR("auto")) == 0){
      if (vb1==0 && cb1==1) {
        cb1=0;
        if(cursor1==3){
          cursor1=0;
        }else{
          cursor1++;
        }
        cursor2=7;
        cursor3=6;
        diasSemana();
        programas();
      }else if (vb2==0 && cb2==1) {
        cb2=0;
        if(cursor2==7){
          cursor2=0;
        }else{
          cursor2++;
          if(cursor2==7){
            escreverConfiguracao();
          }
        }
        if(cursor3!=6){
          cursor3=6;
          programas();
        }
        diasSemana();
      }else if (vb3==0 && cb3==1) {
        cb3=0;
        if(cursor2!=7){
          if(semana[cursor1][cursor2] == 'N'){
            semana[cursor1][cursor2]='S';
          }else{
            semana[cursor1][cursor2]='N';
          }
          diasSemana();
        }
        if(cursor3!=6){
          if(cursor3==0){
            if(inicio[cursor1]==23){ // limite: 23 horas
              inicio[cursor1]=0;
            }else{
              inicio[cursor1]++;
            }
          }else if(cursor3==1){
            if(duracao[cursor1]==90){ // limite: 90 minutos
              duracao[cursor1]=1;
            }else{
              duracao[cursor1]++;
            }
          }else if(cursor3==2){
            if(quantidade[cursor1]==9){ // limite: 9 repetições
              quantidade[cursor1]=1;
            }else{
              quantidade[cursor1]++;
            }
          }else if(cursor3==3){
            if(intervalo[cursor1]==90){ // limite: 90 minutos
              intervalo[cursor1]=1;
            }else{
              intervalo[cursor1]++;
            }
          }else if(cursor3==4){
            if(ciclos[cursor1]==9){ // limite: 9 ciclos
              ciclos[cursor1]=1;
            }else{
              ciclos[cursor1]++;
            }
          }else if(cursor3==5){
            if(intervaloCiclos[cursor1]==16){ // limite: 16 horas
              intervaloCiclos[cursor1]=1;
            }else{
              intervaloCiclos[cursor1]++;
            }
          }
          programas();
        }
      }else if (vb4==0 && cb4==1) {
        cb4=0;
        if(cursor3==6){
          cursor3=0;
        }else{
          cursor3++;
          if(cursor3==6){
            escreverConfiguracao();
          }
        }
        if(cursor2!=7){
          cursor2=7;
          diasSemana();
        }
        programas();
      }
      // Controle de irrigação
      if(now.second()%10==0){ // a cada 10 segundos

        int horaAtual = now.hour();
        int minutoAtual = now.minute();
        int minutoDoDia = horaAtual * 60 + minutoAtual;

        for(int i=0;i<4;i++){
          sol[i] = 1;
          if(semana[i][now.dayOfTheWeek()]=='S'){
            for (int ciclo = 0; ciclo < ciclos[i]; ciclo++) {
              int inicioCicloMinuto = inicio[i] * 60 + ciclo * intervaloCiclos[i] * 60;
              for (int repeticao = 0; repeticao < quantidade[i]; repeticao++) {
                int inicioRega = inicioCicloMinuto + repeticao * (duracao[i] + intervalo[i]);
                int fimRega = inicioRega + duracao[i];
                if (minutoDoDia >= inicioRega && minutoDoDia < fimRega) {
                  sol[i] = 0;
                  break;
                }
              }
              if (sol[i]==0) break;
            }
          }
          digitalWrite(ps[i], sol[i]);
          u8x8.drawString(pl3[i], 3, sol[i] ? "N" : "S");
        }
      }
    }
    if (vb5==0 && cb5==1) {
      cb5=0;
      if(strcmp_P(modo, PSTR("manual")) == 0){
        strcpy(modo, "auto");
      }else{
        strcpy(modo, "manual"); 
      }
      imprimirModo();
      sol[0]=1,sol[1]=1,sol[2]=1,sol[3]=1;
      digitalWrite(pbomba,HIGH);
      digitalWrite(ps[0],HIGH);
      digitalWrite(ps[1],HIGH);
      digitalWrite(ps[2],HIGH);
      digitalWrite(ps[3],HIGH);
      u8x8.drawString(2, 3, "N N N N N");
      escreverConfiguracao();
    }
    if(sol[0]==0 || sol[1]==0 || sol[2]==0 || sol[3]==0){
      digitalWrite(pbomba,LOW);
      u8x8.drawString(2, 3, "S");
    }else{
      digitalWrite(pbomba,HIGH);
      u8x8.drawString(2, 3, "N");
    }

    cb1=vb1;
    cb2=vb2;
    cb3=vb3;
    cb4=vb4;
    cb5=vb5;
    
  }

}

boolean lerConfiguracao() {
  arquivo = SD.open(confArquivo);
  if (arquivo) {
    char buffer[16];
    int idx = 0;
    while (arquivo.available()) {
      char c = arquivo.read();
      if (c == '\r') {
        // Ignora retorno de quebra, não faz nada
        continue;
      }
      if (c == '\n') {
        buffer[idx] = '\0';
        if (idx > 0) {  // só imprime se tem conteúdo
          buffer[idx] = '\0';
            atribuirConfiguracao(buffer);
            // u8x8.drawString(0, 7, buffer);
            // delay(1000);
        }
        idx = 0;
      } else {
        if (idx < sizeof(buffer) - 1) buffer[idx++] = c;
      }
    }
    if (idx > 0) { // última linha sem \n
      buffer[idx] = '\0';
      atribuirConfiguracao(buffer);
    }
    arquivo.close();
  } else {
    strcpy_P(texto, erroLeiArq);
    mensagem(texto,false);
  }
}

void atribuirConfiguracao(char* linha){
  char* propriedade = strtok(linha, "=");
  char* valor = strtok(NULL, "=");
  if (propriedade && valor) {
    if (strcmp_P(propriedade, PSTR("modo")) == 0) {
      strcpy(modo, valor);
    }else if (strcmp_P(propriedade, PSTR("semana1")) == 0) {
      strcpy(semana[0], valor);
    }else if (strcmp_P(propriedade, PSTR("semana2")) == 0) {
      strcpy(semana[1], valor);
    }else if (strcmp_P(propriedade, PSTR("semana3")) == 0) {
      strcpy(semana[2], valor);
    }else if (strcmp_P(propriedade, PSTR("semana4")) == 0) {
      strcpy(semana[3], valor);
    }else if (strcmp_P(propriedade, PSTR("inicio1")) == 0) {
      inicio[0] = (byte)atoi(valor);
    }else if (strcmp_P(propriedade, PSTR("inicio2")) == 0) {
      inicio[1] = (byte)atoi(valor);
    }else if (strcmp_P(propriedade, PSTR("inicio3")) == 0) {
      inicio[2] = (byte)atoi(valor);
    }else if (strcmp_P(propriedade, PSTR("inicio4")) == 0) {
      inicio[3] = (byte)atoi(valor);
    }else if (strcmp_P(propriedade, PSTR("duracao1")) == 0) {
      duracao[0] = (byte)atoi(valor);
    }else if (strcmp_P(propriedade, PSTR("duracao2")) == 0) {
      duracao[1] = (byte)atoi(valor);
    }else if (strcmp_P(propriedade, PSTR("duracao3")) == 0) {
      duracao[2] = (byte)atoi(valor);
    }else if (strcmp_P(propriedade, PSTR("duracao4")) == 0) {
      duracao[3] = (byte)atoi(valor);
    }else if (strcmp_P(propriedade, PSTR("quantidade1")) == 0) {
      quantidade[0] = (byte)atoi(valor);
    }else if (strcmp_P(propriedade, PSTR("quantidade2")) == 0) {
      quantidade[1] = (byte)atoi(valor);
    }else if (strcmp_P(propriedade, PSTR("quantidade3")) == 0) {
      quantidade[2] = (byte)atoi(valor);
    }else if (strcmp_P(propriedade, PSTR("quantidade4")) == 0) {
      quantidade[3] = (byte)atoi(valor);
    }else if (strcmp_P(propriedade, PSTR("intervalo1")) == 0) {
      intervalo[0] = (byte)atoi(valor);
    }else if (strcmp_P(propriedade, PSTR("intervalo2")) == 0) {
      intervalo[1] = (byte)atoi(valor);
    }else if (strcmp_P(propriedade, PSTR("intervalo3")) == 0) {
      intervalo[2] = (byte)atoi(valor);
    }else if (strcmp_P(propriedade, PSTR("intervalo4")) == 0) {
      intervalo[3] = (byte)atoi(valor);
    }else if (strcmp_P(propriedade, PSTR("ciclos1")) == 0) {
      ciclos[0] = (byte)atoi(valor);
    }else if (strcmp_P(propriedade, PSTR("ciclos2")) == 0) {
      ciclos[1] = (byte)atoi(valor);
    }else if (strcmp_P(propriedade, PSTR("ciclos3")) == 0) {
      ciclos[2] = (byte)atoi(valor);
    }else if (strcmp_P(propriedade, PSTR("ciclos4")) == 0) {
      ciclos[3] = (byte)atoi(valor);
    }else if (strcmp_P(propriedade, PSTR("interciclos1")) == 0) {
      intervaloCiclos[0] = (byte)atoi(valor);
    }else if (strcmp_P(propriedade, PSTR("interciclos2")) == 0) {
      intervaloCiclos[1] = (byte)atoi(valor);
    }else if (strcmp_P(propriedade, PSTR("interciclos3")) == 0) {
      intervaloCiclos[2] = (byte)atoi(valor);
    }else if (strcmp_P(propriedade, PSTR("interciclos4")) == 0) {
      intervaloCiclos[3] = (byte)atoi(valor);
    }
  }
}

void configuracaoFabrica(){
  strcpy_P(texto, confModoPadrao);
  strcpy(modo, texto);
  strcpy_P(texto, confSemanaPadrao);
  for(int i=0;i<4;i++){
    strcpy(semana[i], texto);
    inicio[i]=1;
    duracao[i]=1;
    quantidade[i]=1;
    intervalo[i]=1;
    ciclos[i]=1;
    intervaloCiclos[i]=1;
  }
  escreverConfiguracao();
}

void escreverConfiguracao(){
  strcpy_P(texto, loadIn1);
  u8x8.drawString(14, 2, texto);
  strcpy_P(texto, loadIn2);
  u8x8.drawString(14, 3, texto);
  SD.remove(confArquivo);
  arquivo = SD.open(confArquivo, FILE_WRITE);
  if (arquivo) {
    arquivo.print(F("modo="));
    arquivo.println(modo);
    for (int i = 0; i < 4; i++) {
      // Semana
      arquivo.print(F("semana"));
      arquivo.print(i+1);
      arquivo.print(F("="));
      arquivo.println(semana[i]);
      // Inicio
      arquivo.print(F("inicio"));
      arquivo.print(i+1);
      arquivo.print(F("="));
      arquivo.println(inicio[i]);
      // Duracao
      arquivo.print(F("duracao"));
      arquivo.print(i+1);
      arquivo.print(F("="));
      arquivo.println(duracao[i]);
      // Quantidade
      arquivo.print(F("quantidade"));
      arquivo.print(i+1);
      arquivo.print(F("="));
      arquivo.println(quantidade[i]);
      // Intervalo
      arquivo.print(F("intervalo"));
      arquivo.print(i+1);
      arquivo.print(F("="));
      arquivo.println(intervalo[i]);
      // Ciclos
      arquivo.print(F("ciclos"));
      arquivo.print(i+1);
      arquivo.print(F("="));
      arquivo.println(ciclos[i]);
      // Intervalo Ciclos
      arquivo.print(F("interciclos"));
      arquivo.print(i+1);
      arquivo.print(F("="));
      arquivo.println(intervaloCiclos[i]);
    }
    arquivo.close();
    delay(1000);
    strcpy_P(texto, loadOut);
    u8x8.drawString(14, 2, texto);
    u8x8.drawString(14, 3, texto);
  } else {
    strcpy_P(texto, erroEscArq);
    mensagem(texto,false);
  }
}
