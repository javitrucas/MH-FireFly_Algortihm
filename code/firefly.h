// firefly.h
#ifndef FIREFLY_H
#define FIREFLY_H

#include <vector>
#include <string>

#define NUM_FIREFLIES_DEFAULT 40
#define ALPHA_DEFAULT         0.5
#define BETA0_DEFAULT         1.0
#define GAMMA_DEFAULT         0.1
#define LOWER_BOUND_DEFAULT  -100.0
#define UPPER_BOUND_DEFAULT   100.0

// Tres modos de ejecución:
enum class FireflyMode { BASIC, LOCAL_SEARCH, ELITISTA };

struct Firefly {
    std::vector<double> position;
    double fitness;
};

struct FireflyParams {
    int num_fireflies;
    double alpha;
    double beta0;
    double gamma;
    double lower_bound;
    double upper_bound;
    long long max_fes;
    int T;                // para local search
    FireflyMode mode;     // modo de ejecución
};

// Devuelve el mejor fitness encontrado
// El modo se obtiene de params.mode
double run_firefly_algorithm(int dim, int func_id,
                             const FireflyParams& params,
                             const std::string& alg_name);

#endif // FIREFLY_H
