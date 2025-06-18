// firefly.cpp
#include "firefly.h"
#include "cec17.h"
#include <iostream>
#include <vector>
#include <random>
#include <limits>
#include <cmath>
#include <algorithm>

// Generadores globales
static long long current_fes_counter = 0;
static std::mt19937 gen{std::random_device{}()};
static std::uniform_real_distribution<> dis(0.0, 1.0);

inline double clamp_val(double x, double lo, double hi) {
    return std::min(std::max(x, lo), hi);
}

void initialize_firefly(Firefly& ff, int dim, double lo, double hi) {
    ff.position.resize(dim);
    for (int i = 0; i < dim; ++i)
        ff.position[i] = lo + (hi - lo) * dis(gen);
    ff.fitness = std::numeric_limits<double>::infinity();
}

void evaluate_firefly(Firefly& ff) {
    ff.fitness = cec17_fitness(ff.position.data());
    ++current_fes_counter;
}

// Búsqueda local simple: pequeño descenso por coordenadas
double local_search(Firefly& ff, const FireflyParams& params) {
    double best = ff.fitness;
    for (int t = 0; t < params.T; ++t) {
        for (int i = 0; i < (int)ff.position.size(); ++i) {
            double orig = ff.position[i];
            double step = (params.upper_bound - params.lower_bound) * 0.01;
            ff.position[i] = clamp_val(orig + step, params.lower_bound, params.upper_bound);
            evaluate_firefly(ff);
            if (ff.fitness < best) {
                best = ff.fitness;
            } else {
                ff.position[i] = orig;
                ff.fitness = best;
            }
        }
    }
    return best;
}

// Elitismo: preservar siempre el mejor global
void elitist_preserve(std::vector<Firefly>& swarm, const Firefly& global_best) {
    auto worst_it = std::max_element(swarm.begin(), swarm.end(),
        [](auto &a, auto &b){ return a.fitness < b.fitness; });
    if (global_best.fitness < worst_it->fitness)
        *worst_it = global_best;
}

double run_firefly_algorithm(int dim, int func_id,
                             const FireflyParams& params,
                             const std::string& alg_name) {
    cec17_init(alg_name.c_str(), func_id, dim);

    std::vector<Firefly> fireflies(params.num_fireflies);
    for (auto& ff : fireflies)
        initialize_firefly(ff, dim, params.lower_bound, params.upper_bound);

    current_fes_counter = 0;
    for (auto& ff : fireflies)
        evaluate_firefly(ff);

    Firefly best = *std::min_element(fireflies.begin(), fireflies.end(),
                    [](const Firefly& a, const Firefly& b) {
                        return a.fitness < b.fitness;
                    });

    std::cout << "Inicial -> best: " << std::scientific << best.fitness
              << " (FEs: " << current_fes_counter << ")\n";

    int generation = 0;
    long long print_step = std::max(1LL, params.max_fes / 10);

    while (current_fes_counter < params.max_fes) {
        for (auto& fi : fireflies) {
            if (current_fes_counter >= params.max_fes) break;
            // movimiento de Firefly básico
            std::vector<double> move(dim, 0.0);
            for (const auto& fj : fireflies) {
                if (fj.fitness < fi.fitness) {
                    double r2 = 0.0;
                    for (int k = 0; k < dim; ++k)
                        r2 += std::pow(fi.position[k] - fj.position[k], 2);
                    double beta = params.beta0 * std::exp(-params.gamma * std::sqrt(r2));
                    for (int k = 0; k < dim; ++k)
                        move[k] += beta * (fj.position[k] - fi.position[k]);
                }
            }
            double alpha_t = params.alpha * std::pow(0.97, generation);
            for (int k = 0; k < dim; ++k) {
                double rnd = (dis(gen) - 0.5) * (params.upper_bound - params.lower_bound);
                fi.position[k] = clamp_val(fi.position[k] + move[k] + alpha_t * rnd,
                                           params.lower_bound, params.upper_bound);
            }
            evaluate_firefly(fi);
        }

        // Modificaciones por modo
        if (params.mode == FireflyMode::LOCAL_SEARCH) {
            double ls_best = local_search(best, params);
            best.fitness = std::min(best.fitness, ls_best);
        } else if (params.mode == FireflyMode::ELITISTA) {
            elitist_preserve(fireflies, best);
        }

        // actualizar mejor global
        Firefly current_best = *std::min_element(fireflies.begin(), fireflies.end(),
                    [](const Firefly& a, const Firefly& b) {
                        return a.fitness < b.fitness;
                    });
        if (current_best.fitness < best.fitness)
            best = current_best;

        if (current_fes_counter % print_step == 0) {
            std::cout << "FEs " << current_fes_counter
                      << ", best: " << std::scientific << best.fitness << "\n";
        }
        ++generation;
    }

    std::cout << "Final best F" << func_id << " D" << dim
              << ": " << std::scientific << best.fitness
              << " (FEs: " << current_fes_counter << ")\n";
    std::cout << "Error: " << std::scientific << cec17_error(best.fitness) << "\n";

    return best.fitness;
}
