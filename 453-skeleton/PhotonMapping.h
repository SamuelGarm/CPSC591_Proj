#include <array>

struct Photon : public std::array<double, 3> {
	//specify dimension for the KD tree
	static const int DIM = 3;

	// the constructors
	Photon() {}
	Photon(double x, double y, double z)
	{
		(*this)[0] = x;
		(*this)[1] = y;
		(*this)[2] = z;
	}

	//char p[4]; // power packed as 4 chars (RGB) WILL NEED TO CHANGE TO WAVELENGTH
	//char phi, theta; // compressed incident direction
};

void testKD();