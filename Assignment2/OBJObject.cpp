#include "OBJObject.h"
#include "Window.h"
#include <cmath>

using namespace std;

const float VALUE = 1.5f;

OBJObject::OBJObject(const char *filepath, float objID)
{
    this->toWorld = glm::mat4(1.0f);
    this->goBack = glm::mat4(1.0f);
    
    this->angle = 3.0f;
    this->objID = objID;
    this->pointSize = 1.0f;
    
    this->indices = std::vector<unsigned int>();
    this->vertices = std::vector<glm::vec3>();
    this->normals = std::vector<glm::vec3>();
    
    if (this->objID == 3.0f) {
        this->scaleModelCenter(0.2f);
        this->translateX(Window::pointLightPosition.x);
        this->translateY(Window::pointLightPosition.y);
        this->translateZ(Window::pointLightPosition.z);
    }
    
    if (this->objID == 4.0f) {
        this->scaleModelCenter(0.5);
        this->rotate(glm::vec3(1,1,1), -45.0f);
        this->translateX(Window::spotLightPosition.x);
        this->translateY(Window::spotLightPosition.y);
        this->translateZ(Window::spotLightPosition.z);
    }
    
    parse(filepath);
    scaleAndCenter();
    
    // TAKEN AND MODIFIED DIRECTLY FROM CUBE.CPP
    
    // Create array object and buffers. Remember to delete your buffers when the object is destroyed!
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &NORMALS);
    
    // Bind the Vertex Array Object (VAO) first, then bind the associated buffers to it.
    // Consider the VAO as a container for all your buffers.
    glBindVertexArray(VAO);
    
    // Now bind a VBO to it as a GL_ARRAY_BUFFER. The GL_ARRAY_BUFFER is an array containing relevant data to what
    // you want to draw, such as vertices, normals, colors, etc.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData populates the most recently bound buffer with data starting at the 3rd argument and ending after
    // the 2nd argument number of indices. How does OpenGL know how long an index spans? Go to glVertexAttribPointer.
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    // Enable the usage of layout location 0 (check the vertex shader to see what this is)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,// This first parameter x should be the same as the number passed into the line "layout (location = x)" in the vertex shader. In this case, it's 0. Valid values are 0 to GL_MAX_UNIFORM_LOCATIONS.
                          3, // This second line tells us how any components there are per vertex. In this case, it's 3 (we have an x, y, and z component)
                          GL_FLOAT, // What type these components are
                          GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
                          sizeof(glm::vec3), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
                          (GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.
    
    // We've sent the vertex data over to OpenGL, but there's still something missing.
    // In what order should it draw those vertices? That's why we'll need a GL_ELEMENT_ARRAY_BUFFER for this.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    
    glBindVertexArray(NORMALS);
    glBindBuffer(GL_ARRAY_BUFFER, NORMALS);
    glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT, // What type these components are
                          GL_FALSE, // GL_TRUE means the values should be normalized. GL_FALSE means they shouldn't
                          sizeof(glm::vec3), // Offset between consecutive indices. Since each of our vertices have 3 floats, they should have the size of 3 floats in between
                          (GLvoid*)0); // Offset of the first vertex's component. In our case it's 0 since we don't pad the vertices array with anything.
    glEnableVertexAttribArray(1);
    
    // Unbind the currently bound buffer so that we don't accidentally make unwanted changes to it.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // Unbind the VAO now so we don't accidentally tamper with it.
    // NOTE: You must NEVER unbind the element array buffer associated with a VAO!
    glBindVertexArray(0);
    
}

OBJObject::~OBJObject()
{
    // Delete previously generated buffers. Note that forgetting to do this can waste GPU memory in a
    // large project! This could crash the graphics driver due to memory leaks, or slow down application performance!
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &NORMALS);
}

