#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define MAX_X 50
#define MAX_Y 50
#define NUM_PHOTONS 1000000


typedef struct {
    double x, y;
    double dx, dy;
    double energy;
} Photon;


typedef struct {
    double density[MAX_X][MAX_Y];
} Sample;


void initializePhoton(Photon *photon);
void movePhoton(Photon *photon, Sample *sample, double detector[MAX_X]);
double randomUniform();
double calculateMeanFreePath(double density, double energy);
void interact(Photon *photon, double density);
void visualizeSample(Sample *sample);
void visualizeDetector(double detector[MAX_X]);
void outputDetectorData(double detector[MAX_X], const char* filename);  // Added this line


int main() {
    Sample sample;
    double detector[MAX_X] = {0};
    int i, j;

    srand(time(NULL));


    for (i = 0; i < MAX_X; i++) {
        for (j = 0; j < MAX_Y; j++) {
            double dx = i - MAX_X/2;
            double dy = j - MAX_Y/2;
            if (dx*dx + dy*dy < (MAX_X/4)*(MAX_Y/4)) {
                sample.density[i][j] = 1.0;
            } else {
                sample.density[i][j] = 0.1;
            }
        }
    }

    
    for (i = 0; i < NUM_PHOTONS; i++) {
        Photon photon;
        initializePhoton(&photon);
        movePhoton(&photon, &sample, detector);
        
        if (i % (NUM_PHOTONS / 10) == 0) {
            printf("Progress: %d%%\n", i * 100 / NUM_PHOTONS);
        }
    }

    printf("\nSimulation complete. Outputting detector data to file.\n");
    outputDetectorData(detector, "detector_data.txt");

    return 0;
}

void initializePhoton(Photon *photon) {
    photon->x = 0;
    photon->y = randomUniform() * MAX_Y;
    photon->dx = 1;
    photon->dy = 0;
    photon->energy = 100; // keV
}

void movePhoton(Photon *photon, Sample *sample, double detector[MAX_X]) {
    while (photon->x < MAX_X && photon->x >= 0 && photon->y < MAX_Y && photon->y >= 0) {
        int x = (int)photon->x;
        int y = (int)photon->y;
        
        double meanFreePath = calculateMeanFreePath(sample->density[x][y], photon->energy);
        double stepSize = -log(randomUniform()) * meanFreePath;

        photon->x += stepSize * photon->dx;
        photon->y += stepSize * photon->dy;

        if (photon->x >= MAX_X) {
            detector[((int)photon->y) % MAX_X] += photon->energy;
            break;
        }

        interact(photon, sample->density[x][y]);
    }
}

double randomUniform() {
    return (double)rand() / RAND_MAX;
}

double calculateMeanFreePath(double density, double energy) {
    // Simplified model: mean free path inversely proportional to density
    return 10.0 / (density * energy);
}

void interact(Photon *photon, double density) {
    // Simplified interaction model
    if (randomUniform() < 0.8) {
        // Compton scattering
        double angle = randomUniform() * M_PI;
        photon->dx = cos(angle);
        photon->dy = sin(angle);
        photon->energy *= 0.9; // Simplified energy loss
    } else {
        // Photoelectric absorption
        photon->energy = 0;
    }
}

void visualizeSample(Sample *sample) {
    for (int y = 0; y < MAX_Y; y++) {
        for (int x = 0; x < MAX_X; x++) {
            if (sample->density[x][y] > 0.5) {
                printf("█");
            } else {
                printf("·");
            }
        }
        printf("\n");
    }
}

void visualizeDetector(double detector[MAX_X]) {
    double max_value = 0;
    for (int i = 0; i < MAX_X; i++) {
        if (detector[i] > max_value) max_value = detector[i];
    }

    for (int i = 0; i < MAX_X; i++) {
        int height = (int)((detector[i] / max_value) * 20);
        printf("%2d | ", i);
        for (int j = 0; j < height; j++) {
            printf("█");
        }
        printf("\n");
    }
}

void outputDetectorData(double detector[MAX_X], const char* filename) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }
    for (int i = 0; i < MAX_X; i++) {
        fprintf(fp, "%d %f\n", i, detector[i]);
    }
    fclose(fp);
}