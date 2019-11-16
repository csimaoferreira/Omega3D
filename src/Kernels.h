/*
 * Kernels.h - Non-class inner kernels for influence calculations
 *
 * (c)2017-9 Applied Scientific Research, Inc.
 *           Written by Mark J Stock <markjstock@gmail.com>
 */

#pragma once

#ifdef _WIN32
#define __restrict__ __restrict
#endif

#ifdef USE_VC
#include <Vc/Vc>
#endif

#include <cmath>

//
// velocity influence functions
//
// here is the naming system:
//   kernel_NS_MT
//     N is the number of dimensions of the source element (0=point, 2=surface)
//     M is the number of dimensions of the target element
//     S is the type of the source element ('v'=vortex, 's'=source, 'vs'=vortex and source)
//     T is the type of the target element
//         first character is 'p' for a singular point, 'b' for a vortex blob
//         second character is 'g' if gradients must be returned
//

// thick-cored particle on thick-cored point, no gradients
template <class S, class A>
static inline void kernel_0v_0b (const S sx, const S sy, const S sz,
                                const S sr,
                                const S ssx, const S ssy, const S ssz,
                                const S tx, const S ty, const S tz,
                                const S tr,
                                A* __restrict__ tu, A* __restrict__ tv, A* __restrict__ tw) {
  // 30 flops
  const S dx = tx - sx;
  const S dy = ty - sy;
  const S dz = tz - sz;
  S r2 = dx*dx + dy*dy + dz*dz + sr*sr + tr*tr;
#ifdef USE_VC
  r2 = Vc::reciprocal(r2*Vc::sqrt(r2));
#else
  r2 = 1.0 / (r2*std::sqrt(r2));
#endif
  const S dxxw = dz*ssy - dy*ssz;
  const S dyxw = dx*ssz - dz*ssx;
  const S dzxw = dy*ssx - dx*ssy;
  *tu += r2 * dxxw;
  *tv += r2 * dyxw;
  *tw += r2 * dzxw;
}

// thick-cored particle on singular point, no gradients
template <class S, class A>
static inline void kernel_0v_0p (const S sx, const S sy, const S sz,
                                 const S sr,
                                 const S ssx, const S ssy, const S ssz,
                                 const S tx, const S ty, const S tz,
                                 A* __restrict__ tu, A* __restrict__ tv, A* __restrict__ tw) {
  // 28 flops
  const S dx = tx - sx;
  const S dy = ty - sy;
  const S dz = tz - sz;
  S r2 = dx*dx + dy*dy + dz*dz + sr*sr;
#ifdef USE_VC
  r2 = Vc::reciprocal(r2*Vc::sqrt(r2));
#else
  r2 = 1.0 / (r2*std::sqrt(r2));
#endif
  const S dxxw = dz*ssy - dy*ssz;
  const S dyxw = dx*ssz - dz*ssx;
  const S dzxw = dy*ssx - dx*ssy;
  *tu += r2 * dxxw;
  *tv += r2 * dyxw;
  *tw += r2 * dzxw;
}

// same, but source strength only
template <class S, class A>
static inline void kernel_0s_0p (const S sx, const S sy, const S sz,
                                 const S sr,
                                 const S ss,
                                 const S tx, const S ty, const S tz,
                                 A* __restrict__ tu, A* __restrict__ tv, A* __restrict__ tw) {
  // 19 flops
  const S dx = tx - sx;
  const S dy = ty - sy;
  const S dz = tz - sz;
  S r2 = dx*dx + dy*dy + dz*dz + sr*sr;
#ifdef USE_VC
  r2 = (S)ss * Vc::reciprocal(r2*Vc::sqrt(r2));
#else
  r2 = ss / (r2*std::sqrt(r2));
#endif
  *tu += r2 * dx;
  *tv += r2 * dy;
  *tw += r2 * dz;
}