void OBJObject::parse(const char *filepath) 
{
	// Populate the face indices, vertices, and normals vectors with the OBJ Object data
    
    FILE* fp;
    float x,y,z;
    float r,g,b;
    int c1,c2;
    
    unsigned int i1, i2, i3, repeats;
    
    fp = fopen(filepath,"rb");
    
    if (fp == NULL) {
        std::cerr << "error loading file" << std::endl; exit(-1);
        cout << "SOMETHING IS BAD";
    }
    
    //loop while there are still lines to read in
    while(true) {
    
        c1 = fgetc(fp);
        c2 = fgetc(fp);
    
        if (c1 == EOF) {
            break;
        }
        //read in vertices
        // v float float float
        else if (c1=='v' && c2==' ') {
            fscanf(fp, "%f %f %f %f %f %f\n", &x, &y, &z, &r, &g, &b);
            vertices.push_back(glm::vec3(x,y,z));
            x=0.0f;
            y=0.0f;
            z=0.0f;
        }
        //read in vertex normals
        // vn float float float
        else if (c1=='v' && c2=='n') {
            fscanf(fp, "%f %f %f\n", &x, &y, &z);
            normals.push_back(glm::vec3(x,y,z));
            x=0.0f;
            y=0.0f;
            z=0.0f;
        }
        //read in faces
        // f int//int int//int int//int
        else if ((c1=='f' || c2=='f')) {

            //fixes weird bug i was having
            i1=0;
            i2=0;
            i3=0;
            
            fscanf(fp, "%d//%d %d//%d %d//%d\n", &i1, &repeats, &i2, &repeats, &i3, &repeats);
            
            if (i1!=0 && i2!=0 && i3!=0) {
                indices.push_back(i1-1);
                indices.push_back(i2-1);
                indices.push_back(i3-1);
            }
            
        }
    }
    
    //keep track of the initial values for later use
    this->originalVertices = vertices;
    this->originalNormals = normals;
    this->originalIndices = indices;
    
    fclose(fp);
}

void OBJObject::scaleAndCenter()
{
    //CENTER THE OBJECT ABOUT THE ORIGIN
    
    float xmin, ymin, zmin;
    float xmax, ymax, zmax;
    
    //INIT VALUES
    xmin = vertices[0].x;
    xmax = vertices[0].x;
    
    ymin = vertices[0].y;
    ymax = vertices[0].y;
    
    zmin = vertices[0].z;
    zmax = vertices[0].z;

    //LOOP AND FIND MINs AND MAXs
    for (int i=0; i<vertices.size(); i++) {
        
        //find min/max X
        if (vertices[i].x < xmin) {
            xmin = vertices[i].x;
        }
        else if (vertices[i].x > xmax) {
            xmax = vertices[i].x;
        }
        
        //find min/max Y
        if (vertices[i].y < ymin) {
            ymin = vertices[i].y;
        }
        else if (vertices[i].y > ymax) {
            ymax = vertices[i].y;
        }
        
        //find min/max Z
        if (vertices[i].z < zmin) {
            zmin = vertices[i].z;
        }
        else if (vertices[i].z > zmax) {
            zmax = vertices[i].z;
        }
    }
    
    //find center of model
    this->x_center = (xmax+xmin)/2.0f;
    this->y_center = (ymax+ymin)/2.0f;
    this->z_center = (zmax+zmin)/2.0f;
    
    //make center of the object (0.0, 0.0, 0.0)
    for (int i=0; i<vertices.size(); i++) {
        vertices[i].x -= x_center;
        vertices[i].y -= y_center;
        vertices[i].z -= z_center;
    }

    //FIND LONGEST DIMENSION AND SCALE

    this->biggest = vertices[0].x;
    
    for (int i=0; i<vertices.size(); i++) {
        if (vertices[i].x > biggest) {
            this->biggest = vertices[i].x;
        }
        else if (vertices[i].x < -biggest) {
            this->biggest = -vertices[i].x;
        }
        if (vertices[i].y > biggest) {
            this->biggest = vertices[i].y;
        }
        else if (vertices[i].y < -biggest) {
            this->biggest = -vertices[i].y;
        }
        if (vertices[i].z > biggest) {
            this->biggest = vertices[i].z;
        }
        else if (vertices[i].z < -biggest) {
            this->biggest = -vertices[i].z;
        }
    }
    
    //NORMALIZE TO RANGE (-1, 1)
    for (int i=0; i<vertices.size(); i++) {
        vertices[i].x /= biggest;
        vertices[i].y /= biggest;
        vertices[i].z /= biggest;
    }
    
}

