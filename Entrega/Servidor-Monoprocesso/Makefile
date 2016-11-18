all: source/custom_parser.y source/custom_parser.l source/servidor.c
	bison -d source/custom_parser.y -o source/custom_parser.tab.c
	flex -o source/lex.yy.c source/custom_parser.l 
	gcc -o bin/servidor source/servidor.c source/custom_parser.tab.c source/lex.yy.c -ly -ll

linux: source/custom_parser.y source/custom_parser.l source/servidor.c
	bison -d source/custom_parser.y -o source/custom_parser.tab.c
	flex -o source/lex.yy.c source/custom_parser.l 
	gcc -o bin/servidor source/servidor.c source/custom_parser.tab.c source/lex.yy.c -ly -lfl

clean:
	rm -rf bin 
	rm source/lex.yy.c
	rm source/custom_parser.tab.c
	rm source/custom_parser.tab.h

clean-all:
	make clean
	rm -rf webspace
	rm registro.txt

install: 
	mkdir bin
	mkdir webspace
	mkdir webspace/dir1
	mkdir webspace/dir2
	mkdir webspace/dir1/dir11
	echo "<html><head><title>Index</title></head><body>Index</body></html>" > webspace/index.html
	echo "<html><head><title>Texto1</title></head><body>Texto1</body></html>" > webspace/dir1/texto1.html
	echo "<html><head><title>Texto2</title></head><body>Texto2</body></html>" > webspace/dir1/texto2.html
	echo "<html><head><title>Welcome</title></head><body>Welcome</body></html>" > webspace/dir1/dir11/welcome.html
	chmod a-x webspace/dir2
	chmod a-r webspace/dir1/texto2.html

port = 8888

run: 
	./bin/servidor $(port) webspace/ registro.txt
