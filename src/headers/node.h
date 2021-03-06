#ifndef NODE_H
#define NODE_H

#include "base.h"
#include "utils.h"
#include "Eigen/Geometry"
#include <memory>
#include <vector>


namespace parafem
{

    
struct Node: Base
{
public:
    Node(double x, double y, double z);
    Vector3 previous_position;
    Vector3 position;
    Vector3 velocity;
    Vector3 acceleration;
    
    Vector3 external_force;
    Vector3 internal_force;

    void add_external_force(Vector3 force);
    
    double mass_influence = 1;

    void solveEquilibrium(double h, double external_factor=1);
    Vector3 fixed;
    
    int nr;
};

typedef std::shared_ptr<Node> NodePtr;
typedef std::vector<NodePtr> NodeVec;

}
#endif