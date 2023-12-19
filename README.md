Rendering simulation of 'Diffraction Grating' / 'Play of Colour' in opals
The program aims to simulate the play of colour in opals, by simulating an opal macroscopic structure through a voxel grid with various parameters that control the silica structure of opals.
These silica structures cause a diffraction grating effect creating various colours on the surface of opals.

Derived from ['Rendering 'Play of Colour' using Stratified based on Amorphous structure of Opal'](https://books.google.ca/books?hl=en&lr=&id=oz-Sinxlj08C&oi=fnd&pg=PT427&dq=light+diffraction,+opal,+rendering&ots=y_AzHGmWo-&sig=N4BT5j7o0m7SvHPR6No4Gx1BusM#v=onepage&q=light%20diffraction%2C%20opal%2C%20rendering&f=false)

### Technology
C++,
OpenGL,
ImGUI

### Keys:
Q -> Exits Program

WASD -> Navigate 3D Environment

Left Mouse Button -> Hold and pan to pan view

P -> Toggle ImGUI panel

### ImGUI Panel

#### Background Colour Section
Allows user to change the background colour to their liking

#### Clipping Section
Allows to the user to change the world coordinate to set a clipping plane to.
This allows the user to see 'inside' the opal, clipping the voxel grid trimmed to an ellipsoid shape

#### Rendering Settings Section
This is a dynamic area will change based on which rendering style is chosen in the radio buttons beneath

##### When 'Orientation' is chosen
This will not have any rendering settings.
This rendering view, will show the clusters inside the opal, mapped to RGB space based on their 'orientation'
The orientation is which direction that cluster group is facing. This orientation will influence the diffraction grating effect.

##### When 'Greating' is chosen
This rendering view displays the diffraction grating on only the surface of the opal

**Light position**
User can control where the light is positioned in the scene

**kd**
User can control the diffuse factor of the clusters

**ks**
User can control the specular factor of the clusters

**kt**
User can control the transmission factor of the cluster

**Grating Spacing**
How much space is between each grate.
This will influence the size of the diffraction grate.
Recommended setting: 750.0

**Body Colour**
User can control the 'body colour' of the opal, [0,1]
From white to black, this is the colour of the cluster when no light is being reflected

**Use Sanders / Particle Diameter**
This controls the size of the silica particles in the microscopic structure.
This will influence the look of the diffraction grating (dominating colour spectrums).
The 'Use Sanders' checkbox needs to be checked for this function to take effect.
Recommended Setting: 2300.0

##### When 'Ray Trace' is chosen (NOT FULLY WORKING)
WARNING: This will lock down the ImGUI panel, the user will need to hold down the number 1 to return to RGB Orientation mode.
Proceeds to ray trace the scene using a monte carlo ray tracer.

#### Cluster Control Section
This section lets you control the cluster generation of the opal

**Cluster Percentage**
User can control which percentage of the opal is filled with clusters.

**Cluster Generation Type**
User can control what type of cluster generation they would like.
A description dropdown exists for users to get in depth description of what each type is.

_Linear Random_
Each cluster is the same size, and fills the cluster randomly based on a linear random distribution

_Exp Dist Sphere_
Each cluster is a random radius based on an exponential distribution function.

**If Exp Dist Sphere is chosen**
A 'mean radius' float entry will show up for the user to determine what is the mean radius for the spheres.
It defaults to 3.0, recommended settings: 2.0 or lower

**Neighbour Sampling Checkbox**
This checkbox applies to both 'Linear Random' and 'Exp Dist Sphere' Generations
It lets the user decide if the cluster generation will sample neighbouring clusters for their orientation and set the current cluster being generated to the average of the neighbours.

An additional checkbox exists for 'Exp Dist Sphere' when neighbour sampling is selected, to determine the size of the neighbour sample. Additional information is shown in the ImGUI panel explaining these two options in more detail.

#### Cluster Info Section
This section provides info about the clusters.
The dimension size of the cluster (it generates as a rectangle then gets trimmed to an ellipsoid).
The size is the rectangular dimensions
The total voxels, the target cluster percentage, and the actual cluster percentage (these numbers vary as when creating spherical clusters the generation does not stop when it hits exactly the target, it stops after a sphere is generated, then checks the percentage). 
The cluster, void, and empty counts. (Empty are voxels in the grid that do not contribute to the opal properties, they are the voxels that were trimmed during the ellipsoid trimming process).

The user can also poll a certain voxel at the chosen index to see details above that particular voxel.
