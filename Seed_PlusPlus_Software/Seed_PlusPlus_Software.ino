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
  Modificado em:  30/11/2025
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
#include <Adafruit_Fingerprint.h> 


// Configurações I2C
LiquidCrystal_I2C lcd(0x27, 20, 4); 

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


// Esta função foi mantida aqui por questões de segurança - Não coloque demais funções acima desta
void trancaInit(){
  digitalWrite(tranca, HIGH);
}


// Conexão p/ leitura e gravação do sensor biométrico
SoftwareSerial conectSerial(2, 3); //RX e TX
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&conectSerial);


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
      Serial.println("Erro encontrado");
      lcd.print("Algo deu errado");
      signalLed(ledR, 3, 700, 700, HIGH);
    break;

    case erroSen:
      Serial.println("Erro no sensor biométrico");
      lcd.print("(!) Erro critico");
      lcd.setCursor(0,1);
      lcd.print("Reinicie agora");
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
      Serial.println("Sensor biométrico pronto");
      lcd.print("Sensor pronto!");
      signalLed(ledG, 3, 200, 200, LOW);
    break;

    case operExecutada:
      Serial.println("Operação executada");
      lcd.print("Feito!");
      signalLed(ledG, 3, 200, 200, LOW);
    break;

    case operCancel:
      Serial.println("Operação cancelada");
      lcd.print("Operacao cancelada");
      signalLed(ledR, 3, 200, 200, HIGH);
    break;

    case emADM:
      Serial.println("Modo ADM ativo");
      lcd.print("Modo ADM ativo");
      lcd.setCursor(0,1);
      lcd.print("Laboratorio da FnE");
      lcd.setCursor(0,3); 
      lcd.print("Aguardando cmd");
    break;

    case mensg2xCancela:
      lcd.setCursor(0,3);
      lcd.print("2x para cancelar");
      delay(500);
    break;

    case emLeitura:
      Serial.println("Modo leitura");
      lcd.print("Modo Leitura ativo");
      lcd.setCursor(0,1);
      lcd.print("Laboratorio da FnE");
    break;
    
    case nEncontrada:
      Serial.print("Digital não encontrada");
      lcd.print("Nao cadastrada");
      signalLed(ledR, 3, 200, 200, HIGH);
    break;

    case acessPermitido:
      Serial.println("Digital cadastrada e encontrada");
      Serial.print("Acesso: ");
      Serial.println(finger.fingerID); 
      lcd.print("Acesso ID: ");
      lcd.print(finger.fingerID);
      lcd.setCursor(0,1);
      lcd.print("Acesso Liberado");
    break;
  }
}

/* 
 As demais linhas de código devem ficar abaixo deste comentário e acima do void Setup
 Tenha cuidado com as modificações e criações de funções para o projeto.

 Lembre-se: Para uma função ser chamada sem dar erro, ela precisa ser declarada antes. 
*/

// Função que quando chamada, abre a tranca imediatamente e fecha após 3seg
void abrir_tranca(bool acionadoBotao = 0){
  digitalWrite(tranca, LOW);
  if(acionadoBotao == 0){
    mensagem(acessPermitido);
  }
  signalLed(ledR, 0, 0, 0, LOW);
  signalLed(ledG, 1, 3000, 0, LOW);

  
  digitalWrite(tranca, HIGH); 
  if(acionadoBotao == 0){
    mensagem(emLeitura);
  }
  signalLed(ledR, 0, 0, 0, HIGH); 
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
  int id = getNextAvailableID();
  if (id == -1) {
    Serial.println("Memoria cheia");
    lcd.clear(); 
    lcd.setCursor(0,0); 
    lcd.print("Memoria cheia");
    return 0;
  }

  // Indicando o ID que será feito o possível cadastro
  Serial.print("ID livre: ");
  Serial.println(id);
  lcd.clear(); 
  lcd.setCursor(0,0);
  lcd.print("ID livre:");
  lcd.print(id);


  // Início do processo de gravação de uma nova digital do usuário no dispositivo com autenticação em duas etapas
  lcd.setCursor(0,1);
  lcd.print("Coloque dedo 1 / 2");


  // Primeira etapa da autenticação 
  while (finger.getImage() != FINGERPRINT_OK); 
  if (finger.image2Tz(1) != FINGERPRINT_OK) {
    lcd.clear(); 
    lcd.print("Erro na leitura");
    return 0;
  }

  lcd.clear(); 
  lcd.print("Remova o dedo");
  delay(2000);


  // Segunda etapa da autenticação 
  lcd.clear(); 
  lcd.print("Coloque dedo 2 / 2");
  
  while (finger.getImage() != FINGERPRINT_OK);
  if(finger.image2Tz(2) != FINGERPRINT_OK) {
    lcd.clear(); 
    lcd.print("Erro na leitura");
    return 0;
  }
  if(finger.createModel() != FINGERPRINT_OK) {
    lcd.clear(); 
    lcd.print("Erro no modelo");
    return 0;
  }
  if(finger.storeModel(id) != FINGERPRINT_OK) {
    lcd.clear(); 
    lcd.print("Erro ao salvar");
    return 0;
  }
  
  // Fim do processo de gravação de digital e mensagem notificando o feito
  Serial.print("Digital cadastrada ID:");
  Serial.println(id);
  lcd.clear(); 
  lcd.print("Digital salva no ID:");
  lcd.setCursor(0,1);
  lcd.print(id);
  return 1;
}


