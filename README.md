# **Projeto de Otimização Logística com Grafos**

Este repositório contém a implementação de um projeto acadêmico desenvolvido para a disciplina **GCC262 – Grafos e suas Aplicações** da Universidade Federal de Lavras. O trabalho tem como objetivo desenvolver soluções para problemas logísticos utilizando modelagem por meio de grafos.

**Desenvolvido por:**

Leonardo Guimarães Oliveira

Matheus Felipe Godoi Coutinho

**Etapa 1: Pré-processamento dos dados \-** Onde são calculadas diversas métricas importantes que servirão de base para melhorias e soluções nas próximas etapas.

## **Conteúdo do Repositório**

* **main.cpp**  
   Implementação em C++ da etapa de pré-processamento que realiza:

  * Leitura dos dados a partir de um arquivo `.dat`;

  * Construção das estruturas do grafo (nós, arestas e arcos);

  * Cálculo de métricas sobre o grafo, tais como número de vértices, arestas, arcos, vértices e arestas requeridas, densidade, grau mínimo e máximo, intermediação, caminho médio e diâmetro;

  * Execução do algoritmo de Floyd–Warshall para o cálculo dos caminhos mínimos e matriz de predecessores.

* **data/**  
   Pasta contendo os arquivos de entrada (instâncias de teste) com dados no formato esperado pelo programa (ex.: `dados.dat`).

* **results/**  
   Pasta destinada à saída do programa. O arquivo `resultados.txt` é gerado com os resultados obtidos no pré-processamento.

* **Etapa-1\_visualização.ipynb**  
   Notebook utilizado para a visualização e análise dos resultados do algoritmo que serão salvos em um arquivo.txt. Este arquivo é independente da compilação em C++ e foi desenvolvido para facilitar a interpretação e experimentação dos dados e métricas obtidas através da visualização de gráficos e tabelas.

## **Objetivos da Etapa 1**

Nesta primeira fase do projeto, os objetivos principais foram:

* **Modelagem dos Dados:** Representar o problema logístico como um grafo onde os nós representam interseções e as arestas/arcos representam vias de acesso.

* **Leitura e Processamento:** Implementar um parser flexível para ler os dados de entrada, diferenciando seções com vértices, arestas, arcos e seus respectivos conjuntos requeridos.

* **Cálculo de Métricas:** Implementar funções para computar:

  * Número de vértices, arestas e arcos;

  * Número de vértices, arestas e arcos requeridos;

  * Densidade do grafo;

  * Grau mínimo e máximo dos vértices;

  * Intermediação dos nós com base nos caminhos mínimos;

  * Caminho médio e diâmetro do grafo.

* **Algoritmo de Caminhos Mínimos:** Uso do algoritmo de Floyd–Warshall para calcular a matriz de distâncias e de predecessores entre todos os pares de nós.  
    
* **Visualização dos dados:** Salvar os dados e permitir sua visualização no jupyter notebook.

## **Requisitos**

* **Compilador C++:** Recomenda-se o uso do `g++` (suporta C++11 ou superior).

* **Ambiente de Desenvolvimento:** Qualquer IDE ou editor de texto (por exemplo, Visual Studio Code, CLion, ou similar).

* **Python (opcional):** Para a execução do notebook Jupyter (`Etapa-1_visualização.ipynb`), é necessário ter o Jupyter Notebook instalado ou executar de forma online no navegador.  
* **Bibliotecas utilizadas para visualização:** Pandas e Matplotlib.

## **Compilação e Execução**

### **Compilando o Código C++**

Navegue até o diretório raiz do repositório e compile o programa utilizando o seguinte comando (exemplo para g++):

```
g++ -std=c++11 -O2 main.cpp -o grafo_logistica

```

### **Executando o Programa**

Após a compilação, execute o programa:

```
./grafo_logistica

```

O programa solicitará que o usuário informe o nome do arquivo `.dat` de entrada. Se nenhum nome for informado, o programa utiliza o arquivo padrão `data/dados.dat`.

Para utilizar outra base de dados diferente da padrão você deve colocá-la previamente dentro da pasta data/.

Durante a execução, as métricas serão calculadas e salvas no arquivo `results/resultados.txt`.

## **Utilização do Notebook de Visualização**

O arquivo `Etapa-1_visualização.ipynb` contém uma visualização interativa dos resultados obtidos na etapa 1\. Para utilizá-lo:

1. Abra o notebook utilizando o Jupyter Notebook:

```
jupyter notebook Etapa-1_visualizacao.ipynb

```

2.   
   Explore as células e visualize gráficos e tabelas que ilustram as métricas calculadas a partir dos dados processados.  
---

