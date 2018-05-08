#include <iostream>
#include <fstream>
#include <string>
#include <sstream> // AKA String Stream
using namespace std;

#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct ShaderProgramSource
{
    string vertexSource;
    string fragmentSource;

};

static ShaderProgramSource parseShader(const string& filePath) // TODO: Why not just return a stringstream?
{
    ifstream stream(filePath); // ifstream = Input File Stream

    enum class ShaderType  // which type of shader are we currently reading in? Vertex or Fragment?
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1 // represents the index in our ss array
    };

    string line; // the current line that the while loop is on
    stringstream ss[2]; // 1 for the Vertex Shader, 1 for the Fragment Shader (StringStreams are better designed for iteratively concating onto a string)
    ShaderType type = ShaderType::NONE;

    while(getline(stream, line)) // while there are more lines to include within the file
    {
        if(line.find("#shader") != string::npos) // if doesn't equal an invalid string position
        {
            if(line.find("vertex") != string::npos) // if vertex is found on the same line (ie, if we've found the vertex shader)
            {
                type = ShaderType::VERTEX; // i.e. type = 0
            }
            else if(line.find("fragment") != string::npos) // if fragment is found on the same line (ie, if we've found the fragment shader)
            {
                type = ShaderType::FRAGMENT; // i.e. type = 1
            }
        }
        else // in this case we are reading a line below either the vertex or the fragment shader tags
        {
            ss[(int)type] << line << '\n'; // adding the current line to the relevant index in our ss, concat with a newline
        }
    }

    return {ss[0].str(), ss[1].str()};
}

static unsigned int CompileShader(unsigned int type, const string& source) // type is actually of type GLuint, but on paper that's the same as an unsigned int
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str(); // getting the memory address of the string

    // Specifying the source of our shader:
    glShaderSource(id, 1, &src, nullptr); // the shader that we just made (^^)? how many source codes are we giving? a pointer to the pointer to the source code? the length of the string (null if you dont want to cut the string short)?
    // Finally compile the shader source code that we've presented to OpenGL
    glCompileShader(id); // the shader in question (again)

    int result; // the result of the following compile status check
    // Return the value of a specific parameter (the compile status) of id shader object
    // note: in "iv" the "i" states that we are looking for an integer value, the "v" stands for vector (altough in this case it's a pointer to where the result will go)
    glGetShaderiv(id, GL_COMPILE_STATUS, &result); // the shader object we're querying? the parameter we want to get the value of? and the memory address where we will be storing that value?
    if(GL_FALSE == result) // if the result is false, then shader (id) did not compile successfully // Yoda condition: good habit to get into, this is
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char/* * */ message[length]; // TODO: https://youtu.be/71BLZwRGUJE?t=15m58s
        // todo: replace ^^^ with:
        // char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);


        cout << "ERROR::COMPILING_" << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT") << "_SHADER_SOURCE" << endl; // ?: operator: if type == GL_VERTEX_SHADER (rather than a fragment), print "VERTEX", else print "FRAGMENT"
        cout << message << endl;

        glDeleteShader(id); // delete whichever shader in question, given that it failed anyway

        return 0;
    }

    return id;
}

// TODO: will probably want to move this out into it's own class at some point
static unsigned int CreateShader(const string& vertexShader, const string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    // Link these two shaders, now that their respective source code has successfully compiled into actual shaders!
    glAttachShader(program, vs); // the program that we are actually attaching the shader to? the shader that we are attaching?
    glAttachShader(program, fs); // " "
    glLinkProgram(program);
    // Checks to see if the program can successfully execute
    glValidateProgram(program);

    // Deletes the intermediate shaders, now that we have them as part of our program instead:
    glDeleteShader(vs);
    glDeleteShader(fs);

    return(program);
}

int main()
{
    GLFWwindow* window;

    // Note: comments using /*this multi-line style*/ indicate original GLFW documentation code
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    // Set all the required options for GLFW - specifically required for MacOS
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // GLFW 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//  TODO: evaluate:  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); // window not resizable

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "GLFW OpenGL(Cherno)", NULL, NULL);

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
//    glewExperimental = GL_TRUE;

    // Initialise GLEW now that we have a valid OpenGL context (^^^)
    if (GLEW_OK != glewInit()) // Yoda condition, again
        cout << "ERROR::GLEWINIT()_FAILED" << endl;

    // An array of floats to store the triangle's point vertices
    float positions[] = {
        //  Xcoord,Ycoord
            -0.5f, -0.5f, // 1st vertex, made up of only 1 attribute (position) // Index 0
             0.5f, -0.5f, // 2nd vertex, " "                                    // Index 1
             0.5f,  0.5f, // 3rd & final vertex, " "                            // Index 2
            -0.5f,  0.5f                                                        // Index 3
    };

    unsigned int indices[] = { // An index buffer, with each index indexing the coordinates of each polygon/triangle
        0, 1, 2,
        2, 3, 0
    };

    // Defining Vertex Buffer Object
    unsigned int buffer;
    // Defining Vertex Array Object
    GLuint VAO;

    glGenVertexArrays(1, &VAO); // TODO: what does this do?
    glGenBuffers(1, &buffer); // only gen 1 buffer, the memory address to write into
    // Selecting the buffer "target" (like selecting a layer in Photoshop before you edit it)
    glBindVertexArray(VAO); // TODO: what does this do??
    glBindBuffer(GL_ARRAY_BUFFER, buffer); // we only want to use this buffer as an array, and here's the ID of that buffer
    // Detailing info about the buffer and what it will be used for
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW); // how big do we actually need this buffer to be? (the same size as the array), the data that will be copied into the buffer, and a hint to OpenGL as to the expected usage pattern of the buffer (how frequently writen and read?)
    // note: the expected usage pattern is GL_STATIC_DRAW because, STATIC means that we expect the buffer to be written only once, but read many times
    // ie, the positions of the triangles points are only established once, but must be called to each loop to render

    // Enable the first (0th) attribute in the array in question (positions):
    glEnableVertexAttribArray(0); // which attribute in the array are we talking about here? (the first and only one; e.g. index 0)
    // Giving details on each attribute of the vertex array, in this case, the position coordinates:
    // (this is all so OpenGL knows specifically what sort of data this is and, in turn, how to parse & use it)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), 0); // what index (or attribute number) is this we're talking about? how many values per vertices? what is the data type of the values? are the values already normalised (if not, OpenGL will normalise them for us (0-255 -> 0.0f-1.0f)? the amount of bytes between each vertex (i.e. 2 floats)? what is the size of each attribute (e.g. the position) in a vertex? what is the offset (the num of bytes prior) to this attribute (in this case 0, because it's the first attribute of each vertex)?

    // Defining Index Buffer Object
    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    ShaderProgramSource source = parseShader("/Users/TheSpaceEvader/Documents/C++ Projects/GLFW OpenGL(Cherno)/resources/shaders/basic.shader"); // TODO: why doesnt 'resources/shaders/basic.shader' work???
    unsigned int shader = CreateShader(source.vertexSource, source.fragmentSource);
    glUseProgram(shader); // binds the "shader" program

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClearColor(0.2, 0.3, 0.3, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        // (glDraw function call goes in here to draw the triangle!!) For example:
//        glDrawArrays(GL_TRIANGLES, 0, 6); // what sort of shape are we talking about here? from what position in this array do we start working from? how many positions are we talking about?
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr); // what sort of shape are we talking about here? How many indices are we drawing? What type of data is in the index buffer? Give a pointer to the index buffer (although in this case is already bound so not required by default)

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();

    return 0;
}