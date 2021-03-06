#ifndef ELEMENT_H
#define ELEMENT_H

#include "base.h"
#include "node.h"
#include "utils.h"
#include "material.h"
#include "Eigen/Geometry"
#include <Eigen/IterativeLinearSolvers>
#include <memory>
#include <vector>

namespace parafem
{

typedef Eigen::Triplet<double> trip;

Eigen::Matrix2Xd to_matrix(std::vector<Vector2>);
Eigen::Matrix2d find_rot_mat(std::vector<Vector2>, std::vector<Vector2>);
Eigen::Matrix2d find_rot_mat(Eigen::Matrix2Xd in, Eigen::Matrix2Xd out);


struct CoordSys: Base
{
    CoordSys(){};
    CoordSys(Vector3);
    CoordSys(Vector3, Vector3);

    Eigen::Matrix<double, 2, 3> mat;            // matrix to transform global(3d) to local(2d)
    Vector3 n, t1, t2;
    void rotate(std::vector<Vector2>, std::vector<Vector2>);
    void rotate(Eigen::MatrixX2d, Eigen::MatrixX2d);
    void update(Vector3);
    
    void update(Vector3, Vector3);
    Vector2 to_local(Vector3);
    Vector3 to_global(Vector2);
    
};

struct IntegrationPoint: Base
{
    IntegrationPoint(double eta, double zeta, double weight);
    double eta;
    double zeta;
    double weight;
    Vector3 stress;
};

struct Element: Base
{
    std::vector<NodePtr> nodes;
    virtual void geometry_step() = 0;
    virtual void explicit_step(double h) = 0;  // compute the internal forces acting on the nodes.
    virtual void implicit_step(std::vector<trip> & Kt) = 0;
    virtual void implicit_stresses() = 0;
    std::vector<int> get_number();
    virtual Vector3 get_stress()=0;
    bool is_valid = true;
    double dViscous;
    double characteristicLength;
    virtual MaterialPtr get_material() = 0;
};

struct Truss: public Element
{
    Vector3 pressure;
    Vector3 tangent;
    double length;
    Truss(const std::vector<NodePtr>, std::shared_ptr<TrussMaterial>);
    virtual Vector3 get_stress();
    double stress;           // at timestep n
    virtual void geometry_step();
    virtual void explicit_step(double h);
    virtual void implicit_step(std::vector<trip> & Kt);
    virtual void implicit_stresses();
    std::shared_ptr<TrussMaterial> material;
    void add_nodal_pressure(Vector3);
    MaterialPtr get_material();
};

struct LineJoint: public Element
{
// element that connects nodes that are not exactly matching
    LineJoint(const std::vector<NodePtr>, std::shared_ptr<TrussMaterial>);
    virtual void geometry_step();
    virtual void explicit_step(double h);
    virtual void implicit_step(std::vector<trip> & Kt);
    virtual void implicit_stresses();
    virtual Vector3 get_stress();
    std::shared_ptr<TrussMaterial> material;
};

struct Membrane: public Element
{
    Vector3 center;
    CoordSys coordSys;
    double area;
    double pressure;                 // pressure acting on internal forces

    Vector3 calculate_center();
    
    virtual void explicit_step(double h) = 0;
    virtual void implicit_step(std::vector<trip> & Kt) = 0;
    virtual void implicit_stresses() = 0;
    
    std::shared_ptr<MembraneMaterial> material;
    MaterialPtr get_material();
};

struct Membrane3: public Membrane
{
    Membrane3(const std::vector<NodePtr>, std::shared_ptr<MembraneMaterial>);
    virtual void geometry_step();
    virtual void explicit_step(double h);
    virtual void implicit_step(std::vector<trip> & Kt);
    virtual void implicit_stresses();
    Vector3 stress;
    virtual Vector3 get_stress();

    Eigen::Matrix<double, 3, 2> pos_mat, vel_mat;
    Eigen::Matrix<double, 2, 3> B, dN;
};

struct Membrane4: public  Membrane
{
    Membrane4(const std::vector<NodePtr>, std::shared_ptr<MembraneMaterial>, bool reduced_integration=true);
    std::vector<IntegrationPoint> integration_points;  //eta, zeta, weight, stress
    virtual void geometry_step();
    virtual void explicit_step(double h);
    virtual void implicit_step(std::vector<trip> & Kt);
    virtual void implicit_stresses();
    
    // hourglass control
    void initHG();
    Eigen::Vector4d hg_gamma;
    double hg_const;
    Vector2 hg_stress;
    
    
    Eigen::Matrix<double, 4, 2> pos_mat, vel_mat;
    Eigen::Matrix<double, 2, 4> B, dN;
    virtual Vector3 get_stress();
    
};

typedef std::shared_ptr<Truss> TrussPtr;
typedef std::shared_ptr<Membrane> MembranePtr;
typedef std::shared_ptr<Membrane3> Membrane3Ptr;
typedef std::shared_ptr<Membrane4> Membrane4Ptr;
typedef std::shared_ptr<Element> ElementPtr;
typedef std::vector<ElementPtr> ElementVec;
}

#endif
