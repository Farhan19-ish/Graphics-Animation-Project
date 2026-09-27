#ifndef pointLight_h
#define pointLight_h

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include "shader.h"

class PointLight {
public:
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float k_c;
    float k_l;
    float k_q;
    int lightNumber;

    PointLight(float posX, float posY, float posZ, float ambR, float ambG, float ambB, float diffR, float diffG, float diffB, float specR, float specG, float specB, float constant, float linear, float quadratic, int num) {

        position = glm::vec3(posX, posY, posZ);
        ambient = glm::vec3(ambR, ambG, ambB);
        diffuse = glm::vec3(diffR, diffG, diffB);
        specular = glm::vec3(specR, specG, specB);
        k_c = constant;
        k_l = linear;
        k_q = quadratic;
        lightNumber = num;
    }
    void setUpPointLight(Shader& lightingShader) const
    {
        if (lightNumber < 1 || lightNumber > 4)
            return;

        lightingShader.use();
        const std::string lightName = "pointLights[" + std::to_string(lightNumber - 1) + "]";
        lightingShader.setVec3(lightName + ".position", position);
        lightingShader.setVec3(lightName + ".ambient", ambientOn * ambient);
        lightingShader.setVec3(lightName + ".diffuse", diffuseOn * diffuse);
        lightingShader.setVec3(lightName + ".specular", specularOn * specular);
        lightingShader.setFloat(lightName + ".k_c", k_c);
        lightingShader.setFloat(lightName + ".k_l", k_l);
        lightingShader.setFloat(lightName + ".k_q", k_q);
    }
    void turnOff()
    {
        ambientOn = 0.0;
        diffuseOn = 0.0;
        specularOn = 0.0;
    }
    void turnOn()
    {
        ambientOn = 1.0;
        diffuseOn = 1.0;
        specularOn = 1.0;
    }
    void turnAmbientOn()
    {
        ambientOn = 1.0;
    }
    void turnAmbientOff()
    {
        ambientOn = 0.0;
    }
    void turnDiffuseOn()
    {
        diffuseOn = 1.0;
    }
    void turnDiffuseOff()
    {
        diffuseOn = 0.0;
    }
    void turnSpecularOn()
    {
        specularOn = 1.0;
    }
    void turnSpecularOff()
    {
        specularOn = 0.0;
    }
private:
    float ambientOn = 1.0;
    float diffuseOn = 1.0;
    float specularOn = 1.0;
};

#endif /* pointLight_h */
