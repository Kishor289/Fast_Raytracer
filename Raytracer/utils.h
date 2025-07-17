#pragma once

#include "glm/glm.hpp"

#define PI 3.14159

class Tris {
public:
	glm::vec3 v1;
	glm::vec3 v2;
	glm::vec3 v3;
	glm::vec3 n;
	Tris() : v1(0), v2(0), v3(0), n(0) {}
	Tris(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 n) : v1(v1), v2(v2), v3(v3), n(n) {}

	glm::vec3 get_centroid();
};


class material {
public:
	//static material default_material;
	glm::vec3 albedo;

};

class Ray {
public:
	glm::vec3 origin;
	glm::vec3 dir;
};

class AABB {
public:
	glm::vec3 min;
	glm::vec3 max;

	AABB();

	void expand(glm::vec3& pos);

	void expand(AABB& x);
	
	glm::vec3 centroid();

	float surface_area();

};

class BVHNode {
public:
	AABB box;
	int left; //-1 for leaf node
	int right;
	int start;
	int count;
};

class Bucket {
public:
	AABB elements_box;
	int size = 0;
};
float convert_rad(float deg);