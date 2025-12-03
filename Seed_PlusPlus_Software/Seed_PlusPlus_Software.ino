/*
  Programação da tranca elétrica com solenoide e sensor biométrico
  Projeto de automatização: 

  Responsável pelo projeto: Professor Leo Souza
  Nome do projeto desenvolvido: Seed++

  Desenvolvimento do Seed++ project:
  Desenvolvido para: O laboratório da FnE - Física na Escola 
  Desenvolvido por:  Francisco Passos
  Desenvolvido em:   17/06/2025

  Pela última vez - (Software):
  Modificado em:  03/12/2025
  Modificado por: Francisco Passos

  Manual simplicado do software-hardware abaixo - IMPORTANTE:
    * Tranca :: HIGH == ABERTO & LOW == fechado
    * O sensor biométrico funciona com 3.3v
    * Para deletar tudo, é necessário apertar os botões "delete" e "confirm" ao mesmo tempo e, após isso, confirmar apertando o botão "confirm" 3 vezes
    * Para apagar apenas uma digital, é necessário apertar o botão "delete" e confirmar apertando no botão "confirm" 2 vezes
    * para salvar uma nova digital, é necessário confirmar apertando o botão "confirm" 1 vez
    * O processo de salvar e apagar todos ou apenas um usuário por vez só poderão ser executados no modo ADM com a chave de segurança ligada nomeada como "chave ADM" 
    * Para cada alteração, aparecerá uma mensagem na tela confirmando o feito e/ou alertando o(os) usuário(os) sobre o processo que está sendo feito
    * Sempre que o botão cancel for apertado 2 vezes, qualquer operação é cancelada imediatamente
         O primeiro clique é para "chamar" a atenção do dispositivo sobre querer cancelar
         O segundo clique é para, de fato, afirmar que quer cancelar a operação

  Como desenvolvedor inicial deste código, qualquer pessoa pode entrar em contato comigo para retirar dúvidas por intermédio de um dos meus e-mails:
    E-mail pessoal: franciscopassos.contato@gmail.com     
    E-mail reserva: franksteps.contato@gmail.com
    E-mail academy: francisco.alves@dcomp.ufs.br
    E-mail proffss: contato@franksteps.com.br

  Respeite a documentação geral e os comentários deste código. Caso faça alguma mudança, notifique-me pessoalmente e/ou mande uma mensagem por intermédio do e-mail contendo a atualização do programa.
  Não apague nenhum destes comentários. Eles facilitarão o trabalho de qualquer desenvolvedor e sempre que fizer uma modificação, comente sobre.

  Descrição dos demais desenvolvedores abaixo, por favor:
  *
  *
  *
*/

// Bibliotecas utilizadas pelo Seed++
#include <SoftwareSerial.h> 
#include <LiquidCrystal_I2C.h> 
#include <Wire.h>
#include <Adafruit_Fingerprint.h> 
#include <Adafruit_SleepyDog.h>

#define LCD_ADDRESS 0x27

// Configurações de comunicação
LiquidCrystal_I2C lcd(LCD_ADDRESS, 20, 4); 
SoftwareSerial conectSerial(2, 3); //RX e TX
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&conectSerial);

// Variáveis - dispositivos - de funcionamento geral do Seed++
const int ledR = 4;
const int ledG = 6;
const int chaveADM = 7;
const int led_ADM = 13;
const int tranca = 8;

// Variáveis - dispositivos - utilizadas somente no modo de administrador do Seed++
int buttonADM_recode = 9;
int buttonADM_delete = 10;
int buttonADM_cancel = 11;
int buttonADM_confirm = 12;

// Variáveis - estados dos dispositivos - utilizados no modo de administrador e para a brir a tranca no modo leitura do Seed++
bool chaveADM_state = false;
bool buttonADM_recode_state = false;
bool buttonADM_delete_state = false;
bool buttonADM_cancel_state = false;
bool buttonADM_confirm_state = false;


/* 
  Bloco de compatibilização de chamadas 
  Foi usado no switch da função "mensagem". Não retire essa função deste lugar 
*/
enum Mensagem {
  erro,
  erroSen,
  senPronto,
  operExecutada,
  operCancel,
  emADM,
  mensg2xCancela,
  emLeitura,
  nEncontrada,
  acessPermitido
};


