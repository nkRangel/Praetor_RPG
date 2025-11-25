/*
 PROJETO PRATICO - LABORATORIO DE PROGRAMACAO ESTRUTURADA
 Aluno: Pedro Henrique Leite Rangel
 Tema: Sistema de Gerenciamento de Personagens (RPG)
 */

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>

 /* Definicao da Struct (Registro) */
typedef struct {
    char nome[50];
    char classe[30];    /* Ex: Guerreiro, Mago, Paladino */
    int nivel;
    float experiencia;  /* Campo float exigido */
    char status;        /* Controle de exclusao logica (' ' = Ativo, '*' = Removido) */
} Ficha_RPG;

/* Prototipos das funcoes (Modularizacao) */
void limpaBuffer(void);
void lerTexto(char* buffer, int tamanho);
int tamanho(FILE* banco);
void cadastrar(FILE* banco);
void consultar(FILE* banco);
void excluir(FILE* banco); /* Nova funcao adicionada */
void gerarRelatorio(FILE* banco);

/* Funcao utilitaria para limpeza do buffer de entrada (stdin). */
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

    pos_original = ftell(banco);    /* Salva posicao atual */

    fseek(banco, 0, SEEK_END);
    total_bytes = ftell(banco);

    fseek(banco, pos_original, SEEK_SET); /* Restaura o cursor */

    return (int)(total_bytes / sizeof(Ficha_RPG));
}

/* Funcao de Cadastro (Create) */
void cadastrar(FILE* banco) {
    Ficha_RPG ficha;
    char confirmacao;

    ficha.status = ' '; /* Inicializa como ativo */

    printf("\n--- [NOVO REGISTRO] ---\n");
    printf("ID do Sistema: %d\n", tamanho(banco) + 1);

    limpaBuffer();

    printf("Nome do Personagem: ");
    lerTexto(ficha.nome, sizeof(ficha.nome));

    printf("Classe (ex: Bardo, Clerigo): ");
    lerTexto(ficha.classe, sizeof(ficha.classe));

    printf("Nivel (Inteiro): ");
    if (scanf("%d", &ficha.nivel) != 1) {
        printf("Erro: O nivel deve ser numerico.\n");
        return;
    }

    printf("Experiencia/XP (Decimal): ");
    if (scanf("%f", &ficha.experiencia) != 1) {
        printf("Erro: Valor invalido para experiencia.\n");
        return;
    }

    limpaBuffer();

    printf("Confirmar gravacao? (S/N): ");
    if (scanf("%c", &confirmacao) != 1) return;

    if (toupper(confirmacao) == 'S') {
        fseek(banco, 0, SEEK_END);
        fwrite(&ficha, sizeof(Ficha_RPG), 1, banco);
        fflush(banco);
        printf("Sucesso: Personagem registrado.\n");
    }
    else {
        printf("Operacao cancelada.\n");
    }
}

/* Funcao de Consulta (Read) */
void consultar(FILE* banco) {
    int id_busca, total_regs;
    long offset_bytes;
    Ficha_RPG ficha_lida;

    printf("\nInforme o ID para busca: ");
    if (scanf("%d", &id_busca) != 1) {
        limpaBuffer();
        printf("Entrada invalida.\n");
        return;
    }

    total_regs = tamanho(banco);

    if (id_busca <= 0 || id_busca > total_regs) {
        printf("ID %d nao encontrado. Total de registros: %d\n", id_busca, total_regs);
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
        printf("!!! ESTE REGISTRO ESTA EXCLUIDO !!!\n");
    }

    printf("Nome:   %s\n", ficha_lida.nome);
    printf("Classe: %s\n", ficha_lida.classe);
    printf("Nivel:  %d\n", ficha_lida.nivel);
    printf("XP:     %.2f\n", ficha_lida.experiencia);
    printf("-------------------------------\n");
}

