%{
char *unidades[]={"primeiro","um","dois","tres","quatro","cinco",
               "seis", "sete","oito","nove"};
char *interm[]={"dez","onze","doze","treze",
                 "quatorze","quinze","dezesseis",
                 "dezessete","dezoito","dezenove"};
char *dezenas[]={"vinte","trinta","quarenta","cinquenta",
                 "sessenta","setenta","oitenta","noventa"};
char *meses[]={"janeiro","fevereiro","marco","abril","maio",
               "junho","julho","agosto","setembro",
               "outubro","novembro","dezembro"};

int dia, mes, ano;
%}

DIA	^[0-2][0-9][/]
MES	[0-1][0-9][/]
ANO	[0-9][0-9]$

%%

{DIA}	convert(&dia);
{MES}	convert(&mes);
{ANO}	{convert(&ano); imprime();} 
.	;

%%
convert(int *var){
	
	sscanf(yytext,"%d", &(*var));

}

imprime(){
	if(dia >0 && dia <= 31 && mes > 0 && mes < 13){

	if (dia == 1)
		printf("%s", unidades[0]);
	else if(dia/10 == 0)
		printf("%s", unidades[dia]);
	else if(dia/10 == 1)
		printf("%s", interm[dia%10]);
	else{
		printf(" %s", dezenas[dia/10 - 2]);
		if(dia%10 > 0){
			printf(" e %s", unidades[dia%10]);
		}
	}

	
	printf(" de %s de dois mil", meses[mes-1]);
	
	if(ano > 0)
	if(ano/10 == 0){
		printf(" e %s", unidades[ano]);
	}else if(ano/10 == 1)
		printf(" e %s", interm[ano%10]);
	else{
		printf(" %s", dezenas[ano/10 -2]);
		if(ano%10 > 0){
                        printf(" e %s", unidades[ano%10]);
                }
		
	}
	
	}else{
		printf("ERRO!");
	}
}

