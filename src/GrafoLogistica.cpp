#include "GrafoLogistica.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <cctype>
#include <limits>                


const int INF = std::numeric_limits<int>::max() / 3;

#ifdef _WIN32
#include <direct.h>  // _mkdir
#else
#include <sys/stat.h> // mkdir
#endif

using namespace std;

GrafoLogistica::GrafoLogistica()
    : total_nos(0), depot(0), capacidade_veiculo(0)
{
}

void GrafoLogistica::addNo(const string &nome)
{
    if (!mapeamento_nos.count(nome))
    {
        mapeamento_nos[nome] = nos.size();
        nos.push_back(nome);
    }
}

void GrafoLogistica::processarLinha(const string &linha, const string &secao, int &proxId)
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
            servicos.push_back({proxId++, v, v, 0, dem, false});
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
            servicos.push_back({proxId++, u, v, cost, dem, false});
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
            servicos.push_back({proxId++, u, v, cost, dem, true});
        }
    }
}

void GrafoLogistica::floydWarshall()
{
    for (int k = 0; k < total_nos; k++)
        for (int i = 0; i < total_nos; i++)
            for (int j = 0; j < total_nos; j++)
                if (distancias[i][k] + distancias[k][j] < distancias[i][j])
                {
                    distancias[i][j] = distancias[i][k] + distancias[k][j];
                    predecessores[i][j] = predecessores[k][j];
                }
}

void GrafoLogistica::inicializarMatrizes()
{
    total_nos = nos.size();
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

void GrafoLogistica::calcularGraus()
{
    vector<int> graus(total_nos);
    for (int i = 0; i < total_nos; i++)
        graus[i] = lista_adj[i].size();
    metricas.grau_min = total_nos ? *min_element(graus.begin(), graus.end()) : 0;
    metricas.grau_max = total_nos ? *max_element(graus.begin(), graus.end()) : 0;
}

void GrafoLogistica::calcularIntermediacao()
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

float GrafoLogistica::calcularDensidade()
{
    if (total_nos <= 1)
        return 0.0f;
    float con = 2.0f * arestas.size() + arcos.size();
    return con / (total_nos * (total_nos - 1));
}

double GrafoLogistica::calcularCaminhoMedio()
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

int GrafoLogistica::calcularDiametro()
{
    int d = 0;
    for (int i = 0; i < total_nos; i++)
        for (int j = i + 1; j < total_nos; j++)
            if (distancias[i][j] < INF)
                d = max(d, distancias[i][j]);
    return d;
}

vector<GrafoLogistica::Rota> GrafoLogistica::gerarRotas()
{
    vector<Servico> local = servicos;
    vector<Rota> rotas;
    int faltam = local.size();
    while (faltam)
    {
        Rota R;
        R.vis.push_back({true, 0, 1, 1});
        int pos = depot, cap = capacidade_veiculo;
        while (true)
        {
            int idx = -1;
            double best = 1e30;
            for (size_t i = 0; i < local.size(); ++i)
            {
                auto &S = local[i];
                if (S.atendido || S.demanda > cap)
                    continue;
                int dead = min(distancias[pos][S.u], distancias[pos][S.v]);
                double sc = double(dead + S.custo) / max(1, S.demanda);
                if (sc < best)
                {
                    best = sc;
                    idx = i;
                }
            }
            if (idx < 0)
                break;
            auto &S = local[idx];
            bool inv = distancias[pos][S.v] < distancias[pos][S.u];
            int a = inv ? S.v : S.u, b = inv ? S.u : S.v;
            R.custo += distancias[pos][a] + S.custo;
            R.demanda += S.demanda;
            R.vis.push_back({false, S.id, a, b});
            pos = b;
            cap -= S.demanda;
            local[idx].atendido = true;
            --faltam;
        }
        R.custo += distancias[pos][depot];
        R.vis.push_back({true, 0, 1, 1});
        rotas.push_back(move(R));
    }
    return rotas;
}

int GrafoLogistica::custoTotal(const vector<Rota> &rotas) const
{
    int c = 0;
    for (auto &r : rotas)
        c += r.custo;
    return c;
}

void GrafoLogistica::lerDados(const string &arquivo)
{
    ifstream in(arquivo);
    if (!in.is_open())
    {
        cerr << "Erro abrir " << arquivo << endl;
        return;
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
    inicializarMatrizes();
}

void GrafoLogistica::calcularTodasMetricas()
{
    metricas.qtd_vertices = total_nos;
    metricas.qtd_arestas = arestas.size();
    metricas.qtd_arcos = arcos.size();
    metricas.vertices_req = nos_requeridos.size();
    metricas.arestas_req = arestas_requeridas.size();
    metricas.arcos_req = arcos_requeridas.size();
    metricas.densidade = calcularDensidade();
    calcularGraus();
    calcularIntermediacao();
    metricas.caminho_medio = calcularCaminhoMedio();
    metricas.diametro = calcularDiametro();
}

void GrafoLogistica::salvarResultados(const string &nome_arquivo) const
{
    ofstream out(nome_arquivo);
    if (!out.is_open())
    {
        cerr << "Erro salvar " << nome_arquivo << endl;
        return;
    }
    out << fixed << setprecision(2)
        << "1. Vertices: " << metricas.qtd_vertices << "\n"
        << "2. Arestas: " << metricas.qtd_arestas << "\n"
        << "3. Arcos: " << metricas.qtd_arcos << "\n"
        << "4. Vertices req: " << metricas.vertices_req << "\n"
        << "5. Arestas req: " << metricas.arestas_req << "\n"
        << "6. Arcos req: " << metricas.arcos_req << "\n"
        << setprecision(4)
        << "7. Densidade: " << metricas.densidade << "\n"
        << setprecision(2)
        << "8. Grau min: " << metricas.grau_min << "\n"
        << "9. Grau max: " << metricas.grau_max << "\n"
        << "10. Intermediacao:\n";
    for (auto &p : metricas.intermediacao)
        out << "    " << p.first << ": " << p.second << "\n";
    out << "11. Caminho medio: " << metricas.caminho_medio << "\n"
        << "12. Diametro: " << metricas.diametro << "\n";
}

void GrafoLogistica::salvarSolucao(const string &nomeArquivo, long long total_ns)
{
    const int REP = 100;
    vector<Rota> rotas;
    auto t2 = chrono::steady_clock::now();
    for (int i = 0; i < REP; i++)
        rotas = gerarRotas();
    auto t3 = chrono::steady_clock::now();
    double sol_ns = double(chrono::duration_cast<chrono::nanoseconds>(t3 - t2).count()) / REP;

#ifdef _WIN32
    _mkdir("solutions");
#else
    mkdir("solutions", 0755);
#endif
    ofstream out(nomeArquivo);
    if (!out.is_open())
    {
        cerr << "Erro criar " << nomeArquivo << endl;
        return;
    }
    out << fixed << setprecision(0);
    out << custoTotal(rotas) << "\n"
        << rotas.size() << "\n"
        << total_ns << "\n"
        << sol_ns << "\n";
    for (size_t k = 0; k < rotas.size(); ++k)
    {
        auto &R = rotas[k];
        out << "0 1 " << (k + 1) << " " << R.demanda << " " << R.custo << " " << R.vis.size();
        for (auto &v : R.vis)
        {
            if (v.depot)
                out << " (D 0,1,1)";
            else
                out << " (S " << v.id << "," << (v.u + 1) << "," << (v.v + 1) << ")";
        }
        out << "\n";
    }
}
