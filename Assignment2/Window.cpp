#include "Window.h"
#include "OBJObject.h"

const char* window_title = "GLFW Starter Project";
Cube * cube;
GLint shaderProgram;

#define BUNNY_PATH "/Users/lukerohrer/Desktop/CSE167/Assignment2/Assignment2/model.obj"
#define DRAGON_PATH "/Users/lukerohrer/Desktop/CSE167/Assignment2/Assignment2/dragon.obj"
#define BEAR_PATH "/Users/lukerohrer/Desktop/CSE167/Assignment2/Assignment2/bear.obj"

#define SPHERE_PATH "/Users/lukerohrer/Desktop/CSE167/Assignment2/Assignment2/sphere.obj"
#define CONE_PATH "/Users/lukerohrer/Desktop/CSE167/Assignment2/Assignment2/cone.obj"

OBJObject * bunny;
OBJObject * dragon;
OBJObject * bear;
OBJObject * sphere;
OBJObject * cone;

OBJObject * curObj;

std::string Window::objectToRender;

// On some systems you need to change this to the absolute path
#define VERTEX_SHADER_PATH "/Users/lukerohrer/Desktop/CSE167/Assignment2/Assignment2/shader.vert"
#define FRAGMENT_SHADER_PATH "/Users/lukerohrer/Desktop/CSE167/Assignment2/Assignment2/shader.frag"

// Default camera parameters
glm::vec3 cam_pos(0.0f, 0.0f, 20.0f);		// e  | Position of camera
glm::vec3 cam_look_at(0.0f, 0.0f, 0.0f);	// d  | This is where the camera looks at
glm::vec3 cam_up(0.0f, 1.0f, 0.0f);			// up | What orientation "up" is

glm::vec3 Window::pointLightPosition(1.0f, 1.0f, 1.0f);
glm::vec3 Window::spotLightPosition(4.0f, 4.0f, 4.0f);

int Window::width;
int Window::height;

float x;
float y=0.0f;
float z;

char instr = ' ';
char mouseClick = 'N';
int shaderMode = 0;
int dirLightOn = 0;
int scene = 0;
int showSphere = 0;
int showCone = 0;
float spotLightWidth = 0.8f;
float exponentFactor = 10.0f;

double Window::uponClickX = 0.0;
double Window::uponClickY = 0.0;

glm::mat4 Window::P;
glm::mat4 Window::V;

void Window::initialize_objects()
{
	//cube = new Cube();
    
    bunny = new OBJObject(BUNNY_PATH, 0.0f);
    dragon = new OBJObject(DRAGON_PATH, 1.0f);
    bear = new OBJObject(BEAR_PATH, 2.0f);
    
    sphere = new OBJObject(SPHERE_PATH, 3.0f);
    cone = new OBJObject(CONE_PATH, 4.0f);
    
    curObj = bunny;
    //curObj = bear;
    
    objectToRender = "bunny";
    //objectToRender = "bear";
    
    glUniform1i(glGetUniformLocation(shaderProgram, "shaderMode"), 1);
    glUniform1i(glGetUniformLocation(shaderProgram, "dirLight.turnedOn"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "pointLight.turnedOn"), 0);
    glUniform1i(glGetUniformLocation(shaderProgram, "spotLight.turnedOn"), 0);
    
	// Load the shader program. Make sure you have the correct filepath up top
	shaderProgram = LoadShaders(VERTEX_SHADER_PATH, FRAGMENT_SHADER_PATH);
    
}

// Treat this as a destructor function. Delete dynamically allocated memory here.
void Window::clean_up()
{
	//delete(cube);
    delete(bunny);
    delete(dragon);
    delete(bear);
    delete(sphere);
    delete(cone);
	glDeleteProgram(shaderProgram);
}

