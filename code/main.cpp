#include <iostream>
#include <string>
#include <filesystem>
#include <regex>
#include <vector>
#include <fstream>
#include <iomanip>
#include "firefly.h"

namespace fs = std::filesystem;

void crear_directorio_si_no_existe(const fs::path& dir) {
    if (fs::exists(dir)) {
        if (!fs::is_directory(dir)) {
            std::cerr << "Error: " << dir << " existe pero no es un directorio.\n";
            std::exit(EXIT_FAILURE);
        }
    } else {
        fs::create_directories(dir);
    }
}

std::string modo_a_string(FireflyMode modo) {
    switch (modo) {
        case FireflyMode::BASIC:        return "basic";
        case FireflyMode::LOCAL_SEARCH: return "local_search";
        case FireflyMode::ELITISTA:     return "elitista";
    }
    return "unknown";
}

int main() {
    fs::path data_dir = "input_data";
    std::vector<fs::path> files;

    // Obtener archivos del directorio
    for (const auto& entry : fs::directory_iterator(data_dir)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path());
        }
    }

    // Ordenar alfabéticamente
    std::sort(files.begin(), files.end(), [](const auto& a, const auto& b) {
        return a.filename().string() < b.filename().string();
    });

    const std::regex m_format(R"(M_(\d+)_D(\d+)\.txt)");
    std::vector<FireflyMode> modos = {
        FireflyMode::BASIC,
        FireflyMode::LOCAL_SEARCH,
        FireflyMode::ELITISTA
    };

    for (const auto& file_path : files) {
        std::string filename = file_path.filename().string();
        std::smatch match;

        if (!std::regex_match(filename, match, m_format)) {
            std::cout << "⚠️ No válido: " << filename << " → omitiendo\n";
            continue;
        }

        int f = std::stoi(match[1]);
        int dim = std::stoi(match[2]);

        if (dim != 10 && dim != 30 && dim != 50) {
            std::cout << "❌ Dimensión ignorada (" << dim << "): " << filename << "\n";
            continue;
        }

        for (FireflyMode modo : modos) {
            FireflyParams params;
            params.num_fireflies = NUM_FIREFLIES_DEFAULT;
            params.alpha         = ALPHA_DEFAULT;
            params.beta0         = BETA0_DEFAULT;
            params.gamma         = GAMMA_DEFAULT;
            params.lower_bound   = -100.0;
            params.upper_bound   = 100.0;
            params.max_fes       = 10000LL * dim;
            params.T             = 10;
            params.mode          = modo;

            std::string modo_str = modo_a_string(modo);
            std::string alg_name = "MyFireflyD" + std::to_string(dim) + "_" + modo_str;
            fs::path results_dir = "results_" + alg_name;

            crear_directorio_si_no_existe(results_dir);

            fs::path output_file = results_dir / ("results_" + std::to_string(f) + "_" + std::to_string(dim) + ".txt");

            if (fs::exists(output_file)) {
                std::cout << "✅ Ya existe: " << output_file << " → omitiendo\n";
                continue;
            }

            std::cout << "=====================================================\n";
            std::cout << "Función: F" << f
                      << " | Dim=" << dim
                      << " | Modo=" << modo_str
                      << " | MaxFEs=" << params.max_fes << "\n";

            // Esta función debe encargarse de escribir el archivo completo con múltiples milestones
            run_firefly_algorithm(dim, f, params, alg_name);
        }
    }

    return 0;
}
