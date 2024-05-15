#include "kernelVis.h"


void KernelVis::setSmoothingLength(const Real h){
    _h = h;
    _h2 = _h*_h;
    _h3 = _h2*_h;
    _1h2 = 1.0/_h2;
    _1h3 = 1.0/_h3;

    Real h3 = pow(h, 3);

    _c = 15.0/(2*M_PI*h3);
    _gc = 15.0/(2*M_PI*h3);
    _lc = 45.0/(M_PI*h3*h3);
}

Real KernelVis::W(const Vec2f& r) const{
    Real l = r.length();
    return _c*(-l*l*l/2.0 * _1h3 + l*l*_1h2 + _h/(2.0*l) - 1.0);
}

Vec2f KernelVis::gradW(const Vec2f& r) const{
    Real l = r.length();
    return _gc * r * (-3.0*l/2.0 * _1h3 + 2.0*_1h2 - _h/(2*l*l*l));
}

Real KernelVis::laplW(const Vec2f& r) const{
    return _lc * (_h - r.length());
}