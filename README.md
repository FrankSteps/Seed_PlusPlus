# Seed++

**Seed++** é um sistema automatizado de fechadura elétrica projetado com Arduino, utilizando um solenóide e um sensor biométrico para controle seguro de acesso. O projeto foi desenvolvido para a FnEsc.

O sistema permite o **CRUD de usuários**, ou seja, usuários podem ser registrados e removidos individualmente ou todos apagados de uma só vez.

Além de ser um projeto funcional, o objetivo deste dispositivo também é servir como **estudo de sistemas embarcados e de paradigmas de programação aplicados ao desenvolvimento de hardware**.

## Features

* Cadastrar usuários
* Remover um único usuário
* Remover todos os usuários
* Tela LCD e LEDs para feedback do usuário
* Modo administrador e modo leitura
* Barrar o acesso de pessoas não cadastradas
* Liberar o acesso de pessoas cadastradas

## Componentes

* Arduino Nano
* Sensor biométrico: Adafruit Fingerprint
* Solenoide
* Módulo regulador de tensão step-down
* Módulo relé
* LEDs e tela LCD com módulo de comunicação I2C
* Interruptor ON/OFF com chave de segurança
* Botões
* Fonte de 12V

## Como contribuir

O guia para futuros contribuidores está disponível em [CONTRIBUTING.md](CONTRIBUTING.md).

Esse guia tem como objetivo, junto com a documentação do projeto, auxiliar futuros desenvolvedores da FnEsc que desejem trabalhar ou dar continuidade ao sistema.

## Licença

Este projeto está licenciado sob a licença MIT. Para mais detalhes, consulte o arquivo [LICENSE](LICENSE).