// thick-cored particle on thick-cored point, with gradients - 65 flops total
template <class S, class A>
static inline void kernel_0v_0bg (const S sx, const S sy, const S sz,
                                 const S sr,
                                 const S ssx, const S ssy, const S ssz,
                                 const S tx, const S ty, const S tz,
                                 const S tr,
                                 A* __restrict__ tu, A* __restrict__ tv, A* __restrict__ tw,
                                 A* __restrict__ tux, A* __restrict__ tvx, A* __restrict__ twx,
                                 A* __restrict__ tuy, A* __restrict__ tvy, A* __restrict__ twy,
                                 A* __restrict__ tuz, A* __restrict__ tvz, A* __restrict__ twz) {
  // 30 flops
  const S dx = tx - sx;
  const S dy = ty - sy;
  const S dz = tz - sz;
  const S r2 = dx*dx + dy*dy + dz*dz + sr*sr + tr*tr;
#ifdef USE_VC
  const S r3 = Vc::reciprocal(r2*Vc::sqrt(r2));
#else
  const S r3 = 1.0 / (r2*std::sqrt(r2));
#endif
  S dxxw = dz*ssy - dy*ssz;
  S dyxw = dx*ssz - dz*ssx;
  S dzxw = dy*ssx - dx*ssy;
  *tu += r3 * dxxw;
  *tv += r3 * dyxw;
  *tw += r3 * dzxw;

  // accumulate velocity gradients
#ifdef USE_VC
  //const S bbb = -3.f * r3 * Vc::reciprocal(r2);
  const S bbb = S(-3.0) * r3 * Vc::reciprocal(r2);
#else
  const S bbb = -3.0 * r3 / r2;
#endif
  // continuing with grads - this section is 33 flops
  dxxw *= bbb;
  dyxw *= bbb;
  dzxw *= bbb;
  *tux += dx*dxxw;
  *tvx += dx*dyxw + ssz*r3;
  *twx += dx*dzxw - ssy*r3;
  *tuy += dy*dxxw - ssz*r3;
  *tvy += dy*dyxw;
  *twy += dy*dzxw + ssx*r3;
  *tuz += dz*dxxw + ssy*r3;
  *tvz += dz*dyxw - ssx*r3;
  *twz += dz*dzxw;
}

// thick-cored particle on singular point, with gradients - 63 flops total
template <class S, class A>
static inline void kernel_0v_0pg (const S sx, const S sy, const S sz,
                                  const S sr,
                                  const S ssx, const S ssy, const S ssz,
                                  const S tx, const S ty, const S tz,
                                  A* __restrict__ tu, A* __restrict__ tv, A* __restrict__ tw,
                                  A* __restrict__ tux, A* __restrict__ tvx, A* __restrict__ twx,
                                  A* __restrict__ tuy, A* __restrict__ tvy, A* __restrict__ twy,
                                  A* __restrict__ tuz, A* __restrict__ tvz, A* __restrict__ twz) {
  // 30 flops
  const S dx = tx - sx;
  const S dy = ty - sy;
  const S dz = tz - sz;
  const S r2 = dx*dx + dy*dy + dz*dz + sr*sr;
#ifdef USE_VC
  const S r3 = Vc::reciprocal(r2*Vc::sqrt(r2));
#else
  const S r3 = 1.0 / (r2*std::sqrt(r2));
#endif
  S dxxw = dz*ssy - dy*ssz;
  S dyxw = dx*ssz - dz*ssx;
  S dzxw = dy*ssx - dx*ssy;
  *tu += r3 * dxxw;
  *tv += r3 * dyxw;
  *tw += r3 * dzxw;

  // accumulate velocity gradients
#ifdef USE_VC
  //const S bbb = -3.f * r3 * Vc::reciprocal(r2);
  const S bbb = S(-3.0) * r3 * Vc::reciprocal(r2);
#else
  const S bbb = -3.0 * r3 / r2;
#endif
  // continuing with grads - this section is 33 flops
  dxxw *= bbb;
  dyxw *= bbb;
  dzxw *= bbb;
  *tux += dx*dxxw;
  *tvx += dx*dyxw + ssz*r3;
  *twx += dx*dzxw - ssy*r3;
  *tuy += dy*dxxw - ssz*r3;
  *tvy += dy*dyxw;
  *twy += dy*dzxw + ssx*r3;
  *tuz += dz*dxxw + ssy*r3;
  *tvz += dz*dyxw - ssx*r3;
  *twz += dz*dzxw;
}


