#ifndef OPTIMIZER_H
#define OPTIMIZER_H

#include <vector>
#include "Instance.h"
#include "Graph.h"
#include "Route.h"

class Optimizer
{
private:
    const Instance &instance;
    const Graph &graph;

public:
    Optimizer(const Instance &inst, const Graph &g);

    std::vector<Route> gerarRotas();
    int custoTotal(const std::vector<Route> &rotas) const;
};

#endif // OPTIMIZER_H
