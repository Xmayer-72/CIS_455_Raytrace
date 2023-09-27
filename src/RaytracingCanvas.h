#pragma once

#include <cmath>
#include <limits>
#include <vector>

#include "Color.h"
#include "Sphere.h"
#include "CanvasBase.h"
#include "Misc.h"

class RaytracingCanvas final : public CanvasBase
{
private:
    static constexpr float viewport_size        = 1.0;
    static constexpr float projection_plane_z   = 1.0;

    std::vector<Sphere> _spheres;
    std::vector<Color> _sphere_colors;

    vec3f canvas_to_viewport(const vec2i& pt) const{
        return{
            static_cast<float>(pt.x) * viewport_size / static_cast<float>(_width),
            static_cast<float>(pt.y) * viewport_size / static_cast<float>(_height),
            projection_plane_z
        };
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

        return _sphere_colors[closest_sphere_index];          
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