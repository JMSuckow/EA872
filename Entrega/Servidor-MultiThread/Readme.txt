== Atividade 10 ==

Códigos desenvolvidos para a disciplina EA872 da Universidade Estadual de Campinas

Nome: João Marcos Suckow de Barros Rodrigues RA 136253 
Servidor Versão 0.4  

== Conteúdo ==

- Diretório sources: código fonte do servidor incluindo:
	-> servidor.c: 		Programa principal
	-> custom_parser.h: Arquivo Header para o parser
	-> custom_parser.l:	Analisador léxico do parser
	-> custom_parser.y: Analisador semântico do parser

- Readme.txt: Este arquivo

- Makefile: auxílio na instalação


== Método de Instalação ==

Estando no diretório raiz, execute os seguintes passos para instalar:

1- make install
2- make (se estiver em ambiente OSX) ou make linux (se estiver em ambiente linux)

Estes comandos criarão um diretório na raiz que será o servidor web além de um diretório bin contendo o executável

== Método de Execução do programa ==

É possível executar o programa da seguinte maneira:

./bin/servidor <Numero da Porta de comunicação> webspace/ <Nome do arquivo para registro> <Número de threads máximo>

Ou simplesmente executar

make run (Este comando utiliza port=9000 e n=2 como padrão)

É possível alterar o padrão executando:
make run port=<Numero_Customizado> n=<Numero_Customizado>
make run n=<Numero_Customizado>
make run port=<Numero_Customizado>

