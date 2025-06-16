// ~/MH-PractOpt/firefly.cpp

#include "firefly.h"
#include "code/cec17.h" // Incluir el encabezado de CEC2017
#include <iostream>
#include <cmath>     // Para pow, sqrt, exp
#include <random>    // Para generación de números aleatorios moderna en C++
#include <limits>    // Para std::numeric_limits

#include "cec17.h"

// Global counter for function evaluations (could also be passed around)
// For simplicity in this example, it's global, but passing it is safer.
long long current_fes_counter = 0;

// Generador de números aleatorios de C++11 (más robusto que rand())
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0.0, 1.0); // Para números entre 0 y 1

// Función para inicializar una luciernaga con posiciones aleatorias
void initialize_firefly(Firefly *ff, int dim, double lower_bound, double upper_bound) {
    ff->position.resize(dim);
    for (int i = 0; i < dim; i++) {
        ff->position[i] = lower_bound + (upper_bound - lower_bound) * dis(gen);
    }
    ff->fitness = std::numeric_limits<double>::max(); // Inicializar con un valor muy alto para minimización
}

// Función para evaluar el fitness de una luciernaga usando el benchmark CEC2017
void evaluate_firefly(Firefly *ff, int dim, int func_id) {
    ff->fitness = cec17_fitness(ff->position.data()); // Asigna directamente el resultado de la función
    current_fes_counter++; // Incrementa el contador de evaluaciones de función
}

// Implementación del algoritmo Firefly
double run_firefly_algorithm(int dim, int func_id, const FireflyParams& params, const std::string& alg_name) {
    // Inicializar el benchmark CEC2017
    cec17_init(alg_name.c_str(), func_id, dim);
    
    cec17_print_output(); 

    // Crear la población de luciernagas
    std::vector<Firefly> fireflies(params.num_fireflies);
    for (int i = 0; i < params.num_fireflies; i++) {
        fireflies[i].id = i;
        initialize_firefly(&fireflies[i], dim, params.lower_bound, params.upper_bound);
    }

    // Evaluación inicial de la población
    current_fes_counter = 0; // Reset counter for this run
    for (int i = 0; i < params.num_fireflies; i++) {
        evaluate_firefly(&fireflies[i], dim, func_id);
    }

    // Encontrar la mejor luciernaga (el mejor fitness, asumiendo minimización)
    Firefly global_best_firefly = fireflies[0];
    for (int i = 1; i < params.num_fireflies; i++) {
        if (fireflies[i].fitness < global_best_firefly.fitness) {
            global_best_firefly = fireflies[i];
        }
    }

    std::cout << "Iniciando algoritmo Firefly para F" << func_id << ", Dim " << dim << "..." << std::endl;
    std::cout << "Mejor fitness inicial: " << std::scientific << global_best_firefly.fitness << " (FEs: " << current_fes_counter << ")" << std::endl;

    // Bucle principal del algoritmo Firefly
    while (current_fes_counter < params.max_fes) {
        for (int i = 0; i < params.num_fireflies; i++) {
            for (int j = 0; j < params.num_fireflies; j++) {
                if (fireflies[i].fitness > fireflies[j].fitness) { // Luciernaga 'i' es menos brillante que 'j'
                    double r_ij_sq = 0.0; // Distancia al cuadrado
                    for (int k = 0; k < dim; k++) {
                        r_ij_sq += std::pow(fireflies[i].position[k] - fireflies[j].position[k], 2);
                    }
                    double r_ij = std::sqrt(r_ij_sq);

                    // Atractividad: beta = beta0 * exp(-gamma * r^2)
                    double beta = params.beta0 * std::exp(-params.gamma * r_ij_sq);

                    // Componente de movimiento aleatorio (epsilon)
                    // (rand() - 0.5) * (Upper - Lower) es una forma común de escalarlo al rango
                    double random_walk_factor = params.alpha * (dis(gen) - 0.5) * (params.upper_bound - params.lower_bound);

                    // Guardar la posición antigua para posible reversión si el nuevo fitness es peor
                    // std::vector<double> old_position = fireflies[i].position; // No usado en esta versión estándar de FA
                    
                    // Actualizar posición
                    for (int k = 0; k < dim; k++) {
                        fireflies[i].position[k] += beta * (fireflies[j].position[k] - fireflies[i].position[k]) + random_walk_factor;
                        
                        // Asegurarse de que la posición se mantenga dentro de los límites
                        if (fireflies[i].position[k] < params.lower_bound) {
                            fireflies[i].position[k] = params.lower_bound;
                        }
                        if (fireflies[i].position[k] > params.upper_bound) {
                            fireflies[i].position[k] = params.upper_bound;
                        }
                    }

                    // Evaluar la nueva posición
                    evaluate_firefly(&fireflies[i], dim, func_id);

                    // Si la nueva posición es peor, revertir (esto no es estándar en FA, pero puede ser una mejora)
                    // O mantener la nueva posición y el nuevo fitness, y el mejor global se actualizará después.
                    // Para el FA estándar, simplemente aceptamos el movimiento.
                    
                    // Actualizar el mejor global si es necesario
                    if (fireflies[i].fitness < global_best_firefly.fitness) {
                        global_best_firefly = fireflies[i];
                    }
                    
                    // Salir del bucle j si se alcanzó el máximo de FEs
                    if (current_fes_counter >= params.max_fes) break;
                }
            }
            if (current_fes_counter >= params.max_fes) break;
        }
        
        // Imprimir el mejor fitness cada cierto número de FEs o iteraciones
        if (current_fes_counter % (params.max_fes / 10) < params.num_fireflies && current_fes_counter > 0) { 
            // Esto intenta imprimir ~10 veces durante la ejecución
            std::cout << "Mejor fitness actual: " << std::scientific << global_best_firefly.fitness << " (FEs: " << current_fes_counter << ")" << std::endl;
        }
    }

    std::cout << "\nAlgoritmo Firefly finalizado para F" << func_id << ", Dim " << dim << "." << std::endl;
    std::cout << "Mejor fitness final: " << std::scientific << global_best_firefly.fitness << " (FEs: " << current_fes_counter << ")" << std::endl;

    // Calcular y mostrar el error (respecto al óptimo conocido)
    double error = cec17_error(global_best_firefly.fitness);
    std::cout << "Error con respecto al óptimo conocido: " << std::scientific << error << std::endl;

    return global_best_firefly.fitness;
}