//
// influence of 3d linear constant-strength *vortex* panel on target point
//   ignoring the 1/4pi factor, which will be multiplied later
// uses four singular integration points on the source side
//   160 flops
//
template <class S, class A>
static inline void kernel_2_0p (const S sx0, const S sy0, const S sz0,
                                const S sx1, const S sy1, const S sz1,
                                const S sx2, const S sy2, const S sz2,
                                const S ssx, const S ssy, const S ssz,
                                const S tx, const S ty, const S tz,
                                A* __restrict__ tu, A* __restrict__ tv, A* __restrict__ tw) {

  // scale the strength by 1/4, to account for the 4 calls below (3 flops)
  const S strx = S(0.25) * ssx;
  const S stry = S(0.25) * ssy;
  const S strz = S(0.25) * ssz;

  // first source point (37 flops)
  {
    // prepare the source points (9 flops)
    const S sx = (sx0 + sx1 + sx2) / S(3.0);
    const S sy = (sy0 + sy1 + sy2) / S(3.0);
    const S sz = (sz0 + sz1 + sz2) / S(3.0);

    // accumulate the influence (28 flops)
    (void) kernel_0v_0p (sx, sy, sz, (S)0.0,
                        strx, stry, strz,
                        tx, ty, tz,
                        tu, tv, tw);
  }

  // second source point (40)
  {
    const S sx = (S(4.0)*sx0 + sx1 + sx2) / S(6.0);
    const S sy = (S(4.0)*sy0 + sy1 + sy2) / S(6.0);
    const S sz = (S(4.0)*sz0 + sz1 + sz2) / S(6.0);
    (void) kernel_0v_0p (sx, sy, sz, (S)0.0,
                        strx, stry, strz,
                        tx, ty, tz,
                        tu, tv, tw);
  }

  // third source point (40)
  {
    const S sx = (sx0 + S(4.0)*sx1 + sx2) / S(6.0);
    const S sy = (sy0 + S(4.0)*sy1 + sy2) / S(6.0);
    const S sz = (sz0 + S(4.0)*sz1 + sz2) / S(6.0);
    (void) kernel_0v_0p (sx, sy, sz, (S)0.0,
                        strx, stry, strz,
                        tx, ty, tz,
                        tu, tv, tw);
  }

  // final source point (40)
  {
    const S sx = (sx0 + sx1 + S(4.0)*sx2) / S(6.0);
    const S sy = (sy0 + sy1 + S(4.0)*sy2) / S(6.0);
    const S sz = (sz0 + sz1 + S(4.0)*sz2) / S(6.0);
    (void) kernel_0v_0p (sx, sy, sz, (S)0.0,
                        strx, stry, strz,
                        tx, ty, tz,
                        tu, tv, tw);
  }
}


