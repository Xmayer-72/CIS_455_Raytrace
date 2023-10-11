#pragma once

#include <cmath>
#include <limits>
#include <vector>
#include <tuple>

#include "Color.h"
#include "Sphere.h"
#include "CanvasBase.h"
#include "Misc.h"
#include "Light.h"
#include "Mat.h"

class RaytracingCanvas final : public CanvasBase
{
private:
    static constexpr float  viewport_size        = 1.0;
    static constexpr float  projection_plane_z   = 1.0;

    vec3f                   _camera_position;
    Mat                     _camera_orientation;
    unsigned int            _max_ray_recursion;
    std::vector<Sphere>     _spheres;
    std::vector<Color>      _sphere_colors;
    std::vector<Light>      _lights;
    std::vector<float>      _sphere_speculars;
    std::vector<float>      _sphere_reflectivities;

    vec3f canvas_to_viewport(const vec2i& pt) const{
        return{
            static_cast<float>(pt.x) * viewport_size / static_cast<float>(_width),
            static_cast<float>(pt.y) * viewport_size / static_cast<float>(_height),
            projection_plane_z
        };
    }

    vec3f compute_lighting(const vec3f& point, const vec3f& normal, const vec3f& ray_dir, float specular) const{
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

                //shadow
                auto intersection = find_closest_sphere_intersection(
                    point, vec_l, 0.01f, std::numeric_limits<float>::max());

                if (std::get<0>(intersection) >= 0)
                {
                    continue;
                }                

                //difuse
                auto n_dot_l = compute_dot_product(normal, vec_l);

                if (n_dot_l > 0){
                    color_intenstiy.x += light.color_intensity.x * n_dot_l / compute_vector_length(vec_l);
                    color_intenstiy.y += light.color_intensity.y * n_dot_l / compute_vector_length(vec_l);
                    color_intenstiy.z += light.color_intensity.z * n_dot_l / compute_vector_length(vec_l);
                }

                //specular
                auto vec_r = reflect_ray(normal, vec_l);

                auto r_dot_v = compute_dot_product(vec_r, -1 * ray_dir);

                if (r_dot_v > 0){
                    auto a = compute_vector_length(ray_dir);
                    auto b = compute_vector_length(vec_r);
                    auto c = r_dot_v / (b * a);
                    auto s = std::pow(c, specular);
                    color_intenstiy += light.color_intensity * s;
                }
            }
        }

        return color_intenstiy;        
    }

    static vec3f reflect_ray(const vec3f& normal, const vec3f& ray){
        return ((2.0f * compute_dot_product(normal, ray)) * normal) - ray;
    }

    Color trace_ray(const vec3f& camera_pos, const vec3f& ray_dir, float min_t, unsigned int recursion_depth) const{
        auto intersection = find_closest_sphere_intersection(
            camera_pos, ray_dir, min_t, std::numeric_limits<float>::max());
        
        auto closest_sphere_index = std::get<0>(intersection);
        auto closest_t =            std::get<1>(intersection);

        if (closest_sphere_index < 0){
            return Color::black;
        }

        auto sphere =               _spheres                [closest_sphere_index];
        auto sphere_color =         _sphere_colors          [closest_sphere_index];
        auto sphere_specular =      _sphere_speculars       [closest_sphere_index];
        auto sphere_reflectivity =  _sphere_reflectivities  [closest_sphere_index];

        auto point = camera_pos + closest_t * ray_dir;

        auto normal = point - sphere.center;
        normal = normal / compute_vector_length(normal); 

        auto intensity = compute_lighting(point, normal, ray_dir, sphere_specular);

        auto local_color = sphere_color * intensity;

        if (sphere_reflectivity <= 0.0f || recursion_depth <= 0)
        {
            return local_color;
        }
        
        auto reflected_ray = reflect_ray(normal, -ray_dir);

        auto reflected_color = trace_ray(point, reflected_ray, 0.01f, recursion_depth - 1);

        auto c1 = local_color * (1.0f - sphere_reflectivity);
        auto c2 = reflected_color * sphere_reflectivity;

        return c1 + c2;
    }

    std::tuple<int, float> find_closest_sphere_intersection(
            const vec3f& camera_pos, const vec3f& ray_dir, float min_dist, float max_dist)
        const{
        auto closest_t = max_dist;
        auto closest_sphere_index = -1;

        for (int i = 0; i < static_cast<int>(_spheres.size()); ++i){
            vec2f intersections;
            
            auto intersects = instersect_ray_sphere(camera_pos, ray_dir,_spheres[i], intersections);

            if (!intersects){
                continue;
            }

            if (intersections.x < closest_t && intersections.x > min_dist){
                closest_t = intersections.x;
                closest_sphere_index = i;
            }

            if (intersections.y < closest_t && intersections.y > min_dist){
                closest_t = intersections.y;
                closest_sphere_index = i;
            }
        }

        return {closest_sphere_index, closest_t};
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
    RaytracingCanvas(const char* window_title, size_t width, size_t height, unsigned int max_ray_recursion)
        :   CanvasBase(window_title, width, height),
            _camera_position{0.0f, 0.0f, 0.0f},
            _camera_orientation(Mat::get_identity_matrix()),
            _max_ray_recursion(max_ray_recursion) {
    }

    void add_sphere(const Sphere& sphere, const Color& color, float specular, float reflectivity = 1){
        _spheres.push_back(sphere);
        _sphere_colors.push_back(color);
        _sphere_speculars.push_back(specular);
        _sphere_reflectivities.push_back(reflectivity);
    }

    void add_light(const Light& light){
        _lights.push_back(light);
    }

    std::vector<Sphere>& get_spheres(){
        return _spheres;
    }

    std::vector<Light> get_lights(){
        return _lights;
    }

    void set_camera_pos(const vec3f& camera_pos){
        _camera_position = camera_pos;
    }

    void set_camera_orient(const Mat& camera_orientation){
        _camera_orientation = camera_orientation;
    }

    void draw() const{
        auto min_t = projection_plane_z;

        auto half_width = static_cast<int>(_width / 2);
        auto half_height = static_cast<int>(_height / 2);

        for (auto x = -half_width; x < half_width; ++x){
            for (auto y = -half_height; y < half_height; ++y){
                vec2i ray_canvas_pos{x, y};

                //auto ray_dir = canvas_to_viewport(ray_canvas_pos);

                auto ray_dir_v4 = _camera_orientation * canvas_to_viewport(ray_canvas_pos);
                auto ray_dir_v3 = vec3f{ray_dir_v4.x, ray_dir_v4.y, ray_dir_v4.z};

                auto c = trace_ray(_camera_position, ray_dir_v3, min_t, _max_ray_recursion);

                put_pixel(ray_canvas_pos, c);
            }
        }
        
    }
};