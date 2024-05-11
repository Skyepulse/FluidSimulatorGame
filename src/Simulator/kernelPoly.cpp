#include "kernelPoly.h"


void KernelPoly::setSmoothingLength(const Real h){
    _h = h;
    _h2 = _h*_h;
    _h3 = _h2*_h;
    _1h2 = 1.0/_h2;
    _1h3 = 1.0/_h3;

    Real h9 = pow(_h, 9);

    _c = 315.0/(64.0*M_PI*h9);
    _gc = -945.0/(32.0*M_PI*h9);
    _lc = -945.0/(32.0*M_PI*h9);
}

Real KernelPoly::W(const Vec2f& r) const{
    return _c * pow(_h*_h - r.lengthSquare(), 3.0);
}

Vec2f KernelPoly::gradW(const Vec2f& r) const{
    return _gc * r * pow(_h2 - r.lengthSquare(), 2.0);
}

Real KernelPoly::laplW(const Vec2f& r) const{
    Real r2 = r.lengthSquare();
    return _lc * (_h2 - r2) * (3.0*_h2 - 7.0*r2);
}