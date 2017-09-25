#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <cmath>

int main()
{
    const static int NCLASSES = 3;
    const static int DATAPCLS = 20;
    const static int NDATA    = NCLASSES*DATAPCLS;
    const static int LOOPMAX  = 10;
    const static double INF   = 1.0e14;

    std::ofstream sample_file, repvec_file;
    std::string   sample_file_name = "sample.dat";
    std::string   repvec_file_name = "repvec.dat";

    double *x, *y;
    double *repx, *repy;
    int    **belonging;

    x = new double[NDATA];
    y = new double[NDATA];

    repx = new double[NCLASSES];
    repy = new double[NCLASSES];

    belonging = new int*[NCLASSES];
    for(int i=0; i < NCLASSES; i++) {
        belonging[i] = new int[NDATA];
    }


    /**************/
    /* Initialize */
    /**************/
    // Create Sample Data
    std::random_device seed_gen;
    std::default_random_engine engine(seed_gen());

    std::normal_distribution<> dist(0.0, 3.0);

    for (int i=0; i < NCLASSES; i++) {
        std::normal_distribution<> distx(dist(engine), 1.0);
        std::normal_distribution<> disty(dist(engine), 1.0);
        for (int j=0; j < DATAPCLS; j++) {
            x[j+i*DATAPCLS] = distx(engine);
            y[j+i*DATAPCLS] = disty(engine);
        }
    }

    // Initialize Representative Vector
    for (int i=0; i < NCLASSES; i++) {
        repx[i] = dist(engine);
        repy[i] = dist(engine);
    }


    /********/
    /* Loop */
    /********/
    for (int n=0; n < LOOPMAX; n++) {
        // E-step
        for (int i=0; i < NCLASSES; i++) {
            for (int j=0; j < NDATA; j++) {
                belonging[i][j] = 0;
            }
        }
        for (int i=0; i < NDATA; i++) {
            double tmpmin = INF;
            int    cls    = -1;
            for (int j=0; j < NCLASSES; j++) {
                double distance = 0.0;
                distance = std::pow((x[i]-repx[j]),2.0)
                         + std::pow((y[i]-repy[j]),2.0);
                if (distance < tmpmin) {
                    tmpmin = distance;
                    cls    = j; 
                }
            }
            belonging[cls][i] = 1;
        }

        // M-step
        for (int i=0; i < NCLASSES; i++) {
            double factor_x      = 0.0;
            double factor_y      = 0.0;
            double belonging_sum = 0.0;
            for (int j=0; j < NDATA; j++) {
                factor_x      += belonging[i][j]*x[j];
                factor_y      += belonging[i][j]*y[j];
                belonging_sum += belonging[i][j];
            }
            if (belonging_sum < -INF) {
                repx[i] = 0.0;
                repy[i] = 0.0;
            }
            else {
                repx[i] = factor_x/belonging_sum;
                repy[i] = factor_y/belonging_sum;
            }
        }
    }

    /**********/
    /* Output */
    /**********/
    sample_file.open(sample_file_name, std::ios::out);
    repvec_file.open(repvec_file_name, std::ios::out);
    for (int i=0; i < NDATA; i++) {
        sample_file << x[i] << " " << y[i] << std::endl;
    }
    for (int i=0; i < NCLASSES; i++) {
        repvec_file << repx[i] << " " << repy[i] << std::endl;
    }

    /************/
    /* Finalize */
    /************/
    delete[] x;
    delete[] y;
    delete[] repx;
    delete[] repy;
    for(int i=0; i < NCLASSES; i++){
        delete[] belonging[i];
        belonging[i] = 0;
    }
    delete[] belonging;
    x         = 0;
    y         = 0;
    repx      = 0;
    repy      = 0;
    belonging = 0;
}
