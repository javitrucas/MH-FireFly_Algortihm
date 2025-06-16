// ~/MH-PractOpt/firefly.h

#ifndef FIREFLY_H
#define FIREFLY_H

#include <vector> // Para std::vector
#include <string> // Para std::string

// Definiciones de parámetros del algoritmo Firefly
// Puedes hacer que estos sean configurables o pasarlos como argumentos a las funciones
// Por ahora, como defines constantes, las mantenemos aquí
#define NUM_FIREFLIES_DEFAULT 40      // Número de luciernagas
#define ALPHA_DEFAULT         0.5     // Parámetro de aleatoriedad
#define BETA0_DEFAULT         1.0     // Atractividad base (beta_0)
#define GAMMA_DEFAULT         0.01    // Coeficiente de absorción de luz (gamma)
#define LOWER_BOUND_DEFAULT  -100.0   // Límite inferior del espacio de búsqueda (CEC2017)
#define UPPER_BOUND_DEFAULT   100.0   // Límite superior del espacio de búsqueda (CEC2017)

// Estructura para representar una luciernaga
struct Firefly {
    std::vector<double> position; // Posición en el espacio de búsqueda (vector de dimensiones)
    double fitness;               // Valor de aptitud (brillo)
    int id;                       // Identificador (opcional, para depuración)
};

// Estructura para configurar los parámetros del algoritmo Firefly
struct FireflyParams {
    int num_fireflies;
    double alpha;
    double beta0;
    double gamma;
    double lower_bound;
    double upper_bound;
    long long max_fes; // Máximo de evaluaciones de función
};

// Prototipos de funciones del algoritmo Firefly
// La función de evaluación se pasará como un puntero a función
// o a través de un objeto para mayor flexibilidad. Por ahora, asumiremos
// que el entorno CEC2017 ya ha sido inicializado.

/**
 * @brief Ejecuta el algoritmo Firefly.
 *
 * @param dim La dimensión del problema de optimización.
 * @param func_id El ID de la función CEC2017 a evaluar.
 * @param params Parámetros de configuración del algoritmo Firefly.
 * @param alg_name Nombre del algoritmo para la salida de CEC2017 (e.g., "Firefly").
 * @return El mejor valor de aptitud encontrado.
 */
double run_firefly_algorithm(int dim, int func_id, const FireflyParams& params, const std::string& alg_name);

#endif // FIREFLY_H