// ───────────────────────── Orchestrator.cpp ──────────────────────────
#include "Orchestrator.h"
#include "Instance.h"
#include "Graph.h"
#include "Optimizer.h"

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <windows.h>   // WIN32_FIND_DATAA, FindFirstFileA…
#include <fstream>
#include <iomanip>
#include <chrono>

#ifdef _WIN32
  #include <direct.h>      // _mkdir
#else
  #include <sys/stat.h>    // mkdir
  #include <dirent.h>
#endif

using namespace std;

// -----------------------------------------------------------------------------
// Lista todos os arquivos .dat no diretório dataDir
// -----------------------------------------------------------------------------
vector<string> listarInstancias(const string &dataDir)
{
    vector<string> files;
#ifdef _WIN32
    WIN32_FIND_DATAA fd;
    string pattern = dataDir + "\\*.dat";
    HANDLE h = FindFirstFileA(pattern.c_str(), &fd);
    if (h == INVALID_HANDLE_VALUE) return files;
    do {
        files.push_back(fd.cFileName);
    } while (FindNextFileA(h, &fd));
    FindClose(h);
#else
    DIR *d = opendir(dataDir.c_str());
    if (!d) return files;
    struct dirent *ent;
    while ((ent = readdir(d)) != nullptr)
    {
        string name = ent->d_name;
        if (name.size() > 4 && name.substr(name.size() - 4) == ".dat")
            files.push_back(name);
    }
    closedir(d);
#endif
    sort(files.begin(), files.end());
    return files;
}

// -----------------------------------------------------------------------------
// Construtor
// -----------------------------------------------------------------------------
Orchestrator::Orchestrator() = default;

// -----------------------------------------------------------------------------
// Executa para todas as instâncias
// -----------------------------------------------------------------------------
int Orchestrator::run(const string &dataDir,
                      const string &resultsDir,
                      const string &solutionsDir)
{
#ifdef _WIN32
    _mkdir(resultsDir.c_str());
    _mkdir(solutionsDir.c_str());
#else
    mkdir(resultsDir.c_str(), 0755);
    mkdir(solutionsDir.c_str(), 0755);
#endif

    vector<string> insts = listarInstancias(dataDir);
    if (insts.empty()) {
        cerr << "Nenhuma .dat encontrada em " << dataDir << endl;
        return 1;
    }

    for (const auto &nomeArquivo : insts)
    {
        string base = nomeArquivo.substr(0, nomeArquivo.size() - 4);
        string path = dataDir + "/" + nomeArquivo;
        cout << "\nProcessando " << nomeArquivo << " ..." << endl;

        // ---------------------------------------------------------------------
        // Carrega a instância
        // ---------------------------------------------------------------------
        Instance instance;
        if (!instance.lerDados(path)) {
            cerr << "Falha ao ler dados da instância " << nomeArquivo << endl;
            continue;
        }
        const int depot = instance.getDepot();   // 0-based

        // ---------------------------------------------------------------------
        // Calcula métricas do grafo
        // ---------------------------------------------------------------------
        Graph graph(instance.getNos(), instance.getArestas(), instance.getArcos(),
                    instance.getArestasRequeridas(), instance.getArcosRequeridos(),
                    instance.getNosRequeridos());
        graph.calcularTodasMetricas();

        // ---------------------------------------------------------------------
        // Executa a heurística (uma vez para clocks de referência)
        // ---------------------------------------------------------------------
        auto t0 = chrono::steady_clock::now();
        Optimizer optimizer(instance, graph);
        auto rotas = optimizer.gerarRotas();
        auto t1 = chrono::steady_clock::now();
        long long total_ns =
            chrono::duration_cast<chrono::nanoseconds>(t1 - t0).count();

        // ---------------------------------------------------------------------
        // Salva métricas em resultsDir/metrics-*.txt
        // ---------------------------------------------------------------------
        string outMet = resultsDir + "/metrics-" + base + ".txt";
        ofstream outMetFile(outMet);
        if (!outMetFile.is_open()) {
            cerr << "Erro ao salvar métricas em " << outMet << endl;
        } else {
            const auto &m = graph.getMetricas();
            outMetFile << fixed << setprecision(2)
                       << "1. Vertices: " << m.qtd_vertices << "\n"
                       << "2. Arestas: "  << m.qtd_arestas  << "\n"
                       << "3. Arcos: "    << m.qtd_arcos    << "\n"
                       << "4. Vertices req: " << m.vertices_req << "\n"
                       << "5. Arestas req: "  << m.arestas_req  << "\n"
                       << "6. Arcos req: "    << m.arcos_req    << "\n"
                       << setprecision(4)
                       << "7. Densidade: "    << m.densidade    << "\n"
                       << setprecision(2)
                       << "8. Grau min: "     << m.grau_min     << "\n"
                       << "9. Grau max: "     << m.grau_max     << "\n"
                       << "10. Intermediacao:\n";
            for (const auto &p : m.intermediacao)
                outMetFile << "    " << p.first << ": " << p.second << "\n";
            outMetFile << "11. Caminho medio: " << m.caminho_medio << "\n"
                       << "12. Diametro: "       << m.diametro      << "\n";
            cout << "  Métricas -> " << outMet << endl;
        }

        // ---------------------------------------------------------------------
        // Mede tempo médio (REP execuções) e guarda a última solução
        // ---------------------------------------------------------------------
        const int REP = 100;
        double sol_ns = 0.0;
        for (int i = 0; i < REP; ++i) {
            auto a = chrono::steady_clock::now();
            auto r = optimizer.gerarRotas();
            auto b = chrono::steady_clock::now();
            sol_ns += chrono::duration_cast<chrono::nanoseconds>(b - a).count();
            if (i == REP - 1) rotas.swap(r);   // última rotação guardada
        }
        sol_ns /= REP;

        // ---------------------------------------------------------------------
        // Salva solução no formato especificado
        // ---------------------------------------------------------------------
        string outSol = solutionsDir + "/sol-" + base + ".dat";
        ofstream outSolFile(outSol);
        if (!outSolFile.is_open()) {
            cerr << "Erro ao salvar solução em " << outSol << endl;
            continue;
        }

        // Linha 1-4 do cabeçalho
        outSolFile << fixed << setprecision(0)
                   << optimizer.custoTotal(rotas) << "\n"
                   << rotas.size()               << "\n"
                   << total_ns                   << "\n"
                   << sol_ns                     << "\n";

        // Demais linhas: uma por rota
        for (size_t k = 0; k < rotas.size(); ++k)
        {
            const auto &R = rotas[k];
            outSolFile << "0 1 " << (k + 1)      << " "
                       << R.getDemanda()         << " "
                       << R.getCusto()           << " "
                       << R.getVisitas().size();          // visitas (dep+dep+serviços)

            for (const auto &v : R.getVisitas())
            {
                if (v.depot) {
                    outSolFile << " (D 0," << (depot) << "," << (depot) << ")";
                } else {
                    outSolFile << " (S " << v.id
                               << ","    << (v.u + 1)
                               << ","    << (v.v + 1) << ")";
                }
            }
            outSolFile << "\n";
        }
        cout << "  Solução -> " << outSol << endl;
    }
    return 0;
}
