#ifndef ORCHESTRATOR_H
#define ORCHESTRATOR_H

#include <string>

class Orchestrator
{
public:
    Orchestrator();

    int run(const std::string &dataDir, const std::string &resultsDir, const std::string &solutionsDir);
};

#endif // ORCHESTRATOR_H
