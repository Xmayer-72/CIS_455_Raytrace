#pragma once

#include <cmath>
#include <limits>
#include <vector>

#include "Color.h"
#include "Sphere.h"
#include "CanvasBase.h"
#include "Misc.h"
#include "Light.h"

class RaytracingCanvas final : public CanvasBase
{
private:
    static constexpr float viewport_size        = 1.0;
    static constexpr float projection_plane_z   = 1.0;

    std::vector<Sphere> _spheres;
    std::vector<Color>  _sphere_colors;
    std::vector<Light>  _lights;

    vec3f canvas_to_viewport(const vec2i& pt) const{
        return{
            static_cast<float>(pt.x) * viewport_size / static_cast<float>(_width),
            static_cast<float>(pt.y) * viewport_size / static_cast<float>(_height),
            projection_plane_z
        };
    }

    vec3f compute_lighting(const vec3f& point, const vec3f& normal) const{
        vec3f color_intenstiy{0.0f, 0.0f, 0.0f};

        for (auto& light : _lights)
        {
            if(light.light_type == Light::LightType::Ambient){
                color_intenstiy.x += light.color_intensity.x;
                color_intenstiy.y += light.color_intensity.y;
                color_intenstiy.z += light.color_intensity.z;
            }
            else{
                auto vec_l = light.position; //getting pos / dir

                if (light.light_type == Light::LightType::Point){
                    vec_l = vec_l - point;
                }

                auto n_dot_l = compute_dot_product(normal, vec_l);

                if (n_dot_l > 0){
                    color_intenstiy.x += light.color_intensity.x * n_dot_l / compute_vector_length(vec_l);
                    color_intenstiy.y += light.color_intensity.y * n_dot_l / compute_vector_length(vec_l);
                    color_intenstiy.z += light.color_intensity.z * n_dot_l / compute_vector_length(vec_l);
                }
            }
        }

        return color_intenstiy;        
    }

    static Color combine_color_and_intensity(const Color& color, vec3f& intensity){
        auto r_uint = static_cast<unsigned int>(static_cast<float>(color.r) * intensity.x);
        auto g_uint = static_cast<unsigned int>(static_cast<float>(color.g) * intensity.y);
        auto b_uint = static_cast<unsigned int>(static_cast<float>(color.b) * intensity.z);

        r_uint = r_uint < 0 ? 0: r_uint > 255 ? 255: r_uint;
        g_uint = g_uint < 0 ? 0: g_uint > 255 ? 255: g_uint;
        b_uint = b_uint < 0 ? 0: b_uint > 255 ? 255: b_uint;
        

        return Color::custom(
            static_cast<uint8_t>(r_uint),
            static_cast<uint8_t>(g_uint),
            static_cast<uint8_t>(b_uint)
            );
    }

    Color trace_ray(const vec3f& camera_pos, const vec3f& ray_dir, float min_t) const{
        auto closest_t = std::numeric_limits<float>::max();
        auto closest_sphere_index = -1;

        for (int i = 0; i < static_cast<int>(_spheres.size()); ++i){
            vec2f intersections;
            
            auto intersects = instersect_ray_sphere(camera_pos, ray_dir,_spheres[i], intersections);

            if (!intersects){
                continue;
            }

            if (intersections.x < closest_t && intersections.x > min_t){
                closest_t = intersections.x;
                closest_sphere_index = i;
            }

            if (intersections.y < closest_t && intersections.y > min_t){
                closest_t = intersections.y;
                closest_sphere_index = i;
            }
        }

        if (closest_sphere_index < 0){
            return Color::black;
        }

        auto point = camera_pos + closest_t * ray_dir;

        auto normal = point - _spheres[closest_sphere_index].center;
        normal = normal / compute_vector_length(normal); 

        auto intensity = compute_lighting(point, normal);

        auto color = combine_color_and_intensity(
            _sphere_colors[closest_sphere_index],
            intensity);

        return color;
    }

    static bool instersect_ray_sphere(const vec3f& camera_pos, 
                    const vec3f& ray_dir, const Sphere& sphere, vec2f& intersections){
        auto camera_to_sphere_center = camera_pos - sphere.center;

        auto k1 = compute_dot_product(ray_dir, ray_dir);
        auto k2 = 2* compute_dot_product(camera_to_sphere_center, ray_dir);
        auto k3 = compute_dot_product(camera_to_sphere_center, camera_to_sphere_center)
                - sphere.radius * sphere.radius;

        auto discriminant = k2 * k2 - 4 * k1 * k3;

        if (discriminant < 0){
            return false;
        }
        
        auto t1 = (-k2 + std::sqrt(discriminant)) / (2 * k1);
        auto t2 = (-k2 - std::sqrt(discriminant)) / (2 * k1);

        intersections = {t1, t2};
        return true;
    }

public:
    RaytracingCanvas(const char* window_title, size_t width, size_t height)
        :   CanvasBase(window_title, width, height) {

    }

    void add_sphere(const Sphere& sphere, const Color& color){
        _spheres.push_back(sphere);
        _sphere_colors.push_back(color);
    }

    void add_light(const Light& light){
        _lights.push_back(light);
    }

    void draw() const{
        vec3f camera_pos{0.0f, 0.0f, 0.0f};
        auto min_t = projection_plane_z;

        auto half_width = static_cast<int>(_width / 2);
        auto half_height = static_cast<int>(_height / 2);

        for (auto x = -half_width; x < half_width; ++x){
            for (auto y = -half_height; y < half_height; ++y){
                vec2i ray_canvas_pos{x, y};

                auto ray_dir = canvas_to_viewport(ray_canvas_pos);

                auto c = trace_ray(camera_pos, ray_dir, min_t);

                put_pixel(ray_canvas_pos, c);
            }
        }
        
    }
};