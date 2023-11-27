/*
* A file that contains necessary functions and classes for contining data in a voxel based grid
* indexing starts with 0 at (0,0,0) then increases first along the x, then the y, then the z
*/
#pragma once

template <typename T>
class VoxelGrid  {
public:
	VoxelGrid(int x_length, int y_length, int z_length);
	T& at(int x, int y, int z);
	~VoxelGrid();

private:
	//how many voxels are along each axis
	int x_length = 0;
	int y_length = 0;
	int z_length = 0;

	T* data = nullptr;
};

//definitions

template <class T>
T& VoxelGrid<T>::at(int _x, int _y, int _z) {
	int offset_z = y_length * x_length * _z;
	int offset_y = x_length * _y;
	int offset_x = _x;
	int index = offset_x + offset_y + offset_z;
	return data[index];
}

template <class T>
VoxelGrid<T>::VoxelGrid(int _x_length, int _y_length, int _z_length) : x_length(_x_length), y_length(_y_length), z_length(_z_length) {
	// Initialize your voxel grid based on the provided dimensions
	data = new T[_x_length * _y_length * _z_length];  // Allocate memory for the voxel grid
}

template <class T>
VoxelGrid<T>::~VoxelGrid() {
	delete[] data;  // Free the allocated memory in the destructor
}
