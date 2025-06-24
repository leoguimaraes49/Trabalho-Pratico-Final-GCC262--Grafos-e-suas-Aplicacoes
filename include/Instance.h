#ifndef INSTANCE_H
#define INSTANCE_H

#include <string>
#include <vector>
#include <tuple>
#include <map>

struct Servico
{
    int id, u, v, custo, demanda;
    bool dirigido, atendido;
};

class Instance
{
private:
    std::map<std::string, int> mapeamento_nos;
    std::vector<std::string> nos;

    std::vector<std::tuple<int, int, int>> arestas, arcos;
    std::vector<std::pair<int, int>> arestas_requeridas, arcos_requeridas;
    std::vector<int> nos_requeridos;
    std::vector<Servico> servicos;

    int depot;
    int capacidade_veiculo;

    void addNo(const std::string &nome);
    void processarLinha(const std::string &linha, const std::string &secao, int &proxId);

public:
    Instance();

    bool lerDados(const std::string &arquivo);

    const std::vector<std::string>& getNos() const;
    const std::vector<std::tuple<int, int, int>>& getArestas() const;
    const std::vector<std::tuple<int, int, int>>& getArcos() const;
    const std::vector<std::pair<int, int>>& getArestasRequeridas() const;
    const std::vector<std::pair<int, int>>& getArcosRequeridos() const;
    const std::vector<int>& getNosRequeridos() const;
    const std::vector<Servico>& getServicos() const;
    int getDepot() const;
    int getCapacidadeVeiculo() const;
};

#endif // INSTANCE_H