GLFWwindow* Window::create_window(int width, int height)
{
	// Initialize GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return NULL;
	}

	// 4x antialiasing
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ // Because Apple hates comforming to standards
	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window
	GLFWwindow* window = glfwCreateWindow(width, height, window_title, NULL, NULL);

	// Check if the window could not be created
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		fprintf(stderr, "Either GLFW is not installed or your graphics card does not support modern OpenGL.\n");
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window
	glfwMakeContextCurrent(window);

	// Set swap interval to 1
	glfwSwapInterval(1);

	// Get the width and height of the framebuffer to properly resize the window
	glfwGetFramebufferSize(window, &width, &height);
	// Call the resize callback to make sure things get drawn immediately
	Window::resize_callback(window, width, height);

	return window;
}

void Window::resize_callback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	glfwGetFramebufferSize(window, &width, &height); // In case your Mac has a retina display
#endif
	Window::width = width;
	Window::height = height;
	// Set the viewport size. This is the only matrix that OpenGL maintains for us in modern OpenGL!
	glViewport(0, 0, width, height);

	if (height > 0)
	{
		P = glm::perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
		V = glm::lookAt(cam_pos, cam_look_at, cam_up);
	}
}

void Window::idle_callback()
{
	// Call the update function the cube
	//cube->update();
    
    /*if (instr == 'K') {
        spotLightWidth -= 5.0f;
        glUniform1f(glGetUniformLocation(shaderProgram, "spotLight.cutoff"), spotLightWidth);
    }
    else if (instr == 'k') {
        spotLightWidth += 5.0f;
        glUniform1f(glGetUniformLocation(shaderProgram, "spotLight.cutoff"), spotLightWidth);
    }*/
    
    curObj->completeActionRequest(instr);
    
    instr = ' ';
    
}

