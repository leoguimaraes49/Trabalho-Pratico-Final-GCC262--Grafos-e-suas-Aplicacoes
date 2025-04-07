#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <tuple>
#include <queue>
#include <iomanip>
#include <algorithm>
#include <climits>
#include <string>

using namespace std;

const int INF = INT_MAX / 3;

class GrafoLogistica {
private:
    // Mapeamento dos nós: nome para índice e vetor com os nomes
    map<string, int> mapeamento_nos;
    vector<string> nos;
    int total_nos = 0;

    // Matrizes de caminhos mínimos e predecessores (calculadas com Floyd–Warshall)
    vector<vector<int>> distancias;
    vector<vector<int>> predecessores;

    // Dados dos nós, arestas e arcos do grafo
    vector<int> nos_requeridos;
    vector<tuple<int, int, int>> arestas;         // Arestas não direcionadas
    vector<tuple<int, int, int>> arcos;            // Arcos direcionados
    vector<pair<int, int>> arestas_requeridas;
    vector<pair<int, int>> arcos_requeridos;

    // Lista de adjacência (usada nos cálculos, mas componentes conectados foram removidos)
    vector<vector<int>> lista_adj;

    // Estrutura para guardar as métricas calculadas
    struct Metricas {
        int qtd_vertices, qtd_arestas, qtd_arcos;
        int vertices_req, arestas_req, arcos_req;
        double densidade; // ((2 * |arestas| + |arcos|) * 100) / (n * (n - 1))
        int grau_min, grau_max;
        map<string, double> intermediacao;
        double caminho_medio;
        int diametro;
    } metricas;

