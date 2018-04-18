#include <iostream>
using namespace std;

// GLEW - openGL Extension Wrangler
#define  GLEW_STATIC
#include <GL/glew.h>

// GLFW - Graphics Library FrameWork (a more barebones alternative to SFML)
#include <GLFW/glfw3.h>

// Window dimensions
const GLint WIDTH = 800, HEIGHT = 600;

// remember: vertex shaders render the colour of a vertices
const GLchar *vertexShaderSource = "#version 330 core\n" // OpenGL version 3.3
"layout (location = 0) in vec3 position;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(position.x, position.y, position.z, 1.0);\n"
"}\0";

const GLchar *fragmentShaderSource = "#version 330 core\n" // fragment shaders render on a per pixel basis
"out vec4 color;\n"
"void main()\n"
"{\n"
"color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\n\0";

// the MAIN function, from here we start the application and run the game loop
int main()
{
    // Init GLFW
    glfwInit();

    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // GLFW 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // window not resizable

    // Create a GLFW window object that we can use for GLFW's functions
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "GLFW OpenGL", nullptr, nullptr);

    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight); // specifically required for retina display

    if(nullptr == window) // yoda convention
    {
        std::cout << "failed to create GLFW window" << endl;
        glfwTerminate();

        return EXIT_FAILURE; // should be an error, maybe return -1?
    }

    glfwMakeContextCurrent(window);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;

    // Initialise GLEW to setup the OpenGL function pointers
    if (GLEW_OK != glewInit())
    {
        cout << "failed to initialise GLEW" << endl;

        return EXIT_FAILURE;
    }

    // Define the viewport dimensions
    glViewport(0, 0, screenWidth, screenHeight);

    // Vertex Shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLint success;
    GLchar infoLog[512]; // assigning an info array of size 512

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) // check for any compilation errors
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog); // making a reference to infoLog (eg, &infoLog) seems to fix this? TODO investigate
        cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
    }

    // Fragment Shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog); // same again here TODO investigate
        cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
    }

    // Linking the shaders
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Checking for any linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    if(!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog); // same again here TODO investigate
        cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << endl;
    }

    // We can now delete the vertex and fragment shaders as they are now part of our shader program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Creating the array of vertices positions (as floats)
    GLfloat  vertices[] =
    { // note: coordinates on a screen a represented in a range from -1.0 to 1.0 (with 0.0 being in the middle, respectively)
            -0.5f, -0.5f, 0.0f, // bottom left coordinate
            0.5f, -0.5f, 0.0f, // bottom right coordinate
            0.0f, 0.5f, 0.0f, // top middle coordinate
    };

    // Creating the Vertex Buffer Object & Vertex Array Object
    GLuint VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);


    // Game loop
    while (!glfwWindowShouldClose(window))
    {
        // check if any events have been activated (key pressed, mouse moved etc.) and call corresponding responce functions
        glfwPollEvents();

        // Render & Clear the colour buffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear( GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // Terminate GLFW, clearing any resources allocated by GLFW
    glfwTerminate();

    return 0;
}