// Função responsável por guardar as mensagens prontas - Não retire essa função deste lugar
void mensagem(int msgTipo, bool apagar = 0){
  // Apaga a tela por padrão, mas é possível configurar para que ele não apague a tela
  if(apagar == 0){
    lcd.clear();
    lcd.setCursor(0,0);
  }

  // Bloco de mensagens para feedback do usuário/adm :: cada case é uma mensagem
  switch(msgTipo){
    case erro: 
      //mensagem geral de erro
      Serial.println(F("Erro encontrado"));
      lcd.print(F("Algo deu errado"));
      signalLed(ledR, 3, 700, 700, HIGH);
    break;

    case erroSen:
      Serial.println("Erro no sensor biométrico");
      lcd.print(F("(!) Erro critico"));
      lcd.setCursor(0,1);
      lcd.print(F("Reinicie agora"));
      signalLed(ledR, 7, 1000, 1000, HIGH);
      signalLed(ledG, 7, 1000, 1000, HIGH);
      
      //reinicie o Seed++ manualmente se ele parar aqui
      while(true){ 
        for(int i = 0; i < 2; i++){
          digitalWrite(led_ADM, HIGH); 
          delay(1000);
          digitalWrite(led_ADM, LOW); 
          delay(1000);
        }
      }

    case senPronto:
      Serial.println(F("Sensor biométrico pronto"));
      lcd.print(F("Sensor pronto!"));
      signalLed(ledG, 3, 200, 200, LOW);
    break;

    case operExecutada:
      Serial.println(F("Operação executada"));
      lcd.print(F("Feito!"));
      signalLed(ledG, 3, 200, 200, LOW);
    break;

    case operCancel:
      Serial.println(F("Operação cancelada"));
      lcd.print(F("Operacao cancelada"));
      signalLed(ledR, 3, 200, 200, HIGH);
    break;

    case emADM:
      Serial.println(F("Modo ADM ativo"));
      lcd.print(F("Modo ADM ativo"));
      lcd.setCursor(0,1);
      lcd.print(F("Laboratorio da FnE"));
      lcd.setCursor(0,3); 
      lcd.print(F("Aguardando cmd"));
    break;

    case mensg2xCancela:
      lcd.setCursor(0,3);
      lcd.print(F("2x para cancelar"));
      delay(500);
    break;

    case emLeitura:
      Serial.println(F("Modo leitura"));
      lcd.print(F("Modo Leitura ativo"));
      lcd.setCursor(0,1);
      lcd.print(F("Laboratorio FnEsc"));
    break;
    
    case nEncontrada:
      Serial.print(F("Digital não encontrada"));
      lcd.print(F("Nao cadastrada"));
      signalLed(ledR, 3, 200, 200, HIGH);
    break;

    case acessPermitido:
      Serial.println(F("Digital cadastrada e encontrada"));
      Serial.print(F("Acesso: "));
      Serial.println(finger.fingerID); 
      lcd.print(F("Acesso ID: "));
      lcd.print(finger.fingerID);
      lcd.setCursor(0,1);
      lcd.print(F("Acesso Liberado"));
    break;
  }
}

/* 
 As demais linhas de código devem ficar abaixo deste comentário e acima do void Setup
 Tenha cuidado com as modificações e criações de funções para o projeto.

 Lembre-se: Para uma função ser chamada sem dar erro, ela precisa ser declarada antes. 
*/

// Abre a tranca, espera 3 segundos e fecha
void abrir_tranca(bool acionadoPorBotao = false) {
  // Abre a tranca
  digitalWrite(tranca, LOW);
  
  // Chamado quando o sensor encontra uma digital cadastrada
  if (!acionadoPorBotao) {
    mensagem(acessPermitido);
  }
  signalLed(ledR, 0, 0, 0, LOW);       
  signalLed(ledG, 1, 3000, 0, LOW);     

  // Fecha a tranca
  digitalWrite(tranca, HIGH);

  // Chamado quando um dos botões ADM é pressionado no modo Leitura
  if (acionadoPorBotao) mensagem(emLeitura);
  signalLed(ledR, 0, 0, 0, HIGH);
  mensagem(emLeitura);
}


// Função para encontrar e retornar um valor inteiro do próximo ID livre
int getNextAvailableID() {
  for (int id = 1; id <= 127; id++) {
    if (finger.loadModel(id) != FINGERPRINT_OK) {
      return id;
    }
  }
  // caso não tenha nenhum ID disponível
  mensagem(erro);
  return -1;
}


