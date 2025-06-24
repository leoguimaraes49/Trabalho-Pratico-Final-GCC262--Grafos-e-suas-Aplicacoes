#include "Optimizer.h"
#include <limits>
#include <algorithm>

using namespace std;

Optimizer::Optimizer(const Instance &inst, const Graph &g)
    : instance(inst), graph(g)
{}

// -----------------------------------------------------------------------------
// Construtor greedy (Etapa 2)
// -----------------------------------------------------------------------------
vector<Route> Optimizer::gerarRotas()
{
    vector<Servico> local = instance.getServicos();
    vector<Route>   rotas;
    int faltam = static_cast<int>(local.size());

    const auto &dist = graph.getDistancias();
    const int  depot = instance.getDepot();           // ← depósito correto (0-based)

    while (faltam)
    {
        Route R;
        R.addVisita({true, 0, depot, depot});         // começa na depot real
        int pos  = depot;
        int cap  = instance.getCapacidadeVeiculo();
        int demanda_total = 0;
        int custo_total   = 0;

        while (true)
        {
            int    idx  = -1;
            double best = numeric_limits<double>::max();

            for (size_t i = 0; i < local.size(); ++i)
            {
                auto &S = local[i];
                if (S.atendido || S.demanda > cap) continue;

                int dead = min(dist[pos][S.u], dist[pos][S.v]);
                double sc = double(dead + S.custo) / max(1, S.demanda);

                if (sc < best) { best = sc; idx = static_cast<int>(i); }
            }

            if (idx < 0) break;   // não achou mais serviço viável

            auto &S = local[idx];
            bool inv = dist[pos][S.v] < dist[pos][S.u];
            int  a   = inv ? S.v : S.u;
            int  b   = inv ? S.u : S.v;

            custo_total   += dist[pos][a] + S.custo;
            demanda_total += S.demanda;
            R.addVisita({false, S.id, a, b});

            pos  = b;
            cap -= S.demanda;
            S.atendido = true;
            --faltam;
        }

        custo_total += dist[pos][depot];
        R.addVisita({true, 0, depot, depot});         // termina na depot real
        R.setCusto(custo_total);
        R.setDemanda(demanda_total);
        rotas.push_back(std::move(R));
    }
    return rotas;
}

// -----------------------------------------------------------------------------
// Soma dos custos das rotas
// -----------------------------------------------------------------------------
int Optimizer::custoTotal(const vector<Route> &rotas) const
{
    int total = 0;
    for (const auto &r : rotas) total += r.getCusto();
    return total;
}
