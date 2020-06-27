#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define SIZE 100
#define HISTORICO "Historico.csv"
#define tentativas 100

typedef struct {
  char mensagem[SIZE];
  char senha[SIZE];
  char arquivo[SIZE];
  char criptografado[SIZE];
  char checksum[SIZE];
  char descriptografar[SIZE];
  int tamchecksum;
  char operacao[SIZE];
}
dados;
dados db;

int checksum(char * p, int tamanho) {
  int total = 0;
	//Realiza a soma dos valores do checksum e retorna o inteiro no final
  for (int i = 0; i < tamanho; i++) {
    total += p[i];
  }
  return total;
}

int verificaNumero(int i) {
	//Verifica se o número é valido no alfábeto com letras maiusculas, se sim, retorna 1, se não 0
  if (i >= 65 && i <= 90) {
    return 1;
  }
  return 0;
}

int geraNumero() {
	//Declaração das variaveis 
  int numeroAleatorio = rand() % 91;
	//Cria um numero aleatorio entre 0 e 90
  while (!verificaNumero(numeroAleatorio)) {
		//Apos chama a função de verificaNumero, se retornar verdadeiro, sai do laço
    numeroAleatorio = rand() % 91;
  }
	//retorna o numero armazenado
  return numeroAleatorio;
}

void quebrarSenha() {
	//Declaração das variaveis
  FILE * fp;
  char password[SIZE];
  char * hash;
  char mensagem[SIZE];
  int tamanhoMensagem = 0;
	int j = 0;

	//Exige o input do usuario com o nome do arquivo
  printf("Informe o nome do arquivo: ");
  scanf("%[^\n]s", db.arquivo);

	//Verifica se o arquivo existe e é possível acessalo 
  fp = fopen(db.arquivo, "rb");
  if (!fp) {
    printf("Não foi possivel abrir o arquivo");
    exit(0);
  }

	//Lê a primeira linha do mesmo e o após fecha 
  fread(db.criptografado, 1, SIZE, fp);
  fclose(fp);

	//Faz o split da mensagem com o checksum e guarda dentro das variaveis
  strcpy(db.mensagem, strtok(db.criptografado, "><"));
  strcpy(password, strtok(NULL, "<"));

	//Tira o sinal de maior que esta ainda ao final do checksum 
  for (int i = 0; password[i] != '>'; i++) {
    if (password[i] == '>') {
      password[i] = '\0';
    }
  }

	//Converte o checksum para inteiro e mede o tamanho da mensagem criptografada
  db.tamchecksum = atoi(password);
  tamanhoMensagem = strlen(db.mensagem);

	//Realiza alocação de memória dinâmica para gerar senhas e quebrar
  hash = (char * ) malloc(tamanhoMensagem * sizeof(char));
	//Verifica se foi possível alocar memória
  if (!hash) {
    printf("Não foi possivel alocar memória");
  }

	//Enquanto j for falso, fica executando infinitamente, até que o a senha gerada seja igual ao valor do checksum
  while (!j) {

		//Cria a senha aleatória utilizando a função de gerar número
    int i = 0;
    while (i < tamanhoMensagem) {
      int numero = geraNumero();
      hash[i] = numero;
      i++;
    }

		//Como o hash é uma alocação de memória, ele não possui "tamanho", para isso é gravado os valores dentro da variavel checksum
    for (int i = 0; i < tamanhoMensagem; i++) {
      db.checksum[i] = hash[i];
    }

		//Chama a função do checksum passando o hash gerado aleatoriamente junto com o tamanho da mensagem
    int senha = checksum(db.checksum, tamanhoMensagem);

		//Se o hash gerado for igual ao checksum, ele realiza o xor entre a mensagem e o checksum e encerra o código
    if (senha == db.tamchecksum) {
      for (int i = 0; i < tamanhoMensagem; i++) {
        db.descriptografar[i] = db.mensagem[i] ^ db.checksum[i];
      }

      printf("Mensagem: %s\n", db.descriptografar);
      printf("Checksum: %d\n", senha);
      printf("Criptografada: %s\n\n", db.mensagem);
      j = 1;
    }

		//Se não, realiza o print das informações e volta ao inicio do laço
    if (senha != db.tamchecksum) {

      printf("Senha Incorreta!\n");
      printf("Nível de erro: %d\n", db.tamchecksum - senha);
      printf("Criptografada: %s\n\n", db.mensagem);

    }
  }
	//Esvazia a memória que foi alocada para gerar o hash
  free(hash);
}

