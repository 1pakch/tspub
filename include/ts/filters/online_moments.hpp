// online_moments.hpp - online estimators for mean, variance and covariance.

#ifndef ONLINE_MEAN_HPP
#define ONLINE_MEAN_HPP 

#include <cmath>

#include <ts/filters/validity.hpp>


namespace ts {

namespace filters {

namespace impl {

double secondMomentDenominator(size_t n, bool besselCorrection)
{
  return besselCorrection ? n-1 : n;
}

} // namespace impl


/// A filter to calculate the mean of a sequence. Uses an iterative algorithm
/// which avoids the loss of precision on large inputs. See, for instance,
/// equation (4) in  
///
/// Finch, T. (2009). "Incremental calculation of weighted mean and variance".
/// University of Cambridge.
///
class OnlineMean: public DeterministicallyValidFilter
{
 private:

  double mu_ = 0; ///< current estimate
 
 public:
  
  /// Needs at least one observation
  OnlineMean():
    DeterministicallyValidFilter(1)
  {}  

  /// Returns the current estimate
  double value() const {
    return (
      DeterministicallyValidFilter::ready() >= 1 ? mu_ : na::na<double>()
    );
  }

  /// Processes the next value
  double operator() (double x)
  {
    CountingFilter::inc();
    mu_ = mu_ + (x - mu_) / n_processed();
    return mu_;
  }

};


/// A filter to calculate the variance of a sequence with an unknown mean.
/// Uses the Welford's algorithm which avoids the overflow on large inputs.
/// See, for instance, equation (44) in  
///
/// Finch, T. (2009). "Incremental calculation of weighted mean and variance".
/// University of Cambridge.
///
/// original paper
///
/// Welford, B. P. (1962). "Note on a method for calculating corrected sums
/// of squares and products". Technometrics 4(3):419–420.
/// 
/// or the Wikipedia page "Algorithms for calculating variance".
/// 
class OnlineVarUnknownMean: public DeterministicallyValidFilter
{
 private:

  double mu_ = 0; ///< current estimate of the mean
  double M2_ = 0; ///< sum of squared distances from the mean
 
 public:

  /// Needs at least two observations
  OnlineVarUnknownMean():
    DeterministicallyValidFilter(2)
  {}

  /// returns the current estimate of the variance
  double value() const
  {
    return M2_/ impl::secondMomentDenominator(n_processed(), true);
  }

  /// Returns the current estimate of the mean
  double mean() const { return mu_; }

  /// Processes the next value
  void operator() (double x)
  {
    CountingFilter::inc();
    auto delta = (x - mu_);
    mu_ += delta / n_processed();
    M2_ += delta * (x - mu_);
  }
};


/// A filter to calculate the covariance of two sequences with unknown means.
/// Uses a straightforward extension of Welford's algorithm which avoids the
/// overflow on large inputs. 
/// 
class OnlineCovUnknownMeans: public DeterministicallyValidFilter
{
 private:

  double m1_ = 0; ///< current estimate of the mean of the first input
  double m2_ = 0; ///< current estimate of the mean of the second input
  double M11_ = 0; ///< n times the variance of the first input
  double M22_ = 0; ///< n times the variance of the second input
  double M12_ = 0; ///< n times the covariance
 
 public:
  
  /// Needs at least two observations
  OnlineCovUnknownMeans():
    DeterministicallyValidFilter(2)
  {}

  /// current estimate of the covariance
  double cov() const
  {
    return M12_/ impl::secondMomentDenominator(n_processed(), true);
  }
 
  /// current estimate of the variance of the first input
  double var1() const
  {
    return M11_/ impl::secondMomentDenominator(n_processed(), true);
  }
  
  /// current estimate of the covariance of the second input
  double var2() const
  {
    return M22_/ impl::secondMomentDenominator(n_processed(), true);
  }

  /// current estimate of the correlation
  double corr() const
  {
    return M12_/ std::sqrt(M11_ * M22_);
  }

  /// Processes the next values
  void operator() (double x1, double x2)
  {
    CountingFilter::inc();
    double delta1 = (x1 - m1_);
    double delta2 = (x2 - m2_);
    // update the mean and the var of the first input
    m1_+= delta1 / n_processed();
    M11_ += delta1 * (x1 - m1_);
    // update the mean and the var of the second input
    m2_+= delta2 / n_processed();
    M22_ += delta2 * (x2 - m2_);
    // update the covariance
    M12_ += (x1 - m1_) * delta2;
    // M12_ += (x2 - mu2_) * delta1; // also works
 
  }
};


/// A filter to calculate the variance of a sequence with an _known_ mean.
/// 
class OnlineVarKnownMean: public DeterministicallyValidFilter
{
 private:

  const double mu_; ///< the estimate for the mean
  double M2_ = 0; ///< sum of squared distances from the mean
 
 public:

  /// Needs at least two observations
  OnlineVarKnownMean(double mu):
    DeterministicallyValidFilter(1),
    mu_(mu)
  {}

  /// returns the current estimate of the variance
  double value() const
  {
    return M2_/ impl::secondMomentDenominator(n_processed(), false);
  }

  /// Returns the current estimate of the mean
  double mean() const { return mu_; }

  /// Processes the next value
  void operator() (double x)
  {
    CountingFilter::inc();
    auto delta = (x - mu_);
    M2_ += delta * delta;
  }
};

/// A filter to calculate the covariance of two sequences with _known_ means.
/// Uses a straightforward extension of Welford's algorithm which avoids the
/// overflow on large inputs. 
/// 
class OnlineCovKnownMeans: public DeterministicallyValidFilter
{
 private:

  const double m1_ = 0; ///< the mean of the first input
  const double m2_ = 0; ///< the mean of the second input
  double M11_ = 0; ///< n times the variance of the first input
  double M22_ = 0; ///< n times the variance of the second input
  double M12_ = 0; ///< n times the covariance
 
 public:
  
  /// Needs at least two observations
  OnlineCovKnownMeans(double mu1, double mu2):
    DeterministicallyValidFilter(1),
    m1_(mu1),
    m2_(mu2)
  {}

  /// current estimate of the covariance
  double cov() const
  {
    return M12_/ impl::secondMomentDenominator(n_processed(), false);
  }

  /// current estimate of the variance of the first input
  double var1() const
  {
    return M11_/ impl::secondMomentDenominator(n_processed(), false);
  }
  
  /// current estimate of the covariance of the second input
  double var2() const
  {
    return M22_/ impl::secondMomentDenominator(n_processed(), false);
  }

  /// current estimate of the correlation
  double corr() const
  {
    return M12_/ std::sqrt(M11_ * M22_);
  }

  /// Processes the next values
  void operator() (double x1, double x2)
  {
    CountingFilter::inc();
    double delta1 = (x1 - m1_);
    double delta2 = (x2 - m2_);
    M11_ += delta1 * delta1;
    M22_ += delta2 * delta2;
    M12_ += delta1 * delta2; 
    //std::cout << "d1 = " <<  delta2 << std::endl;
    //std::cout << "d2 = " <<  delta2 << std::endl;
    //std::cout << "M12 = " <<  M12_ << std::endl;
  }
};



} // namespace filters

} // namespace ts


#endif /* ONLINE_MEAN_HPP */
