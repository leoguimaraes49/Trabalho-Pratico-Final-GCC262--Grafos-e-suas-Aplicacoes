#include "Graph.h"
#include <algorithm>
#include <limits>
#include <map>

using namespace std;

const int INF = numeric_limits<int>::max() / 3;

Graph::Graph(const vector<string> &nos,
             const vector<tuple<int, int, int>> &arestas,
             const vector<tuple<int, int, int>> &arcos,
             const vector<pair<int, int>> &arestas_requeridas,
             const vector<pair<int, int>> &arcos_requeridas,
             const vector<int> &nos_requeridos)
    : nos(nos), arestas(arestas), arcos(arcos),
      arestas_requeridas(arestas_requeridas), arcos_requeridas(arcos_requeridas),
      nos_requeridos(nos_requeridos)
{
    total_nos = (int)nos.size();
    inicializarMatrizes();
    calcularTodasMetricas();
}

void Graph::floydWarshall()
{
    for (int k = 0; k < total_nos; k++)
        for (int i = 0; i < total_nos; i++)
            for (int j = 0; j < total_nos; j++)
                if (distancias[i][k] < INF && distancias[k][j] < INF && distancias[i][k] + distancias[k][j] < distancias[i][j])
                {
                    distancias[i][j] = distancias[i][k] + distancias[k][j];
                    predecessores[i][j] = predecessores[k][j];
                }
}

void Graph::inicializarMatrizes()
{
    distancias.assign(total_nos, vector<int>(total_nos, INF));
    predecessores.assign(total_nos, vector<int>(total_nos, -1));
    lista_adj.assign(total_nos, vector<int>());

    for (int i = 0; i < total_nos; i++)
    {
        distancias[i][i] = 0;
        predecessores[i][i] = i;
    }
    for (auto &a : arestas)
    {
        int u, v, c;
        tie(u, v, c) = a;
        if (c < distancias[u][v])
        {
            distancias[u][v] = distancias[v][u] = c;
            predecessores[u][v] = u;
            predecessores[v][u] = v;
        }
    }
    for (auto &a : arcos)
    {
        int u, v, c;
        tie(u, v, c) = a;
        if (c < distancias[u][v])
        {
            distancias[u][v] = c;
            predecessores[u][v] = u;
        }
    }
    for (auto &a : arestas)
    {
        int u, v, c;
        tie(u, v, c) = a;
        lista_adj[u].push_back(v);
        lista_adj[v].push_back(u);
    }
    for (auto &a : arcos)
    {
        int u, v, c;
        tie(u, v, c) = a;
        lista_adj[u].push_back(v);
        lista_adj[v].push_back(u);
    }

    metricas.intermediacao.clear();
    for (const auto &n : nos)
        metricas.intermediacao[n] = 0.0;

    floydWarshall();
}

void Graph::calcularGraus()
{
    vector<int> graus(total_nos);
    for (int i = 0; i < total_nos; i++)
        graus[i] = lista_adj[i].size();
    metricas.grau_min = total_nos ? *min_element(graus.begin(), graus.end()) : 0;
    metricas.grau_max = total_nos ? *max_element(graus.begin(), graus.end()) : 0;
}

void Graph::calcularIntermediacao()
{
    for (int s = 0; s < total_nos; s++)
        for (int t = 0; t < total_nos; t++)
            if (s != t && predecessores[s][t] != -1)
            {
                int at = t;
                while (predecessores[s][at] != s)
                {
                    at = predecessores[s][at];
                    metricas.intermediacao[nos[at]]++;
                }
            }
}

float Graph::calcularDensidade()
{
    if (total_nos <= 1)
        return 0.0f;
    float con = 2.0f * arestas.size() + arcos.size();
    return con / (total_nos * (total_nos - 1));
}

double Graph::calcularCaminhoMedio()
{
    long long sum = 0;
    int cnt = 0;
    for (int i = 0; i < total_nos; i++)
        for (int j = i + 1; j < total_nos; j++)
            if (distancias[i][j] < INF)
            {
                sum += distancias[i][j];
                cnt++;
            }
    return cnt ? double(sum) / cnt : 0.0;
}

int Graph::calcularDiametro()
{
    int d = 0;
    for (int i = 0; i < total_nos; i++)
        for (int j = i + 1; j < total_nos; j++)
            if (distancias[i][j] < INF)
                d = max(d, distancias[i][j]);
    return d;
}

void Graph::calcularTodasMetricas()
{
    metricas.qtd_vertices = total_nos;
    metricas.qtd_arestas = (int)arestas.size();
    metricas.qtd_arcos = (int)arcos.size();
    metricas.vertices_req = (int)nos_requeridos.size();
    metricas.arestas_req = (int)arestas_requeridas.size();
    metricas.arcos_req = (int)arcos_requeridas.size();
    metricas.densidade = calcularDensidade();
    calcularGraus();
    calcularIntermediacao();
    metricas.caminho_medio = calcularCaminhoMedio();
    metricas.diametro = calcularDiametro();
}

int Graph::getTotalNos() const
{
    return total_nos;
}

const vector<vector<int>>& Graph::getDistancias() const
{
    return distancias;
}

const vector<string>& Graph::getNos() const
{
    return nos;
}

const Graph::Metricas& Graph::getMetricas() const
{
    return metricas;
}
