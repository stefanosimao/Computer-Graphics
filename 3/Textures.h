//
//  Textures.h
//  Raytracer
//
//  Created by Piotr Didyk on 19.08.21.
//  Student: Stefano Gonçalves Simao


#ifndef Textures_h
#define Textures_h


#include "glm/glm.hpp"

glm::vec3 checkerboardTexture(glm::vec2 uv){

    int temp = floor(30 * uv.s) + floor(30 * uv.t);
    return glm::vec3(temp % 2);
}
glm::vec3 rainbowTexture(glm::vec2 uv){

    int temp = floor(60 * uv.s + 30 * uv.t);

    if (temp % 3 == 0){
        return glm::vec3(0.99f,0.01f,0.01f);
    }
    else if(temp % 3 == 1){
        return glm::vec3(0.01f, 0.99f, 0.01f);
    }
    else{
        return glm::vec3(0.01f, 0.01f, 0.99f);
    }
}

#endif /* Textures_h */