void gravarHistorico(char operacao[SIZE], char nomearquivo[SIZE], char mensagem[SIZE], char password[SIZE]) {
	//Declaração das variaveis
  FILE * fp;
  fp = fopen(HISTORICO, "r+");
	//Tenta abrir o arquivo, caso contrário cria e grava a primeira linha;
  if (!fp) {
    fp = fopen(HISTORICO, "w");
    fprintf(fp, "%s;%s;%s;%s", operacao, nomearquivo, mensagem, password);
    fclose(fp);
    exit(0);
  }
	//Vai até o final do arquivo e grava o log dentro do arquivo
  fseek(fp, 0, SEEK_END);
  fprintf(fp, "\n");
  fprintf(fp, "%s;%s;%s;%s", operacao, nomearquivo, mensagem, password);
  fclose(fp);
}

void consultaHistorico() {
	//Declaração das variaveis e limpa o console
  system("clear");
  FILE * p;
  p = fopen(HISTORICO, "r+");
  char tmp[SIZE];

	//Tenta realizar a leitura do arquivo
  if (!p) {
    printf("Arquivo ainda não criado, favor verificar as operações...");
    exit(0);
  }

	//Se sim, faz um while até o final do arquivo com um split, printando as informações de cada linha e ao final fecha o arquivo
  printf("\nHistórico de Operações:\n");

  while (!feof(p)) {
    fgets(tmp, BUFSIZ, p);

    strcpy(db.operacao, strtok(tmp, ";"));
    strcpy(db.arquivo, strtok(NULL, ";"));
    strcpy(db.mensagem, strtok(NULL, ";"));
    strcpy(db.senha, strtok(NULL, ";"));

    printf("\nOperação: %s", db.operacao);
    printf("\nArquivo: %s", db.arquivo);
    printf("\nMensagem: %s", db.mensagem);
    printf("\nSenha: %s", db.senha);

  }

  fclose(p);

}

void descriptografar() {
	//Declaração das variáveis
  FILE * fp;
  char * p = db.checksum;
  char password[SIZE];
  int tamanhoMensagem = 0;
  int xor = 1;

	//Exige o input do usuário
  system("clear");
  printf("--- D E S C R I P T O G R A F I A --\n");
  printf("Informe o nome do arquivo: ");
  scanf("%[^\n]s", db.arquivo);
  getchar();
  printf("Informe a senha: ");
  scanf("%[^\n]s", db.senha);

	//Tenta abrir o arquivo, caso contrário realiza o print e para a execução do código
  fp = fopen(db.arquivo, "rb");
  if (!fp) {
    printf("Não foi possivel abrir o arquivo");
    exit(0);
  }

	//Lê a primeira linha e fecha o arquivo
  fread(db.criptografado, 1, SIZE, fp);
  fclose(fp);

	//Realiza o Split da mensagem e Checksum
  strcpy(db.mensagem, strtok(db.criptografado, "><"));
  strcpy(password, strtok(NULL, "<"));

	//Troca o sinal de maior no final por um \0
  for (int i = 0; password[i] != '>'; i++) {
    if (password[i] == '>') {
      password[i] = '\0';
    }
  }

	//Converte o valor de checksum e mede o tamanho da mensagem criptografada
  db.tamchecksum = atoi(password);
  tamanhoMensagem = strlen(db.mensagem);

	//Inverte a senha e monta o checksum
  for (int i = strlen(db.senha); i > 0; i--) {
    *p = toupper(db.senha[i - 1]);
    p++;
  }
  while (strlen(db.checksum) != tamanhoMensagem) {
    for (int i = strlen(db.senha); i > 0; i--) {
      *p = toupper(db.senha[i - 1] + xor);
      p++;
      if (strlen(db.checksum) == tamanhoMensagem) {
        break;
      }
    }
    xor++;
  }

	//Verifica se a senha esta "correta", baseado no valor do checksum
  int senha = checksum(db.checksum, tamanhoMensagem);
  if (senha != db.tamchecksum) {
    printf("Senha Incorreta!\n");
    printf("Nível de erro: %d", db.tamchecksum - senha);
    exit(0);
  }

	//Se estiver, realiza o xor do checksum com a mensagem criptografada
  for (int i = 0; i < tamanhoMensagem; i++) {
    db.descriptografar[i] = db.mensagem[i] ^ db.checksum[i];
  }

	//Faz o print das informações
  printf("Mensagem: %s\n", db.mensagem);
  printf("Password: %d\n", db.tamchecksum);
  printf("Tamanho Mensagem: %d\n", tamanhoMensagem);
  printf("Checksum: %s\n", db.checksum);
  printf("Mensagem Descriptografada: %s\n", db.descriptografar);

	//Grava o log da função descriptografar
  gravarHistorico("DESCRIPTOGRAFAR", db.arquivo, db.descriptografar, db.senha);
}

