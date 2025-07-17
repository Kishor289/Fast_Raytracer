#pragma once
#include "glm/glm.hpp"
#include "utils.h"

#include "Object.h"
#include "camera.h"

#include <iostream>

class Scene {
public:
	std::vector<Object> ObjectList;
	Camera cam;
	AABB global_bb;

	Scene(Camera cam) : cam(cam){}
	void addObj(Object obj) {
		ObjectList.push_back(obj);
	}
	void gen_global_BB() {
		for (auto i : ObjectList) {
			global_bb.expand(i.BVH_Tree[0].box);
		}
	}

	
};