void OBJObject::draw(GLuint shaderProgram) 
{
    //TAKEN AND MODIFIED DIRECTLY FROM CUBE.CPP
    
    // Calculate the combination of the model and view (camera inverse) matrices
    glm::mat4 modelview = Window::V * toWorld;
    
    uModel = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(uModel, 1, GL_FALSE, &toWorld[0][0]);

    // We need to calcullate this because modern OpenGL does not keep track of any matrix other than the viewport (D)
    // Consequently, we need to forward the projection, view, and model matrices to the shader programs
    // Get the location of the uniform variables "projection" and "modelview"
    uProjection = glGetUniformLocation(shaderProgram, "projection");
    uModelview = glGetUniformLocation(shaderProgram, "modelview");
    // Now send these values to the shader program
    glUniformMatrix4fv(uProjection, 1, GL_FALSE, &Window::P[0][0]);
    glUniformMatrix4fv(uModelview, 1, GL_FALSE, &modelview[0][0]);
    
    glUseProgram(shaderProgram);
    
    // Now draw the cube. We simply need to bind the VAO associated with it.
    glBindVertexArray(VAO);
    // Tell OpenGL to draw with triangles, using 36 indices, the type of the indices, and the offset to start from
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    // Unbind the VAO when we're done so we don't accidentally draw extra stuff or tamper with its bound buffers
    glBindVertexArray(0);
}

void OBJObject::update()
{
    //call directly in idle_callback()
    //this->spin(1.0f);
}

void OBJObject::rotate(glm::vec3 axis, float angle) {
    if (angle > 360.0f || angle < -360.0f) angle = 0.0f;
    
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle / 180.0f * glm::pi<float>(), axis);
    this->toWorld = rotation * this->toWorld;
}

void OBJObject::spin(float deg)
{
    glm::vec3 spinVector;
    
    //this->angle += deg;
    if (this->angle > 360.0f || this->angle < -360.0f) this->angle = 0.0f;
    
    //x axis
    if (this->objID == 0.0f) {
        spinVector = glm::vec3(1.0f, 0.0f, 0.0f);
    }
    //y axis
    else if (this->objID == 1.0f) {
        spinVector = glm::vec3(0.0f, 1.0f, 0.0f);
    }
    //z axis
    else if (this->objID == 2.0f) {
        spinVector = glm::vec3(0.0f, 0.0f, 1.0f);
    }
    
    // This creates the matrix to rotate the object
    //this->toWorld = glm::rotate(toWorld, 3.0f / 180.0f * glm::pi<float>(), spinVector);
    //this->goBack = glm::rotate(goBack, 3.0f / 180.0f * glm::pi<float>(), spinVector);
    
    this->toWorld = glm::rotate(toWorld, this->angle / 180.0f * glm::pi<float>(), spinVector);
}

// 'p' scales down and 'P' scales up each point by quantity 'value'
void OBJObject::scalePointSize(float value) {
   //this->toWorld = glm::scale(glm::mat4(1.0f),glm::vec3(value, value, value)) * toWorld;
    float newValue = this->pointSize * value;
    this->pointSize = newValue;
    glPointSize(newValue);
}

// 'a' translates left (-) and 'd' translates right (+) along X-axis by quantity 'value'
void OBJObject::translateX(float value) {
    glm::mat4 newWorld;
    newWorld = glm::translate(glm::mat4(1.0f), glm::vec3(value, 0.0f, 0.0f)) * this->toWorld;
    this->toWorld = newWorld;
    this->goBack = glm::inverse(glm::translate(glm::mat4(1.0f), glm::vec3(value, 0.0f, 0.0f))) * newWorld;
}

