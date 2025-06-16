// ~/MH-PractOpt/main.cpp

#include <iostream>  // Para std::cout, std::endl
#include <string>    // Para std::string
#include "firefly.h" // Incluir el encabezado de tu algoritmo Firefly

// El benchmark CEC2017 es manejado por firefly.cpp internamente a través de cec17.h,
// y se enlaza con la librería cec17_lib.

int main() {
    // Configura los parámetros del algoritmo Firefly
    FireflyParams params;
    params.num_fireflies = NUM_FIREFLIES_DEFAULT; // Usamos los valores por defecto de firefly.h
    params.alpha         = ALPHA_DEFAULT;
    params.beta0         = BETA0_DEFAULT;
    params.gamma         = GAMMA_DEFAULT;
    params.lower_bound   = LOWER_BOUND_DEFAULT;
    params.upper_bound   = UPPER_BOUND_DEFAULT;

    // Puedes variar estos para probar diferentes funciones y dimensiones
    int dimension = 10;   // Por ejemplo: 2, 5, 10, 30, 50, 100
    int function_id = 1;  // Por ejemplo: 1 a 30 (aunque F2 tiene inconsistencias)

    // Calcula el número máximo de evaluaciones de función según el estándar CEC2017
    params.max_fes = (long long)10000 * dimension;

    // Nombre del algoritmo para los archivos de salida del benchmark CEC2017
    std::string algorithm_name = "MyFireflyAlgorithm";

    std::cout << "-----------------------------------------------------" << std::endl;
    std::cout << "Iniciando experimento con Algoritmo Firefly" << std::endl;
    std::cout << "Función: F" << function_id << ", Dimensión: " << dimension << std::endl;
    std::cout << "Max FEs: " << params.max_fes << std::endl;
    std::cout << "-----------------------------------------------------" << std::endl;

    // Ejecutar el algoritmo Firefly
    double final_best_fitness = run_firefly_algorithm(dimension, function_id, params, algorithm_name);

    std::cout << "\n-----------------------------------------------------" << std::endl;
    std::cout << "Experimento finalizado." << std::endl;
    std::cout << "La mejor aptitud encontrada por " << algorithm_name << " para F" << function_id 
              << " (D=" << dimension << ") es: " << std::scientific << final_best_fitness << std::endl;
    std::cout << "-----------------------------------------------------" << std::endl;

    return 0;
}