#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>

#include "statistics.h"
#include "time_series.h"
#include "attractor.h"
#include "rqa.h"
#include "statistics/statistics.h"

#define TRUE 1
#define FALSE 0

using namespace std;

int main(int argc, char* argv[]) {
    bool from_file = false;
    std::vector<std::vector<double>> data;

    TimeSeries *time_series = NULL;
    double rows = 0,column = 0;
    unsigned bins = 0;
    Attractor *attractor = NULL;
    RecurrencePlot *rp;
    std::string file_name;
    unsigned dimension = 0;
    unsigned delay = 0;
    unsigned window = 1;
    double threshold = 0;
    //time series statistics
    Mean<double> ts_mean;
    StDeviation<double> ts_std;
    //use a multiplot of standard deviation as threshold
    double th_std = 0;
    //###########################################################################################
    //                                        Setting Data Up
    //###########################################################################################
    for (size_t i = 1; i < argc; ++i) {
        if(std::string(argv[i]) == "--dimension" || std::string(argv[i]) == "-d")
            if(i + 1 < argc) { 
                dimension = atoi(argv[i + 1]);
                std::cerr << "Dimension: " << dimension << std::endl;
            }

        if(std::string(argv[i]) == "--delay" || std::string(argv[i]) == "-tau")
            if(i + 1 < argc) {
                delay = atoi(argv[i + 1]);
                std::cerr << "Delay: " << delay << std::endl;
            }

        if(std::string(argv[i]) == "--threshold" || std::string(argv[i]) == "-th")
            if(i + 1 < argc) {
                threshold = atof(argv[i + 1]);
                std::cerr << "Threshold: " << threshold << std::endl;
            }

        if(std::string(argv[i]) == "--window" || std::string(argv[i]) == "-w")
            if(i + 1 < argc) {
                window = atof(argv[i + 1]);
                std::cerr << "Window: " << window << std::endl;
            }

        if(std::string(argv[i]) == "--threshold_std" || std::string(argv[i]) == "-th_std")
            if(i + 1 < argc) {
                th_std = atof(argv[i + 1]);
                //setting a fake threshold
                threshold = 1000;
                std::cerr << "Th_std: " << th_std << std::endl;
            }



        if(std::string(argv[i]) == "--column" || std::string(argv[i]) == "-c")
            if(i + 1 < argc) 
                column = atoi(argv[i + 1]);

        if( std::string(argv[i]) == "--bins")
            if(i + 1 < argc) 
                bins = atoi(argv[i + 1]);

        if(std::string(argv[i]) == "--file" || std::string(argv[i]) == "-f")
            if(i + 1 < argc){ 
                data =  ReadFile<double>(std::string(argv[i+1]));
                file_name = std::string(argv[i+1]);
                std::cerr << "Reading file:" << file_name  << std::endl;
                from_file = true; 
            }
        if(std::string(argv[i]) == "--file_name" || std::string(argv[i]) == "-name")
            file_name = std::string(argv[i+1]);

    }
    if(!from_file)
        data = ReadStdin<double>();


    //Creating basics objects
    for (size_t i = 1; i < argc; ++i) {
        if(std::string(argv[i]) == "-ts" || std::string(argv[i]) == "--time_series"){
            time_series = new  TimeSeries(data, column);
        }
        if(std::string(argv[i]) == "-att" || std::string(argv[i]) == "--attractor"){
            attractor = new  Attractor(data);
        }
    }

    if(time_series){
        for (size_t i = 0; i < time_series->size(); ++i)
        {
            ts_mean((*time_series)[i]);
            ts_std((*time_series)[i]);
        }
    }

    //Using time series
    for (size_t i = 1; i < argc; ++i)
    {
        if(std::string(argv[i]) == "-att_from_ts"){
            if(time_series) {
                attractor = new Attractor(*time_series, dimension, delay);
                std::cerr << ">> Conjurating Attractor From Time Series" << std::endl;
            }
        }
    }
    //Using attractor
    if(threshold == 0){
        if(attractor) {
            std::cerr << ">> Trying to gess the threshold" << std::endl;
            threshold = FindThreshold(*attractor, 2, 0.1);
        }
    }
    if(th_std != 0){
        if(time_series) {
            threshold = ts_std * th_std;
        }
    }
    for (size_t i = 1; i < argc; ++i)
    {
        if(std::string(argv[i]) == "-rp_from_att"){
            if(attractor){
                std::cerr << ">> Conjurating Recurrence Plot From Attractor" << std::endl;
                rp = new RecurrencePlot(*attractor, threshold);
            }
            
            if( (i + 1) <= argc && (std::string(argv[i + 1]) == "--print" || std::string(argv[i + 1]) == "-p"))
                rp->PrintOnScreen();
        }
    }

    //###########################################################################################
    //                                       Making Analysis 
    //###########################################################################################
    for(size_t i = 1; i < argc; ++i)
    {
        if((std::string(argv[i]) == "--mutual_information") || (std::string(argv[i]) == "-mi")){
            double normalize = 1;
            if(i + 1 < argc) 
                if( std::string(argv[i + 1]) == "--normalize")
                    normalize = MutualInformation(*time_series, 0, bins); 

            std::cout << "#delay x mutual_info" << std::endl;
            for (size_t i = 0; i < 0.01 * time_series->size(); ++i)
                std::cout << i << " " << MutualInformation(*time_series, i, bins) / normalize << std::endl; 
        } 

        if((std::string(argv[i]) == "--FalseNearestNeighbors") || (std::string(argv[i]) == "-fnn")){
            std::cerr << ">> Nearest Neighbors Max Dimension: " << dimension  << std::endl; 
            std::cerr << ">> Delay: " << delay  << std::endl; 
            std::cerr << ">> Threshold: " << threshold << std::endl; 
            std::vector<unsigned> nff =  FalseNearestNeighbors(*time_series, delay, dimension, threshold, false);
            for (size_t i = 0; i < nff.size(); ++i)
                std::cout << i + 1 << " " << nff[i] << std::endl; 
        } 

        if((std::string(argv[i]) == "--patterns_measures") || (std::string(argv[i]) == "-pm")) {
            if(rp){
/*
                for (size_t i = 0; i < rp->size(); ++i)
                    for (size_t j = i; j < rp->size(); ++j)
                        (*rp)[i][j] = 0;

             //Treiler window
             for (size_t i = window; i < rp->size(); ++i)
                    for (size_t j = i; j > i - window - 1; --j)
                        (*rp)[i][j] = 0;
             for (size_t i = 0; i < window; ++i)
                    for (size_t j = i; j != -1 ; --j)
                        (*rp)[i][j] = 0;
              
*/
                RecurrenceAnalytics analytics(*rp);
                std::cout << file_name  << " "; 
                std::cout << analytics.RR()  << " "; 
                std::cout << analytics.DET()  << " "; 
                std::cout << analytics.L()  << " "; 
                std::cout << analytics.LMax()  << " "; 
                std::cout << " "; 
                std::cout << analytics.LAM()  << " "; 
                std::cout << analytics.TT()  << " "; 
                std::cout << analytics.VMax()  << " "; 
                std::cout << " "; 
                std::cout << analytics.RATIO()  << " "; 
                std::cout << analytics.NumberOfDiagonals()  << " "; 
                std::cout << analytics.NumberOfRecurrence()  << " "; 
                //std::cout << ts_mean  << " "; 
                //std::cout << ts_std << " "; 

                std::cout << std::endl; 
            }

        }
        /*
           if((std::string(argv[i]) == "--cross_correlation") && (i+2 < argc)){
           TimeSeries ts1(std::string(argv[i+1]));
           if(i + 2 < argc && std::string(argv[i+2]) == "--normalize" )
           normalize = MutualInformation(ts,0); 
           std::cout << "#delay mutual_info" << std::endl;
           for (size_t i = 0; i < 0.01*ts.Size(); ++i)
           {
           std::cout << i << " " << MutualInformation(ts,i) / normalize << std::endl; 
           }

           } 
           */
    }
    delete time_series;
    delete attractor;
    return 0; 
}