// 'w' translates up (+) and 's' translates down (-) along Y-axis by quantity 'value'
void OBJObject::translateY(float value) {
    glm::mat4 newWorld;
    newWorld = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, value, 0.0f)) * this->toWorld;
    this->toWorld = newWorld;
    this->goBack = glm::inverse(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, value, 0.0f))) * newWorld;
}

// 'z' translates into (-) and 'y' translates away from (+) the screen along Z-axis by quantity 'value'
void OBJObject::translateZ(float value) {
    glm::mat4 newWorld;
    newWorld = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, value)) * this->toWorld;
    this->toWorld = newWorld;
    this->goBack = glm::inverse(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, value))) * newWorld;
}

// 'c' scales down and 'C' scales up the model about its center by quantity 'value'
void OBJObject::scaleModelCenter(float value) {
    /*for (unsigned int i = 0; i < vertices.size(); ++i)
    {
        vertices[i].x *= value;
        vertices[i].y *= value;
        vertices[i].z *= value;
    }*/
    
    glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(value,value,value));
    this->toWorld = scaleMatrix * this->toWorld;
    
}

// 'r' resets the objects back to the center of the screen (orientation/scale persist)
void OBJObject::resetPosition() {
    //glm::mat4 newWorld;
    //newWorld = glm::inverse(this->goBack) * this->toWorld;
    //this->toWorld = newWorld;
    //this->goBack = this->goBack * newWorld;
    
    this->toWorld = glm::mat4(1.0f);
}

// 'r' resets the objects back to the center of the screen (orientation/scale persist)
void OBJObject::moveToStartPosition()
{
    this->toWorld = glm::mat4(1.0f);
    //bunny
    //if (this->objID == 0.0f) {
    //    this->translateX(1.0f);
    //}
    //dragon
    //else if (this->objID == 1.0f) {
    //    this->translateX(-1.0f);
    //    this->translateY(-1.0f);
    //}
}

// 'R' resets the orientation and scale factor of the objects (screen position persists)
void OBJObject::resetScale()
{
    this->vertices = this->originalVertices;
    this->normals = this->originalNormals;
    glPointSize(1.0f);
}

// 'R' resets the orientation and scale factor of the objects (screen position persists)
void OBJObject::resetOrientation()
{
    this->angle = 3.0f;
}

// complete the action request
void OBJObject::completeActionRequest(char instruction)
{
    //this->spin(1.0f);
    
//    // 'p' lowercase --> scale down points
//    if (instruction == 'p') {
//        this->scalePointSize(0.5f);
//    }
    // 'P' uppercase --> scale up points
//    else if (instruction == 'P') {
//        this->scalePointSize(2.0f);
//    }
    // 'a' lowercase --> move left
    if (instruction == 'a') {
        this->translateX(-VALUE);
    }
    // 'd' lowercase --> move right
    else if (instruction == 'd') {
        this->translateX(VALUE);
    }
    // 'w' lowercase --> move up
    else if (instruction == 'w') {
        this->translateY(VALUE);
    }
    // 's' lowercase --> move down
    else if (instruction == 's') {
        this->translateY(-VALUE);
    }
    // 'z' lowercase --> move into
    else if (instruction == 'z') {
        this->translateZ(VALUE);
    }
    // 'Z' lowercase --> move out of
    else if (instruction == 'Z') {
        this->translateZ(-VALUE);
    }
    // 'c' lowercase --> scale down about center
    else if (instruction == 'c') {
        this->scaleModelCenter(0.8f);
    }
    // 'C' lowercase --> scale up about center
    else if (instruction == 'C') {
        this->scaleModelCenter(1.25f);
    }
    // 'r' lowercase --> reset position
    else if (instruction == 'r') {
        //this->resetPosition();
        this->moveToStartPosition();
    }
    // 'R' lowercase --> reset orientation/scale
//    else if (instruction == 'R') {
//        this->resetScale();
        //this->resetOrientation();
//    }
    else if (instruction == 'l') {
        
    }
    
    
}

