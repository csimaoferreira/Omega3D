/*
 * BoundaryFeature.h - GUI-side descriptions of boundary features
 *
 * (c)2017-20 Applied Scientific Research, Inc.
 *            Mark J Stock <markjstock@gmail.com>
 *            Blake B Hillier <blakehillier@mac.com>
 */

#pragma once

#include "Omega3D.h"
#include "Body.h"
#include "ElementPacket.h"
#include "Feature.h"
#include "Simulation.h"
#include "json/json.hpp"

#include <iostream>
#include <vector>
#include <memory>

//
// Abstract class for any boundary feature present initially
//
class BoundaryFeature : public Feature {
public:
  explicit
  BoundaryFeature(std::shared_ptr<Body> _bp,
                  bool _ext,
                  float _x,
                  float _y,
                  float _z)
    : Feature(true),
      m_bp(_bp),
      m_external(_ext),
      m_x(_x),
      m_y(_y),
      m_z(_z)
    {}

  virtual ~BoundaryFeature() = default;
  virtual BoundaryFeature* copy() const = 0;

  std::shared_ptr<Body> get_body() { return m_bp; }

  virtual void debug(std::ostream& os) const = 0;
  virtual std::string to_string() const = 0;
  virtual std::string to_short_string() const = 0;
  virtual void from_json(const nlohmann::json) = 0;
  virtual nlohmann::json to_json() const = 0;
  virtual void create() = 0;
  virtual ElementPacket<float> init_elements(const float) const = 0;
  //virtual std::vector<float> step_elements(const float) const = 0;
  void set_body(std::shared_ptr<Body> _bp) { m_bp = _bp; }
  virtual void generate_draw_geom() = 0;
  virtual ElementPacket<float> get_draw_packet() { return m_draw; }
#ifdef USE_IMGUI
  static int obj_movement_gui(int &, char*, char*, char*, char*, char*, char*);
  static bool draw_creation_gui(std::vector<std::unique_ptr<BoundaryFeature>> &, Simulation&);
  virtual bool draw_info_gui(const std::string) = 0;
#endif

protected:
  std::shared_ptr<Body> m_bp;
  bool m_external;
  float m_x;
  float m_y;
  float m_z;
  ElementPacket<float> m_draw;
};

std::ostream& operator<<(std::ostream& os, BoundaryFeature const& ff);

//
// Parser for converting json object to new feature
//
void parse_boundary_json(std::vector<std::unique_ptr<BoundaryFeature>>&,
                         std::shared_ptr<Body>,
                         const nlohmann::json);

//
// Concrete class for a sphere or ovoid
//
class Ovoid : public BoundaryFeature {
public:
  Ovoid(std::shared_ptr<Body> _bp = nullptr,
                   bool _ext = true,
                   float _x = 0.0,
                   float _y = 0.0,
                   float _z = 0.0,
                   float _sx = 1.0,
                   float _sy = 1.0,
                   float _sz = 1.0)
    : BoundaryFeature(_bp, _ext, _x, _y, _z),
      m_sx(_sx),
      m_sy(_sy),
      m_sz(_sz)
    {}
  Ovoid* copy() const override { return new Ovoid(*this); }

  void debug(std::ostream& os) const override;
  std::string to_string() const override;
  std::string to_short_string() const override { return "ovoid"; }
  void from_json(const nlohmann::json) override;
  nlohmann::json to_json() const override;
  void create() override { }
  ElementPacket<float> init_elements(const float) const override;
#ifdef USE_IMGUI
  bool draw_info_gui(const std::string) override;
#endif
  void generate_draw_geom() override;

protected:
  float m_sx;
  float m_sy;
  float m_sz;
};


