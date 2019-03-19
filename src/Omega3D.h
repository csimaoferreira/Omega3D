/*
 * Omega3D.h - Useful definitions for anywhere in the code
 *
 * (c)2018-9 Applied Scientific Research, Inc.
 *           Written by Mark J Stock <markjstock@gmail.com>
 */

#pragma once

#include <cstdlib>
#include <vector>

const size_t Dimensions = 3;

// Use this for indexes into panels or bodies
#include <cstdint>
using Int = uint32_t;

// solver order and method
enum solution_t {
  direct_cpu   = 1,
  direct_vc    = 2,
  direct_glsl  = 3,
  treecode_cpu = 4,
  treecode_vc  = 5
};

// element type
enum elem_t {
  active   = 1,  // active vorticity
  reactive = 2,  // active once strength is solved
  inert    = 3   // does not affect flow
};

// movement type
enum move_t {
  lagrangian = 1, // moves with local velocity
  bodybound  = 2, // moves with attached body
  fixed      = 3  // does not move
};

//
// Helper class for passing arbitrary elements around
//
template<class S>
struct ElementPacket {
  ElementPacket<S>() = default;
  ~ElementPacket<S>() = default;

  ElementPacket<S>(ElementPacket<S> const&) = default; //allow copy
  ElementPacket<S>(ElementPacket<S>&&) = default; //allow move
  ElementPacket<S>& operator=(ElementPacket<S> const&) = default; //allow copy
  ElementPacket<S>& operator=(ElementPacket<S>&&) = default; //allow move

  std::vector<S> x;
  std::vector<Int> idx;
  std::vector<S> val;
};