void Window::display_callback(GLFWwindow* window)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Use the shader of programID
	glUseProgram(shaderProgram);
	
    //////////////////////////////////////////////////////////////////////////////////////////
    
    switch (int(curObj->objID)) {
            
        case 0: {
            //bunny
            
            //set material to green (SUPER SHINY, NO DIFFUSE)
            glUniform1i(glGetUniformLocation(shaderProgram, "material.turnedOn"), 1);
            glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), 0.0215f, 0.1745f, 0.0215f);
            glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), 0.0f, 0.0f, 0.0f);
            glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), 0.9f, 1.0f, 0.9f);
            glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 5.0f);
            
            break;
        }
        case 1: {
            //dragon
            
            //set material to blue (NO SHINY, SUPER DIFFUSE)
            glUniform1i(glGetUniformLocation(shaderProgram, "material.turnedOn"), 1);
            glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), 0.1f, 0.18725f, 0.1745f);
            glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), 0.7f, 1.0f, 0.9f);
            glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), 0.0f, 0.0f, 0.0f);
            glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 0.0f);
            
            break;
        }
        case 2: {
            //bear
            
            //set material to gold (SUPER SHINY, SUPER DIFFUSE)
            glUniform1i(glGetUniformLocation(shaderProgram, "material.turnedOn"), 1);
            glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), 0.2f, 0.18f, 0.07f);
            glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), 0.75164f, 0.60648f, 0.22648f);
            glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), 0.626281f, 0.555802f, 0.366065f);
            glUniform1f(glGetUniformLocation(shaderProgram, "material.shininess"), 0.4f);
            
            break;
        }
        case 3: {
            glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), 0.2f, 0.4f, 0.1f);
            glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), 0.0f, 0.0f, 0.0f);
            glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), 0.0f, 0.0f, 0.0f);
            
            break;
        }
        case 4: {
            glUniform3f(glGetUniformLocation(shaderProgram, "material.ambient"), 0.5f, 0.1f, 0.2f);
            glUniform3f(glGetUniformLocation(shaderProgram, "material.diffuse"), 0.0f, 0.0f, 0.0f);
            glUniform3f(glGetUniformLocation(shaderProgram, "material.specular"), 0.0f, 0.0f, 0.0f);
            
            break;
        }
    }
    
    //does the y-axis rotation
    y += 0.03f;
    x = sin(y);
    z = cos(y);
    
    //directional light (neutral color)
    //glUniform1i(glGetUniformLocation(shaderProgram, "dirLight.turnedOn"), 1);
    glUniform3f(glGetUniformLocation(shaderProgram, "dirLight.direction"), x, -1.0f, z);
    glUniform3f(glGetUniformLocation(shaderProgram, "dirLight.ambient"), 0.03f, 0.03f, 0.03f);
    glUniform3f(glGetUniformLocation(shaderProgram, "dirLight.diffuse"), 0.2f, 0.2f, 0.2f);
    glUniform3f(glGetUniformLocation(shaderProgram, "dirLight.specular"), 0.7f, 0.7f, 0.7f);
    
    //point lights (weird green color)
    //glUniform1i(glGetUniformLocation(shaderProgram, "pointLight.turnedOn"), 1);
    glUniform3f(glGetUniformLocation(shaderProgram, "pointLight.position"), pointLightPosition.x, pointLightPosition.y, pointLightPosition.z);
    glUniform3f(glGetUniformLocation(shaderProgram, "pointLight.ambient"), 0.2f, 0.4f, 0.1f);
    glUniform3f(glGetUniformLocation(shaderProgram, "pointLight.diffuse"), 0.4f, 0.7f, 0.1f);
    glUniform3f(glGetUniformLocation(shaderProgram, "pointLight.specular"), 0.4f, 0.7f, 0.1f);
    glUniform1f(glGetUniformLocation(shaderProgram, "pointLight.linear"), 0.2f);
    
    //spotlight
    //glUniform1i(glGetUniformLocation(shaderProgram, "spotLight.turnedOn"), 0);
    glUniform3f(glGetUniformLocation(shaderProgram, "spotLight.position"), spotLightPosition.x, spotLightPosition.y, spotLightPosition.z);
    glUniform3f(glGetUniformLocation(shaderProgram, "spotLight.ambient"), 0.5f, 0.1f, 0.2f);
    glUniform3f(glGetUniformLocation(shaderProgram, "spotLight.diffuse"), 0.8f, 0.8f, 0.8f);
    glUniform3f(glGetUniformLocation(shaderProgram, "spotLight.specular"), 0.9f, 0.9f, 0.9f);
    glUniform3f(glGetUniformLocation(shaderProgram, "spotLight.directionCone"), -5.0f, -5.0f, -5.0f);
    glUniform1f(glGetUniformLocation(shaderProgram, "spotLight.cutoff"), spotLightWidth);
    glUniform1f(glGetUniformLocation(shaderProgram, "spotLight.exponent"), exponentFactor);
    glUniform1f(glGetUniformLocation(shaderProgram, "spotLight.quadratic"), 0.01f);
    
    //////////////////////////////////////////////////////////////////////////////////////////
    
	// Render the cube
	curObj->draw(shaderProgram);
	// Gets events, including input such as keyboard and mouse or window resizing
    if (showSphere == 1) {
        sphere->draw(shaderProgram);
    }
    if (showCone == 1) {
        cone->draw(shaderProgram);
    }
    
	glfwPollEvents();
	// Swap buffers
	glfwSwapBuffers(window);
}

