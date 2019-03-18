#ifndef OBJOBJECT_H
#define OBJOBJECT_H

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenGL/glext.h>
//#include <OpenGL/gl.h> // Remove this line in future projects
#else
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class OBJObject
{
private:

public:
    
    // BELOW MOVED FROM PRIVATE TO PUBLIC
    std::vector<unsigned int> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    
    glm::mat4 toWorld;
    glm::mat4 goBack;
    // ABOVE MOVED FROM PRIVATE TO PUBLIC
    
	OBJObject(const char* filepath, float spinID);
    ~OBJObject();

	void parse(const char* filepath);
	void draw(GLuint);
    void scaleAndCenter();
    
    void spin(float);
    void update();
    
    void scalePointSize(float);
    void translateX(float);
    void translateY(float);
    void translateZ(float);
    void scaleModelCenter(float);
    void resetPosition();
    void moveToStartPosition();
    void resetScale();
    void resetOrientation();
    void completeActionRequest(char);
    void rotate(glm::vec3, float);
    
    std::vector<unsigned int> originalIndices;
    std::vector<glm::vec3> originalVertices;
    std::vector<glm::vec3> originalNormals;
    
    float angle;
    float objID;
    float pointSize;
    char instruction;
    float x_center, y_center, z_center, biggest;
    
    // These variables are needed for the shader program
    GLuint VBO, VAO, EBO, NORMALS;
    GLuint uProjection, uModelview, uModel;
    
};

#endif
