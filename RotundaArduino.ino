//
// ROTUNDA ARDUINO - Versao 3.0
// By Clederson T. Przybysz - clederson_p@yahoo.com.br
// expressoarduino.blogspot.com
// Criação: Dezembro/2019
//
//Copyright Notes Interface 48 Controles:
//O SOFTWARE É FORNECIDO "NO ESTADO EM QUE SE ENCONTRAM", SEM GARANTIA DE QUALQUER TIPO, EXPRESSA OU IMPLÍCITA, MAS NÃO SE LIMITANDO ÀS GARANTIAS DE COMERCIALIZAÇÃO.  
//EM NENHUMA CIRCUNSTÂNCIA, O AUTOR/TITULAR DE DIREITOS AUTORAIS SE RESPONSABILIZA POR QUALQUER RECLAMAÇÃO, DANOS OU OUTRA RESPONSABILIDADE, 
//SEJA EM AÇÃO DE CONTRATO, DELITO OU DE OUTRA FORMA, DECORRENDO DESTE SOFTWARE OU RELACIONADO AO SEU  USO.
//

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Stepper.h>
#include <EEPROM.h>

//Inicia Display
#define SCREEN_WIDTH 128 // OLED Pixels Largura 
#define SCREEN_HEIGHT 32 // OLED Pixels Altura 
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Inicia Motor de Passo
Stepper myStepper(512, 8, 10, 9, 11);

#define PassosVolta 2048
#define pinFimCurso 12
#define pinDown 3
#define pinUp 4
#define pinConfirma 5
#define pinCancela 6
#define pinInverte 7
#define pinReleInversao 13

int posAtual;
byte saidaAtual=1;
byte saidaSelecionada=1;
byte InverterPonte=0;
byte PonteInvertida=0;
byte b1;
byte b2;
byte b3;
byte b4;
byte b5;
byte nSaidas;
byte DirecaoPadrao; //0-Horario 1-Antihorario
byte UltimaDirecao;
byte OffSet;

void setup() {
  //Serial.begin(19200);

  pinMode(pinFimCurso, INPUT);
  pinMode(pinDown, INPUT_PULLUP);
  pinMode(pinUp, INPUT_PULLUP);
  pinMode(pinConfirma, INPUT_PULLUP);
  pinMode(pinCancela, INPUT_PULLUP);
  pinMode(pinInverte, INPUT_PULLUP);
  pinMode(pinReleInversao, OUTPUT);
  
  //Carrega Numero Saidas
  CarregaNumeroSaidas();

  //Inicia Display SSD1306
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  //Mensagem Inicial Display
  display.clearDisplay();
  display.setCursor(8,0);
  display.setTextSize(1);           
  display.setTextColor(SSD1306_WHITE); 
  display.println(F("Virador Locomotivas"));             
  display.println(F("Expresso Arduino"));
  display.setCursor(0,25);
  display.println(F("Movendo Posicao 1"));
  display.display();
  
  myStepper.setSpeed(10);
  //Move Posicao Inicial
  MovePosicaoZero();
  

  //Aguarda Modo Config
  byte x=0;
  do {
    delay(10);          
    if (digitalRead(pinInverte)==0) {
      MenuConfig();
      MovePosicaoZero();
      x=100;
    }
    x++;
  } while (x < 100);
  
  delay(1000);

  display.clearDisplay();
  MostraPosicao();
  //Serial.println("Saida 1");
  posAtual=0;
  
}

void loop() {
  byte v1=digitalRead(pinDown);
  byte v2=digitalRead(pinUp);
  byte v3=digitalRead(pinConfirma);
  byte v4=digitalRead(pinCancela);
  byte v5=digitalRead(pinInverte);
  //Botao Down
  if (b1!=v1) {
    b1=v1;
    if (b1==0) {
      saidaSelecionada=saidaSelecionada-1;
      if (saidaSelecionada<1) saidaSelecionada= nSaidas;
      MostraPosicao();
    }
    delay(100);
  }
  //Botao Up
  if (b2!=v2) {
    b2=v2;
    if (b2==0) {
      saidaSelecionada=saidaSelecionada+1;
      if (saidaSelecionada>nSaidas) saidaSelecionada=1;
      MostraPosicao();
    }
    delay(100);
  }
  //Botao OK
  if (b3!=v3) {
    b3=v3;
    if (b3==0) {
      MoveSaida(saidaSelecionada); 
    }
    delay(100);
  }
  //Botao Cancelar
  if (b4!=v4) {
    b4=v4;
    if (b4==0) {
      saidaSelecionada=saidaAtual;
      MostraPosicao();
    }
    delay(100);
  }
  //Botao Inverter
  if (b5!=v5) {
    b5=v5;
    if (b5==0) {
      //Inverter
      InverterPonte = !InverterPonte;
      //Serial.print("Inverter:");
      //Serial.println(InverterPonte);
      MostraPosicao();
    }
    delay(100);
  }
  
}


