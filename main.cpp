#include <iostream>
using namespace std;

#include <GL/glew.h>
#include <GLFW/glfw3.h>

int main(void)
{
    GLFWwindow* window;

    // Note: comments using /*this multi-line style*/ indicate original GLFW documentation code
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "GLFW OpenGL(Cherno)", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */

    glfwMakeContextCurrent(window);

    // Initialise GLEW now that we have a valid OpenGL context (^^^)
    if (glewInit() != GLEW_OK)
        cout << "ERROR::GLEWINIT FAILED" << endl;

    // An array of floats to store the triangle's point vertices
    float positions[6] = {
        //  Xcoord Ycoord
            -0.5f, -0.5f, // 1st vertex, made up of only 1 attribute (position)
             0.0f,  0.5f, // 2nd vertex, " "
             0.5f, -0.5f  // 3rd & final vertex, " "
    };

    // Defining OpenGL Vertex Buffer
    unsigned int buffer;
    glGenBuffers(1, &buffer); // only gen 1 buffer, the memory address to write into
    // Selecting the buffer "target" (like selecting a layer in Photoshop before you edit it)
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

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        // (glDraw function call goes in here to draw the triangle!!)
        // For example: glDrawArrays(GL_TRIANGLES, 0, 3); // what sort of shape are we talking about here? from what position in this array do we start working from? how many positions are we talking about?

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}