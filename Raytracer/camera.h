#pragma once
#include "glm/glm.hpp"

class Camera {
public:
    //parameters
    float focal_length;
    float viewport_height;
    float viewport_width;
    float d_fov;
    float d_focus_angle;

    int img_height;
    int img_width;

    glm::vec3 look_at = glm::vec3(0, 0, -1);
    glm::vec3 look_from = glm::vec3(0, 0, 0); //help to get view dir
    double roll_angle;


    //default constructor
    Camera() : focal_length(0), viewport_height(0), viewport_width(0), img_height(0), img_width(0) {}

    //custom constructor
    Camera(float focal_length, float d_fov, int img_height, int img_width)
        : focal_length(focal_length), d_fov(d_fov), img_height(img_height), img_width(img_width) {

        viewport_height = 2 * focal_length * tan(convert_rad(d_fov / 2));
        viewport_width = (viewport_height * img_width) / img_height;
    }
};