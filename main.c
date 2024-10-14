#include <stdbool.h>
#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 400;

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  (void)window;
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

// hardcoded shader sources since they're very simple.
const char *vertex_shader_src = "#version 330 core\n"
  "layout (location = 0) in vec3 aPos;\n"
  "void main() { gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0); }\0";

const char *fragment_shader_src_blue = "#version 330 core\n"
  "out vec4 FragColor;\n"
  "void main() { FragColor = vec4(0.36f, 0.81f, 0.98f, 1.0f); }\0";
const char *fragment_shader_src_pink = "#version 330 core\n"
  "out vec4 FragColor;\n"
  "void main() { FragColor = vec4(0.96f, 0.62f, 0.72f, 1.0f); }\0";
const char *fragment_shader_src_white = "#version 330 core\n"
  "out vec4 FragColor;\n"
  "void main() { FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f); }\0";

int main() {
  if (!glfwInit()) {
    printf("Failed to initialise GLFW\n");
    return 1;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "hello_trans", NULL, NULL);
  if (window == NULL) {
    printf("Failed to create GLFW window\n");
    glfwTerminate();
    return 1;
  }
  glfwMakeContextCurrent(window);

  glfwSwapInterval(1); // enable vsync

  if (glewInit()) {
    printf("Failed to initialise GLEW\n");
    return 1;
  }

  // first, build and compile our simple vertex shader.
  unsigned int vertex_shader;
  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vertex_shader_src, NULL);
  glCompileShader(vertex_shader);

  // second, build and compile our fragment shaders.
  unsigned int fragment_shader_blue, fragment_shader_pink, fragment_shader_white;

  fragment_shader_blue = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader_blue, 1, &fragment_shader_src_blue, NULL);
  glCompileShader(fragment_shader_blue);

  fragment_shader_pink = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader_pink, 1, &fragment_shader_src_pink, NULL);
  glCompileShader(fragment_shader_pink);

  fragment_shader_white = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader_white, 1, &fragment_shader_src_white, NULL);
  glCompileShader(fragment_shader_white);

  // create and activate shader program objects to use our compiled shaders.
  unsigned int shader_program_blue, shader_program_pink, shader_program_white;

  shader_program_blue = glCreateProgram();
  glAttachShader(shader_program_blue, vertex_shader);
  glAttachShader(shader_program_blue, fragment_shader_blue);
  glLinkProgram(shader_program_blue);
  
  shader_program_pink = glCreateProgram();
  glAttachShader(shader_program_pink, vertex_shader);
  glAttachShader(shader_program_pink, fragment_shader_pink);
  glLinkProgram(shader_program_pink);
  
  shader_program_white = glCreateProgram();
  glAttachShader(shader_program_white, vertex_shader);
  glAttachShader(shader_program_white, fragment_shader_white);
  glLinkProgram(shader_program_white);

  // set up vertex data, buffers, and their associated attributes.
  // vertice order: top right, bottom right, bottom left, top left
  float stripe_blue_top[] = {
     1.0f,  1.0f,  0.0f,
     1.0f,  0.6f,  0.0f,
    -1.0f,  0.6f,  0.0f,
    -1.0f,  1.0f,  0.0f
  };
  float stripe_pink_top[] = {
     1.0f,  0.6f,  0.0f,
     1.0f,  0.2f,  0.0f,
    -1.0f,  0.2f,  0.0f,
    -1.0f,  0.6f,  0.0f
  };
  float stripe_white[] = {
     1.0f,  0.2f,  0.0f,
     1.0f, -0.2f,  0.0f,
    -1.0f, -0.2f,  0.0f,
    -1.0f,  0.2f,  0.0f
  };
  float stripe_pink_bottom[] = {
     1.0f, -0.2f,  0.0f,
     1.0f, -0.6f,  0.0f,
    -1.0f, -0.6f,  0.0f,
    -1.0f, -0.2f,  0.0f
  };
  float stripe_blue_bottom[] = {
     1.0f, -0.6f,  0.0f,
     1.0f, -1.0f,  0.0f,
    -1.0f, -1.0f,  0.0f,
    -1.0f, -0.6f,  0.0f
  };
  
  unsigned int stripe_indices[] = {
    0, 1, 3,  // 1st triangle
    1, 2, 3   // 2nd triangle
  };

  // VBO, VAO, EBO (indexed drawing for the bars to limit overhead)
  unsigned int VBOs[5], VAOs[5], EBOs[5];
  glGenVertexArrays(5, VAOs);
  glGenBuffers(5, VBOs);
  glGenBuffers(5, EBOs);

  // first, bind VAOs to configure vertex attributes of each stripe. our vertex
  // data is tightly packed, so we can just set the stride for each VA as 0.
  // second, bind and set vertex buffers of each stripe.
  // third, set the vertex attribute's pointers on each stripe.

  // stripe_blue_top[]
  glBindVertexArray(VAOs[0]);

  glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(stripe_blue_top), stripe_blue_top, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(stripe_indices), stripe_indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glEnableVertexAttribArray(0);

  // stripe_pink_top[]
  glBindVertexArray(VAOs[1]);

  glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(stripe_pink_top), stripe_pink_top, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(stripe_indices), stripe_indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glEnableVertexAttribArray(0);
  
  // stripe_white[]
  glBindVertexArray(VAOs[2]);

  glBindBuffer(GL_ARRAY_BUFFER, VBOs[2]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(stripe_white), stripe_white, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[2]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(stripe_indices), stripe_indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glEnableVertexAttribArray(0);
  
  // stripe_pink_bottom[]
  glBindVertexArray(VAOs[3]);

  glBindBuffer(GL_ARRAY_BUFFER, VBOs[3]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(stripe_pink_bottom), stripe_pink_bottom, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[3]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(stripe_indices), stripe_indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glEnableVertexAttribArray(0);
  
  // stripe_blue_bottom[]
  glBindVertexArray(VAOs[4]);

  glBindBuffer(GL_ARRAY_BUFFER, VBOs[4]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(stripe_blue_bottom), stripe_blue_bottom, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[4]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(stripe_indices), stripe_indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
  glEnableVertexAttribArray(0);
  
  // make sure we resize our viewport alongside our window.
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe polygons
  
  // main render loop
  while (!glfwWindowShouldClose(window)) {
    processInput(window); // handle input
    
    // rendering
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // clear screen with black
    glClear(GL_COLOR_BUFFER_BIT);
    
    // draw each stripe in their associated shader program groups.
    glUseProgram(shader_program_blue);
    glBindVertexArray(VAOs[0]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(VAOs[4]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    glUseProgram(shader_program_pink);
    glBindVertexArray(VAOs[1]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(VAOs[3]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    glUseProgram(shader_program_white);
    glBindVertexArray(VAOs[2]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  
  glfwTerminate();

  return 0;
}
