/*
 PROJETO PRÁTICO - LABORATÓRIO DE PROGRAMAÇÃO ESTRUTURADA
 Aluno: Pedro Henrique Leite Rangel
 Tema: Sistema de Gerenciamento de Personagens (RPG)
 */

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>

 /* Definição da Struct (Registro) */
typedef struct {
    char nome[50];
    char classe[30];    /* Ex: Guerreiro, Mago, Paladino */
    int nivel;
    float experiencia;  /* Campo float exigido */
    char status;        /* Controle de exclusão lógica (' ' = Ativo, '*' = Removido) */
} Ficha_RPG;

/* Protótipos das funções (Modularização) */
void limpaBuffer(void);
void lerTexto(char* buffer, int tamanho);
int tamanho(FILE* banco);
void cadastrar(FILE* banco);
void consultar(FILE* banco);
void gerarRelatorio(FILE* banco);

/* Função utilitária para limpeza do buffer de entrada (stdin). */
void limpaBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* Wrapper */
void lerTexto(char* buffer, int tamanho) {
    fgets(buffer, tamanho, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
}

/* Calcula o total de registros no arquivo. */
int tamanho(FILE* banco) {
    long pos_original, total_bytes;

    pos_original = ftell(banco);    /* Salva posição atual */

    fseek(banco, 0, SEEK_END);
    total_bytes = ftell(banco);

    fseek(banco, pos_original, SEEK_SET); /* Restaura o cursor */

    return (int)(total_bytes / sizeof(Ficha_RPG));
}

/* Função de Cadastro (Create) */
void cadastrar(FILE* banco) {
    Ficha_RPG ficha;
    char confirmacao;

    ficha.status = ' '; /* Inicializa como ativo */

    printf("\n--- [NOVO REGISTRO] ---\n");
    printf("ID do Sistema: %d\n", tamanho(banco) + 1);

    limpaBuffer();

    printf("Nome do Personagem: ");
    lerTexto(ficha.nome, sizeof(ficha.nome));

    printf("Classe (ex: Bardo, Clérigo): ");
    lerTexto(ficha.classe, sizeof(ficha.classe));

    printf("Nível (Inteiro): ");
    if (scanf("%d", &ficha.nivel) != 1) {
        printf("Erro: O nível deve ser numérico.\n");
        return;
    }

    printf("Experiência/XP (Decimal): ");
    if (scanf("%f", &ficha.experiencia) != 1) {
        printf("Erro: Valor inválido para experiência.\n");
        return;
    }

    limpaBuffer();

    printf("Confirmar gravação? (S/N): ");
    if (scanf("%c", &confirmacao) != 1) return;

    if (toupper(confirmacao) == 'S') {
        fseek(banco, 0, SEEK_END);
        fwrite(&ficha, sizeof(Ficha_RPG), 1, banco);
        fflush(banco);
        printf("Sucesso: Personagem registrado.\n");
    }
    else {
        printf("Operação cancelada.\n");
    }
}

/* Função de Consulta (Read) */
void consultar(FILE* banco) {
    int id_busca, total_regs;
    long offset_bytes;
    Ficha_RPG ficha_lida;

    printf("\nInforme o ID para busca: ");
    if (scanf("%d", &id_busca) != 1) {
        limpaBuffer();
        printf("Entrada inválida.\n");
        return;
    }

    total_regs = tamanho(banco);

    if (id_busca <= 0 || id_busca > total_regs) {
        printf("ID %d não encontrado. Total de registros: %d\n", id_busca, total_regs);
        return;
    }

    offset_bytes = (long)(id_busca - 1) * sizeof(Ficha_RPG);

    fseek(banco, offset_bytes, SEEK_SET);

    if (fread(&ficha_lida, sizeof(Ficha_RPG), 1, banco) != 1) {
        printf("Erro de leitura no disco.\n");
        return;
    }

    printf("\n--- [DETALHES DO REGISTRO %d] ---\n", id_busca);

    if (ficha_lida.status == '*') {
        printf("[AVISO: ESTE REGISTRO ESTÁ EXCLUÍDO]\n");
    }

    printf("Nome:   %s\n", ficha_lida.nome);
    printf("Classe: %s\n", ficha_lida.classe);
    printf("Nível:  %d\n", ficha_lida.nivel);
    printf("XP:     %.2f\n", ficha_lida.experiencia);
    printf("-------------------------------\n");
}

void gerarRelatorio(FILE* banco) {
    FILE* relatorio;
    Ficha_RPG ficha;
    int total = tamanho(banco);
    int i;
    char status_str[15];

    /* Abre um arquivo de texto para escrita */
    relatorio = fopen("relatorio_rpg.txt", "w");
    if (relatorio == NULL) {
        printf("Erro ao criar arquivo de relatório.\n");
        return;
    }

    printf("\nGerando relatório...\n");

    /* Cabeçalho do arquivo TXT */
    fprintf(relatorio, "=================================================================\n");
    fprintf(relatorio, "                    RELATORIO GERAL DE FICHAS                    \n");
    fprintf(relatorio, "=================================================================\n");
    fprintf(relatorio, "ID  %-20s %-15s %-6s %-10s %s\n", "NOME", "CLASSE", "NIVEL", "XP", "STATUS");
    fprintf(relatorio, "-----------------------------------------------------------------\n");

    /* Volta para o início do arquivo binário para ler tudo */
    fseek(banco, 0, SEEK_SET);

    /* Loop para ler registro por registro */
    for (i = 0; i < total; i++) {
        fread(&ficha, sizeof(Ficha_RPG), 1, banco);

        /* Define o texto do status */
        if (ficha.status == '*')
            strcpy(status_str, "[EXCLUIDO]");
        else
            strcpy(status_str, "Ativo");

        /* Escreve formatado no arquivo TXT */
        fprintf(relatorio, "%03d %-20s %-15s %-6d %-10.2f %s\n",
            i + 1,              /* ID */
            ficha.nome,
            ficha.classe,
            ficha.nivel,
            ficha.experiencia,
            status_str);
    }

    fprintf(relatorio, "=================================================================\n");
    fprintf(relatorio, "Total de registros processados: %d\n", total);

    fclose(relatorio); /* Fecha o arquivo TXT */
    
    fseek(banco, 0, SEEK_END);

    printf("Sucesso! Arquivo 'relatorio_rpg.txt' criado na pasta do projeto.\n");
}

/* Função Principal */
int main(void) {
    FILE* banco_dados;
    int opcao = 0;

    setlocale(LC_ALL, "Portuguese");

    /* Abre arquivo binário (leitura/escrita) */
    banco_dados = fopen("rpg_database.dat", "r+b");
    if (banco_dados == NULL) {
        banco_dados = fopen("rpg_database.dat", "w+b");
        if (banco_dados == NULL) {
            printf("Erro Crítico: Falha ao criar arquivo de banco de dados.\n");
            return 1;
        }
    }

    while (opcao != 4) { 
        printf("\n=== GERENCIADOR DE FICHAS (RPG) ===\n");
        printf("1. Cadastrar Personagem\n");
        printf("2. Consultar Personagem\n");
        printf("3. Gerar Relatorio (.txt)\n"); 
        printf("4. Sair\n");
        printf("Registros no banco: %d\n", tamanho(banco_dados));
        printf("Selecione: ");

        if (scanf("%d", &opcao) != 1) {
            limpaBuffer();
            printf("Opção inválida. Digite um número.\n");
            continue;
        }

        switch (opcao) {
        case 1:
            cadastrar(banco_dados);
            break;
        case 2:
            consultar(banco_dados);
            break;
        case 3:
            gerarRelatorio(banco_dados); 
            break;
        case 4:
            printf("Encerrando aplicação...\n");
            break;
        default:
            printf("Opção desconhecida.\n");
        }
    }

    fclose(banco_dados);
    return 0;
}