    // ---------------------------------------------------------------
    // FUNÇÕES AUXILIARES DE MANIPULAÇÃO DE STRINGS
    // ---------------------------------------------------------------
    // Remove espaços em branco do início e fim da string
    string trim(const string &str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == string::npos)
            return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, last - first + 1);
    }

    // Separa a linha em tokens usando espaço ou tabulação
    vector<string> split(const string &linha) {
        vector<string> tokens;
        istringstream iss(linha);
        string token;
        while (iss >> token)
            tokens.push_back(token);
        return tokens;
    }

    // ---------------------------------------------------------------
    // PROCESSAMENTO DAS LINHAS CONFORME A SEÇÃO IDENTIFICADA
    // ---------------------------------------------------------------
    void processarLinha(const string &linha, const string &secao) {
        // Ignora linhas que contenham "DEMAND" ou "COST"
        if (linha.find("DEMAND") != string::npos || linha.find("COST") != string::npos)
            return;
        vector<string> tokens = split(linha);
        if (tokens.empty())
            return;

        if (secao == "nodes") {
            string nome = tokens[0];
            if (mapeamento_nos.find(nome) == mapeamento_nos.end()) {
                mapeamento_nos[nome] = nos.size();
                nos.push_back(nome);
            }
        }
        else if (secao == "reqnodes") {
            string nome = tokens[0];
            if (!nome.empty() && nome[0] == 'N')
                nome = nome.substr(1);
            if (mapeamento_nos.find(nome) == mapeamento_nos.end()) {
                mapeamento_nos[nome] = nos.size();
                nos.push_back(nome);
            }
            nos_requeridos.push_back(mapeamento_nos[nome]);
        }
        else if (secao == "reqedges") {
            if (tokens.size() >= 4) {
                int idx = isalpha(tokens[0][0]) ? 1 : 0;
                if (tokens.size() - idx < 3)
                    return;
                string no1 = tokens[idx], no2 = tokens[idx+1];
                int custo = stoi(tokens[idx+2]);
                if (!no1.empty() && no1[0] == 'N')
                    no1 = no1.substr(1);
                if (!no2.empty() && no2[0] == 'N')
                    no2 = no2.substr(1);
                if (mapeamento_nos.find(no1) == mapeamento_nos.end()) {
                    mapeamento_nos[no1] = nos.size();
                    nos.push_back(no1);
                }
                if (mapeamento_nos.find(no2) == mapeamento_nos.end()) {
                    mapeamento_nos[no2] = nos.size();
                    nos.push_back(no2);
                }
                int u = mapeamento_nos[no1], v = mapeamento_nos[no2];
                arestas_requeridas.push_back(make_pair(u, v));
                arestas.push_back(make_tuple(u, v, custo));
            }
        }
        else if (secao == "edges") {
            int idx = (tokens.size() >= 4 && isalpha(tokens[0][0])) ? 1 : 0;
            if (tokens.size() - idx < 3)
                return;
            string no1 = tokens[idx], no2 = tokens[idx+1];
            int custo = stoi(tokens[idx+2]);
            if (!no1.empty() && no1[0]=='N')
                no1 = no1.substr(1);
            if (!no2.empty() && no2[0]=='N')
                no2 = no2.substr(1);
            if (mapeamento_nos.find(no1) == mapeamento_nos.end()) {
                mapeamento_nos[no1] = nos.size();
                nos.push_back(no1);
            }
            if (mapeamento_nos.find(no2) == mapeamento_nos.end()) {
                mapeamento_nos[no2] = nos.size();
                nos.push_back(no2);
            }
            int u = mapeamento_nos[no1], v = mapeamento_nos[no2];
            arestas.push_back(make_tuple(u, v, custo));
        }
        else if (secao == "reqarcs") {
            if (tokens.size() >= 4) {
                int idx = isalpha(tokens[0][0]) ? 1 : 0;
                if (tokens.size() - idx < 3)
                    return;
                string no1 = tokens[idx], no2 = tokens[idx+1];
                int custo = stoi(tokens[idx+2]);
                if (!no1.empty() && no1[0]=='N')
                    no1 = no1.substr(1);
                if (!no2.empty() && no2[0]=='N')
                    no2 = no2.substr(1);
                if (mapeamento_nos.find(no1) == mapeamento_nos.end()) {
                    mapeamento_nos[no1] = nos.size();
                    nos.push_back(no1);
                }
                if (mapeamento_nos.find(no2) == mapeamento_nos.end()) {
                    mapeamento_nos[no2] = nos.size();
                    nos.push_back(no2);
                }
                int u = mapeamento_nos[no1], v = mapeamento_nos[no2];
                arcos_requeridos.push_back(make_pair(u, v));
                arcos.push_back(make_tuple(u, v, custo));
            }
        }
        else if (secao == "arcs") {
            int idx = (tokens.size() >= 4 && isalpha(tokens[0][0])) ? 1 : 0;
            if (tokens.size() - idx < 3)
                return;
            string no1 = tokens[idx], no2 = tokens[idx+1];
            int custo = stoi(tokens[idx+2]);
            if (!no1.empty() && no1[0]=='N')
                no1 = no1.substr(1);
            if (!no2.empty() && no2[0]=='N')
                no2 = no2.substr(1);
            if (mapeamento_nos.find(no1) == mapeamento_nos.end()) {
                mapeamento_nos[no1] = nos.size();
                nos.push_back(no1);
            }
            if (mapeamento_nos.find(no2) == mapeamento_nos.end()) {
                mapeamento_nos[no2] = nos.size();
                nos.push_back(no2);
            }
            int u = mapeamento_nos[no1], v = mapeamento_nos[no2];
            arcos.push_back(make_tuple(u, v, custo));
        }
    }

    // ---------------------------------------------------------------
    // CONSTRUÇÃO DAS MATRIZES DE CUSTO, PREDECESSORES E DA LISTA DE ADJACÊNCIA
    // ---------------------------------------------------------------
    void inicializarMatrizes() {
        total_nos = nos.size();
        distancias.assign(total_nos, vector<int>(total_nos, INF));
        predecessores.assign(total_nos, vector<int>(total_nos, -1));
        for (int i = 0; i < total_nos; i++) {
            distancias[i][i] = 0;
            predecessores[i][i] = i;
        }
        // Insere as arestas (não direcionadas)
        for (auto &a : arestas) {
            int u, v, custo;
            tie(u, v, custo) = a;
            if (custo < distancias[u][v]) {
                distancias[u][v] = custo;
                distancias[v][u] = custo;
                predecessores[u][v] = u;
                predecessores[v][u] = v;
            }
        }
        // Insere os arcos (direcionados)
        for (auto &a : arcos) {
            int u, v, custo;
            tie(u, v, custo) = a;
            if (custo < distancias[u][v]) {
                distancias[u][v] = custo;
                predecessores[u][v] = u;
            }
        }
        // Cria a lista de adjacência
        lista_adj.assign(total_nos, vector<int>());
        for (auto &a : arestas) {
            int u, v, custo;
            tie(u, v, custo) = a;
            lista_adj[u].push_back(v);
            lista_adj[v].push_back(u);
        }
        for (auto &a : arcos) {
            int u, v, custo;
            tie(u, v, custo) = a;
            lista_adj[u].push_back(v);
            lista_adj[v].push_back(u);
        }
        // Inicializa o mapa de intermediação para cada nó com valor zero
        for (const auto &nome : nos)
            metricas.intermediacao[nome] = 0.0;
        // Calcula a matriz de caminhos mínimos e os predecessores
        floydWarshall();
    }

    // ---------------------------------------------------------------
    // ALGORITMO DE FLOYD–WARSHALL
    // ---------------------------------------------------------------
    void floydWarshall() {
        for (int k = 0; k < total_nos; k++) {
            for (int i = 0; i < total_nos; i++) {
                for (int j = 0; j < total_nos; j++) {
                    if (distancias[i][k] + distancias[k][j] < distancias[i][j]) {
                        distancias[i][j] = distancias[i][k] + distancias[k][j];
                        predecessores[i][j] = predecessores[k][j];
                    }
                }
            }
        }
    }

    // ---------------------------------------------------------------
    // CÁLCULO DAS MÉTRICAS (exceto componentes conectados)
    // ---------------------------------------------------------------
    void calcularGraus() {
        vector<int> graus(total_nos, 0);
        for (int i = 0; i < total_nos; i++)
            graus[i] = lista_adj[i].size();
        metricas.grau_min = *min_element(graus.begin(), graus.end());
        metricas.grau_max = *max_element(graus.begin(), graus.end());
    }

    void calcularIntermediacao() {
        for (int s = 0; s < total_nos; s++) {
            for (int t = 0; t < total_nos; t++) {
                if (s == t || predecessores[s][t] == -1)
                    continue;
                int atual = t;
                while (predecessores[s][atual] != s && predecessores[s][atual] != -1) {
                    atual = predecessores[s][atual];
                    metricas.intermediacao[nos[atual]]++;
                }
            }
        }
    }

    double calcularDensidade() {
        if (total_nos <= 1)
            return 0.0;
        double numConexoes = 2.0 * arestas.size() + arcos.size();
        return (numConexoes * 100.0) / (total_nos * (total_nos - 1));
    }

    double calcularCaminhoMedio() {
        double soma = 0.0;
        int cont = 0;
        for (int i = 0; i < total_nos; i++) {
            for (int j = 0; j < total_nos; j++) {
                if (i != j && distancias[i][j] < INF) {
                    soma += distancias[i][j];
                    cont++;
                }
            }
        }
        return (cont > 0) ? (soma / cont) : 0.0;
    }

    int calcularDiametro() {
        int diam = 0;
        for (int i = 0; i < total_nos; i++) {
            for (int j = 0; j < total_nos; j++) {
                if (i != j && distancias[i][j] < INF)
                    diam = max(diam, distancias[i][j]);
            }
        }
        return diam;
    }

