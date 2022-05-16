# 3D-GE-Design-Shading-and-Diffuse-Reflection
## Parameter Setting
---
### Cube Setting
Cube Length = 100, Start Point of the cube(xc, yc, zc) = (0, 0, 10), Virtual Camera Location E(ex, ey, ez) = (200, 200, 200)
### Coordinate axis
X axis = RED, Y axis = Green, Z axis = BLUE
### Diffuse Reflection
Reflectivity: kD = (kR, kG, kB) = (0.9, 0, 0), Color display Range: 20 - 255
### Light Source
Ps = (xs, ys, zs) = (40, 60, 220)
## Formula Reference
---
### Convert 3D Point to 2D Point for Viewer
![image](https://user-images.githubusercontent.com/91507316/168557474-e309482a-17dd-43d7-9e25-76ac8a0cb9a8.png)
![image](https://user-images.githubusercontent.com/91507316/168557519-e9656c91-8b45-494d-9f37-e4a5199a06e8.png)
### Diffuse Reflection
---
#### Boundary
DDA Algorithm
#### interior
r = √((Ps_x – Pi_x)2 + (Ps_y – Pi_y)2 + (Ps_z – Pi_z)2), cosφ = √((Ps_x – Pi_x)2 / r

Reflectivity = (kD x 255 x Cosφ) / r2

Post-Processing: Color(20 - 255) = Reflectivity x display_scaling + display_shifting
### Decoration
---
#### Left Surface of The Cube
Fix x value to size and modify y and z value to form Rotating Square
#### Right Surface of The Cube
Fix y value to size and modify x and z value to form Tree Branch
