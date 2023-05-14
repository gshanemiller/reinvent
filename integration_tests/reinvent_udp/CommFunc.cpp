#include "CommFunc.h"

const double pi = 3.1415926;

double CommFunc::Abs(const double &x)
{
	std::complex<double> cld(x);
	double ldAbs = abs(cld);
	return(ldAbs);
}

double CommFunc::sum(const std::vector<double> &x)
{
    unsigned long int size = x.size();
    double d_buf=0.0;
    for(unsigned long int i=0; i<size; i++) d_buf+=x[i];
    return (double)d_buf;
}

int CommFunc::sum(const std::vector<int> &x)
{
    unsigned long int size = x.size();
    int d_buf=0.0;
    for(unsigned long int i=0; i<size; i++) d_buf+=x[i];
    return d_buf;
}


double CommFunc::mean(const std::vector<double> &x)
{
    unsigned long int size = x.size();
    double d_buf=0.0;
    for(unsigned long int i=0; i<size; i++) d_buf+=x[i];
    d_buf/=(double)size;
    return (double)d_buf;
}

double CommFunc::median(const std::vector<double> &x)
{
    std::vector<double> b(x);
    unsigned long int size = b.size();
    if(size==1) return b[0];
    std::stable_sort(b.begin(), b.end());
    if(size%2==1) return b[(size-1)/2];
    else return (b[size/2]+b[size/2-1])/2;
}

double CommFunc::var(const std::vector<double> &x)
{
    unsigned long int size = x.size();
    if(size<=1) return(0.0);
    unsigned long int i=0;
    double mu=0.0, s2=0.0;
    for(i=0; i<size; i++) mu+=x[i];
    mu/=(double)size;
    for(i=0; i<size; i++) s2+=(x[i]-mu)*(x[i]-mu);
    s2/=(double)(size-1);
    return (double)s2;
}

double CommFunc::sd(const std::vector<double> &x)
{
    return sqrt(CommFunc::var(x));
}

double CommFunc::cor(const std::vector<double> &x, const std::vector<double> &y)
{
    double cr=0;
    double sd_x=sd(x);
    double sd_y=sd(y);
    if (sd_x> 0 && sd_y >0) cr=cov(x,y)/sd_x/sd_y;
    if (sd_x== 0 && sd_y ==0) cr=1;
    return cr;
}

double CommFunc::cov(const std::vector<double> &x, const std::vector<double> &y)
{
    unsigned long int size = x.size();
    unsigned long int i=0;
    double mu1=0.0, mu2=0.0, c=0.0;
    for(i=0; i<size; i++){
        mu1+=x[i];
        mu2+=y[i];
    }
    mu1/=(double)size;
    mu2/=(double)size;

    for(i=0; i<size; i++) c+=(x[i]-mu1)*(y[i]-mu2);
    c/=(double)(size-1);
    return c;
}

bool CommFunc::FloatEqual(double lhs, double rhs)
{
    if (Abs(lhs - rhs) < 0.000001) return true;
	return false;
}

bool CommFunc::FloatNotEqual(double lhs, double rhs)
{
    if (Abs(lhs - rhs) >= 0.000001) return true;
	return false;
}

double CommFunc::Sqr(const double &a)
{
	return a*a;
}

double CommFunc::Max(const double &a, const double &b)
{
	return b > a ? (b) : (a);
}

double CommFunc::Min(const double &a, const double &b)
{
	return b < a ? (b) : (a);
}

double CommFunc::Sign(const double &a, const double &b)
{
	return b >= 0 ? (a >= 0 ? a : -a) : (a >= 0 ? -a : a);
}

int CommFunc::rand_seed(void)
{
  	std::stringstream str_strm;
	str_strm << std::time(NULL);
	std::string seed_str=str_strm.str();
    std::reverse(seed_str.begin(), seed_str.end());
    seed_str.erase(seed_str.begin()+7, seed_str.end());
	return(abs(std::atoi(seed_str.c_str())));
}

void CommFunc::FileExist(std::string filename)
{
    std::ifstream ifile(filename.c_str());
    if(!ifile) throw("Error: can not open the file ["+filename+"] to read.");
}


// zero-based rank
std::vector<unsigned long int> CommFunc::ras_rank(std::vector<double> &x)
{
    unsigned long int n=x.size();
    std::vector<unsigned long int> r(n,0); //initialize rank array
    for(unsigned long int i=1;i<n;i++)
        for(unsigned long int j=0;j<i;j++)
            if(x[j]<=x[i]) r[i]++;
            else r[j]++;
    return r;
}



std::vector<std::string> CommFunc::split(std::string str, std::string sep)
{
    char* cstr=const_cast<char*>(str.c_str());
    char* current;
    std::vector<std::string> arr;
    current=std::strtok(cstr,sep.c_str());
    while(current!=NULL){
        arr.push_back(current);
        current=strtok(NULL,sep.c_str());
    }
    return arr;
}