// Função para cadastrar nova digital lida no sensor e retornar um valor booleano (verdadeiro ou falso)
bool enrollFingerprint() {
  Watchdog.disable();
  
  int id = getNextAvailableID();
  if (id == -1) {
    Serial.println(F("Memoria cheia"));
    lcd.clear(); 
    lcd.setCursor(0,0); 
    lcd.print(F("Memoria cheia"));
    return 0;
  }

  // Indicando o ID que será feito o possível cadastro
  Serial.print(F("ID livre: "));
  Serial.println(id);
  lcd.clear(); 
  lcd.setCursor(0,0);
  lcd.print(F("ID livre: "));
  lcd.print(id);


  // Início do processo de gravação de uma nova digital do usuário no dispositivo com autenticação em duas etapas
  lcd.setCursor(0,1);
  lcd.print(F("Coloque dedo 1 / 2"));


  // Primeira etapa da autenticação 
  while (finger.getImage() != FINGERPRINT_OK); 
  if (finger.image2Tz(1) != FINGERPRINT_OK) {
    lcd.clear(); 
    lcd.print(F("Erro na leitura"));
    return 0;
  }

  lcd.clear(); 
  lcd.print(F("Remova o dedo"));
  delay(2000);


  // Segunda etapa da autenticação 
  lcd.clear(); 
  lcd.print(F("Coloque dedo 2 / 2"));
  
  while (finger.getImage() != FINGERPRINT_OK);
  if(finger.image2Tz(2) != FINGERPRINT_OK) {
    lcd.clear(); 
    lcd.print(F("Erro na leitura"));
    return 0;
  }
  if(finger.createModel() != FINGERPRINT_OK) {
    lcd.clear(); 
    lcd.print(F("Erro no modelo"));
    return 0;
  }
  if(finger.storeModel(id) != FINGERPRINT_OK) {
    lcd.clear(); 
    lcd.print(F("Erro ao salvar"));
    return 0;
  }

  // Fim do processo de gravação de digital e mensagem notificando o feito
  Serial.print(F("Digital cadastrada ID:"));
  Serial.println(id);
  lcd.clear(); 
  lcd.print(F("Digital salva no ID:"));
  lcd.setCursor(0,1);
  lcd.print(id);
  
  Watchdog.enable(7000);
  return 1;
}


// Função para apagar a digital lida no sensor que retorna um valor booleano
bool deleteFingerprintByScan() {
  Watchdog.disable();
  
  Serial.println(F("Coloque o dedo p/ apagar"));
  lcd.clear(); 
  lcd.print(F("Coloque o dedo para"));
  lcd.setCursor(0,1);
  lcd.print(F("apagar"));

  // Responsável por converter a digital lida no sensor para binário  
  while (finger.getImage() != FINGERPRINT_OK);
  if (finger.image2Tz() != FINGERPRINT_OK) {
    Serial.println(F("Erro na conversao"));
    lcd.clear(); 
    lcd.print(F("Erro na leitura"));
    return 0;
  }

  // Procura a digital
  if (finger.fingerSearch() != FINGERPRINT_OK) {
    mensagem(nEncontrada);
    delay(3000);
    return 0;
  }

  int id = finger.fingerID;
  Serial.print(F("Digital ID: "));
  Serial.println(finger.fingerID);

  // Verificando se a digital foi, de fato, apagada
  if (finger.deleteModel(id) == FINGERPRINT_OK) {
    Serial.println(F("Digital apagada"));
    lcd.clear(); 
    lcd.print(F("Digital apagada"));
    lcd.setCursor(0,1);
    lcd.print(F("ID: "));
    lcd.print(id);
    return 1;
  } else {
    Serial.println(F("Erro ao apagar a digital"));
    lcd.clear(); 
    lcd.print(F("Erro ao apagar"));
    return 0;
  }
  
  Watchdog.enable(7000);
}