// Função para apagar a digital lida no sensor que retorna um valor booleano
bool deleteFingerprintByScan() {
  Serial.println("Coloque o dedo p/ apagar");
  lcd.clear(); 
  lcd.print("Coloque o dedo para");
  lcd.setCursor(0,1);
  lcd.print("apagar");

  // Responsável por converter a digital lida no sensor para binário  
  while (finger.getImage() != FINGERPRINT_OK);
  if (finger.image2Tz() != FINGERPRINT_OK) {
    Serial.println("Erro na conversao");
    lcd.clear(); 
    lcd.print("Erro na leitura");
    return 0;
  }

  // Procura a digital
  if (finger.fingerSearch() != FINGERPRINT_OK) {
    mensagem(nEncontrada);
    delay(3000);
    return 0;
  }

  int id = finger.fingerID;
  Serial.print("Digital ID: ");
  Serial.println(finger.fingerID);

  // Verificando se a digital foi, de fato, apagada
  if (finger.deleteModel(id) == FINGERPRINT_OK) {
    Serial.println("Digital apagada");
    lcd.clear(); 
    lcd.print("Digital apagada");
    lcd.setCursor(0,1);
    lcd.print("ID:");
    lcd.print(id);
    return 1;
  } else {
    Serial.println("Erro ao apagar a digital");
    lcd.clear(); 
    lcd.print("Erro ao apagar");
    return 0;
  }
}