void MostraPosicao() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);            
  display.setCursor(8,0);
  display.println(F("Virador Locomotivas")); 
  display.print(F("Atual:")); 
  display.setCursor(70,10);
  display.print(F("Selec:"));
  display.setTextSize(2);   
  display.setCursor(20,18);
  display.print(saidaAtual);
  display.setCursor(83,18);
  display.print(saidaSelecionada);
  //Flag Inverter
  if (InverterPonte==1) {
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    display.setCursor(110,18);  
    display.print(F("i"));
  }
  else
  {
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(110,18);  
    display.print(F(" "));
  }
  display.display();
  //Serial.print("Saida Selecionada:");
  //Serial.println(saidaSelecionada);
}

void MostraMovendo() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(8,0);
  display.println(F("Virador Locomotivas")); 
  display.setCursor(15,15);
  display.print(F("Movendo Posicao ")); 
  display.print(saidaSelecionada);
  display.display();
}

void MoveSaida(byte SaidaSelecionada) {
  //Atualiza Display
  MostraMovendo();
  
  SaidaSelecionada = SaidaSelecionada-1;
  int PosicaoDestino=CarregaPosicaoSaida(SaidaSelecionada);
  MovePosicao(PosicaoDestino);
  saidaAtual=SaidaSelecionada+1;
  
  MostraPosicao();
}

void MovePosicao(int PosicaoDestino) {
  int nPassos;
  byte NovaDirecao;
  //Serial.print("De:");
  //Serial.print(posAtual);
  //Serial.print(" Para:");
  //Serial.println(PosicaoDestino);
  
  //Flag Invertido Adicona 1/2 Volta
  if (PonteInvertida==1) {
    PosicaoDestino=PosicaoDestino+PassosVolta/2;
    //Serial.print("+Invertido ");
    //Serial.print(PassosVolta/2);
    //Serial.print(":");
    //Serial.println(PosicaoDestino);
  }
  //Flag Inverter Adicona 1/2 Volta
  if (InverterPonte==1) {
    PosicaoDestino=PosicaoDestino+PassosVolta/2;
    InverterPonte=0;
    PonteInvertida = !PonteInvertida;
    //Serial.print("+Inverter ");
    //Serial.print(PassosVolta/2);
    //Serial.print(":");
    //Serial.println(PosicaoDestino);
    
    //Liga/Desliga Rele Inversao da Ponte
    if (PonteInvertida==1) {
      //Inverte Polaridade Ponte
      digitalWrite(pinReleInversao,HIGH);
    }
    else {
      //Retorna Polaridade Ponte
      digitalWrite(pinReleInversao,LOW);
    }
  }

  //Maior que volta completa Desconta 1 Volta
  if (PosicaoDestino>=PassosVolta) {
    PosicaoDestino = PosicaoDestino - PassosVolta;
    //Serial.print("-Volta Completa ");
    //Serial.print(PassosVolta);
    //Serial.print(":");
    //Serial.println(PosicaoDestino);  
  }

  //Calcula Numero de Passos para Nova Posicao
  nPassos = PosicaoDestino-posAtual;
  //Serial.print("=PosicaoDestino:");
  //Serial.print(PosicaoDestino); 
  //Serial.print(" Passos:");
  //Serial.println(nPassos); 
    
  //Calcular Menor Caminho
  if (abs(nPassos)>PassosVolta/2) {
    if (nPassos>0) {
      nPassos = nPassos-PassosVolta;
    }
    else
    {
      nPassos = PassosVolta+nPassos;
    }
    //Serial.print("Menor Caminho:");
    //Serial.println(nPassos); 
  }
  //Serial.println("");

  //Aplica OffSet
  if (nPassos>0) {
    if (UltimaDirecao==1) {
      nPassos=nPassos+OffSet;
      UltimaDirecao=0;
    }
  }
  else
  {
    if (UltimaDirecao==0) {
      nPassos=nPassos-OffSet;
      UltimaDirecao=1;
    }
  }
  

  //Move Ponte
  if (DirecaoPadrao==0) nPassos=nPassos*-1; //Inverte Direcao dos Passos para Sentido Horario
  myStepper.step(nPassos);
  posAtual = PosicaoDestino;

  
}