//
// Concrete class for a cube or rectangular solid
//
class SolidRect : public BoundaryFeature {
public:
  SolidRect(std::shared_ptr<Body> _bp = nullptr,
                   bool _ext = true,
                   float _x = 0.0,
                   float _y = 0.0,
                   float _z = 0.0,
                   float _sx = 1.0,
                   float _sy = 1.0,
                   float _sz = 1.0)
    : BoundaryFeature(_bp, _ext, _x, _y, _z),
      m_sx(_sx),
      m_sy(_sy),
      m_sz(_sz)
    {}
  SolidRect* copy() const override { return new SolidRect(*this); }

  void debug(std::ostream& os) const override;
  std::string to_string() const override;
  std::string to_short_string() const override { return "rectangular prism"; }
  void from_json(const nlohmann::json) override;
  nlohmann::json to_json() const override;
  void create() override { }
  ElementPacket<float> init_elements(const float) const override;
#ifdef USE_IMGUI
  bool draw_info_gui(const std::string) override;
#endif
  void generate_draw_geom() override;
protected:
  float m_sx;
  float m_sy;
  float m_sz;
};


//
// Concrete class for a flat boundary rectangle/quad
//
class BoundaryQuad : public BoundaryFeature {
public:
  BoundaryQuad(std::shared_ptr<Body> _bp = nullptr,
            float _x = 0.0,
            float _y = 0.0,
            float _z = 0.0,
            float _x1 = 1.0,
            float _y1 = 0.0,
            float _z1 = 0.0,
            float _x2 = 1.0,
            float _y2 = 1.0,
            float _z2 = 0.0,
            float _x3 = 0.0,
            float _y3 = 1.0,
            float _z3 = 0.0,
            float _bcx = 0.0,
            float _bcy = 0.0,
            float _bcz = 0.0)
    : BoundaryFeature(_bp, true, _x, _y, _z),
      m_x1(_x1),
      m_y1(_y1),
      m_z1(_z1),
      m_x2(_x2),
      m_y2(_y2),
      m_z2(_z2),
      m_x3(_x3),
      m_y3(_y3),
      m_z3(_z3),
      m_bcx(_bcx),
      m_bcy(_bcy),
      m_bcz(_bcz)
    {}
  BoundaryQuad* copy() const override { return new BoundaryQuad(*this); }

  void debug(std::ostream& os) const override;
  std::string to_string() const override;
  std::string to_short_string() const override { return "rectangular plane"; }
  void from_json(const nlohmann::json) override;
  nlohmann::json to_json() const override;
  void create() override { }
  ElementPacket<float> init_elements(const float) const override;
#ifdef USE_IMGUI
  bool draw_info_gui(std::string) override;
#endif
  void generate_draw_geom() override;

protected:
  float m_x1, m_y1, m_z1;
  float m_x2, m_y2, m_z2;
  float m_x3, m_y3, m_z3;
  float m_bcx, m_bcy, m_bcz;
};


//
// Concrete class for geometry from a file (fluid is outside)
//
class ExteriorFromFile : public BoundaryFeature {
public:
  ExteriorFromFile(std::shared_ptr<Body> _bp = nullptr,
                   bool _ext = true,
                   float _x = 0.0,
                   float _y = 0.0,
                   float _z = 0.0,
                   float _sx = 1.0,
                   float _sy = 1.0,
                   float _sz = 1.0,
                   std::string _infile = "input.obj")
    : BoundaryFeature(_bp, _ext, _x, _y, _z),
      m_sx(_sx),
      m_sy(_sy),
      m_sz(_sz),
      m_infile(_infile)
    {}
  ExteriorFromFile* copy() const override { return new ExteriorFromFile(*this); }
  
  void debug(std::ostream& os) const override;
  std::string to_string() const override;
  std::string to_short_string() const override { return "file mesh"; }
  void from_json(const nlohmann::json) override;
  nlohmann::json to_json() const override;
  void create() override { }
  ElementPacket<float> init_elements(const float) const override;
#ifdef USE_IMGUI
  bool draw_info_gui(const std::string) override;
#endif
  void generate_draw_geom() override;

protected:
  float m_sx;
  float m_sy;
  float m_sz;
  std::string m_infile;
};

