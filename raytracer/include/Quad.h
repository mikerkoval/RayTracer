#ifndef _QUAD_H
#define _QUAD_H

#include "Object.h"
#include "Vect.h"
#include "Color.h"
#include "Ray.h"
#include <cmath>

// A flat quad defined by a center, normal, and two half-extents.
// Default orientation: lies in the XZ plane (normal = 0,1,0).
// Use rotateX/Y/Z to orient as walls/ceiling.
class Quad : public Object {
    Vect center;
    double width;   // half-extent along local X
    double depth;   // half-extent along local Z
    Color color;

    // Local axes (updated by rotate calls)
    Vect normal  { 0, 1, 0 };
    Vect tangent { 1, 0, 0 };
    Vect bitangent { 0, 0, 1 };

public:
    Quad(Vect center, double width, double depth, Color color)
        : center(center), width(width), depth(depth), color(color) {}

    virtual void rotateX(double a) {
        // Rotate normal, tangent, bitangent around X axis
        auto rot = [&](Vect v) {
            return Vect(v.getX(),
                        v.getY()*cos(a) - v.getZ()*sin(a),
                        v.getY()*sin(a) + v.getZ()*cos(a));
        };
        normal    = rot(normal).normalize();
        tangent   = rot(tangent).normalize();
        bitangent = rot(bitangent).normalize();
    }

    virtual void rotateY(double a) {
        auto rot = [&](Vect v) {
            return Vect(v.getX()*cos(a) + v.getZ()*sin(a),
                        v.getY(),
                       -v.getX()*sin(a) + v.getZ()*cos(a));
        };
        normal    = rot(normal).normalize();
        tangent   = rot(tangent).normalize();
        bitangent = rot(bitangent).normalize();
    }

    virtual void rotateZ(double a) {
        auto rot = [&](Vect v) {
            return Vect(v.getX()*cos(a) - v.getY()*sin(a),
                        v.getX()*sin(a) + v.getY()*cos(a),
                        v.getZ());
        };
        normal    = rot(normal).normalize();
        tangent   = rot(tangent).normalize();
        bitangent = rot(bitangent).normalize();
    }

    virtual void translate(Vect v) {
        center = center.add(v);
    }

    virtual Color getColor(Vect p) {
        if (texture) {
            Vect d = p.add(center.negative());
            double u = d.dotProduct(tangent)   / width   * 0.5 + 0.5;
            double v = d.dotProduct(bitangent) / depth   * 0.5 + 0.5;
            if (u < 0) u = 0; if (u > 1) u = 1;
            if (v < 0) v = 0; if (v > 1) v = 1;
            return texture->getColor(u, v);
        }
        return color;
    }

    virtual Vect getNormalAt(Vect point) {
        return normal;
    }

    virtual double findIntersection(Ray ray) {
        Vect d = ray.getDirection();
        Vect o = ray.getOrigin();

        double denom = d.dotProduct(normal);
        if (fabs(denom) < 1e-9) return -1;

        Vect oc = center.add(o.negative());
        double t = oc.dotProduct(normal) / denom;
        if (t < 0.001) return -1;

        Vect hit = o.add(d.mult(t));
        Vect local = hit.add(center.negative());

        double u = local.dotProduct(tangent);
        double v = local.dotProduct(bitangent);

        if (fabs(u) > width * 0.5 || fabs(v) > depth * 0.5) return -1;

        return t;
    }
};

#endif