/*
 Função de retorno de número inteiro que é usado nos botões indicando o número necessário de vezes em que um determinado botão 
 precisa ser precionado (ou não) para uma operação ser considerada como válida ou inválida
*/
int verificarAcao(int confirmacoesNecessarias, int cancelamentosNecessarios, unsigned long timeoutMs = 6000) {
  int confirmCount = 0;
  int cancelCount = 0;
  unsigned long startTime = millis();

  // Sempre em loop até que haja um valor de retorno "return"
  while (true) {
    // Espera a ação do usuário, se não tiver, o sistema apenas retorna 0 == cancelado
    if (millis() - startTime > timeoutMs) {
      Serial.println(F("Timeout na espera de acao"));
      signalLed(ledR, 3, 200, 200, HIGH);
      Watchdog.reset();
      return 0; 
    }

    // Verifica contador do botão de confirmação 
    if (digitalRead(buttonADM_confirm) == LOW) {
      confirmCount++;
      Serial.print(F("Confirmações: "));
      Serial.println(confirmCount);
      delay(250); 
      if (confirmCount >= confirmacoesNecessarias) {
        signalLed(ledG, 3, 200, 200, LOW);
        return 1;  // confirmado
      }
    }

    // Verifica contador do botão de cancelamento
    if (digitalRead(buttonADM_cancel) == LOW) {
      cancelCount++;
      Serial.print(F("Cancelamentos: "));
      Serial.println(cancelCount);
      delay(250);
      if (cancelCount >= cancelamentosNecessarios) {
        signalLed(ledR, 3, 200, 200, HIGH);
        return 0;  // cancelado
      } 
    }
  }
}


// Indique o led; indique o n; indique o tempo ligado; indique o tempo desligado e indique o estado final
void signalLed (int led, int vezes, int tempo0, int tempo1, bool estadof){
  for(int i = 0; i < vezes; i++){
    digitalWrite(led, HIGH);
    delay(tempo0);  
    digitalWrite(led, LOW);
    delay(tempo1);  
  }
  digitalWrite(led, estadof);
}


// Funções para veridicar a comunicação dos componentes com o Seed++
void VerifySensor(){
  if (!finger.verifyPassword()) {
    while (1); 
  } else {
    Watchdog.reset();
  }
}

bool checkLCD(){
  Wire.beginTransmission(LCD_ADDRESS);
  byte error = Wire.endTransmission();
  return (error == 0); // 0 = respondeu = TRUE
}


// Função responsável pelo modo de administrador - Gravar ou apagar UMA ou TODAS as digitais
void chaveADM_on(){
  while(digitalRead(chaveADM) == HIGH){
    // verificando o sistema
    Watchdog.reset();
    VerifySensor();
    if (!checkLCD()) {
      while(1); // watchdog vai reiniciar
    } else {
      Watchdog.reset();
    }
    
    // Indicando as variáveis de estado dos botões aos seus respectivos botões:
    buttonADM_recode_state = digitalRead(buttonADM_recode);
    buttonADM_delete_state = digitalRead(buttonADM_delete);
    buttonADM_cancel_state = digitalRead(buttonADM_cancel);
    buttonADM_confirm_state = digitalRead(buttonADM_confirm);

    // CADASTRAR UMA NOVA DIGITAL
    if(buttonADM_recode_state == LOW){
      Watchdog.disable();
      
      Serial.println(F("Cadastrar digital"));
      lcd.clear(); 
      lcd.print(F("Cadastrando"));
      lcd.setCursor(0,1); 
      lcd.print(F("Confirma 1x"));
      mensagem(mensg2xCancela, 1);

      int resultado = verificarAcao(1, 2);
      if (resultado == 1) {
        if(enrollFingerprint()){
          Serial.println(F("Digital cadastrada"));
          lcd.clear();
          lcd.print(F("Digital cadastrada"));
          mensagem(operExecutada);
          delay(3000);
          mensagem(emADM);
        } else {
          Serial.println(F("Falha no cadastro"));
          mensagem(erro);
          delay(3000);
          mensagem(emADM);
        }
      } else {
        Serial.println(F("Cadastro cancelado"));
        mensagem(operCancel);
        delay(3000);
        mensagem(emADM);
      }
      Watchdog.enable(7000);
    }

    // APAGAR UMA DIGITAL
    if(buttonADM_delete_state == LOW && buttonADM_confirm_state == HIGH){
      Watchdog.disable();
      
      Serial.println(F("Apagar UMA digital"));
      lcd.clear(); 
      lcd.print(F("Apagar 1 digital"));
      lcd.setCursor(0,1); 
      lcd.print(F("Confirma 2x"));
      mensagem(mensg2xCancela, 1);

      int resultado = verificarAcao(2, 2);

      if (resultado == 1) {
        if(deleteFingerprintByScan()){
          Serial.println(F("Digital apagada"));
          lcd.clear();
          lcd.print(F("Digital apagada!"));
          mensagem(operExecutada);
          delay(3000);
          mensagem(emADM);
        } else {
          Serial.println(F("Falha ao apagar"));
          mensagem(erro);
          delay(3000);
          mensagem(emADM);
        }
      } else {
        Serial.println(F("Exclusão única cancelada"));
        mensagem(operCancel);
        delay(3000);
        mensagem(emADM);
      }
      Watchdog.enable(7000);
    }

    // APAGAR TODAS AS DIGITAIS CADASTRADAS
    if(buttonADM_delete_state == LOW && buttonADM_confirm_state == LOW){
      Watchdog.disable();
      
      Serial.println(F("Apagar TODAS as digitais"));
      lcd.clear(); 
      lcd.print(F("Apagar TUDO"));
      lcd.setCursor(0,1); 
      lcd.print(F("Confirme 3x"));
      mensagem(mensg2xCancela, 1);

      int resultado = verificarAcao(3, 2);
      if (resultado == 1) {
        finger.emptyDatabase(); // Apaga todas as digitais cadastradas
        Serial.println(F("Todas apagadas"));
        lcd.clear(); 
        lcd.print(F("Todas apagadas"));
        mensagem(operExecutada);
        delay(3000);
        mensagem(emADM);
      } else {
        Serial.println(F("Exclusão total cancelada"));
        mensagem(operCancel);
        delay(3000);
        mensagem(emADM);
      }
      Watchdog.enable(7000);
    }
    delay(50);
    Watchdog.reset();
  }
  Serial.println(F("Saindo do modo ADM"));
  delay(1000);
}


