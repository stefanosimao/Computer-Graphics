/**
@file main.cpp
*/

#include <iostream>
#include <fstream>
#include <cmath>
#include <ctime>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#include "Image.h"
#include "Material.h"

using namespace std;

/**
 Class representing a single ray.
 */
class Ray{
public:
    glm::vec3 origin; ///< Origin of the ray
    glm::vec3 direction; ///< Direction of the ray
	/**
	 Contructor of the ray
	 @param origin Origin of the ray
	 @param direction Direction of the ray
	 */
    Ray(glm::vec3 origin, glm::vec3 direction) : origin(origin), direction(direction){
    }
};


class Object;

/**
 Structure representing the even of hitting an object
 */
struct Hit{
    bool hit; ///< Boolean indicating whether there was or there was no intersection with an object
    glm::vec3 normal; ///< Normal vector of the intersected object at the intersection point
    glm::vec3 intersection; ///< Point of Intersection
    float distance; ///< Distance from the origin of the ray to the intersection point
    Object *object; ///< A pointer to the intersected object
	glm::vec2 uv; ///< Coordinates for computing the texture (texture coordinates)
};

/**
 General class for the object
 */
class Object{
	
protected:
	glm::mat4 transformationMatrix; ///< Matrix representing the transformation from the local to the global coordinate system
	glm::mat4 inverseTransformationMatrix; ///< Matrix representing the transformation from the global to the local coordinate system
	glm::mat4 normalMatrix; ///< Matrix for transforming normal vectors from the local to the global coordinate system
	
public:
	glm::vec3 color; ///< Color of the object
	Material material; ///< Structure describing the material of the object
	/** A function computing an intersection, which returns the structure Hit */
    virtual Hit intersect(Ray ray) = 0;

	/** Function that returns the material struct of the object*/
	Material getMaterial(){
		return material;
	}
	/** Function that set the material
	 @param material A structure describing the material of the object
	*/
	void setMaterial(Material material){
		this->material = material;
	}
	/** Functions for setting up all the transformation matrices
	 @param matrix The matrix representing the transformation of the object in the global coordinates */
	void setTransformation(glm::mat4 matrix){
		
		transformationMatrix = matrix;

		/* ----- Assignment 5 ---------
		 Set the two remaining matrices
		*/
		inverseTransformationMatrix = glm::inverse(transformationMatrix);
		normalMatrix = glm::transpose(inverseTransformationMatrix);
		 
		 
	}
};

/**
 Implementation of the class Object for sphere shape.
 */
class Sphere : public Object{
private:
    float radius; ///< Radius of the sphere
    glm::vec3 center; ///< Center of the sphere

public:
	/**
	 The constructor of the sphere
	 @param radius Radius of the sphere
	 @param center Center of the sphere
	 @param color Color of the sphere
	 */
    Sphere(float radius, glm::vec3 center, glm::vec3 color) : radius(radius), center(center){
		this->color = color;
    }
	Sphere(float radius, glm::vec3 center, Material material) : radius(radius), center(center){
		this->material = material;
	}

	Sphere(Material material)
	{
		this->material = material;
	}

	/** Implementation of the intersection function*/
    Hit intersect(Ray ray){

		/* Old code
        glm::vec3 c = center - ray.origin;

        float cdotc = glm::dot(c,c);
        float cdotd = glm::dot(c, ray.direction);

        Hit hit;

        float D = 0;
		if (cdotc > cdotd*cdotd){
			D =  sqrt(cdotc - cdotd*cdotd);
		}
        if(D<=radius){
            hit.hit = true;
            float t1 = cdotd - sqrt(radius*radius - D*D);
            float t2 = cdotd + sqrt(radius*radius - D*D);

            float t = t1;
            if(t<0) t = t2;
            if(t<0){
                hit.hit = false;
                return hit;
            }

			hit.intersection = ray.origin + t * ray.direction;
			hit.normal = glm::normalize(hit.intersection - center);
			hit.distance = glm::distance(ray.origin, hit.intersection);
			hit.object = this;
			
			hit.uv.s = (asin(hit.normal.y) + M_PI/2)/M_PI;
			hit.uv.t = (atan2(hit.normal.z,hit.normal.x) + M_PI) / (2*M_PI);
        }
		else{
            hit.hit = false;
		}
		return hit;
		*/

		Hit hit;
		hit.hit = false;

		glm::vec3 new_origin = inverseTransformationMatrix * glm::vec4(ray.origin, 1.0);
		glm::vec3 new_direction = glm::normalize(inverseTransformationMatrix * glm::vec4(ray.direction, 0.0));
		
		glm::vec3 c = - new_origin;
		float cdotc = glm::dot(c,c);
		float cdotd = glm::dot(c, new_direction);
		float D = 0;
		if (cdotc > cdotd*cdotd){
			D =  sqrt(cdotc - cdotd*cdotd);
		}
		int radius = 1;
		if(D<=radius){
		    hit.hit = true;
		    float t1 = cdotd - sqrt(radius*radius - D*D);
		    float t2 = cdotd + sqrt(radius*radius - D*D);

		    float t = t1;
		    if(t<0) t = t2;
		    if(t<0){
		        hit.hit = false;
		        return hit;
		    }

			glm::vec3 new_p = new_origin + t * new_direction;

			glm::vec3 new_normal = glm::normalize(glm::vec3(new_p));
			glm::vec3 p = transformationMatrix * glm::vec4(new_p, 1.0);
			glm::vec3 normal = normalMatrix * glm::vec4(new_normal, 0.0);
			normal = glm::normalize(normal);

			hit.intersection = p;
			hit.normal = normal;
			hit.distance = glm::distance(ray.origin, p);
			hit.object = this;

			hit.uv.s = (asin(hit.normal.y) + M_PI/2)/M_PI;
			hit.uv.t = (atan2(hit.normal.z,hit.normal.x) + M_PI) / (2*M_PI);
		}
		else{
		    hit.hit = false;
		}
		return hit;
	}
};


