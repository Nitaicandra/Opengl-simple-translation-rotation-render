// 4DOpengl.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <glew.h>
#include <glfw3.h>

#include <fstream>
#include<sstream>
#include<string>
#include<iostream>

#include <glm.hpp>
#include <vec2.hpp>
#include <vec3.hpp>
#include <vec4.hpp>
#include <mat4x4.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

//ERROR CHECKING============================================================================================================================
#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall( const char* function, const char* file, int line) 
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << "): " << function << " "<< file << ":"<< line << std::endl;
        return false;
    }
    return true;
}

//READ shader file===========================================================================================================================
struct ShaderProgramSource 
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath) {

    std::ifstream stream(filepath);
    std::string line;

    enum class ShaderType 
    {
        NONE = -1, VERTEX = 0,FRAGMENT= 1
    };

    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {

            if (line.find("vertex") != std::string::npos) 
            {
                type = ShaderType::VERTEX;
            }


            else if (line.find("fragment") != std::string::npos) 
            {
                type = ShaderType::FRAGMENT;
            }

        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }

    return{ ss[0].str(), ss[1].str() };
}


//compile and create shader===============================================================================================================================

static unsigned int CompileShader( unsigned int type, const std::string& source)
{

   unsigned int id = glCreateShader(type);
   const char* src = &source[0];
   GLCall(glShaderSource(id, 1, &src, nullptr););
   GLCall(glCompileShader(id););

   //  Error handling
   int result;
   glGetShaderiv(id, GL_COMPILE_STATUS, &result);
   if (result == GL_FALSE)
   {
       int length;
       GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length););
       char* message = (char*)alloca(length * sizeof(char));
       GLCall(glGetShaderInfoLog(id, length, &length, message););
       std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment")<<"Shader!" <<std::endl;
       std::cout << message << std::endl;
       GLCall(glDeleteShader(id););
       return 0;
   }


   return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) 
{

    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program););
    GLCall(glValidateProgram(program););

    GLCall(glDeleteShader(vs););
    GLCall(glDeleteShader(fs););

    return program;

}
//===========================================================================================================================================================
// framebuffer
void framebuffer_resize_callback(GLFWwindow* window, int fbw, int fbh) {
    glViewport(0, 0, fbw, fbh);
}
// INPUT---------------------------------------------------------------------------------------------------------------------------------------------------------- 
void updateInput(GLFWwindow* window, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale) {
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        position.z += 0.01f;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        position.z -= 0.01f;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        position.x += 0.01f;
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        position.x -= 0.01f;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        rotation.y -= 0.5f;
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        rotation.y += 0.5f;
    }
    
}

