# Embarcatech Comunicação

## Descrição
Este projeto implementa um software que em conjunto com a ferramenta de desenvolvimento BitDogLab faz com que leituras no computador sejam exibidas
no display da ferramenta utilizando UART e i2c

## Funcionalidades
- caracteres escritos no Serial Monitor serão vistos no Display da ferramente.
- Uso de Interrupção e Debounce para utilizar os botões da ferramenta.
- Uso de Pio para manipulação de uma matriz de leds 5x5.
- Uso de protocolos UART e i2c para comunicação do computador com a ferramenta 

## Estrutura do Projeto
```
pio_matrix/
├── .gitignore
├── CMakeLists.txt          # Configuração do build
├── diagram.json            # Esquema de conexões
├── OneShot.c              # Lógica principal do sistema
├── wokwi.toml              # Configuração da simulação no Wokwi
```

## Requisitos
- **Hardware**: Microcontrolador compatível (Raspberry Pi Pico), LEDs
- **Software**:
  - Ambiente de desenvolvimento C/C++
  - Simulador Wokwi (opcional)
  - CMake

## Como Compilar e Executar
1. Clone o repositório e acesse a pasta do projeto:
   ```sh
   git clone https://github.com/HiagoMCarvalho/EmbarcatechComunicacao.git
   cd pio_matrix
   ```
2. Configure e compile o projeto:
   ```sh
   mkdir build && cd build
   cmake ..
   make
   ```
3. Carregue o firmware no Raspberry Pi Pico copiando o arquivo `.uf2` para a unidade correspondente.

## Simulação no Wokwi
1. Acesse [Wokwi](https://wokwi.com/) e importe os arquivos `diagram.json` e `wokwi.toml`.
2. Inicie a simulação para visualizar o funcionamento do projeto sem precisar de hardware físico.

## Licença
Este projeto é distribuído sob a licença MIT. Consulte o arquivo `LICENSE` para mais detalhes.

## Video
Assista ao vídeo contendo a explicação: <https://drive.google.com/file/d/14CTu97s2ZZxbUF-SWXtwmjgAwx3TfLLy/view?usp=drive_link>

## Autor
Desenvolvido por <https://github.com/HiagoMCarvalho>.


