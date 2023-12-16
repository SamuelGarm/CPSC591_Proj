#pragma once

struct Photon {
	glm::vec3 pos = glm::vec3(0);
	glm::vec3 dir = glm::vec3(0, 1, 0);
	float wavelength = 0;
	// the constructors
	Photon() {}
	Photon(glm::vec3 _pos, glm::vec3 _dir, float _wavelength)
	{
		dir = _dir;
		pos = _pos;
		wavelength = _wavelength;
	}
};