/* Funcao de Exclusao Logica (Update Status) */
void excluir(FILE* banco) {
    int id_busca, total_regs;
    long offset_bytes;
    Ficha_RPG ficha;
    char confirmacao;

    printf("\n--- [EXCLUIR PERSONAGEM] ---\n");
    printf("Informe o ID para excluir: ");
    if (scanf("%d", &id_busca) != 1) {
        limpaBuffer();
        printf("Entrada invalida.\n");
        return;
    }

    total_regs = tamanho(banco);

    if (id_busca <= 0 || id_busca > total_regs) {
        printf("ID %d nao encontrado.\n", id_busca);
        return;
    }

    /* Calcula a posicao e le o registro */
    offset_bytes = (long)(id_busca - 1) * sizeof(Ficha_RPG);
    fseek(banco, offset_bytes, SEEK_SET);
    fread(&ficha, sizeof(Ficha_RPG), 1, banco);

    /* Verifica se ja esta excluido */
    if (ficha.status == '*') {
        printf("Erro: O registro %d ja se encontra excluido.\n", id_busca);
        return;
    }

    /* Mostra dados para confirmacao de seguranca */
    printf("\nVoce selecionou: %s (Nvl %d - %s)\n", ficha.nome, ficha.nivel, ficha.classe);
    printf("Tem certeza que deseja EXCLUIR este personagem? (S/N): ");
    
    limpaBuffer();
    scanf("%c", &confirmacao);

    if (toupper(confirmacao) == 'S') {
        ficha.status = '*'; /* Marca como excluido */
        
        /* Volta o cursor para a posicao correta e sobrescreve */
        fseek(banco, offset_bytes, SEEK_SET);
        fwrite(&ficha, sizeof(Ficha_RPG), 1, banco);
        fflush(banco); /* Garante a gravacao imediata */
        
        printf("Registro excluido com sucesso.\n");
    } else {
        printf("Exclusao cancelada.\n");
    }
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
        printf("Erro ao criar arquivo de relatorio.\n");
        return;
    }

    printf("\nGerando relatorio...\n");

    /* Cabecalho do arquivo TXT */
    fprintf(relatorio, "=================================================================\n");
    fprintf(relatorio, "                    RELATORIO GERAL DE FICHAS                    \n");
    fprintf(relatorio, "=================================================================\n");
    fprintf(relatorio, "ID  %-20s %-15s %-6s %-10s %s\n", "NOME", "CLASSE", "NIVEL", "XP", "STATUS");
    fprintf(relatorio, "-----------------------------------------------------------------\n");

    /* Volta para o inicio do arquivo binario para ler tudo */
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
    
    /* Restaura cursor para o final (boa pratica) */
    fseek(banco, 0, SEEK_END);

    printf("Sucesso! Arquivo 'relatorio_rpg.txt' criado na pasta do projeto.\n");
}

/* Funcao Principal */
int main(void) {
    FILE* banco_dados;
    int opcao = 0;

    /* Acentos removidos, setlocale pode ser opcional agora, mas mantido por seguranca */
    setlocale(LC_ALL, "Portuguese"); 

    /* Abre arquivo binario (leitura/escrita) */
    banco_dados = fopen("rpg_database.dat", "r+b");
    if (banco_dados == NULL) {
        banco_dados = fopen("rpg_database.dat", "w+b");
        if (banco_dados == NULL) {
            printf("Erro Critico: Falha ao criar arquivo de banco de dados.\n");
            return 1;
        }
    }

    /* Menu atualizado com a opcao 4 */
    while (opcao != 5) { 
        printf("\n=== GERENCIADOR DE FICHAS (RPG) ===\n");
        printf("1. Cadastrar Personagem\n");
        printf("2. Consultar Personagem\n");
        printf("3. Gerar Relatorio (.txt)\n"); 
        printf("4. Excluir Personagem\n");
        printf("5. Sair\n");
        printf("Registros no banco: %d\n", tamanho(banco_dados));
        printf("Selecione: ");

        if (scanf("%d", &opcao) != 1) {
            limpaBuffer();
            printf("Opcao invalida. Digite um numero.\n");
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
            excluir(banco_dados);
            break;
        case 5:
            printf("Encerrando aplicacao...\n");
            break;
        default:
            printf("Opcao desconhecida.\n");
        }
    }

    fclose(banco_dados);
    return 0;
}
