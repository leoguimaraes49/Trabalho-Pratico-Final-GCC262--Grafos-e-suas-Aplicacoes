#ifndef ROUTE_H
#define ROUTE_H

#include <vector>

struct Visita
{
    bool depot;
    int id, u, v;
};

class Route
{
private:
    std::vector<Visita> visitas;
    int demanda;
    int custo;

public:
    Route();

    void addVisita(const Visita &v);
    int getDemanda() const;
    int getCusto() const;
    const std::vector<Visita>& getVisitas() const;

    void setDemanda(int d);
    void setCusto(int c);
};

#endif // ROUTE_H
