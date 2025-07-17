#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "utils.h"
#include "obj_loader.h"



class Object {
public:
	std::vector<Tris> Tri_data;
	std::vector<int> Tri_index;
	std::vector<BVHNode> BVH_Tree;
	glm::mat4 transform = glm::mat4(1.0f); //obj to world
	glm::mat4 inv_transform = glm::mat4(1.0f); // world to obj
	material m;

	int leaf_limit = 4;
	int BUCKET_SIZE = 12;

	Object() {};
	void load_obj(const char* file);

	void set_pos_rot(glm::vec3 pos, glm::vec3 rot);

	void set_material(material mat);

	Object(const char* file, glm::vec3 pos, glm::vec3 rot, material m);

	void Transform_ray_W2O(Ray& r);

	void BuildBVH(int start, int count);

};