class Plane : public Object{

private:
	glm::vec3 normal;
	glm::vec3 point;

public:
	Plane(glm::vec3 point, glm::vec3 normal) : point(point), normal(normal){
	}
	Plane(glm::vec3 point, glm::vec3 normal, Material material) : point(point), normal(normal){
		this->material = material;
	}
	Hit intersect(Ray ray){
		
		Hit hit;
		hit.hit = false;
		float DdotN = glm::dot(ray.direction, normal);
		if(DdotN < 0){
			
			float PdotN = glm::dot (point-ray.origin, normal);
			float t = PdotN/DdotN;
			
			if(t > 0){
				hit.hit = true;
				hit.normal = normal;
				hit.distance = t;
				hit.object = this;
				hit.intersection = t * ray.direction + ray.origin;
			}
		}
		return hit;
	}
};

class Cone : public Object{
public:
	Cone(Material material){
		this->material = material;
	}
	Hit intersect(Ray ray){
		
		Hit hit;
		hit.hit = false;
		
		glm::vec3 new_origin = inverseTransformationMatrix * glm::vec4(ray.origin, 1.0);
		glm::vec3 new_direction = glm::normalize(inverseTransformationMatrix * glm::vec4(ray.direction, 0.0));
		Ray newRay(new_origin, new_direction);

		float a = new_direction.x * new_direction.x + new_direction.z * new_direction.z - new_direction.y * new_direction.y;
		float b = 2 * new_origin.x * new_direction.x + 2 * new_origin.z * new_direction.z - 2 * new_origin.y * new_direction.y;
		float c = new_origin.x * new_origin.x + new_origin.z * new_origin.z - new_origin.y * new_origin.y;

		float t1 = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
		float t2 = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);

		glm::vec3 new_p1 = new_origin + t1 * new_direction;
		glm::vec3 new_p2 = new_origin + t2 * new_direction;
		float t;

		if (new_p1.y <= 0 || new_p1.y >= 1)
		{
			if (new_p2.y <= 0 || new_p2.y >= 1){
				hit.hit = false;
				return hit;
			} 
			else{
				t = t2;
			}
		} else {
			t = t1;
		}

		glm::vec3 new_p = new_origin + t * new_direction;

		glm::vec3 new_normal = glm::normalize(glm::vec3(new_p.x, -new_p.y, new_p.z));
		glm::vec3 p = transformationMatrix * glm::vec4(new_p, 1.0);
		glm::vec3 normal = normalMatrix * glm::vec4(new_normal, 0.0);
		normal = glm::normalize(normal);


		Plane cap(glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
		Hit capH = cap.intersect(newRay);
		float dist = glm::distance(ray.origin, p);

		if (glm::distance(capH.intersection, glm::vec3(0.0, 1.0, 0.0)) <= 1){
			glm::vec3 cap_global = transformationMatrix * glm::vec4(capH.intersection, 1.0);
			glm::vec3 cap_normal = normalMatrix * glm::vec4(glm::vec3(0.0, 1.0, 0.0), 0.0);
			cap_normal = glm::normalize(cap_normal);
			float capDist = glm::distance(ray.origin, capH.intersection);
			if (capDist < dist)
			{
				hit.hit = true;
				hit.object = this;
				hit.intersection = capH.intersection;
				hit.normal = cap_normal;
				hit.distance = capDist;

				return hit;
			}
		}

		hit.hit = true;
		hit.object = this;
		hit.intersection = p;
		hit.normal = normal;
		hit.distance = dist;
		
		return hit;
	}
};


/**
 Light class
 */
