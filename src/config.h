#ifndef CONFIG_H
#define CONFIG_H

// RFID via protocolo de comunicação SPI
#define RFID_RST_PIN  9
#define RFID_SDA_PIN  10
#define RFID_MOSI_PIN 11
#define RFID_MISO_PIN 12
#define RFID_SCK_PIN  13


// Para os botões multiplexados
#define BUTTONS_PIN A3


// Para feedback do usuário pela tela LCD via protocolo de comunicação I2C
#define LCD_SDA_PIN A4
#define LCD_SCL_PIN A5
#define LCD_ADDRESS 0x27


// Para feedback do usuário por LEDs
#define LED_ERROR 3
#define LED_SUCSS 4
#define LED_ADMIN 5


// Para controle de acesso
#define LOCK_PIN    2
#define ADM_KEY_PIN 6


// Valores analógicos dos botões (calibre conforme necessário)
#define RECODE_MIN  800
#define RECODE_MAX  1023

#define DELETE_MIN  600
#define DELETE_MAX  800

#define CANCEL_MIN  300
#define CANCEL_MAX  600

#define CONFIRM_MIN 0
#define CONFIRM_MAX 300


#endif