//INPUT-----------------------------------------------------------------------------------------------------------------------------------------------------------
int main(void)
{
    
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    //Frame buffer stuff
    int framebufferWidth = 0;
    int framebufferHeight = 0;
    //glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);

    // Make the window's context current 
    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    // vsync
    glfwSwapInterval(1);

    // check if glew is ok and set positions
    if (glewInit() != GLEW_OK) {
        std::cout << "ERROR";
    }
    //positions
    float positions[] = {
       -0.5f, -0.5f, 0.5f,         1.f,0.f,0.f,
        0.5f, -0.5f, 0.5f,         1.f,0.f,0.f,
        0.5f,  0.5f, 0.5f,         1.f,0.f,0.f,
       -0.5f,  0.5f, 0.5f,         1.f,0.f,0.f,

       -0.5f, -0.5f,-0.5f,         0.f,0.f,1.f,
       0.5f, -0.5f, -0.5f,         0.f,0.f,1.f,
       0.5f,  0.5f, -0.5f,         0.f,0.f,1.f,
      -0.5f,  0.5f, -0.5f,         0.f,0.f,1.f
    };
    //index buffer
    unsigned int indices[] = {
        0,1,
        2,3,
        1,2,
        3,0,

        4,5,
        6,7,
        5,6,
        7,4,

        4,0,
        5,1,
        6,2,
        7,3,
    };
    // Vertex Array
    unsigned int vao;
    GLCall(glGenVertexArrays( 1, &vao));
    GLCall(glBindVertexArray(vao));

    //buffers and attributes
    unsigned int buffer;
    GLCall(glGenBuffers(1, &buffer););
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer););
    GLCall(glBufferData(GL_ARRAY_BUFFER, 48 * sizeof(float), positions, GL_STATIC_DRAW););


    GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0));
    GLCall(glEnableVertexAttribArray(0));

    GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (const void*)12));
    GLCall(glEnableVertexAttribArray(1));

    unsigned int ibo;
    GLCall(glGenBuffers(1, &ibo););
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo););
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 24 * sizeof(unsigned int), indices, GL_STATIC_DRAW););

    // vertex and fragment shaders
    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");

    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    GLCall(glUseProgram(shader));

    // Uniforms color---------------------------------------------------------------------------
    GLCall(int location = glGetUniformLocation(shader, "u_Color"));
    GLCall(glUniform4f(location, 0.2f, 0.3f, 0.8f, 1.0f));

    float r = 0.0f;
    float increment = 0.05f;


    //Model Matrix-----------------------------------------------------------------------------------------------------------------------------
    glm::vec3 position(0.f);
    glm::vec3 rotation(0.f);
    glm::vec3 scale(0.5f);
    glm::mat4 ModelMatrix(1.f);

    //position.z = -2;
    
    ModelMatrix = glm::translate(ModelMatrix, position);
    ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));
    ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.y), glm::vec3(0.f, 1.f, 0.f));
    ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
    ModelMatrix = glm::scale(ModelMatrix, scale);

    GLCall(glUniformMatrix4fv(glGetUniformLocation(shader, "ModelMatrix"), 1, GL_FALSE, glm::value_ptr(ModelMatrix)));

    //View Matrix-------------------------------------------------------------------------------------------------------------------------------
    glm::vec3 camPosition(0.f, 0.f, 2.f);
    glm::vec3 worldUp = glm::vec3(0.f,1.f,0.f);
    glm::vec3 camFront = glm::vec3(0.f, 0.f, -1.f);
    glm::mat4 ViewMatrix(1.f);
    ViewMatrix = glm::lookAt(camPosition, camPosition + camFront, worldUp);

    float fov = 45.f;
    float nearPlane = 0.1f;
    float farPlane = 1000.f;
    glm::mat4 ProjectionMatrix(1.f);

    ProjectionMatrix = glm::perspective(
    glm::radians(fov),
        static_cast<float>(framebufferWidth) / framebufferHeight,
        nearPlane, farPlane
    );
    GLCall(glUniformMatrix4fv(glGetUniformLocation(shader, "ViewMatrix"), 1, GL_FALSE, glm::value_ptr(ViewMatrix)));

    
    GLCall(glUniformMatrix4fv(glGetUniformLocation(shader, "ProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(ProjectionMatrix)));
    
    //GLCall(glUniformMatrix4fv(glGetUniformLocation(shader, "ModelMatrix"), 1, GL_FALSE, glm::value_ptr(ModelMatrix)));
   

    //-OPTIONS------------------------------------------------------------------------------------------------------------------------------
    
    
    GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE););
    // GLCall(glFrontFace(GL_CCW));
    //GLCall(glEnable(GL_DEPTH_TEST));


    /* Loop until the user closes the window ===================================================================================================================================*/
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        GLCall(glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT));

        

        //uniforms and color stuff- might need to go after draw elements---------------------------------------------------------------
        GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));
        if (r > 1.0f)increment = -0.05;
                else if(r < 0.0f) increment = 0.05f;

        r += increment;
        //ROTATIONModelMatrix----------------------------------------------------------------------------------------
        
       // position.x += 0.001f;
        ModelMatrix = glm::mat4(1.f);
        ModelMatrix = glm::translate(ModelMatrix, position);
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.y), glm::vec3(0.f, 1.f, 0.f));
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
        ModelMatrix = glm::scale(ModelMatrix, scale);

        
        GLCall(int local = glGetUniformLocation(shader, "ModelMatrix"));
        GLCall(glUniformMatrix4fv(local, 1, GL_FALSE, glm::value_ptr(ModelMatrix)));

        //View Matrix----------------------------------------------------------------------------------------------
        ProjectionMatrix = glm::mat4(1.f);
        ProjectionMatrix = glm::perspective(
            glm::radians(fov),
            static_cast<float>(framebufferWidth) / framebufferHeight,
            nearPlane, farPlane
        );
        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
        GLCall(glUniformMatrix4fv(glGetUniformLocation(shader, "ProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(ProjectionMatrix)));
        
        //DRAW----------------------------------------------------------------------------------------------------------
        GLCall(glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, nullptr););

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
        updateInput(window, position, rotation, scale);
    }

    GLCall(glDeleteProgram(shader););
    glfwTerminate();
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