// Função responsável pelo modo leitura
void chaveADM_off() {
  mensagem(emLeitura);
  
  while(digitalRead(chaveADM) == LOW){
    
    // verificando a comunicação dos componentes com o Seed++
    Watchdog.reset();
    VerifySensor();
    if (!checkLCD()) {
      while(1); // watchdog vai reiniciar
    } else {
      Watchdog.reset();
    }
    
    // Indicando as variáveis de estado dos botões aos seus respectivos botões:
    buttonADM_recode_state = digitalRead(buttonADM_recode);
    buttonADM_delete_state = digitalRead(buttonADM_delete);
    buttonADM_cancel_state = digitalRead(buttonADM_cancel);
    buttonADM_confirm_state = digitalRead(buttonADM_confirm);

    
    // Quando a porta estiver no modo de leitura, a porta pode ser aberta pelo botão de confirmar
    if (buttonADM_confirm_state == LOW || buttonADM_delete_state == LOW 
      || buttonADM_recode_state == LOW || buttonADM_cancel_state == LOW){
      // abrir a tranca sem mostrar o ID
      abrir_tranca(1);
    }

    if(finger.getImage() != FINGERPRINT_OK) continue;
    if(finger.image2Tz() != FINGERPRINT_OK) continue;
    
    if(finger.fingerSearch() != FINGERPRINT_OK){      
      mensagem(nEncontrada);
      delay(1500);
      mensagem(emLeitura);
      continue; 
    }
    // abrir tranca mostrando a mensagem com ID
    abrir_tranca();
  }
}

// Função de inicialização do software
void setup() {
  // Essa configuração foi mantida aqui por questões de segurança
  pinMode(tranca, OUTPUT);
  digitalWrite(tranca, HIGH);

  Serial.begin(9600);
  finger.begin(57600);
  Watchdog.disable();

  // Configurandos todos os componentes do seed++
  pinMode(ledR, OUTPUT);
  pinMode(ledG, OUTPUT);
  pinMode(led_ADM, OUTPUT);
  pinMode(chaveADM, INPUT_PULLUP);
  pinMode(buttonADM_confirm, INPUT_PULLUP);
  pinMode(buttonADM_recode, INPUT_PULLUP);
  pinMode(buttonADM_delete, INPUT_PULLUP);
  pinMode(buttonADM_cancel, INPUT_PULLUP);
  digitalWrite(ledR, HIGH);
  digitalWrite(ledG, LOW);

  // Iniciando o software e fazendo as ultimas configurapões - display adm
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.clear();

  // Verificando a comunicação com o sensor
  Watchdog.enable(7000);
  VerifySensor();
  if (!checkLCD()) {
    while(1); // watchdog vai reiniciar o arduino
  }
  Watchdog.reset();
}

// Função de loop - apenas responsável pela troca dos modos
void loop(){
  chaveADM_state = digitalRead(chaveADM);
  if(chaveADM_state == HIGH){
    // Modo administrador
    digitalWrite(led_ADM, HIGH);
    Watchdog.reset();
    mensagem(emADM);
    chaveADM_on(); 
  }else{
    // Modo leitura
    digitalWrite(led_ADM, LOW);
    Watchdog.reset();
    mensagem(emLeitura);
    chaveADM_off(); 
  }
}