/*
 Função de retorno de número inteiro que é usado nos botões indicando o número necessário de vezes em que um determinado botão 
 precisa ser precionado (ou não) para uma operação ser considerada como válida ou inválida
*/
int verificarAcao(int confirmacoesNecessarias, int cancelamentosNecessarios, unsigned long timeoutMs = 7000) {
  int confirmCount = 0;
  int cancelCount = 0;
  unsigned long startTime = millis();

  // Sempre em loop até que haja um valor de retorno "return"
  while (true) {
    //cronômetro que vai de 0 à o número em milisec que está na variável "timeoutMs"
    if (millis() - startTime > timeoutMs) {
      // Timeout atingido
      Serial.println("Timeout na espera de acao");
      signalLed(ledR, 3, 200, 200, HIGH);
      return 0;  // Considera como cancelado
    }

    // Verifica contador do botão de confirmação 
    if (digitalRead(buttonADM_confirm) == LOW) {
      confirmCount++;
      Serial.print("Confirmações: ");
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
      Serial.print("Cancelamentos: ");
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

// Função responsável pelo modo de administrador - Gravar ou apagar UMA ou TODAS as digitais
void chaveADM_on(){
  while(digitalRead(chaveADM) == HIGH){
    // Indicando as variáveis de estado dos botões aos seus respectivos botões:
    buttonADM_recode_state = digitalRead(buttonADM_recode);
    buttonADM_delete_state = digitalRead(buttonADM_delete);
    buttonADM_cancel_state = digitalRead(buttonADM_cancel);
    buttonADM_confirm_state = digitalRead(buttonADM_confirm);

    // CADASTRAR UMA NOVA DIGITAL
    if(buttonADM_recode_state == LOW){
      Serial.println("Cadastrar digital");
      lcd.clear(); 
      lcd.print("Cadastrando");
      lcd.setCursor(0,1); 
      lcd.print("Confirma 1x");
      mensagem(mensg2xCancela, 1);

      int resultado = verificarAcao(1, 2);
      if (resultado == 1) {
        if(enrollFingerprint()){
          Serial.println("Digital cadastrada");
          lcd.clear();
          lcd.print("Digital cadastrada");
          mensagem(operExecutada);
          delay(3000);
          mensagem(emADM);
        } else {
          Serial.println("Falha no cadastro");
          mensagem(erro);
          delay(3000);
          mensagem(emADM);
        }
      } else {
        Serial.println("Cadastro cancelado");
        mensagem(operCancel);
        delay(3000);
        mensagem(emADM);
      }
    }

    // APAGAR UMA DIGITAL
    if(buttonADM_delete_state == LOW && buttonADM_confirm_state == HIGH){
      Serial.println("Apagar UMA digital");
      lcd.clear(); 
      lcd.print("Apagar 1 digital");
      lcd.setCursor(0,1); 
      lcd.print("Confirma 2x");
      mensagem(mensg2xCancela, 1);

      int resultado = verificarAcao(2, 2);

      if (resultado == 1) {
        if(deleteFingerprintByScan()){
          Serial.println("Digital apagada");
          lcd.clear();
          lcd.print("Digital apagada!");
          mensagem(operExecutada);
          delay(3000);
          mensagem(emADM);
        } else {
          Serial.println("Falha ao apagar");
          mensagem(erro);
          delay(3000);
          mensagem(emADM);
        }
      } else {
        Serial.println("Exclusão única cancelada");
        mensagem(operCancel);
        delay(3000);
        mensagem(emADM);
      }
    }

    // APAGAR TODAS AS DIGITAIS CADASTRADAS
    if(buttonADM_delete_state == LOW && buttonADM_confirm_state == LOW){
      Serial.println("Apagar TODAS as digitais");
      lcd.clear(); 
      lcd.print("Apagar TUDO");
      lcd.setCursor(0,1); 
      lcd.print("Confirme 3x");
      mensagem(mensg2xCancela, 1);

      int resultado = verificarAcao(3, 2);
      if (resultado == 1) {
        finger.emptyDatabase(); // Apaga todas as digitais cadastradas
        Serial.println("Todas apagadas");
        lcd.clear(); 
        lcd.print("Todas apagadas");
        mensagem(operExecutada);
        delay(3000);
        mensagem(emADM);
      } else {
        Serial.println("Exclusão total cancelada");
        mensagem(operCancel);
        delay(3000);
        mensagem(emADM);
      }
    }
    delay(50);
  }
  Serial.println("Saindo do modo ADM");
  delay(1000);
}


// Função responsável pelo modo normal - modo somente de leitura de digitais cadastradas
void chaveADM_off() {
  mensagem(emLeitura);
  while(digitalRead(chaveADM) == LOW){
    // Indicando as variáveis de estado dos botões aos seus respectivos botões:
    buttonADM_recode_state = digitalRead(buttonADM_recode);
    buttonADM_delete_state = digitalRead(buttonADM_delete);
    buttonADM_cancel_state = digitalRead(buttonADM_cancel);
    buttonADM_confirm_state = digitalRead(buttonADM_confirm);

    
    // Quando a porta estiver no modo de leitura, a porta pode ser aberta pelo botão de confirmar
    if (buttonADM_confirm_state == LOW || buttonADM_delete_state == LOW 
      || buttonADM_recode_state == LOW || buttonADM_cancel_state == LOW){
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
    abrir_tranca();
  }
}

// Função de inicialização do software
void setup() {
  pinMode(tranca, OUTPUT);
  trancaInit();
  
  Serial.begin(9600);
  finger.begin(57600);


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


  // Verificando se o sensor está funcionando e emitindo uma mensagem
  if(finger.verifyPassword()){
    mensagem(senPronto);
  }else{
    mensagem(erroSen);
  }
}

// Função de loop - apenas responsável pela troca dos modos
void loop(){
  chaveADM_state = digitalRead(chaveADM);
  if(chaveADM_state == HIGH){
    // Modo administrador
    digitalWrite(led_ADM, HIGH);
    mensagem(emADM);
    chaveADM_on(); 
  }else{
    // Modo leitura
    digitalWrite(led_ADM, LOW);
    mensagem(emLeitura);
    chaveADM_off(); 
  }
}
