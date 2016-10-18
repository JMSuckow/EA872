

COMENTARIO         ^[#]+[^!\n]*[\n]
VAZIO		[\n\t ]+
%%


{COMENTARIO}({VAZIO})?   ;
.              ECHO;

%%

