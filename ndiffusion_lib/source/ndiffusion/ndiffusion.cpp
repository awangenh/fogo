#include <ndiffusion/ndiffusion.hpp>

//----------------------------------------------------------------------------------------------------------------------------------//
// Perona-Malik diffusivity function
//----------------------------------------------------------------------------------------------------------------------------------//

template <typename result_type, typename param_type, typename lambda_type>
result_type perona_malik<result_type,param_type,lambda_type>::calculate(param_type& gradient) {
  result_type pt1 = color::abs(gradient);
  
  result_type res = (pt1 * pt1) / (m_lambda * m_lambda);
  res = 1. + res;
  res = 1. / res;

  return res;
}

//----------------------------------------------------------------------------------------------------------------------------------//
// Stabilized TV flow diffusivity function
//----------------------------------------------------------------------------------------------------------------------------------//

template <typename result_type, typename param_type, typename e_type, typename p_type>
result_type stabilized_tvflow<result_type,param_type,e_type,p_type>::calculate(param_type& gradient) {
  param_type pt1 = color::abs(gradient);
  pt1 *= pt1;
  e_type pt2 = m_e;
  
  result_type res = pt1 + pt2;
  res = color::pow(res,m_p/2.);
  res = 1. / res;

  return res;
}

//----------------------------------------------------------------------------------------------------------------------------------//
