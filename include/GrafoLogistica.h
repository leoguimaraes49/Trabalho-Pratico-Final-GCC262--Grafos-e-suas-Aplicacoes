#ifndef GRAFOLOGISTICA_H
#define GRAFOLOGISTICA_H

#include <string>
#include <vector>
#include <map>
#include <tuple>

struct Servico
{
    int id, u, v, custo, demanda;
    bool dirigido, atendido;
};

class GrafoLogistica
{
private:
    std::map<std::string, int> mapeamento_nos;
    std::vector<std::string> nos;
    int total_nos;

    std::vector<std::vector<int>> distancias, predecessores, lista_adj;
    std::vector<std::tuple<int, int, int>> arestas, arcos;
    std::vector<std::pair<int, int>> arestas_requeridas, arcos_requeridas;
    std::vector<int> nos_requeridos;
    std::vector<Servico> servicos;

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

    int depot;
    int capacidade_veiculo;

    void addNo(const std::string &nome);
    void processarLinha(const std::string &linha, const std::string &secao, int &proxId);
    void floydWarshall();
    void inicializarMatrizes();
    void calcularGraus();
    void calcularIntermediacao();
    float calcularDensidade();
    double calcularCaminhoMedio();
    int calcularDiametro();

public:
    struct Visita
    {
        bool depot;
        int id, u, v;
    };
    struct Rota
    {
        std::vector<Visita> vis;
        int demanda;
        int custo;
    };

    GrafoLogistica();

    void lerDados(const std::string &arquivo);
    void calcularTodasMetricas();
    void salvarResultados(const std::string &nome_arquivo) const;
    void salvarSolucao(const std::string &nomeArquivo, long long total_ns);
    std::vector<Rota> gerarRotas();
    int custoTotal(const std::vector<Rota> &rotas) const;
};

#endif // GRAFOLOGISTICA_H