// same, but for source source terms
template <class S, class A>
static inline void kernel_2s_0p (const S sx0, const S sy0, const S sz0,
                                const S sx1, const S sy1, const S sz1,
                                const S sx2, const S sy2, const S sz2,
                                const S ss,
                                const S tx, const S ty, const S tz,
                                A* __restrict__ tu, A* __restrict__ tv, A* __restrict__ tw) {

  // scale the strength by 1/4, to account for the 4 calls below (3 flops)
  const S strx = S(0.25) * ss;

  // first source point (37 flops)
  {
    // prepare the source points (9 flops)
    const S sx = (sx0 + sx1 + sx2) / S(3.0);
    const S sy = (sy0 + sy1 + sy2) / S(3.0);
    const S sz = (sz0 + sz1 + sz2) / S(3.0);

    // accumulate the influence (28 flops)
    (void) kernel_0s_0p (sx, sy, sz, (S)0.0,
                        strx,
                        tx, ty, tz,
                        tu, tv, tw);
  }

  // second source point (40)
  {
    const S sx = (S(4.0)*sx0 + sx1 + sx2) / S(6.0);
    const S sy = (S(4.0)*sy0 + sy1 + sy2) / S(6.0);
    const S sz = (S(4.0)*sz0 + sz1 + sz2) / S(6.0);
    (void) kernel_0s_0p (sx, sy, sz, (S)0.0,
                        strx,
                        tx, ty, tz,
                        tu, tv, tw);
  }

  // third source point (40)
  {
    const S sx = (sx0 + S(4.0)*sx1 + sx2) / S(6.0);
    const S sy = (sy0 + S(4.0)*sy1 + sy2) / S(6.0);
    const S sz = (sz0 + S(4.0)*sz1 + sz2) / S(6.0);
    (void) kernel_0s_0p (sx, sy, sz, (S)0.0,
                        strx,
                        tx, ty, tz,
                        tu, tv, tw);
  }

  // final source point (40)
  {
    const S sx = (sx0 + sx1 + S(4.0)*sx2) / S(6.0);
    const S sy = (sy0 + sy1 + S(4.0)*sy2) / S(6.0);
    const S sz = (sz0 + sz1 + S(4.0)*sz2) / S(6.0);
    (void) kernel_0s_0p (sx, sy, sz, (S)0.0,
                        strx,
                        tx, ty, tz,
                        tu, tv, tw);
  }
}


//
// influence of 3d linear constant-strength *vortex* panel on thick-cored target vortex
//   ignoring the 1/4pi factor, which will be multiplied later
// uses four singular integration points on the source side
//   168 flops
//
template <class S, class A>
static inline void kernel_2_0b (const S sx0, const S sy0, const S sz0,
                                const S sx1, const S sy1, const S sz1,
                                const S sx2, const S sy2, const S sz2,
                                const S ssx, const S ssy, const S ssz,
                                const S tx, const S ty, const S tz, const S tr,
                                A* __restrict__ tu, A* __restrict__ tv, A* __restrict__ tw) {

  // scale the strength by 1/4, to account for the 4 calls below (3 flops)
  const S strx = S(0.25) * ssx;
  const S stry = S(0.25) * ssy;
  const S strz = S(0.25) * ssz;

  // first source point (39 flops)
  {
    // prepare the source points (9 flops)
    const S sx = (sx0 + sx1 + sx2) / S(3.0);
    const S sy = (sy0 + sy1 + sy2) / S(3.0);
    const S sz = (sz0 + sz1 + sz2) / S(3.0);

    // accumulate the influence (30 flops)
    (void) kernel_0v_0b (sx, sy, sz, (S)0.0,
                        strx, stry, strz,
                        tx, ty, tz, tr,
                        tu, tv, tw);
  }

  // second source point (42)
  {
    const S sx = (S(4.0)*sx0 + sx1 + sx2) / S(6.0);
    const S sy = (S(4.0)*sy0 + sy1 + sy2) / S(6.0);
    const S sz = (S(4.0)*sz0 + sz1 + sz2) / S(6.0);
    (void) kernel_0v_0b (sx, sy, sz, (S)0.0,
                        strx, stry, strz,
                        tx, ty, tz, tr,
                        tu, tv, tw);
  }

  // third source point (42)
  {
    const S sx = (sx0 + S(4.0)*sx1 + sx2) / S(6.0);
    const S sy = (sy0 + S(4.0)*sy1 + sy2) / S(6.0);
    const S sz = (sz0 + S(4.0)*sz1 + sz2) / S(6.0);
    (void) kernel_0v_0b (sx, sy, sz, (S)0.0,
                        strx, stry, strz,
                        tx, ty, tz, tr,
                        tu, tv, tw);
  }

  // final source point (42)
  {
    const S sx = (sx0 + sx1 + S(4.0)*sx2) / S(6.0);
    const S sy = (sy0 + sy1 + S(4.0)*sy2) / S(6.0);
    const S sz = (sz0 + sz1 + S(4.0)*sz2) / S(6.0);
    (void) kernel_0v_0b (sx, sy, sz, (S)0.0,
                        strx, stry, strz,
                        tx, ty, tz, tr,
                        tu, tv, tw);
  }
}


