#ifndef SPOT_LIGHT_H
#define SPOT_LIGHT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

class SpotLight
{
public:
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float cutOff;
    float outerCutOff;
    float k_c;
    float k_l;
    float k_q;
    int lightNumber;

    SpotLight(
        const glm::vec3& lightPosition,
        const glm::vec3& lightDirection,
        float innerCutOffDegrees,
        float outerCutOffDegrees,
        const glm::vec3& lightAmbient,
        const glm::vec3& lightDiffuse,
        const glm::vec3& lightSpecular,
        float constant,
        float linear,
        float quadratic,
        int number)
        : position(lightPosition),
          direction(glm::normalize(lightDirection)),
          ambient(lightAmbient),
          diffuse(lightDiffuse),
          specular(lightSpecular),
          cutOff(glm::cos(glm::radians(innerCutOffDegrees))),
          outerCutOff(glm::cos(glm::radians(outerCutOffDegrees))),
          k_c(constant),
          k_l(linear),
          k_q(quadratic),
          lightNumber(number)
    {
    }

    void setUpSpotLight(Shader& lightingShader) const
    {
        if (lightNumber < 1 || lightNumber > 2)
            return;

        lightingShader.use();
        const std::string lightName = "spotLights[" + std::to_string(lightNumber - 1) + "]";
        lightingShader.setVec3(lightName + ".position", position);
        lightingShader.setVec3(lightName + ".direction", direction);
        lightingShader.setFloat(lightName + ".cutOff", cutOff);
        lightingShader.setFloat(lightName + ".outerCutOff", outerCutOff);
        lightingShader.setVec3(lightName + ".ambient", ambientOn * ambient);
        lightingShader.setVec3(lightName + ".diffuse", diffuseOn * diffuse);
        lightingShader.setVec3(lightName + ".specular", specularOn * specular);
        lightingShader.setFloat(lightName + ".k_c", k_c);
        lightingShader.setFloat(lightName + ".k_l", k_l);
        lightingShader.setFloat(lightName + ".k_q", k_q);
    }

    void turnOff()
    {
        ambientOn = 0.0f;
        diffuseOn = 0.0f;
        specularOn = 0.0f;
    }

    void turnOn()
    {
        ambientOn = 1.0f;
        diffuseOn = 1.0f;
        specularOn = 1.0f;
    }

    void turnAmbientOn() { ambientOn = 1.0f; }
    void turnAmbientOff() { ambientOn = 0.0f; }
    void turnDiffuseOn() { diffuseOn = 1.0f; }
    void turnDiffuseOff() { diffuseOn = 0.0f; }
    void turnSpecularOn() { specularOn = 1.0f; }
    void turnSpecularOff() { specularOn = 0.0f; }

private:
    float ambientOn = 1.0f;
    float diffuseOn = 1.0f;
    float specularOn = 1.0f;
};

#endif
