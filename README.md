# GlassOverFlow

#### A cool game made by three cool guys


Build project:
mkdir build
cmake -B build
make -C build

Update submodules:
git submodule init
git submodule update

shading:
https://developer.download.nvidia.com/presentations/2010/gdc/Direct3D_Effects.pdf

### Kernels
https://glowinggoo.com/sph/bin/kelager.06.pdf

Density:

$$W_{\text{poly6}}(r, h) = \frac{315}{64\pi h^9} (h^2 - |r|^2)^3$$
$$\nabla W_{\text{poly6}}(r, h) = -\frac{945}{32\pi h^9}r(h^2 - |r|^2)^2$$
$$\nabla^2 W_{\text{poly6}}(r, h) = -\frac{945}{32\pi h^9}(h^2 - |r|^2)(3h^2 - 7|r|^2)$$

Pressure: (be careful in 0)

$$W_{\text{spiky}}(r, h) = \frac{15}{\pi h^6}(h-|r|)^3$$
$$\nabla W_{\text{spiky}}(r, h) = -\frac{45}{\pi h^6}\frac{r}{|r|}(h-|r|)^2$$
$$\nabla^2 W_{\text{spiky}}(r, h) = -\frac{90}{\pi h^6}\frac{1}{|r|}(h-|r|)(h-2|r|)$$

Viscosity:
$$W_\text{vis}(r, h) = \frac{15}{2\pi h^3}\left(-\frac{|r|^3}{2h^3}+\frac{|r|^2}{h^2}+\frac{h}{2|r|}-1\right)$$
$$\nabla W_\text{vis}(r, h) = \frac{15}{2\pi h^3}r\left(-\frac{3|r|}{2h^3}+\frac{2}{h^2}-\frac{h}{2|r|^3}\right)$$
$$\nabla^2 W_\text{vis}(r, h) = \frac{45}{\pi h^6}(h-|r|)$$

## PROJECT ADVANCEMENT

###TODO
- Solver: update, viscosity, density, pressure, acc + vel, pos update, time integration, kernels.
- Engine: Circles, arrow, shape abstraction
- Game Design: UI prototype, level design prototype
- 