void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key press
	if (action == GLFW_PRESS)
	{
		// Check if escape was pressed
		if (key == GLFW_KEY_ESCAPE)
		{
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
        // 'p' lowercase
        else if (key == GLFW_KEY_P && mods!=0x0001) {
            instr = 'p';
        }
        // 'P' uppercase
        else if (key == GLFW_KEY_P && GLFW_MOD_SHIFT && mods==0x0001) {
            instr = 'P';
        }
        // 'a' lowercase
        else if (key == GLFW_KEY_A && mods!=0x0001) {
            instr = 'a';
        }
        // 'd' lowercase
        else if (key == GLFW_KEY_D && mods!=0x0001) {
            instr = 'd';
        }
        // 'w' lowercase
        //else if (key == GLFW_KEY_W && mods!=0x0001) {
        //    instr = 'w';
        //}
        // 's' lowercase
        else if (key == GLFW_KEY_S && mods!=0x0001) {
            instr = 's';
        }
        // 'z' lowercase
        else if (key == GLFW_KEY_Z && mods!=0x0001) {
            instr = 'z';
        }
        // 'Z' lowercase
        else if (key == GLFW_KEY_Z && GLFW_MOD_SHIFT && mods==0x0001) {
            instr = 'Z';
        }
        // 'c' lowercase
        else if (key == GLFW_KEY_C && mods!=0x0001) {
            instr = 'c';
        }
        // 'C' lowercase
        else if (key == GLFW_KEY_C && GLFW_MOD_SHIFT && mods==0x0001) {
            instr = 'C';
        }
        // 'r' lowercase
        else if (key == GLFW_KEY_R && mods!=0x0001) {
            instr = 'r';
        }
        // 'R' lowercase
        else if (key == GLFW_KEY_R && GLFW_MOD_SHIFT && mods==0x0001) {
            instr = 'R';
        }
        else if (key == GLFW_KEY_5) {
            curObj = bunny;
            objectToRender = "bunny";
        }
        else if (key == GLFW_KEY_6) {
            curObj = dragon;
            objectToRender = "dragon";
        }
        else if (key == GLFW_KEY_7) {
            curObj = bear;
            objectToRender = "bear";
        }
        else if (key == GLFW_KEY_N && mods!=0x0001) {
            if (shaderMode == 1) {
                glUniform1i(glGetUniformLocation(shaderProgram, "shaderMode"), 0);
                shaderMode = 0;
            }
            else {
                glUniform1i(glGetUniformLocation(shaderProgram, "shaderMode"), 1);
                shaderMode = 1;
            }
        }
        //directional only
        else if (key == GLFW_KEY_1) {
            glUniform1i(glGetUniformLocation(shaderProgram, "dirLight.turnedOn"), 1);
            glUniform1i(glGetUniformLocation(shaderProgram, "pointLight.turnedOn"), 0);
            glUniform1i(glGetUniformLocation(shaderProgram, "spotLight.turnedOn"), 0);
            dirLightOn = 1;
            scene=1;
            showSphere = 0;
            showCone = 0;
        }
        else if (key == GLFW_KEY_2) {
            glUniform1i(glGetUniformLocation(shaderProgram, "dirLight.turnedOn"), 1);
            glUniform1i(glGetUniformLocation(shaderProgram, "pointLight.turnedOn"), 1);
            glUniform1i(glGetUniformLocation(shaderProgram, "spotLight.turnedOn"), 0);
            dirLightOn = 1;
            scene=2;
            showSphere = 1;
            showCone = 0;
            
        }
        else if (key == GLFW_KEY_3) {
            glUniform1i(glGetUniformLocation(shaderProgram, "dirLight.turnedOn"), 1);
            glUniform1i(glGetUniformLocation(shaderProgram, "pointLight.turnedOn"), 0);
            glUniform1i(glGetUniformLocation(shaderProgram, "spotLight.turnedOn"), 1);
            dirLightOn = 1;
            scene=3;
            showSphere = 0;
            showCone = 1;
        }
        else if (key == GLFW_KEY_4) {
            if (dirLightOn == 1) {
                glUniform1i(glGetUniformLocation(shaderProgram, "dirLight.turnedOn"), 0);
                dirLightOn = 0;
            }
            else {
                glUniform1i(glGetUniformLocation(shaderProgram, "dirLight.turnedOn"), 1);
                dirLightOn = 1;
            }
            scene=4;
        }
        // W
        else if (key == GLFW_KEY_W && GLFW_MOD_SHIFT && mods==0x0001) {
            spotLightWidth += 0.1f;
            if (spotLightWidth > 1.5f) {spotLightWidth = 1.5f;}
            glUniform1f(glGetUniformLocation(shaderProgram, "spotLight.cutoff"), spotLightWidth);
        
        }
        // w
        else if (key == GLFW_KEY_W && mods!=0x0001) {
            spotLightWidth -= 0.1f;
            if (spotLightWidth < 0.0f) {spotLightWidth = 0.0f;}
            glUniform1f(glGetUniformLocation(shaderProgram, "spotLight.cutoff"), spotLightWidth);
           
        }
        // E
        else if (key == GLFW_KEY_E && GLFW_MOD_SHIFT && mods==0x0001) {
            exponentFactor *= 1.25f;
            //if (exponentFactor > 10.f) {exponentFactor = 10.0f;}
            glUniform1f(glGetUniformLocation(shaderProgram, "spotLight.exponent"), exponentFactor);
            
        }
        // e
        else if (key == GLFW_KEY_E && mods!=0x0001) {
            exponentFactor *= 0.8f;
            //if (exponentFactor < 0.0f) {exponentFactor = 0.0f;}
            glUniform1f(glGetUniformLocation(shaderProgram, "spotLight.exponent"), exponentFactor);
            
        }
	}
}

