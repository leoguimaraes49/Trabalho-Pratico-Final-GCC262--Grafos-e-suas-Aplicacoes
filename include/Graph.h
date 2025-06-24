#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <string>
#include <tuple>
#include <map>

class Graph
{
private:
    int total_nos;
    std::vector<std::string> nos;

    std::vector<std::vector<int>> distancias, predecessores, lista_adj;
    std::vector<std::tuple<int, int, int>> arestas, arcos;
    std::vector<std::pair<int, int>> arestas_requeridas, arcos_requeridas;
    std::vector<int> nos_requeridos;

    struct Metricas
    {
        int qtd_vertices;
        int qtd_arestas;
        int qtd_arcos;
        int vertices_req;
        int arestas_req;
        int arcos_req;
        float densidade;
        int grau_min;
        int grau_max;
        std::map<std::string, double> intermediacao;
        double caminho_medio;
        int diametro;
    } metricas;

    void floydWarshall();
    void inicializarMatrizes();
    void calcularGraus();
    void calcularIntermediacao();
    float calcularDensidade();
    double calcularCaminhoMedio();
    int calcularDiametro();

public:
    Graph(const std::vector<std::string> &nos,
          const std::vector<std::tuple<int, int, int>> &arestas,
          const std::vector<std::tuple<int, int, int>> &arcos,
          const std::vector<std::pair<int, int>> &arestas_requeridas,
          const std::vector<std::pair<int, int>> &arcos_requeridas,
          const std::vector<int> &nos_requeridos);

    void calcularTodasMetricas();

    int getTotalNos() const;
    const std::vector<std::vector<int>>& getDistancias() const;
    const std::vector<std::string>& getNos() const;
    const Metricas& getMetricas() const;
};

#endif // GRAPH_H
