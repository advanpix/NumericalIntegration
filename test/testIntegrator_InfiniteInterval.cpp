/**
 * \file testIntegrator_InfiniteInterval.cpp
 * This file is a unit test for Integrator.h and its' associated files.
 * The test is an asymptotic function which decays rapidly toward zero
 * with an infinitely diminishing tail.
 */

#include <NumericalIntegration.h>

#include <iostream>
#include <fstream>
#include <iomanip>

using namespace Eigen;

////////////////////////// example from Quadpackcpp ///////////////////////////
/**
 * This integrand has an infinite interval. It is well-behaved and tends to 0 quickly.
 */
template<typename Scalar>
class IntegrandInfiniteFunctor
{
public:
    Scalar operator()(const Scalar& param) const
    {
        using std::pow;
        using std::exp;
        return pow(param, Scalar(2.)) * exp(-param * pow(Scalar(2.), -m_alpha));
    }

    /**
    * A paramater for varying the upper bound.
    */
    void setAlpha(const Scalar& alpha) {m_alpha = alpha;}

    static Scalar integrateInfinite(const Scalar& alpha)
    {
        using std::pow;
        using std::exp;
        return (exp(Scalar(40.)) - Scalar(841.)) * pow(2., Scalar(3.) * alpha + Scalar(1.)) / exp(Scalar(40.));
    }

private:
    Scalar m_alpha;
};

/**
 * \param epsilon Relative machine precision.
 */
template <typename Scalar>
Scalar desiredRelativeError()
{
  return Eigen::NumTraits<Scalar>::epsilon() * 50.;
}

template <typename Scalar>
typename Eigen::Integrator<Scalar>::QuadratureRule quadratureRules(const size_t& i)
{
  static const typename Eigen::Integrator<Scalar>::QuadratureRule quadratureRules[12] =
    {
        Eigen::Integrator<Scalar>::GaussKronrod15,
        Eigen::Integrator<Scalar>::GaussKronrod21,
        Eigen::Integrator<Scalar>::GaussKronrod31,
        Eigen::Integrator<Scalar>::GaussKronrod41,
        Eigen::Integrator<Scalar>::GaussKronrod51,
        Eigen::Integrator<Scalar>::GaussKronrod61,
        Eigen::Integrator<Scalar>::GaussKronrod71,
        Eigen::Integrator<Scalar>::GaussKronrod81,
        Eigen::Integrator<Scalar>::GaussKronrod91,
        Eigen::Integrator<Scalar>::GaussKronrod101,
        Eigen::Integrator<Scalar>::GaussKronrod121,
        Eigen::Integrator<Scalar>::GaussKronrod201
    };

  return quadratureRules[i];
}

int test_pow(void)
{
    std::ofstream fout;
    fout.open("test/testOutput/Infinite_Interval_integration_test_output.txt");

    std::cout<<"\nTesting Interval [0->Alpha], F(x) = x^2 * exp(-x * 2^(-alpha))\n";
     
    // typedef float Scalar;
    typedef double Scalar;
    // typedef long double Scalar;
    // typedef mpfr::mpreal Scalar;
    // Scalar::set_default_prec(500);
    
    typedef Eigen::Integrator<Scalar> IntegratorType;
    typedef IntegrandInfiniteFunctor<Scalar> IntegrandInfiniteFunctorType;

    IntegratorType eigenIntegrator(1000);  // \detail The number of subintervals must be increased by more than 100X the precision requested.
    IntegrandInfiniteFunctorType integrandInfiniteFunctor;

    bool success = true;
    const Scalar alphaLimit = 18.;
    const size_t numRules = 12;

    for (Scalar alpha = 0.; alpha < alphaLimit; ++alpha)
    {
        success = true;
        integrandInfiniteFunctor.setAlpha(alpha);
        
        for (size_t i = 0; i < numRules; ++i)
        {
            Eigen::Integrator<Scalar>::QuadratureRule quadratureRule = quadratureRules<Scalar>(i);

            using std::pow;
            Scalar actual = eigenIntegrator.quadratureAdaptive(integrandInfiniteFunctor, Scalar(0.), Scalar(40. * pow(2., alpha)), Scalar(0.), desiredRelativeError<Scalar>(), quadratureRule);
            Scalar expected = IntegrandInfiniteFunctorType::integrateInfinite(alpha);

            using std::abs;
            using std::isnan;
            
            if(abs((Scalar)(expected - actual)) > desiredRelativeError<Scalar>() * abs(expected) 
                || isnan(abs((Scalar)(expected - actual))))
            {
                success = false;

                if(i == numRules-1)
                {
                    fout << "\nPeak Test could not pass Alpha = " << alpha
                         << "\nrule " << i << "\n abs(expected - actual) = " << abs(expected - actual)
                         << "\n desiredRelativeError<Scalar>() * abs(expected) = "
                         << desiredRelativeError<Scalar>() * abs(expected) << std::endl;
                          
                    fout << "errorCode = " << eigenIntegrator.errorCode() << "\nTest aborted after Fail";

                    std::cout << "\nTest aborted after failing for Alpha = " << alpha
                              << "\n\tTest Failed.\n" << std::endl;
                    return EXIT_FAILURE;
                }
            }
            else
            {
                fout << "\nrule " << i << "\n abs(expected - actual) = " << abs(expected - actual)
                     << "\n desiredRelativeError<Scalar>() * abs(expected) = "
                     << desiredRelativeError<Scalar>() * abs(expected) << std::endl;
                          
                fout << "errorCode = " << eigenIntegrator.errorCode() << std::endl;
                fout << "alpha = " << alpha << std::endl;
                fout << "Success!\n";
                success = true;
                break;
            }
        }
    }

    fout.close();

    if(success)
    {
        std::cout << std::endl << "\tTest Succeeded!\n" << std::endl;
        return EXIT_SUCCESS;
    }
    else
    {
        std::cout << "\tTest Failed.\n" << std::endl;
        return EXIT_FAILURE;
    }
}

int main(void)
{
    int ret = EXIT_SUCCESS;
    ret += test_pow();
    return EXIT_SUCCESS;
}