class Light{
public:
	glm::vec3 position; ///< Position of the light source
	glm::vec3 color; ///< Color/intentisty of the light source
	Light(glm::vec3 position): position(position){
		color = glm::vec3(1.0);
	}
	Light(glm::vec3 position, glm::vec3 color): position(position), color(color){
	}
};

vector<Light *> lights; ///< A list of lights in the scene
glm::vec3 ambient_light(0.001,0.001,0.001);
vector<Object *> objects; ///< A list of all objects in the scene


/** Function for computing color of an object according to the Phong Model
 @param point A point belonging to the object for which the color is computer
 @param normal A normal vector the the point
 @param uv Texture coordinates
 @param view_direction A normalized direction from the point to the viewer/camera
 @param material A material structure representing the material of the object
*/
glm::vec3 PhongModel(glm::vec3 point, glm::vec3 normal, glm::vec2 uv, glm::vec3 view_direction, Material material){

	glm::vec3 color(0.0);
	for(int light_num = 0; light_num < lights.size(); light_num++){

		glm::vec3 light_direction = glm::normalize(lights[light_num]->position - point);
		glm::vec3 reflected_direction = glm::reflect(-light_direction, normal);

		float NdotL = glm::clamp(glm::dot(normal, light_direction), 0.0f, 1.0f);
		float VdotR = glm::clamp(glm::dot(view_direction, reflected_direction), 0.0f, 1.0f);

		
		glm::vec3 diffuse_color = material.diffuse;
		if(material.texture){
			diffuse_color = material.texture(uv);
		}
		
		glm::vec3 diffuse = diffuse_color * glm::vec3(NdotL);
		glm::vec3 specular = material.specular * glm::vec3(pow(VdotR, material.shininess));
		
		
		// distance to the light
		float r = glm::distance(point,lights[light_num]->position);
		r = max(r, 0.1f);
		

		color += lights[light_num]->color * (diffuse + specular) / r/r;
	}
	color += ambient_light * material.ambient;
	
	color = glm::clamp(color, glm::vec3(0.0), glm::vec3(1.0));
	return color;
}

/**
 Functions that computes a color along the ray
 @param ray Ray that should be traced through the scene
 @return Color at the intersection point
 */
glm::vec3 trace_ray(Ray ray){

	Hit closest_hit;

	closest_hit.hit = false;
	closest_hit.distance = INFINITY;

	for(int k = 0; k<objects.size(); k++){
		Hit hit = objects[k]->intersect(ray);
		if(hit.hit == true && hit.distance < closest_hit.distance)
			closest_hit = hit;
	}

	glm::vec3 color(0.0);

	if(closest_hit.hit){
		color = PhongModel(closest_hit.intersection, closest_hit.normal, closest_hit.uv, glm::normalize(-ray.direction), closest_hit.object->getMaterial());
	}else{
		color = glm::vec3(0.0, 0.0, 0.0);
	}
	return color;
}
/**
 Function defining the scene
 */
