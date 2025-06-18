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
    for (int i = 0; i < dim; ++i) ff.position[i] = lo + (hi - lo) * dis(gen);
    ff.fitness = std::numeric_limits<double>::infinity();
}

void evaluate_firefly(Firefly& ff) {
    ff.fitness = cec17_fitness(ff.position.data());
    ++current_fes_counter;
}

// Memetic Solis-Wets local search
void memetic_local_search(Firefly& ff, const FireflyParams& params) {
    int dim = ff.position.size();
    double sigma = (params.upper_bound - params.lower_bound) * 0.1;
    std::vector<double> delta(dim, sigma);
    std::vector<double> probe(dim);
    double best = ff.fitness;

    while (std::any_of(delta.begin(), delta.end(), [](double d){return d>1e-6;}) 
           && current_fes_counter < params.max_fes) {
        for (int i = 0; i < dim; ++i) {
            std::normal_distribution<> nd(0.0, delta[i]);
            probe = ff.position;
            probe[i] = clamp_val(probe[i] + nd(gen), params.lower_bound, params.upper_bound);
            Firefly temp{probe, std::numeric_limits<double>::infinity()};
            evaluate_firefly(temp);
            if (temp.fitness < best) {
                ff = temp;
                best = temp.fitness;
                delta[i] *= 1.2;
            } else {
                delta[i] *= 0.5;
            }
        }
    }
}

// Elitist archive with reinjection
void elitist_archive(std::vector<Firefly>& swarm,
                     std::vector<Firefly>& archive,
                     int size) {
    archive.insert(archive.end(), swarm.begin(), swarm.end());
    std::sort(archive.begin(), archive.end(), [](auto& a, auto& b){return a.fitness<b.fitness;});
    if ((int)archive.size()>size) archive.resize(size);
    if (!archive.empty()) {
        std::uniform_int_distribution<> ud(0, archive.size()-1);
        int idx = ud(gen);
        auto worst_it = std::max_element(swarm.begin(), swarm.end(),
                             [](auto& a, auto& b){return a.fitness<b.fitness;});
        *worst_it = archive[idx];
    }
}

// Main Firefly run
double run_firefly_algorithm(int dim, int func_id,
                             const FireflyParams& params,
                             const std::string& alg_name) {
    cec17_init(alg_name.c_str(), func_id, dim);

    std::vector<Firefly> swarm(params.num_fireflies);
    for (auto& ff:swarm) initialize_firefly(ff, dim, params.lower_bound, params.upper_bound);

    current_fes_counter = 0;
    for (auto& ff:swarm) evaluate_firefly(ff);
    Firefly best = *std::min_element(swarm.begin(), swarm.end(),
                        [](auto&a,auto&b){return a.fitness<b.fitness;});
    long long last_imp = current_fes_counter;
    std::vector<Firefly> archive;
    long long print_step = std::max(1LL, params.max_fes/10);
    long long ls_budget = params.max_fes*0.2;
    int generation = 0;

    std::cout<<"Inicial -> best: "<<std::scientific<<best.fitness
             <<" (FEs: "<<current_fes_counter<<")\n";

    while(current_fes_counter<params.max_fes) {
        for(auto& fi:swarm) {
            if(current_fes_counter>=params.max_fes) break;
            std::vector<double> move(dim);
            for(auto& fj:swarm) if(fj.fitness<fi.fitness) {
                double r2=0;
                for(int k=0;k<dim;++k) r2+=std::pow(fi.position[k]-fj.position[k],2);
                double beta=params.beta0*std::exp(-params.gamma*std::sqrt(r2));
                for(int k=0;k<dim;++k) move[k]+=beta*(fj.position[k]-fi.position[k]);
            }
            double alpha_t=params.alpha*std::pow(0.97,generation);
            for(int k=0;k<dim;++k) {
                double rnd=(dis(gen)-0.5)*(params.upper_bound-params.lower_bound);
                fi.position[k]=clamp_val(fi.position[k]+move[k]+alpha_t*rnd,
                                         params.lower_bound, params.upper_bound);
            }
            evaluate_firefly(fi);
        }
        auto curr_best=*std::min_element(swarm.begin(), swarm.end(),
                             [](auto&a,auto&b){return a.fitness<b.fitness;});
        if(curr_best.fitness<best.fitness) {
            best=curr_best;
            last_imp=current_fes_counter;
            std::cout<<"Mejora global: "<<best.fitness
                     <<" en FEs="<<current_fes_counter<<"\n";
        }
        // Memetic hibridación
        if(params.mode==FireflyMode::LOCAL_SEARCH) {
            if((generation%5==0 && ls_budget>0) || (current_fes_counter-last_imp>print_step)) {
                memetic_local_search(best, params);
                ls_budget -= (current_fes_counter-last_imp);
            }
        }
        // Elitismo
        if(params.mode==FireflyMode::ELITISTA) {
            elitist_archive(swarm, archive, 5);
        }
        if(current_fes_counter%print_step==0) {
            std::cout<<"FEs "<<current_fes_counter
                     <<", best: "<<std::scientific<<best.fitness<<"\n";
        }
        ++generation;
    }
    std::cout<<"Final best F"<<func_id<<" D"<<dim
             <<": "<<std::scientific<<best.fitness
             <<" (FEs: "<<current_fes_counter<<")\n";
    std::cout<<"Error: "<<std::scientific<<cec17_error(best.fitness)<<"\n";
    return best.fitness;
}
