#pragma once
#include <iterator>
#include <cmath>
#include <complex>

template<typename X, typename Y, typename InputIt_x>
struct LinearInterpolator{

    double xstart, dx;
    X x_; // array of indep. variable
    Y y_; // array of dep. variable
    int even_; // Bool, true for evenly spaced grids
    InputIt_x x_lower_bound, x_upper_bound;
    InputIt_x x_lower_it, x_upper_it, x0_it;
    double x_lower, x_upper, h; 
    std::complex<double> y_lower, y_upper;

    LinearInterpolator(X &x, Y &y, int even){
        // Constructor of struct, sets struct members

        even_ = even;
        if(even == 1){
            x_ = x;
            y_ = y;
            xstart = x[0];
            dx = x[1]-x[0];
        }
        else{
            x_ = x;
            y_ = y;
            //x0_it = x;
            //std::cout << *x0_it << std::endl;
        }
    }

    void set_interp_start(InputIt_x x_start){
        // Sets iterator pointing to first element of time-array
        x0_it = x_start; 
    }
    
    void set_interp_bounds(InputIt_x lower_it, InputIt_x upper_it){
        // Sets iterators for lower and upper bounds within which search for
        // nearest neighbours is performed for interpolation. 
        x_lower_bound = lower_it;
        x_upper_bound = upper_it;
    }

    void update_interp_bounds(){
        x_lower_bound = x_upper_it;
    }

    void update_interp_bounds_reverse(){
        x_upper_bound = x_lower_it;
    }

    std::complex<double> operator() (double x)  {
        // Does linear interpolation 
        std::complex<double> result; 
        
        if(even_ == 1){
            int i=int((x-xstart)/dx);
            std::complex<double> y0=y_[i];
            std::complex<double> y1=y_[i+1];
            double x1 = xstart + (i+1)*dx;
            double x0 = xstart + i*dx; 
            result = (y0+(y1-y0)*(x-xstart-dx*i)/dx);
            std::cout << "lower: " << x0 << ", x: " << x << ", upper: " << x1 << std::endl;
            //std::cout << (y0*(x1-x) + y1*(x-x0))/(x1-x0) << ", " << result << std::endl;
        }
        else{

            x_upper_it = std::upper_bound(x_lower_bound, x_upper_bound, x);
            x_lower_it = x_upper_it-1; 
            x_lower = *x_lower_it;
            x_upper = *x_upper_it;
            y_lower = y_[(x_lower_it - x0_it)];
            y_upper = y_[(x_upper_it - x0_it)];
            result = (y_lower*(x_upper-x) + y_upper*(x-x_lower))/(x_upper - x_lower);
            std::cout << std::setprecision(7) << "lower: " << x_lower << ", x: " << x << ", upper: " << x_upper << std::endl;
        }
        return result;
    }

    std::complex<double> expit (double x)  {
        // Does linear interpolation when the input is ln()-d
        int i=int((x-xstart)/dx);
        std::complex<double> y0=y_[i];
        std::complex<double> y1=y_[i+1];
        return std::exp(y0+(y1-y0)*(x-xstart-dx*i)/dx);
    }
   
};

