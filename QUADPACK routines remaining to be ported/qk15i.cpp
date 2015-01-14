/**
 * \file
 * \brief - Integeration rules for computing i = integeral of f*w over (lowerLimit, upperLimit), with error estimate j = integeral of abs(f*w) over (lowerLimit, upperLimit).
 *          The original (infinite-bounded integeration range is mapped to the interval (0,1) and (lowerLimit, upperLimit) is a part of (0,1).
 *          it is the purpose to compute i = integeral of transformed integerand over (lowerLimit, upperLimit), j = integeral of abs(transformed integerand) over (lowerLimit, upperLimit).
 *
 * \sa R. Piessens, E. de Doncker-Kapenger, C. Ueberhuber, D. Kahaner, QUADPACK, A Subroutine Package for Automatic integeration, Springer Verlag, 1983.
 *
 * \details - 15-point transformed gauss-kronrod rules. The abscissae and weights are given for the interval (-1,1). Because of symmetry only the positive abscissae and their corresponding weights are given.
 *
 * \param[] finiteBound - Finite bound of original integeration range (set to zero if infiniteBoundKey = +2)
 * \param[] infiniteBoundKey - If infiniteBoundKey = -1, the original interval is (-infiniteBoundKey,finiteBound),
 *          If infiniteBoundKey = +2, the original interval is (-infiniteBoundKey,+infiniteBoundKey) and the integeral is computed as the sum of two integerals, one over (-infiniteBoundKey,0) and one over (0,+infiniteBoundKey).
 * \param[] integral - Approximation to the integeral i. Integral is computed by applying the 15-point kronrod rule(resultKronrod) obtained by optimal addition of abscissae to the 7-point gauss rule(resultGauss).
 * \param[] m_estimatedError - Estimate of the modulus of the absolute error, which should equal or exceed abs(i-integral)
 * \param[] absIntegral - real approximation to the integeral j
 * \param[] m_estimatedError - Approximation to the integeral of abs((transformed integerand)-i/(upperLimit-lowerLimit)) over (lowerLimit, upperLimit). 
 * \param[] The abscissae and weights are supplied for the interval (-1,1).  because of symmetry only the positive abscissae and their corresponding weights are given.
 * \param[] xgk - Abscissae of the 15-point Kronrod rule xgk(2), xgk(4), ... abscissae of the 7-point Gauss rule xgk(1), xgk(3), ...  abscissae which are optimally added to the 7-point gauss rule
 * \param[] wgk - weights of the 15-point kronrod rule
 * \param[] wg - weights of the 7-point gauss rule, corresponding to the abscissae xgk(2), xgk(4), ... wg(1), wg(3), ... are set to zero.
 * \param[] center  - mid point of the interval
 * \param[] halfLength  - half-length of the interval
 * \param[] absc*  - abscissa
 * \param[] tabsc* - transformed abscissa
 * \param[] fValue*  - function value
 * \param[] resultGauss   - integral of the 7-point gauss formula
 * \param[] resultKronrod   - integral of the 15-point kronrod formula
 * \param[] resultKronrodh  - approximation to the mean value of the transformed integerand over (lowerLimit, upperLimit), i.e. to i/(upperLimit-lowerLimit)
 *
 * \returns The approximation to the integeral.
 */

    real a,absc,absc1,absc2,m_estimatedError,b,finiteBound,center,dinfiniteBoundKey,r1mach,,f,fc,fsum,fValue1,fValue2,fv1,fv2,halfLength,absIntegral,m_estimatedError,resultGauss,resultKronrod,resultKronrodh,integral,tabsc1,tabsc2,,wg,wgk,xgk
    int infiniteBoundKey;
    int j;
    int min0;

    fv1(7);
    fv2(7);
    xgk(8);
    wgk(8);
    wg(8);

    xgk
    {
        0.9914553711208126,
        0.9491079123427585,
        0.8648644233597691,
        0.7415311855993944,
        0.5860872354676911,
        0.4058451513773972,
        0.2077849550078985,
        0.0000000000000000
    }.finished();

    wgk
    {
        0.02293532201052922,
        0.06309209262997855,
        0.1047900103222502,
        0.1406532597155259,
        0.1690047266392679,
        0.1903505780647854,
        0.2044329400752989,
        0.2094821410847278
    }.finished();

    wg
    {
        0.0000000000000000,
        0.1294849661688697,
        0.0000000000000000,
        0.2797053914892767,
        0.0000000000000000,
        0.3818300505051189,
        0.0000000000000000,
        0.4179591836734694
    }.finished();

      Eigen::NumTraits<Scalar>::epsilon() = r1mach(4)
      (std::numeric_limits<Scalar>::min)() = r1mach(1)
      dinfiniteBoundKey = min0(1,infiniteBoundKey)

      center = 0.5*(lowerLimit+upperLimit)
      halfLength = 0.5*(upperLimit-lowerLimit)
      tabsc1 = finiteBound+dinfiniteBoundKey*(1.-center)/center
      fValue1 = f(tabsc1)
      if(infiniteBoundKey == 2) fValue1 = fValue1+f(-tabsc1)
      fc = (fValue1/center)/center

    // Compute the 15-point kronrod approximation to the integeral, and estimate the error.
      resultGauss = wg(8)*fc
      resultKronrod = wgk(8)*fc
      absIntegral = abs(resultKronrod)
      do 10 j=1,7
        absc = halfLength*xgk(j)
        absc1 = center-absc
        absc2 = center+absc
        tabsc1 = finiteBound+dinfiniteBoundKey*(1.-absc1)/absc1
        tabsc2 = finiteBound+dinfiniteBoundKey*(1.-absc2)/absc2
        fValue1 = f(tabsc1)
        fValue2 = f(tabsc2)
        if(infiniteBoundKey == 2) fValue1 = fValue1+f(-tabsc1)
        if(infiniteBoundKey == 2) fValue2 = fValue2+f(-tabsc2)
        fValue1 = (fValue1/absc1)/absc1
        fValue2 = (fValue2/absc2)/absc2
        fv1(j) = fValue1
        fv2(j) = fValue2
        fsum = fValue1+fValue2
        resultGauss = resultGauss+wg(j)*fsum
        resultKronrod = resultKronrod+wgk(j)*fsum
        absIntegral = absIntegral+wgk(j)*(abs(fValue1)+abs(fValue2))
   10 continue
      resultKronrodh = resultKronrod*0.5
      m_estimatedError = wgk(8)*abs(fc-resultKronrodh)
      do 20 j=1,7
        m_estimatedError = m_estimatedError+wgk(j)*(abs(fv1(j)-resultKronrodh)+abs(fv2(j)-resultKronrodh))
   20 continue
      integral = resultKronrod*halfLength
      m_estimatedError = m_estimatedError*halfLength
      absIntegral = absIntegral*halfLength
      m_estimatedError = abs((resultKronrod-resultGauss)*halfLength)
      if(m_estimatedError != 0. && m_estimatedError != 0.e0) m_estimatedError = m_estimatedError*(std::min)(1.,(0.2e+03*m_estimatedError/m_estimatedError)**1.5 )
      if(absIntegral > (std::numeric_limits<Scalar>::min)()/(5.*Eigen::NumTraits<Scalar>::epsilon())) m_estimatedError = (std::max)((Eigen::NumTraits<Scalar>::epsilon()*5.)*absIntegral,m_estimatedError)
      return
      end
