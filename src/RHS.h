/*
 * RHS.h - Non-class velocity-to-right-hand-side calculations
 *
 * (c)2017-9 Applied Scientific Research, Inc.
 *           Written by Mark J Stock <markjstock@gmail.com>
 */

#pragma once

#include "Omega3D.h"
#include "VectorHelper.h"
#include "Points.h"
#include "Surfaces.h"

#include <iostream>
#include <vector>
#include <cassert>


template <class S>
std::vector<S> vels_to_rhs_points (Points<S> const& targ) {
  std::cout << "    NOT converting vels to RHS vector for " << targ.to_string() << std::endl;

  //auto start = std::chrono::system_clock::now();
  //float flops = 0.0;

  // size the return vector
  size_t ntarg  = targ.get_n();
  std::vector<S> rhs;
  rhs.resize(ntarg);

  //auto end = std::chrono::system_clock::now();
  //std::chrono::duration<double> elapsed_seconds = end-start;
  //const float gflops = 1.e-9 * flops / (float)elapsed_seconds.count();
  //printf("    points_on_points_coeff: [%.4f] seconds at %.3f GFlop/s\n", (float)elapsed_seconds.count(), gflops);

  return rhs;
}

template <class S>
std::vector<S> vels_to_rhs_panels (Surfaces<S> const& targ) {
  std::cout << "    convert vels to RHS vector for" << targ.to_string() << std::endl;

  // pull references to the element arrays
  //const std::array<Vector<S>,Dimensions>& tx = targ.get_pos();
  //const std::vector<Int>&                 ti = targ.get_idx();
  const std::array<Vector<S>,Dimensions>& x1 = targ.get_x1();
  const std::array<Vector<S>,Dimensions>& x2 = targ.get_x2();
  const std::array<Vector<S>,Dimensions>& norm = targ.get_norm();
  //const Vector<S>&                        area = targ.get_area();
  const std::array<Vector<S>,Dimensions>& tu = targ.get_vel();
  const std::vector<Vector<S>>&           tb = targ.get_bcs();

  //std::cout << "size of x1 is " << x1.size() << " by " << x1[0].size() << std::endl;
  //std::cout << "size of x2 is " << x2.size() << " by " << x2[0].size() << std::endl;
  //std::cout << "size of norm is " << norm.size() << " by " << norm[0].size() << std::endl;
  //std::cout << "size of tu is " << tu.size() << " by " << tu[0].size() << std::endl;
  //std::cout << "size of tb is " << tb.size() << " by " << tb[0].size() << std::endl;

  // check for data
  assert(x1[0].size() == x2[0].size());
  assert(x1[0].size() == norm[0].size());
  assert(x1[0].size() == tu[0].size());
  assert(tb.size() > 0);
  assert(x1[0].size() == tb[0].size());

  // find array sizes
  const size_t ntarg  = targ.get_npanels();
  const size_t nunkn  = tb.size();

  // prepare the rhs vector
  std::vector<S> rhs;
  rhs.resize(nunkn*ntarg);

  // convert velocity and boundary condition to RHS values
  if (nunkn == 1) {
    // normal-only
    for (size_t i=0; i<ntarg; i++) {
      rhs[i] = -(tu[0][i]*norm[0][i] + tu[1][i]*norm[1][i] + tu[2][i]*norm[2][i]);// / area[i];
    }
  } else if (nunkn == 2) {
    // dot product of x1 tangent with local velocity, applying normalization
    for (size_t i=0; i<ntarg; i++) {
      rhs[2*i+0] = -(tu[0][i]*x1[0][i] + tu[1][i]*x1[1][i] + tu[2][i]*x1[2][i]);// / area[i];
      rhs[2*i+1] = -(tu[0][i]*x2[0][i] + tu[1][i]*x2[1][i] + tu[2][i]*x2[2][i]);// / area[i];
    }
  } else if (nunkn == 3) {
    // two tangentials then the normal
    for (size_t i=0; i<ntarg; i++) {
      rhs[3*i+0] = -(tu[0][i]*x1[0][i] + tu[1][i]*x1[1][i] + tu[2][i]*x1[2][i]);// / area[i];
      rhs[3*i+1] = -(tu[0][i]*x2[0][i] + tu[1][i]*x2[1][i] + tu[2][i]*x2[2][i]);// / area[i];
      rhs[3*i+2] = -(tu[0][i]*norm[0][i] + tu[1][i]*norm[1][i] + tu[2][i]*norm[2][i]);// / area[i];
    }
  }

  // include the influence of the boundary condition (normally zero)
  //   nunkn=1 is normal-only
  //   nunkn=2 is tangential-only (x1, x2)
  //   nunkn=3 is tangential and normal (x1, x2, norm)
  for (size_t i=0; i<ntarg; i++) {
    for (size_t j=0; j<nunkn; ++j) rhs[nunkn*i+j] -= tb[j][i];

    //std::cout << "  elem " << i << " vel is " << tu[0][i] << " " << tu[1][i] << std::endl;
    //std::cout << "  elem " << i << " rhs is " << rhs[i] << std::endl;
  }

  return rhs;
}


// helper struct for dispatching through a variant
struct RHSVisitor {
  // source collection, target collection
  std::vector<float> operator()(Points<float> const& targ)   { return vels_to_rhs_points<float>(targ); } 
  std::vector<float> operator()(Surfaces<float> const& targ) { return vels_to_rhs_panels<float>(targ); } 
};