void sceneDefinition (){

	Material green_diffuse;
	green_diffuse.ambient = glm::vec3(0.03f, 0.1f, 0.03f);
	green_diffuse.diffuse = glm::vec3(0.3f, 1.0f, 0.3f);

	Material red_specular;
	red_specular.diffuse = glm::vec3(1.0f, 0.2f, 0.2f);
	red_specular.ambient = glm::vec3(0.01f, 0.02f, 0.02f);
	red_specular.specular = glm::vec3(0.5);
	red_specular.shininess = 10.0;

	Material blue_specular;
	blue_specular.ambient = glm::vec3(0.02f, 0.02f, 0.1f);
	blue_specular.diffuse = glm::vec3(0.2f, 0.2f, 1.0f);
	blue_specular.specular = glm::vec3(0.6);
	blue_specular.shininess = 100.0;

	//objects.push_back(new Sphere(1.0, glm::vec3(1,-2,8), blue_specular));
	//objects.push_back(new Sphere(0.5, glm::vec3(-1,-2.5,6), red_specular));

	Sphere *sphere1 = new Sphere(blue_specular);
	sphere1->setTransformation(glm::translate(glm::vec3(1, -2, 8)));
	objects.push_back(sphere1);

	Sphere *sphere2 = new Sphere(red_specular);
	sphere2->setTransformation(glm::translate(glm::vec3(-1, -2.5, 6)) * glm::scale(glm::vec3(0.5, 0.5, 0.5)));
	objects.push_back(sphere2);


	// ------ Assignment 5 -------
	
	// You can remove the green sphere as it should be replaced with a green cone
	//objects.push_back(new Sphere(1.0, glm::vec3(3,-2,6), green_diffuse));
	
	
	//Textured sphere
	Material textured;
	textured.texture = &rainbowTexture;
	//objects.push_back(new Sphere(7.0, glm::vec3(-6,4,23), textured));

	Sphere *sphere3 = new Sphere(textured);
	sphere3->setTransformation(glm::translate(glm::vec3(-6, 4, 23)) * glm::scale(glm::vec3(7, 7, 7)));
	objects.push_back(sphere3);

	//Planes
	Material red_diffuse;
	red_diffuse.ambient = glm::vec3(0.09f, 0.06f, 0.06f);
	red_diffuse.diffuse = glm::vec3(0.9f, 0.6f, 0.6f);
		
	Material blue_diffuse;
	blue_diffuse.ambient = glm::vec3(0.06f, 0.06f, 0.09f);
	blue_diffuse.diffuse = glm::vec3(0.6f, 0.6f, 0.9f);
	objects.push_back(new Plane(glm::vec3(0,-3,0), glm::vec3(0.0,1,0)));
	objects.push_back(new Plane(glm::vec3(0,1,30), glm::vec3(0.0,0.0,-1.0), green_diffuse));
	objects.push_back(new Plane(glm::vec3(-15,1,0), glm::vec3(1.0,0.0,0.0), red_diffuse));
	objects.push_back(new Plane(glm::vec3(15,1,0), glm::vec3(-1.0,0.0,0.0), blue_diffuse));
	objects.push_back(new Plane(glm::vec3(0,27,0), glm::vec3(0.0,-1,0)));
	objects.push_back(new Plane(glm::vec3(0,1,-0.01), glm::vec3(0.0,0.0,1.0), green_diffuse));

	Material yellow_specular;
	yellow_specular.ambient = glm::vec3(0.01f, 0.05f, 0.0f);
	yellow_specular.diffuse = glm::vec3(0.60f, 0.6f, 0.0f);
	yellow_specular.specular = glm::vec3(0.7);
	yellow_specular.shininess = 100.0;

	Cone *cone1 = new Cone(yellow_specular);
	float angle1 = M_PI;
	glm::mat4 rotationMatrix1 = glm::rotate(angle1, glm::vec3(1, 0, 0));
	cone1->setTransformation(glm::translate(glm::vec3(5, 9, 14)) * glm::scale(glm::vec3(3, 12, 3)) * rotationMatrix1);
	objects.push_back(cone1);

	Cone *cone2 = new Cone(green_diffuse);
	float angle2 = ((M_PI / 2) - atan(1.0 / 3.0));
	glm::mat4 rotationMatrix2 = glm::rotate(angle2-0.01f, glm::vec3(0, 0, 1));
	cone2->setTransformation(glm::translate(glm::vec3(6, -3, 7)) * rotationMatrix2 * glm::scale(glm::vec3(1, 3, 1)));
	objects.push_back(cone2);

	lights.push_back(new Light(glm::vec3(0, 26, 5), glm::vec3(1.0, 1.0, 1.0)));
	lights.push_back(new Light(glm::vec3(0, 1, 12), glm::vec3(0.1)));
	lights.push_back(new Light(glm::vec3(0, 5, 1), glm::vec3(0.4)));
}

/**
 Function performing tonemapping of the intensities computed using the raytracer
 @param intensity Input intensity
 @return Tonemapped intensity in range (0,1)
 */
glm::vec3 toneMapping(glm::vec3 intensity){
	float gamma = 1.0/2.0;
	float alpha = 12.0f;
	return glm::clamp(alpha * glm::pow(intensity, glm::vec3(gamma)), glm::vec3(0.0), glm::vec3(1.0));
}

int main(int argc, const char * argv[]) {

    clock_t t = clock(); // variable for keeping the time of the rendering

    int width = 1024; //width of the image
    int height = 768; // height of the image
    float fov = 90; // field of view

	sceneDefinition(); // Let's define a scene

	Image image(width,height); // Create an image where we will store the result

    float s = 2*tan(0.5*fov/180*M_PI)/width;
    float X = -s * width / 2;
    float Y = s * height / 2;

    for(int i = 0; i < width ; i++)
        for(int j = 0; j < height ; j++){

			float dx = X + i*s + s/2;
            float dy = Y - j*s - s/2;
            float dz = 1;

			glm::vec3 origin(0, 0, 0);
            glm::vec3 direction(dx, dy, dz);
            direction = glm::normalize(direction);

            Ray ray(origin, direction);

			image.setPixel(i, j, toneMapping(trace_ray(ray)));

        }

    t = clock() - t;
    cout<<"It took " << ((float)t)/CLOCKS_PER_SEC<< " seconds to render the image."<< endl;
    cout<<"I could render at "<< (float)CLOCKS_PER_SEC/((float)t) << " frames per second."<<endl;

	// Writing the final results of the rendering
	if (argc == 2){
		image.writeImage(argv[2]);
	}else{
		image.writeImage("./result.ppm");
	}

    return 0;
}