void criptografar() {
		//Declaração das variáveis
    FILE * fp;
    char * p = db.checksum;
    int xor = 1;

		//Limpa o console e pede o input do usuário
    system("cls");
    printf("--- C R I P T O G R A F I A --\n");
    printf("Informe a mensagem a ser criptografada: ");
    scanf("%[^\n]s", db.mensagem);
    getchar();
    printf("Informe o nome do arquivo: ");
    scanf("%[^\n]s", db.arquivo);
    fp = fopen(db.arquivo, "wb");
    getchar();
    printf("Informe a senha: ");
    scanf("%[^\n]s", db.senha);

		//Inverte a senha utilizando um ponteiro
    for (int i = strlen(db.senha); i > 0; i--) {
      *p = toupper(db.senha[i - 1]);
      p++;
    }

		//Constroi o checksum da senha, utilizando como base o tamanho da frase
    while (strlen(db.checksum) < strlen(db.mensagem)) {
      for (int i = strlen(db.senha); i > 0; i--) {
        * p = toupper(db.senha[i - 1] + xor);
        p++;
        if (strlen(db.checksum) == strlen(db.mensagem)) {
          break;
        }
      }
      xor++;
    }

		//Realiza o XOR entre a mensagem e o checksum
    for (int i = 0; i < strlen(db.mensagem); i++) {
      db.criptografado[i] = db.mensagem[i] ^ db.checksum[i];
    }

		//Chama a função para realizar a soma dos caracteres do checksum
    db.tamchecksum = checksum(db.checksum, strlen(db.checksum));

		//Printa as informações do Checksum
    printf("Checksum: %s\n", db.checksum);
    printf("Checksum INT: %d\n", db.tamchecksum);

		//Realiza a gravação no arquivo utilizando a estrutura do arquivo
    fprintf(fp, "<");
    fwrite(db.criptografado, 1, strlen(db.mensagem), fp);
    fprintf(fp, ">");
    fprintf(fp, "<%d>\n", db.tamchecksum);

	fclose(fp);

	//Chama a função gravar histórico, que escreve os "logs" do sistema
	gravarHistorico("CRIPTOGRAFIA", db.arquivo, db.mensagem, db.senha);
}

int main(int argc, char *argv[] ) {
 if (argc != 2){
		printf("Número de argumentos inválido");
		exit(0);
	}
	
	if(strcmp(argv[1], "-c") == 0){
		criptografar();
		return 0;
	}

	if(strcmp(argv[1], "-d") == 0){
		descriptografar();
		return 0;
	}

	if(strcmp(argv[1], "-q") == 0){
		quebrarSenha();
		return 0;
	}

	if(strcmp(argv[1], "-1") == 0){
		consultaHistorico();
		return 0;
	}

	else{
		printf("Opção inválida selecionada");
		return 0;
	}
	
 return 0;
}