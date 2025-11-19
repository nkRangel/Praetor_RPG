# ⚔️ PraetorRPG: Sistema de Gerenciamento de Fichas

> "Persistência de dados e estratégia."

## Sobre o Projeto
Este repositório contém a implementação do projeto final da disciplina de **Laboratório de Programação Estruturada**. O objetivo foi desenvolver um sistema de gerenciamento de dados (CRUD) em **C Puro**, focado na manipulação de arquivos binários e alocação estática.

O sistema simula um "Grimório" ou banco de dados de personagens de RPG, permitindo cadastrar, consultar e exportar dados de forma persistente.

## Funcionalidades Implementadas
O código fonte (`.c`) atende aos seguintes requisitos técnicos:
* **Persistência Binária:** Uso de `fwrite`/`fread` para salvar structs complexas em disco (`.dat`).
* **Acesso Direto:** Utilização de `fseek` para consulta rápida de registros por ID.
* **Relatórios em Texto:** Exporta todo o banco de dados para um arquivo legível (`relatorio_rpg.txt`).
* **Sanitização de Dados:** Funções para limpeza de buffer e leitura segura de strings.

---

## Como Compilar e Executar

### Opção 1: Dev C++ (Recomendado)
1. Abra o arquivo `PraetorRPG.c` no Dev C++.
2. Pressione `F11` (Compile & Run).

### Opção 2: GCC (Linux/Terminal)
```bash
gcc PraetorRPG.c -o sistema_rpg
./sistema_rpg
