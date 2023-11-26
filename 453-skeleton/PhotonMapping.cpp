#include "PhotonMapping.h"
#include "KDTree.h"

#include <random>
#include <vector>
#include <iostream>

void testKD() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> distr(0, 10);

	std::vector<Photon> photons;
	for (int i = 0; i < 100; i++) {
		Photon photon;
		photon[0] = distr(gen);
		photon[1] = distr(gen);
		photon[2] = distr(gen);
		photons.push_back(photon);
	};

	kdt::KDTree<Photon> kdtree(photons);
	const Photon query(5, 5, 5); //use  a point to query
	std::vector<int> results = kdtree.knnSearch(query, 5);
	for (int i : results)
		std::cout << "X: " << photons[i][0] << "  Y: " << photons[i][1] << "  Z: " << photons[i][2] << '\n';

}