public:
    // ---------------------------------------------------------------
    // LEITURA DOS DADOS DO ARQUIVO (Parser Flexível)
    // ---------------------------------------------------------------
    void lerDados(const string &arquivo) {
        ifstream entrada(arquivo);
        if (!entrada.is_open()) {
            cerr << "Erro ao abrir o arquivo " << arquivo << endl;
            return;
        }
        string linha;
        string secao = "none";
        while (getline(entrada, linha)) {
            linha = trim(linha);
            if (linha.empty())
                continue;
            // Ignora cabeçalhos desnecessários
            if (linha.find("Name:") != string::npos ||
                linha.find("Optimal value:") != string::npos ||
                linha.find("#Vehicles:") != string::npos ||
                linha.find("Capacity:") != string::npos ||
                linha.find("Depot Node:") != string::npos ||
                linha.find("#Nodes:") != string::npos ||
                linha.find("#Edges:") != string::npos ||
                linha.find("#Arcs:") != string::npos ||
                linha.find("#Required") != string::npos)
                continue;
            // Define a seção atual conforme a palavra-chave
            if (linha.substr(0, 3) == "ReN" || linha.substr(0, 4) == "ReN.") {
                secao = "reqnodes";
                continue;
            }
            if (linha.substr(0, 3) == "ReE" || linha.substr(0, 4) == "ReE.") {
                secao = "reqedges";
                continue;
            }
            if (linha.substr(0, 4) == "EDGE") {
                secao = "edges";
                continue;
            }
            if (linha.substr(0, 4) == "ReA.") {
                secao = "reqarcs";
                continue;
            }
            if (linha.substr(0, 3) == "ARC") {
                secao = "arcs";
                continue;
            }
            processarLinha(linha, secao);
        }
        entrada.close();
        inicializarMatrizes();
    }

    // ---------------------------------------------------------------
    // CALCULA TODAS AS MÉTRICAS (exceto componentes conectados)
    // ---------------------------------------------------------------
    void calcularTodasMetricas() {
        metricas.qtd_vertices = total_nos;
        metricas.qtd_arestas = arestas.size();
        metricas.qtd_arcos = arcos.size();
        metricas.vertices_req = nos_requeridos.size();
        metricas.arestas_req = arestas_requeridas.size();
        metricas.arcos_req = arcos_requeridos.size();
        metricas.densidade = calcularDensidade();
        // Cálculo de componentes conectados removido conforme o enunciado
        calcularGraus();
        calcularIntermediacao();
        metricas.caminho_medio = calcularCaminhoMedio();
        metricas.diametro = calcularDiametro();
    }

    // ---------------------------------------------------------------
    // IMPRIME OS RESULTADOS EM UM std::ostream (pode ser cout ou ofstream)
    // ---------------------------------------------------------------
    void imprimirResultados(ostream &out) const {
        out << fixed << setprecision(2);
        out << "1. Vértices: " << metricas.qtd_vertices << "\n";
        out << "2. Arestas: " << metricas.qtd_arestas << "\n";
        out << "3. Arcos: " << metricas.qtd_arcos << "\n";
        out << "4. Vértices requeridos: " << metricas.vertices_req << "\n";
        out << "5. Arestas requeridas: " << metricas.arestas_req << "\n";
        out << "6. Arcos requeridos: " << metricas.arcos_req << "\n";
        out << "7. Densidade: " << metricas.densidade << "%\n";
        out << "8. Grau mínimo: " << metricas.grau_min << "\n";
        out << "9. Grau máximo: " << metricas.grau_max << "\n";
        out << "10. Intermediação:\n";
        for (const auto &par : metricas.intermediacao)
            out << "    " << par.first << ": " << par.second << "\n";
        out << "11. Caminho médio: " << metricas.caminho_medio << "\n";
        out << "12. Diâmetro: " << metricas.diametro << "\n";
    }

    // Exibe os resultados no console
    void exibirResultados() const {
        imprimirResultados(cout);
    }

    // Salva os resultados em um arquivo de texto
    void salvarResultados(const string &nome_arquivo) const {
        ofstream saida(nome_arquivo);
        if (!saida.is_open()) {
            cerr << "Erro ao abrir o arquivo " << nome_arquivo << " para salvar os resultados." << endl;
            return;
        }
        imprimirResultados(saida);
        saida.close();
    }
};

int main() {
    GrafoLogistica grafo;

    grafo.lerDados("data/dados.dat"); // Certifique-se de que o arquivo "dados.dat" esteja no mesmo diretório
    grafo.calcularTodasMetricas();
    
    // Exibe os resultados no console
    grafo.exibirResultados();
    
    // Salva os resultados em um arquivo TXT
    grafo.salvarResultados("results/resultados.txt");

    return 0;
}