//
// influence of 3d linear constant-strength *vortex* panel on thick-cored target vortex with gradients
//   ignoring the 1/4pi factor, which will be multiplied later
// uses four singular integration points on the source side
//   308 flops
//
template <class S, class A>
static inline void kernel_2_0bg (const S sx0, const S sy0, const S sz0,
                                 const S sx1, const S sy1, const S sz1,
                                 const S sx2, const S sy2, const S sz2,
                                 const S ssx, const S ssy, const S ssz,
                                 const S tx, const S ty, const S tz, const S tr,
                                 A* __restrict__ tu, A* __restrict__ tv, A* __restrict__ tw,
                                 A* __restrict__ tux, A* __restrict__ tvx, A* __restrict__ twx,
                                 A* __restrict__ tuy, A* __restrict__ tvy, A* __restrict__ twy,
                                 A* __restrict__ tuz, A* __restrict__ tvz, A* __restrict__ twz) {

  // scale the strength by 1/4, to account for the 4 calls below (3 flops)
  const S strx = S(0.25) * ssx;
  const S stry = S(0.25) * ssy;
  const S strz = S(0.25) * ssz;

  // first source point (74 flops)
  {
    // prepare the source points (9 flops)
    const S sx = (sx0 + sx1 + sx2) / S(3.0);
    const S sy = (sy0 + sy1 + sy2) / S(3.0);
    const S sz = (sz0 + sz1 + sz2) / S(3.0);

    // accumulate the influence (65 flops)
    (void) kernel_0v_0bg (sx, sy, sz, (S)0.0,
                          strx, stry, strz,
                          tx, ty, tz, tr,
                          tu, tv, tw,
                          tux, tvx, twx, tuy, tvy, twy, tuz, tvz, twz);
  }

  // second source point (77)
  {
    const S sx = (S(4.0)*sx0 + sx1 + sx2) / S(6.0);
    const S sy = (S(4.0)*sy0 + sy1 + sy2) / S(6.0);
    const S sz = (S(4.0)*sz0 + sz1 + sz2) / S(6.0);
    (void) kernel_0v_0bg (sx, sy, sz, (S)0.0,
                          strx, stry, strz,
                          tx, ty, tz, tr,
                          tu, tv, tw,
                          tux, tvx, twx, tuy, tvy, twy, tuz, tvz, twz);
  }

  // third source point (77)
  {
    const S sx = (sx0 + S(4.0)*sx1 + sx2) / S(6.0);
    const S sy = (sy0 + S(4.0)*sy1 + sy2) / S(6.0);
    const S sz = (sz0 + S(4.0)*sz1 + sz2) / S(6.0);
    (void) kernel_0v_0bg (sx, sy, sz, (S)0.0,
                          strx, stry, strz,
                          tx, ty, tz, tr,
                          tu, tv, tw,
                          tux, tvx, twx, tuy, tvy, twy, tuz, tvz, twz);
  }

  // final source point (77)
  {
    const S sx = (sx0 + sx1 + S(4.0)*sx2) / S(6.0);
    const S sy = (sy0 + sy1 + S(4.0)*sy2) / S(6.0);
    const S sz = (sz0 + sz1 + S(4.0)*sz2) / S(6.0);
    (void) kernel_0v_0bg (sx, sy, sz, (S)0.0,
                          strx, stry, strz,
                          tx, ty, tz, tr,
                          tu, tv, tw,
                          tux, tvx, twx, tuy, tvy, twy, tuz, tvz, twz);
  }
}


