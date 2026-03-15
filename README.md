# Jogo "Cidade do Amor"

Este é um jogo desenvolvido por Rubens Takiguti Ribeiro entre 14 e 15 de Março de 2026 a partir das idéias de sua filha Luana, de 7 anos. Este projeto é de propósito pessoal/lúdico.

# Características

Jogo em C com biblioteca Allegro 5.

Baixar dependências para compilação:

```sh
sudo apt install liballegro5-dev liballegro-audio5-dev liballegro-acodec5-dev liballegro-image5-dev
```

Compilação no Linux:

```sh
gcc *.c -o game $(pkg-config --cflags --libs allegro-5 allegro_primitives-5 allegro_main-5 allegro_audio-5 allegro_acodec-5 allegro_image-5 allegro_font-5)
```

Caso seja passado o parâmetro `-DDEBUG`, o jogo é compilado com modo debug.