unsigned long int CommFunc::ras_FileLineNumber(std::string file_name)
{
    unsigned long int number_of_lines = 0;
    std::ifstream ifile(file_name.c_str());
    if(!ifile)
    {
        std::cout << "Error: can not open the file ["+ file_name +"] to read." << std::endl;
        return 0;
    }
    
    if(!ifile) throw("Error: can not open the file ["+file_name+"] to read.");
    
    std::string line;
    while (getline(ifile, line)){
        number_of_lines++;
    }
    ifile.clear();
    ifile.close();
    return number_of_lines;
}



unsigned long int CommFunc::ras_FileColNumber(std::string file_name, std::string sep)
{
    unsigned long int number_of_cols = 0;
    std::ifstream ifile(file_name.c_str());
    if(!ifile)
    {
        std::cout << "Error: can not open the file ["+ file_name +"] to read." << std::endl;
        return 0;
    }
    
    if(!ifile) throw("Error: can not open the file ["+file_name+"] to read.");
    
    std::vector<std::string> st1;
    
    std::string line;
    if (getline(ifile, line)){
        st1=CommFunc::split(line,sep);
        number_of_cols=st1.size();
    }
    ifile.clear();
    ifile.close();
    return number_of_cols;
}


double CommFunc::RationalApproximation(double t)
{
    // Abramowitz and Stegun formula 26.2.23.
    // The absolute value of the error should be less than 4.5 e-4.
    double c[] = {2.515517, 0.802853, 0.010328};
    double d[] = {1.432788, 0.189269, 0.001308};
    return t - ((c[2]*t + c[1])*t + c[0]) / (((d[2]*t + d[1])*t + d[0])*t + 1.0);
}

// inverse normal CDF
double CommFunc::NormalCDFInverse(double p)
{
    // 0<p<1
    
    // See article above for explanation of this section.
    if (p < 0.5)
    {
        // F^-1(p) = - G^-1(p)
        return -RationalApproximation( sqrt(-2.0*log(p)) );
    }
    else
    {
        // F^-1(p) = G^-1(1-p)
        return RationalApproximation( sqrt(-2.0*log(1-p)) );
    }
}



// normal CDF
double CommFunc::NormalCDF(double x, double mu, double sigma)
{
    // mu=E(X), sigma=\sqrt(Var(X))
    // erf(x) is in cmath.h for C++11
    return .5*(1+erf((x-mu)/(sqrt(2)*sigma)));
}


// normal PDF
double CommFunc::NormalPDF(double x, double mu, double sigma)
{
    // mu=E(X), sigma=\sqrt(Var(X))
    return 1/(sqrt(2.0*pi)*sigma) * exp(-0.5*pow((x-mu)/sigma,2) );
}

void CommFunc::summarize(const std::vector<double>& data) {
    // statistics
    if (data.size()==0)
    {
        std::cout << "# Error: No data!" << std::endl;
        return;
    }
    
    double data_size = data.size();
    double data_min  = *std::min_element(std::begin(data), std::end(data));
    double data_max  = *std::max_element(std::begin(data), std::end(data));
    double data_mean = CommFunc::mean(data);
    double data_sd   = CommFunc::sd(data);

    std::cout << "# RTT (units us) Histogram" << std::endl;
    std::cout << "# NumSamples = " << data_size << std::endl;
    std::cout << "# Min        = " << data_min << std::endl;
    std::cout << "# Max        = " << data_max << std::endl;
    std::cout << "# Mean       = " << data_mean << std::endl;
    std::cout << "# Stddev     = " << data_sd << std::endl;

    int nbin=log2(data_size)+1;
    
    std::vector<long int> hist(nbin,0);
    double bin_width = (data_max-data_min)/nbin;
    for (unsigned int idata=0; idata<data_size; idata++)
    {
        int ibin = (data[idata]-data_min)*nbin/(data_max-data_min);
        if (ibin<nbin)
            hist[ibin]++;
        else
            hist[nbin-1]++; // for data_max
    }
    
    std::string binformat("[%12.6f, %12.6f) |%9u| ");
    unsigned bin_max = *std::max_element(std::begin(hist), std::end(hist));
    unsigned dot_count = 1;
    unsigned maxdots = 40;
    std::string dot_symbol("*");

    if (bin_max > maxdots)
    {
        dot_count = bin_max/maxdots;
    }
    
    std::cout << "# each " << dot_symbol << " represents a count of " << dot_count << std::endl;
    std::cout << "# --------------------------------------" << std::endl;
    for (int ibin=0; ibin<nbin; ibin++)
    {
        //std::cout << data_min+ << hist[ibin] << std::endl;
        printf(binformat.c_str(), data_min+ibin*bin_width, data_min+(ibin+1)*bin_width, hist[ibin]);
        for (int idots=0; idots<hist[ibin]/dot_count; idots++) printf("%s",dot_symbol.c_str());
        
        printf("\n");
    }
    std::cout << "# --------------------------------------" << std::endl;
}
