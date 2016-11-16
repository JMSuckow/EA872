all: custom_parser.y custom_parser.l servidor.c
	bison -d custom_parser.y
	flex custom_parser.l
	gcc -o servidor servidor.c custom_parser.tab.c lex.yy.c -ly -ll

linux: custom_parser.y custom_parser.l servidor.c
	bison -d custom_parser.y
	flex custom_parser.l
	gcc -o servidor servidor.c custom_parser.tab.c lex.yy.c -ly -lfl

install: 
	mkdir meu-webspace2
	mkdir meu-webspace2/dir1
	mkdir meu-webspace2/dir2
	mkdir meu-webspace2/dir1/dir11
	echo "<html><head><title>Index</title></head><body>Index</body></html>" > meu-webspace2/index.html
	echo "<html><head><title>Texto1</title></head><body>Texto1</body></html>" > meu-webspace2/dir1/texto1.html
	echo "<html><head><title>Texto2</title></head><body>Texto2</body></html>" > meu-webspace2/dir1/texto2.html
	echo "<html><head><title>Welcome</title></head><body>Welcome</body></html>" > meu-webspace2/dir1/dir11/welcome.html
	chmod a-x meu-webspace2/dir2
	chmod a-r meu-webspace2/dir1/texto2.html

port = 8889
n = 2

run: 
	./servidor $(port) ../meu-webspace/ ../registro.txt $(n)
