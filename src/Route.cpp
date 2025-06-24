#include "Route.h"

Route::Route()
    : demanda(0), custo(0)
{
}

void Route::addVisita(const Visita &v)
{
    visitas.push_back(v);
}

int Route::getDemanda() const
{
    return demanda;
}

int Route::getCusto() const
{
    return custo;
}

const std::vector<Visita>& Route::getVisitas() const
{
    return visitas;
}

void Route::setDemanda(int d)
{
    demanda = d;
}

void Route::setCusto(int c)
{
    custo = c;
}