//
// influence of 3d linear constant-strength *vortex* panel on singular target point with gradients
//   ignoring the 1/4pi factor, which will be multiplied later
// uses four singular integration points on the source side
//   300 flops
//
template <class S, class A>
static inline void kernel_2_0pg (const S sx0, const S sy0, const S sz0,
                                 const S sx1, const S sy1, const S sz1,
                                 const S sx2, const S sy2, const S sz2,
                                 const S ssx, const S ssy, const S ssz,
                                 const S tx, const S ty, const S tz,
                                 A* __restrict__ tu, A* __restrict__ tv, A* __restrict__ tw,
                                 A* __restrict__ tux, A* __restrict__ tvx, A* __restrict__ twx,
                                 A* __restrict__ tuy, A* __restrict__ tvy, A* __restrict__ twy,
                                 A* __restrict__ tuz, A* __restrict__ tvz, A* __restrict__ twz) {

  // scale the strength by 1/4, to account for the 4 calls below (3 flops)
  const S strx = S(0.25) * ssx;
  const S stry = S(0.25) * ssy;
  const S strz = S(0.25) * ssz;

  // first source point (72 flops)
  {
    // prepare the source points (9 flops)
    const S sx = (sx0 + sx1 + sx2) / S(3.0);
    const S sy = (sy0 + sy1 + sy2) / S(3.0);
    const S sz = (sz0 + sz1 + sz2) / S(3.0);

    // accumulate the influence (63 flops)
    (void) kernel_0v_0pg (sx, sy, sz, (S)0.0,
                          strx, stry, strz,
                          tx, ty, tz,
                          tu, tv, tw,
                          tux, tvx, twx, tuy, tvy, twy, tuz, tvz, twz);
  }

  // second source point (75)
  {
    const S sx = (S(4.0)*sx0 + sx1 + sx2) / S(6.0);
    const S sy = (S(4.0)*sy0 + sy1 + sy2) / S(6.0);
    const S sz = (S(4.0)*sz0 + sz1 + sz2) / S(6.0);
    (void) kernel_0v_0pg (sx, sy, sz, (S)0.0,
                          strx, stry, strz,
                          tx, ty, tz,
                          tu, tv, tw,
                          tux, tvx, twx, tuy, tvy, twy, tuz, tvz, twz);
  }

  // third source point (75)
  {
    const S sx = (sx0 + S(4.0)*sx1 + sx2) / S(6.0);
    const S sy = (sy0 + S(4.0)*sy1 + sy2) / S(6.0);
    const S sz = (sz0 + S(4.0)*sz1 + sz2) / S(6.0);
    (void) kernel_0v_0pg (sx, sy, sz, (S)0.0,
                          strx, stry, strz,
                          tx, ty, tz,
                          tu, tv, tw,
                          tux, tvx, twx, tuy, tvy, twy, tuz, tvz, twz);
  }

  // final source point (75)
  {
    const S sx = (sx0 + sx1 + S(4.0)*sx2) / S(6.0);
    const S sy = (sy0 + sy1 + S(4.0)*sy2) / S(6.0);
    const S sz = (sz0 + sz1 + S(4.0)*sz2) / S(6.0);
    (void) kernel_0v_0pg (sx, sy, sz, (S)0.0,
                          strx, stry, strz,
                          tx, ty, tz,
                          tu, tv, tw,
                          tux, tvx, twx, tuy, tvy, twy, tuz, tvz, twz);
  }
}


