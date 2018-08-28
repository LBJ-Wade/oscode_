#include "test-airy.hpp"

Vector background(Scalar t, int d){
    // Gives background at a given time t
    Vector result = Vector::Ones(d);
    return result*std::pow(t, 1.0/4.0);
};

Vector ana_s(Scalar t, int order){
    Vector result(4);
    result << std::complex<double>(0.0, 1.0)*2.0/3.0*std::pow(t, 3.0/2.0), -1.0/4.0*std::log(t), std::complex<double>(0.0, 1.0)*(-5.0)/48.0*std::pow(t, -3.0/2.0), -5.0/64.0*std::pow(t, -3.0);
    return result.head(order+1);
}

Vector ana_ds(Scalar t, int order){
    Vector result(4);
    result << std::complex<double>(0.0, 1.0)*std::pow(t, 1/2.0), -1.0/(4.0*t), std::complex<double>(0.0, 1.0)*5.0/32.0*std::pow(t, -5.0/2.0), 5.0/192.0*std::pow(t, -4);
    return result.head(order+1);
}

Vector airy(double t){
    Vector result(2);
    result << boost::math::airy_ai(-t), -boost::math::airy_ai_prime(-t);
    return result; 
}


TEST_CASE("setting up a system of ODEs"){
    
    double t = 1.0;
    Vector ic(4);                                        
    ic << 1.0, 1.0, background(t, 2);
    Vector y = ic.tail(2);
    de_system my_system(F, DF, w, Dw, DDw, g, Dg, DDg);
     
    REQUIRE( std::real(my_system.w(y)) == Approx(std::pow(t, 1/2.0)) );
    REQUIRE( std::real(my_system.dw(y)) == Approx(1/2.0*std::pow(t, -1/2.0)) );
    REQUIRE( std::real(my_system.ddw(y)) == Approx(-1/4.0*std::pow(t, -3/2.0)) );
    REQUIRE( std::real(my_system.dg(y)) == Approx(0.0) );
    REQUIRE( std::real(my_system.ddg(y)) == Approx(0.0) );

};

TEST_CASE("pure RKF integration"){};

TEST_CASE("dS functions analytic forms"){

    int order = 1;
    double t = 1.0;
    Vector ic(4);                                      
    int d = ic.size()+(order+2)/2;
    ic << airy(t), background(t, 2);
    de_system my_system(F, DF, w, Dw, DDw, g, Dg, DDg);   
    Solution my_solution(my_system, ic, 1.0, f_end);
    Vector y_bg = my_solution.y.segment(2, d-(order+2)/2-2);
    for(int i=0; i<(order+1); i++){
        CHECK(std::real(my_solution.wkbsolver->dS(y_bg)(i)) == Approx(std::real(ana_ds(t,order)(i))) );
        CHECK(std::imag(my_solution.wkbsolver->dS(y_bg)(i)) == Approx(std::imag(ana_ds(t,order)(i))) );
    };
};

TEST_CASE("Single RKF step"){

    int order = 1;
    double t = 1.0;
    double step = 0.1;
    Vector ic(4);                                      
    int d = ic.size()+(order+2)/2;
    ic << airy(t), background(t, 2);
    de_system my_system(F, DF, w, Dw, DDw, g, Dg, DDg);   
    Solution my_solution(my_system, ic, 1.0, f_end);
    Step rkf_step = my_solution.step(&my_solution.rkfsolver, my_solution.f_tot, my_solution.y, step); 
    t+=step;
    for(int i=0; i<2; i++){
        CHECK(std::real(rkf_step.y(i)) == Approx(std::real(airy(t)(i))));
        CHECK(std::imag(rkf_step.y(i)) == Approx(std::imag(airy(t)(i))));
    };
    for(int i=2; i<(d-(order+2)/2); i++){
        CHECK(std::real(rkf_step.y(i)) == Approx(std::real(background(t,2)(0))));
        CHECK(std::imag(rkf_step.y(i)) == Approx(std::imag(background(t,2)(0))));
    };
    for(int i=(d-(order+2)/2); i<d; i++){
        CHECK(std::real(rkf_step.y(i)) == Approx(std::real(ana_s(t,order)(2*(i-d+(order+2)/2)) - ana_s(t-step,order)(2*(i-d+(order+2)/2)))));
        CHECK(std::imag(rkf_step.y(i)) == Approx(std::imag(ana_s(t,order)(2*(i-d+(order+2)/2)) - ana_s(t-step,order)(2*(i-d+(order+2)/2)))));
    };
};

TEST_CASE("RKWKB integration of Airy"){};