void MovePosicaoZero() {
  while (digitalRead(pinFimCurso)==1) {
    if (DirecaoPadrao==0) {
      myStepper.step(-1); 
    }
    else
    {
      myStepper.step(1);
    }
    
  }
  digitalWrite(pinReleInversao,LOW);
  UltimaDirecao=0;
}

void ItemMenu(byte nMenu, byte linhaInicial, byte nSelecao, char TextoMenu[]) {
  if (nMenu<linhaInicial) return;
  if (nMenu==linhaInicial+nSelecao) {
    display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
  }
  else
  {
    display.setTextColor(SSD1306_WHITE);
  }
  display.println(TextoMenu); 
}


void MenuConfig() {
  byte nMenu=0;
  byte sMenu=0;
  byte lMenu=0;
  byte SairConfig=1;
  int vSaida=0;
  while (SairConfig==1) {
    //Serial.print("Menu:");
    //Serial.print(nMenu);
    //Serial.print(" SubMenu:");
    //Serial.print(sMenu);
    //Serial.print(" Linha:");
    //Serial.println(lMenu);
    
    //Exibe Menu de Configuracao
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);            
    display.setCursor(8,0);
    display.println(F("Configuracao Virador")); 
    switch (nMenu) {
      case 0:
        ItemMenu(0,lMenu,sMenu,"Numero Saidas"); 
        ItemMenu(1,lMenu,sMenu,"Posicao Saida"); 
        ItemMenu(2,lMenu,sMenu,"Direcao Padrao"); 
        ItemMenu(3,lMenu,sMenu,"Off-Set"); 
        ItemMenu(4,lMenu,sMenu,"Fim"); 
        break;
      case 1:
        display.println("");
        display.print(F("Numero Saidas: ")); 
        display.println(sMenu); 
        break;
      case 2:
        display.print(F("Saida: ")); 
        display.println(sMenu); 
        break;
      case 3:
        display.print(F("Saida: ")); 
        display.println(sMenu); 
        display.print(F("Posicao: ")); 
        display.println(vSaida); 
        display.display();
        MovePosicao(vSaida);
        break;
     case 4:
        display.print(F("Direcao Padrao: ")); 
        if (sMenu==0) {
          display.println(F("Horario")); 
        }
        else
        {
          display.println(F("Anti-Horario"));  
        }
        break;
     case 5:
        display.print(F("Off-Set (Passos): ")); 
        display.println(sMenu); 
    }
    display.display();

    //Aguarda Tecla Precionada
    byte Tecla=0;
    do {
      if (digitalRead(pinDown)==0) Tecla=1; 
      if (digitalRead(pinUp)==0) Tecla=2;
      if (digitalRead(pinConfirma)==0) Tecla=3;
      if (digitalRead(pinCancela)==0) Tecla=4;
      if (nMenu==3) {
        delay(100);
      }
      else
      {
        delay(200);
      } 
    //Serial.print("Menu:");
    //Serial.print(nMenu);
    //Serial.print(" SubMenu:");
    //Serial.print(sMenu);
    //Serial.print(" Tecla:");
    //Serial.println(Tecla);
    } while (Tecla==0);
    
    //Trata Tecla Precionada
    switch (Tecla) {
      
      case 1: //Down
        if (nMenu==0) {
          sMenu--;
          if (sMenu==255) {
            sMenu=0;
            lMenu--;
            if (lMenu==255) lMenu=0;
          }
         
        }
        else if (nMenu==3) {
          vSaida--;
          if (vSaida==0) vSaida=PassosVolta-1;
        }
        else if (nMenu==4) {
          sMenu = !sMenu;
        }
        else if (nMenu==5) {
          sMenu--;
        }
        else
        {
          sMenu--;
          if (nMenu==2&&sMenu<2) sMenu=nSaidas;
        }
        break;
      
      case 2: //Botao Up
        if (nMenu==0) {
          sMenu++;
          if (sMenu==3) {
            sMenu=2;
            lMenu++;
            if (lMenu==3) lMenu=2;
          }
        }
        else if (nMenu==3) {
          vSaida++;
          if (vSaida==PassosVolta) vSaida=1;
        }
        else if (nMenu==4) {
          sMenu = !sMenu;
        }
        else if (nMenu==5) {
          sMenu++;
        }
        else 
        {
          sMenu++;
          if (nMenu==2&&sMenu>nSaidas) sMenu=2;
        }
        break;
     
     case 3: //Botao Confirma
        if (nMenu==0) { 
          if (sMenu+lMenu==0) {
            //Numero de Saidas
            sMenu=nSaidas;
            nMenu=1; 
          }
          else if (sMenu+lMenu==1) {
            //Posicao Saidas
            sMenu=2;
            nMenu=2;
          }
          else if (sMenu+lMenu==2) {
            //Direcao Padrao
            sMenu=DirecaoPadrao;
            nMenu=4;
          }
          else if (sMenu+lMenu==3) {
            //Off-Set
            sMenu=OffSet;
            nMenu=5;
          }
          else if (sMenu+lMenu==4) SairConfig=0;
        }
        else if (nMenu==1) { 
          //Grava Saidas
          nSaidas = sMenu;
          GravaNumeroSaidas(nSaidas);
          //Volta Menu
          nMenu=0; 
          sMenu=0;
          lMenu=0;
        }
        else if (nMenu==2) { 
          //Carrega Posicao Saida Selecionada
          vSaida=CarregaPosicaoSaida(sMenu-1);
          //Menu Posicao Saida
          nMenu=3; 
        }
        else if (nMenu==3) { 
          //Grava Posicao Saida 
          GravaPosicaoSaida(sMenu-1,vSaida);
          //Menu Seleciona Saida
          nMenu=2; 
        } 
        else if (nMenu==4) { 
          //Grava Direcao Padrao 
          EEPROM.write(7, sMenu);
          DirecaoPadrao=sMenu;
          delay(300);
          nMenu=0; 
          sMenu=0;
          lMenu=0;
        }
        else if (nMenu==5) { 
          //Grava OffSet
          EEPROM.write(8, sMenu);
          OffSet=sMenu;
          delay(300);
          nMenu=0; 
          sMenu=0;
          lMenu=0;
        }
        break;
    //Cancela
    case 4:
        if (nMenu==3) {
          nMenu=2;  
        }
        else
        {
          nMenu=0; 
          sMenu=0;
          lMenu=0;
        }
        
        break;
    }
  }
}

