all: custom_parser.y custom_parser.l servidor.c
	bison -d custom_parser.y
	flex custom_parser.l
	gcc -o servidor servidor.c custom_parser.tab.c lex.yy.c -ly -ll

port = 8889

run: 
	./servidor $(port) ../meu-webspace/ ../registro.txt
