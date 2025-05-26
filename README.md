
# Projeto de Otimização Logística com Grafos

Este repositório contém a implementação de um projeto acadêmico desenvolvido para a disciplina **GCC262 – Grafos e suas Aplicações** da Universidade Federal de Lavras. O trabalho tem como objetivo desenvolver soluções para problemas logísticos utilizando modelagem por meio de grafos.

**Desenvolvido por:**  
Leonardo Guimarães Oliveira  
Matheus Felipe Godoi Coutinho

---

## Etapa 1: Pré-processamento dos dados  
Onde são calculadas diversas métricas importantes que servirão de base para melhorias e soluções nas próximas etapas.

### Conteúdo do Repositório
- **main.cpp**  
  Implementação em C++ da etapa de pré-processamento que realiza:  
  - Leitura dos dados a partir de um arquivo `.dat`;  
  - Construção das estruturas do grafo (nós, arestas e arcos);  
  - Cálculo de métricas:  
    - Número de vértices, arestas, arcos  
    - Número de vértices, arestas e arcos requeridos  
    - Densidade, grau mínimo e máximo  
    - Intermediação, caminho médio e diâmetro  
  - Execução de Floyd–Warshall para caminhos mínimos e matriz de predecessores.

- **data/**  
  Arquivos de entrada (`*.dat`).

- **results/**  
  Saída de métricas gerada por `main.cpp`.

- **Etapa-1_visualização.ipynb**  
  Notebook Jupyter para explorar graficamente as métricas.

---

## Etapa 2: Solução Inicial  
Na Fase 2, implementamos um **algoritmo construtivo** que, partindo de rota vazias, vai construindo uma solução válida para o problema de atendimento de serviços (vértices, arestas e arcos requeridos) sob restrição de capacidade dos veículos.  

### Requisitos da Solução Inicial  
- **Capacidade**: cada rota não ultrapassa a capacidade máxima do veículo.  
- **Cobertura**: cada serviço (vértice, aresta ou arco requerido) é atendido por **exatamente uma** rota.  
- **Contagem**: se uma rota passa mais de uma vez por um mesmo vértice, aresta ou arco requerido, deve-se contar **apenas uma vez** tanto a demanda quanto o custo desse serviço.  

### Formato de Saída  
Para cada instância `<nome_instancia>.dat`, é gerado um arquivo `solutions/sol-<nome_instancia>.dat` contendo no início:

```

<custo total>
<# rotas>
<clock_ticks da primeira chamada a gerarRotas()>
<média de clock_ticks das demais chamadas>
```

E, em seguida, uma linha por rota no padrão:

```
0 1 <id_rota> <demanda_total> <custo_rota> <#visitas> (D 0,1,1) … (S <serviço_id>,<u>,<v>) …
```

### Automação e Processamento em Lote

O `main.cpp` foi estendido para:

1. Criar as pastas `results/` e `solutions/` se não existirem.
2. Listar **todas** as instâncias `*.dat` em `data/`.
3. Para cada arquivo:

   * Executar Etapa 1 e salvar métricas em `results/metrics-<base>.txt`.
   * Executar o construtivo da Etapa 2 e salvar em `solutions/sol-<base>.dat`.


---


---

## Requisitos

* **Compilador C++17** (`g++` recomendado).
* **Python 3** + **Jupyter Notebook** (para o notebook).
* Bibliotecas Python: **pandas**, **matplotlib**.

---

## Compilação e Execução

```bash
g++ -std=c++17 -O2 main.cpp -o grafo_logistica
./grafo_logistica
```

* Gera `results/metrics-*.txt` e `solutions/sol-*.dat`.

---

## Visualização no Notebook

```bash
jupyter notebook Etapa-1_visualizacao.ipynb
```

Explore gráficos e tabelas das métricas calculadas na Etapa 1.