void CarregaNumeroSaidas() {
  //Le Configuracoes
  DirecaoPadrao=EEPROM.read(7);
  OffSet=EEPROM.read(8);
  nSaidas= EEPROM.read(9);
  
  if (nSaidas==0) {
    //Valor Inicial com 4 Saidas
    //Serial.println("Grava Padrao Saidas");
    nSaidas=4;
    GravaNumeroSaidas(nSaidas);
    GravaPosicaoSaida(1,512);
    GravaPosicaoSaida(2,1024);
    GravaPosicaoSaida(3,1536);
  }
}

void GravaNumeroSaidas(byte nSai) {
  //Grava Posicao da Saida 
  EEPROM.write(9, nSai);
  delay(300);
}

int CarregaPosicaoSaida(byte Saida) {
  int PosicaoSaida=0;
  if (Saida>0) {
    // Carrega Posicao da Saida 
    PosicaoSaida = EEPROM.read(8+Saida*2)*256;
    PosicaoSaida = PosicaoSaida + EEPROM.read(9+Saida*2);
    if (PosicaoSaida==0) PosicaoSaida=1;
  }
  return PosicaoSaida;
}

void GravaPosicaoSaida(byte Saida, int Posicao) {
  // Grava Posicao da Saida 
  EEPROM.write(8+Saida*2, Posicao/256);
  EEPROM.write(9+Saida*2, Posicao%256);
  delay(300);
}
