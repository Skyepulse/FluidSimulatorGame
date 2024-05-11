#include "kernelSpiky.h"


void KernelSpiky::setSmoothingLength(const Real h){
    _h = h;

    Real h6 = pow(h, 6);

    _c = 15.0/(M_PI*h6);
    _gc = -45.0/(M_PI*h6);
    _lc = -90.0/(M_PI*h6);
}

Real KernelSpiky::W(const Vec2f& r) const{
    return _c*pow(_h - r.length(), 3.0);
}

Vec2f KernelSpiky::gradW(const Vec2f& r) const{
    Real l = r.length();
    return _gc * r / l * pow(_h - l, 2.0);
}

Real KernelSpiky::laplW(const Vec2f& r) const{
    Real l = r.length();
    return _lc * 1.0/l * (_h - l)*(_h - 2.0*l);
}