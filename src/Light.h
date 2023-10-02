#pragma once

#include "Vec.h"

class Light
{
private:

public:
    enum class LightType{Ambient, Directional, Point};

    LightType   light_type;
    vec3f       color_intensity;  //can use vec3 for color
    vec3f       position;   //tech. position / direction

    Light(LightType light_type, const vec3f& intensity, const vec3f& position = {0.0f, 0.0f, 0.0f})
        :   light_type(light_type),
            color_intensity(intensity),
            position(position){
    }
};