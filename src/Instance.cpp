#include "Instance.h"
#include "utils.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>

using namespace std;

Instance::Instance()
    : depot(0), capacidade_veiculo(0)
{
}

void Instance::addNo(const string &nome)
{
    if (!mapeamento_nos.count(nome))
    {
        mapeamento_nos[nome] = nos.size();
        nos.push_back(nome);
    }
}

void Instance::processarLinha(const string &linha, const string &secao, int &proxId)
{
    vector<string> tk = split(linha);
    if (tk.empty())
        return;
    auto pegaDemanda = [&](int idxCost) -> int
    {
        for (size_t k = idxCost + 1; k < tk.size(); ++k)
            if (tk[k] == "DEMAND" && k + 1 < tk.size())
                return safeStoi(tk[k + 1], 1);
        return 1;
    };
    if (secao == "nodes" || secao == "reqnodes")
    {
        string nome = tk[0];
        if (!nome.empty() && (nome[0] == 'N' || nome[0] == 'n'))
            nome = nome.substr(1);
        addNo(nome);
        if (secao == "reqnodes")
        {
            int v = mapeamento_nos[nome];
            nos_requeridos.push_back(v);
            int dem = (tk.size() > 2 ? safeStoi(tk.back(), 1) : 1);
            servicos.push_back({proxId++, v, v, 0, dem, false, false});
        }
    }
    else if (secao == "reqedges" || secao == "edges")
    {
        int idx = isalpha(tk[0][0]) ? 1 : 0;
        string s1 = tk[idx], s2 = tk[idx + 1];
        if (s1[0] == 'N' || s1[0] == 'n')
            s1 = s1.substr(1);
        if (s2[0] == 'N' || s2[0] == 'n')
            s2 = s2.substr(1);
        int cost = safeStoi(tk[idx + 2]);
        int demF = pegaDemanda(idx + 2);
        int dem = (tk.size() > idx + 3 ? safeStoi(tk[idx + 3], demF) : demF);
        addNo(s1);
        addNo(s2);
        int u = mapeamento_nos[s1], v = mapeamento_nos[s2];
        arestas.emplace_back(u, v, cost);
        if (secao == "reqedges")
        {
            arestas_requeridas.emplace_back(u, v);
            servicos.push_back({proxId++, u, v, cost, dem, false, false});
        }
    }
    else if (secao == "reqarcs" || secao == "arcs")
    {
        int idx = isalpha(tk[0][0]) ? 1 : 0;
        string s1 = tk[idx], s2 = tk[idx + 1];
        if (s1[0] == 'N' || s1[0] == 'n')
            s1 = s1.substr(1);
        if (s2[0] == 'N' || s2[0] == 'n')
            s2 = s2.substr(1);
        int cost = safeStoi(tk[idx + 2]);
        int demF = pegaDemanda(idx + 2);
        int dem = (tk.size() > idx + 3 ? safeStoi(tk[idx + 3], demF) : demF);
        addNo(s1);
        addNo(s2);
        int u = mapeamento_nos[s1], v = mapeamento_nos[s2];
        arcos.emplace_back(u, v, cost);
        if (secao == "reqarcs")
        {
            arcos_requeridas.emplace_back(u, v);
            servicos.push_back({proxId++, u, v, cost, dem, true, false});
        }
    }
}

bool Instance::lerDados(const string &arquivo)
{
    ifstream in(arquivo);
    if (!in.is_open())
    {
        cerr << "Erro abrir " << arquivo << endl;
        return false;
    }
    string linha, secao = "none";
    int proxId = 1;
    while (getline(in, linha))
    {
        linha = trim(linha);
        if (linha.empty() || linha == "-1")
            continue;
        string ll = linha;
        transform(ll.begin(), ll.end(), ll.begin(), ::tolower);
        if (ll.find("depot node:") != string::npos)
        {
            depot = safeStoi(split(linha).back());
            continue;
        }
        if (ll.find("capacity:") != string::npos)
        {
            capacidade_veiculo = safeStoi(split(linha).back());
            continue;
        }
        if (ll.rfind("ren", 0) == 0)
            secao = "reqnodes";
        else if (ll.rfind("ree", 0) == 0)
            secao = "reqedges";
        else if (ll.rfind("edge", 0) == 0)
            secao = "edges";
        else if (ll.rfind("rea.", 0) == 0)
            secao = "reqarcs";
        else if (ll.rfind("arc", 0) == 0)
            secao = "arcs";
        else
            processarLinha(linha, secao, proxId);
    }
    in.close();
    return true;
}

const vector<string>& Instance::getNos() const
{
    return nos;
}

const vector<tuple<int, int, int>>& Instance::getArestas() const
{
    return arestas;
}

const vector<tuple<int, int, int>>& Instance::getArcos() const
{
    return arcos;
}

const vector<pair<int, int>>& Instance::getArestasRequeridas() const
{
    return arestas_requeridas;
}

const vector<pair<int, int>>& Instance::getArcosRequeridos() const
{
    return arcos_requeridas;
}

const vector<int>& Instance::getNosRequeridos() const
{
    return nos_requeridos;
}

const vector<Servico>& Instance::getServicos() const
{
    return servicos;
}

int Instance::getDepot() const
{
    return depot;
}

int Instance::getCapacidadeVeiculo() const
{
    return capacidade_veiculo;
}
