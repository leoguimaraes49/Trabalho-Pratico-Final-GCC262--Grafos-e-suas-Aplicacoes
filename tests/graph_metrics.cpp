#include "Graph.h"

#include <cassert>
#include <string>
#include <tuple>
#include <vector>

int main()
{
    const std::vector<std::string> nos = {"1", "2", "3"};
    const std::vector<std::tuple<int, int, int>> arestas = {{0, 1, 1}, {1, 2, 1}};
    Graph grafo(nos, arestas, {}, {}, {}, {});

    const auto antes = grafo.getMetricas().intermediacao;
    assert(antes.at("1") == 0.0);
    assert(antes.at("2") == 2.0);
    assert(antes.at("3") == 0.0);

    grafo.calcularTodasMetricas();
    assert(grafo.getMetricas().intermediacao == antes);
}