void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        //set the value for case switch in cursor_position_callback
        mouseClick = 'L';
        //save the cursor position for later use
        glfwGetCursorPos(window, &uponClickX, &uponClickY);
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        //set the value for case switch in cursor_position_callback
        mouseClick = 'N';
        //save the cursor position for later use
        glfwGetCursorPos(window, &uponClickX, &uponClickY);
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        //set the value for case switch in cursor_position_callback
        mouseClick = 'R';
        //save the cursor position for later use
        glfwGetCursorPos(window, &uponClickX, &uponClickY);
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        //set the value for case switch in cursor_position_callback
        mouseClick = 'N';
        //save the cursor position for later use
        glfwGetCursorPos(window, &uponClickX, &uponClickY);
    }
}


glm::vec3 Window::trackBallMapping(float x, float y) {
    glm::vec3 v;
    float d;
    
    v.x = (2.0f * x - width) / (width);
    v.y = (height - 2.0f * y) / (height);
    v.z = 0.0f;
    
    d = glm::length(v);
    
    if (d < 1.0f) {
        d = d;
    }
    else {
        d = 1.0f;
    }
    
    v.z = sqrtf(1.0001f - d*d);
    glm::normalize(v);
    
    return v;
}

void Window::cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    
    glm::vec3 direction;
    float pixel_diff;
    float rot_angle, zoom_factor;
    glm::vec3 curPoint;
    
    switch (mouseClick) {
     
        case 'L': {
            
            //get the current cursor position
            double curX=0.0;
            double curY=0.0;
            glfwGetCursorPos(window, &curX, &curY);
            curPoint = trackBallMapping(curX, curY);
            //get the cursor position from when the left mouse button was clicked
            glm::vec3 prevPoint = trackBallMapping(uponClickX, uponClickY);
            direction = curPoint - prevPoint;
            
            float velocity = glm::length(direction);
            if (velocity > 0.0001) {
                glm::vec3 rotAxis;
                //axis by cross product of points
                rotAxis = glm::cross(prevPoint, curPoint);
                //angle by acos( dot(a,b) / (length(a)*length(b)) )
                rot_angle = glm::acos(glm::dot(curPoint, prevPoint) / (glm::length(curPoint)*glm::length(prevPoint)));
                
                curObj->rotate(rotAxis, rot_angle);
            }
         
            //prevPoint = curPoint;
            break;
        }
            
        case 'R': {
            
            // get the current cursor position
            double curX=0.0;
            double curY=0.0;
            glfwGetCursorPos(window, &curX, &curY);
            
            // calculate how much the object should move, based on the cursor
            double xMovement = 0.03f * (curX - uponClickX);
            double yMovement = 0.03f * (curY - uponClickY);
   
            // translate to follow the cursor
            curObj->translateX(xMovement);
            curObj->translateY(-yMovement);
            
            uponClickX = curX;
            uponClickY = curY;
            
            break;
        }
            
        case 'N': {
            
            //get the current cursor position
            double curX=0.0;
            double curY=0.0;
            glfwGetCursorPos(window, &curX, &curY);
            
            uponClickX = curX;
            uponClickY = curY;
            
            break;
        }
    }
}

void Window::scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    
    //scroll wheel (vertical) provides offsets along the y-axis
    curObj->translateZ(1.5f*yoffset);
}


