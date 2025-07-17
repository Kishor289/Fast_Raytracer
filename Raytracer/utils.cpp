#define GLM_ENABLE_EXPERIMENTAL

#include "utils.h"

glm::vec3 Tris::get_centroid() {
		return (this->v1 + this->v2 + this->v3) / 3.0f;
}


AABB::AABB() {
	this->min = glm::vec3(FLT_MAX);
	this->max = glm::vec3(-FLT_MAX);
}

void AABB::expand(glm::vec3& pos) {
	this->min = glm::min(min, pos);
	this->max = glm::max(max, pos);

}

void AABB::expand(AABB& x) {
	this->min = glm::min(min, x.min);
	this->max = glm::max(max, x.max);

}

glm::vec3 AABB::centroid() {
	return ((this->min + this->max) * 0.5f);

}

float AABB::surface_area() {
	glm::vec3 diff = (this->max - this->min);
	return (diff.x * diff.y + diff.x * diff.z + diff.y * diff.z) * 2.0f;
}

float convert_rad(float deg) {
	//180 deg = pi 
	return (deg / 180) * PI;
}