#pragma once

#include <iostream>
#include <cstdio>
#include <limits>
#include <complex>
#include <vector>
#include <algorithm>
#include <ctime>
#include <fstream>
#include <cmath>
#include <cstring>

namespace CommFunc
{
	  double Abs(const double &x);
    double sum(const std::vector<double> &x);
    int    sum(const std::vector<int> &x);
    double mean(const std::vector<double> &x);
    double median(const std::vector<double> &x);
    double var(const std::vector<double> &x);
    double sd(const std::vector<double> &x);
    double cov(const std::vector<double> &x, const std::vector<double> &y);
    double cor(const std::vector<double> &x, const std::vector<double> &y);
	  bool FloatEqual(double lhs, double rhs);
	  bool FloatNotEqual(double lhs, double rhs);
	  double Sqr(const double &a);
	  double Max(const double &a, const double &b);
	  double Min(const double &a, const double &b);
	  double Sign(const double &a, const double &b);
	  int rand_seed(); //positive value, return random seed using the system time
    void FileExist(std::string filename);
    std::vector<unsigned long int> ras_rank(std::vector<double> &x);
    std::vector<std::string> split(std::string str, std::string sep);
    unsigned long int ras_FileLineNumber(std::string file_name);
    unsigned long int ras_FileColNumber(std::string file_name, std::string sep);
    // normal Distribution
    double NormalCDFInverse(double p);
    double RationalApproximation(double t);
    double NormalCDF(double x, double mu, double sigma);
    double NormalPDF(double x, double mu, double sigma);
    void summarize(const std::vector<double>& data);
}
