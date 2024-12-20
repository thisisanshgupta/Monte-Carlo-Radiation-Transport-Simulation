#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm> // for std::max_element

#define MAX_X 50
#define MAX_Y 50
#define NUM_PHOTONS 1000000

struct Photon {
    double x, y;
    double dx, dy;
    double energy;
};

struct Sample {
    double density[MAX_X][MAX_Y];
};

void initializePhoton(Photon &photon);
void movePhoton(Photon &photon, Sample &sample, std::vector<double> &detector);
double randomUniform();
double calculateMeanFreePath(double density, double energy);
void interact(Photon &photon, double density);
void visualizeSample(const Sample &sample);
void visualizeDetector(const std::vector<double> &detector);
void outputDetectorData(const std::vector<double> &detector, const std::string &filename);

int main() {
    Sample sample;
    std::vector<double> detector(MAX_X, 0);

    srand(static_cast<unsigned>(time(nullptr)));

    // Initialize the sample density
    for (int i = 0; i < MAX_X; i++) {
        for (int j = 0; j < MAX_Y; j++) {
            double dx = i - MAX_X / 2;
            double dy = j - MAX_Y / 2;
            if (dx * dx + dy * dy < (MAX_X / 4) * (MAX_Y / 4)) {
                sample.density[i][j] = 1.0;
            } else {
                sample.density[i][j] = 0.1;
            }
        }
    }

    // Simulate photon movements
    for (int i = 0; i < NUM_PHOTONS; i++) {
        Photon photon;
        initializePhoton(photon);
        movePhoton(photon, sample, detector);

        if (i % (NUM_PHOTONS / 10) == 0) {
            std::cout << "Progress: " << (i * 100 / NUM_PHOTONS) << "%\n";
        }
    }

    std::cout << "\nSimulation complete. Outputting detector data to file.\n";
    outputDetectorData(detector, "detector_data.txt");

    return 0;
}

void initializePhoton(Photon &photon) {
    photon.x = 0;
    photon.y = randomUniform() * MAX_Y;
    photon.dx = 1;
    photon.dy = 0;
    photon.energy = 100; // keV
}

void movePhoton(Photon &photon, Sample &sample, std::vector<double> &detector) {
    while (photon.x < MAX_X && photon.x >= 0 && photon.y < MAX_Y && photon.y >= 0) {
        int x = static_cast<int>(photon.x);
        int y = static_cast<int>(photon.y);

        double meanFreePath = calculateMeanFreePath(sample.density[x][y], photon.energy);
        double stepSize = -std::log(randomUniform()) * meanFreePath;

        photon.x += stepSize * photon.dx;
        photon.y += stepSize * photon.dy;

        if (photon.x >= MAX_X) {
            detector[static_cast<int>(photon.y) % MAX_X] += photon.energy;
            break;
        }

        interact(photon, sample.density[x][y]);
    }
}

double randomUniform() {
    return static_cast<double>(rand()) / RAND_MAX;
}

double calculateMeanFreePath(double density, double energy) {
    return 10.0 / (density * energy);
}

void interact(Photon &photon, double density) {
    if (randomUniform() < 0.8) {
        double angle = randomUniform() * M_PI;
        photon.dx = std::cos(angle);
        photon.dy = std::sin(angle);
        photon.energy *= 0.9; // Simplified energy loss
    } else {
        photon.energy = 0; // Photoelectric absorption
    }
}

void visualizeSample(const Sample &sample) {
    for (int y = 0; y < MAX_Y; y++) {
        for (int x = 0; x < MAX_X; x++) {
            std::cout << (sample.density[x][y] > 0.5 ? "█" : "·");
        }
        std::cout << '\n';
    }
}

void visualizeDetector(const std::vector<double> &detector) {
    double max_value = *std::max_element(detector.begin(), detector.end());
    for (size_t i = 0; i < detector.size(); i++) {
        int height = static_cast<int>((detector[i] / max_value) * 20);
        std::cout << i << " | ";
        for (int j = 0; j < height; j++) {
            std::cout << " ";
        }
        std::cout << '\n';
    }
}

void outputDetectorData(const std::vector<double> &detector, const std::string &filename) {
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "Error opening file!\n";
        return;
    }
    for (size_t i = 0; i < detector.size(); i++) {
        file << i << " " << detector[i] << '\n';
    }
}
