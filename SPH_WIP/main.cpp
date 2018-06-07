/*
File Name: main.cpp
Copyright © 2018
Original authors: Srinivasan Thiagarajan, Sanketh Bhat, Benjamin Evans
Refactored by Sanketh Bhat, Benjamin Evans
Written under the supervision of David I. Schwartz, Ph.D., and
supported by a professional development seed grant from the B. Thomas
Golisano College of Computing & Information Sciences
(https://www.rit.edu/gccis) at the Rochester Institute of Technology.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or (at
your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

Description:
In this example we demonstrate the use of SPH to emulate fluid motion.
Fluid mechanics can be implemented in 2 ways: Eularian based or Lagragian
based. Eularian based fluid simulation are done using a grid. There are points
on the grid, and the particles contained within the grid follow a specific set
of rules. In Eularian approach, you need to account for conservation of mass
explicitly.
The lagragian approach accoutns for conservation of mass implicitly, Since each
cluster of particles interact with each other and are separatly accountable.
The lagragian approach, considers the forces caused by all the surrounding particles.
It interpolates between the position of the surrounding particles to get the overall
force acting on the selected particle. This process of interpolation is called SPH.

In SPH, we use smoothing kernels to inetrpolate based on the distance from the particle.
We use different kernels for different "aspects" of fluid properties. For pressure, we
implement a spike kernel, as the pressure sould increase drastically as the distance
gets smaller. But we use a poly6 smoothing kernel for density distributions and surface
tension. We use the gradient or laplacian of the kernel, based on whichever one is more applicable.
For more info see the referenced papers.

In SPH fluid simulation, the particles each consist of mass, velocity and acceleration.
The particles expereince density change, forces due to pressure, Viscosity, surface tension
and collision amongst themselves.

Press "SHIFT" to start simulation
Use "SPACE" to toggle gravity in x-axis, or use "W" to toggle gravity in y-axis.

References:
Nicholas Gallagher
Lagrangian Fluid Dynamics Using Smoothed Particles Hydrodynamics by Micky Kelager
Particle-Based Fluid Simulation for Interactive Applications by Matthias Muller, David Charypar and Markus Gross
*/


#include "All_Includes.h"




#pragma region Global Data member
// Global data members
// This is your reference to your shader program.
// This will be assigned with glCreateProgram().
// This program will run on your GPU.
GLuint program;

// These are your references to your actual compiled shaders
GLuint vertex_shader;
GLuint fragment_shader;

// This is a reference to your uniform MVP matrix in your vertex shader
GLuint uniMVP;
GLuint color;

glm::mat4 view;
glm::mat4 proj;
glm::mat4 PV;
glm::mat4 MVP;


double time = 0.0;
double timebase = 0.0;
double accumulator = 0.0;
double physicsStep = 0.022; // This is the number of milliseconds we intend for the physics to update.


							// Reference to the window object being created by GLFW.
GLFWwindow* window;
#pragma endregion

#pragma region Helper_functions
// Functions called only once every time the program is executed.
std::string readShader(std::string fileName)
{
	std::string shaderCode;
	std::string line;

	// We choose ifstream and std::ios::in because we are opening the file for input into our program.
	// If we were writing to the file, we would use ofstream and std::ios::out.
	std::ifstream file(fileName, std::ios::in);

	// This checks to make sure that we didn't encounter any errors when getting the file.
	if (!file.good())
	{
		std::cout << "Can't read file: " << fileName.data() << std::endl;

		// Return so we don't error out.
		return "";
	}

	// ifstream keeps an internal "get" position determining the location of the element to be read next
	// seekg allows you to modify this location, and tellg allows you to get this location
	// This location is stored as a streampos member type, and the parameters passed in must be of this type as well
	// seekg parameters are (offset, direction) or you can just use an absolute (position).
	// The offset parameter is of the type streamoff, and the direction is of the type seekdir (an enum which can be ios::beg, ios::cur, or ios::end referring to the beginning, 
	// current position, or end of the stream).
	file.seekg(0, std::ios::end);					// Moves the "get" position to the end of the file.
	shaderCode.resize((unsigned int)file.tellg());	// Resizes the shaderCode string to the size of the file being read, given that tellg will give the current "get" which is at the end of the file.
	file.seekg(0, std::ios::beg);					// Moves the "get" position to the start of the file.

													// File streams contain two member functions for reading and writing binary data (read, write). The read function belongs to ifstream, and the write function belongs to ofstream.
													// The parameters are (memoryBlock, size) where memoryBlock is of type char* and represents the address of an array of bytes are to be read from/written to.
													// The size parameter is an integer that determines the number of characters to be read/written from/to the memory block.
	file.read(&shaderCode[0], shaderCode.size());	// Reads from the file (starting at the "get" position which is currently at the start of the file) and writes that data to the beginning
													// of the shaderCode variable, up until the full size of shaderCode. This is done with binary data, which is why we must ensure that the sizes are all correct.

	file.close(); // Now that we're done, close the file and return the shaderCode.

	return shaderCode;
}

// This method will consolidate some of the shader code we've written to return a GLuint to the compiled shader.
// It only requires the shader source code and the shader type.
GLuint createShader(std::string sourceCode, GLenum shaderType)
{
	// glCreateShader, creates a shader given a type (such as GL_VERTEX_SHADER) and returns a GLuint reference to that shader.
	GLuint shader = glCreateShader(shaderType);
	const char *shader_code_ptr = sourceCode.c_str(); // We establish a pointer to our shader code string
	const int shader_code_size = sourceCode.size();   // And we get the size of that string.

													  // glShaderSource replaces the source code in a shader object
													  // It takes the reference to the shader (a GLuint), a count of the number of elements in the string array (in case you're passing in multiple strings), a pointer to the string array 
													  // that contains your source code, and a size variable determining the length of the array.
	glShaderSource(shader, 1, &shader_code_ptr, &shader_code_size);
	glCompileShader(shader); // This just compiles the shader, given the source code.

	GLint isCompiled = 0;

	// Check the compile status to see if the shader compiled correctly.
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

	if (isCompiled == GL_FALSE)
	{
		char infolog[1024];
		glGetShaderInfoLog(shader, 1024, NULL, infolog);

		// Print the compile error.
		std::cout << "The shader failed to compile with the error:" << std::endl << infolog << std::endl;

		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		glDeleteShader(shader); // Don't leak the shader.

								// NOTE: I almost always put a break point here, so that instead of the program continuing with a deleted/failed shader, it stops and gives me a chance to look at what may 
								// have gone wrong. You can check the console output to see what the error was, and usually that will point you in the right direction.
	}

	return shader;
}

// Initialization code
void init()
{
	// Initializes the glew library
	glewInit();

	// Enables the depth test, which you will want in most cases. You can disable this in the render loop if you need to.
	glEnable(GL_DEPTH_TEST);

	// Read in the shader code from a file.
	std::string vertShader = readShader("../VertexShader.glsl");
	std::string fragShader = readShader("../FragmentShader.glsl");

	// createShader consolidates all of the shader compilation code
	vertex_shader = createShader(vertShader, GL_VERTEX_SHADER);
	fragment_shader = createShader(fragShader, GL_FRAGMENT_SHADER);

	// A shader is a program that runs on your GPU instead of your CPU. In this sense, OpenGL refers to your groups of shaders as "programs".
	// Using glCreateProgram creates a shader program and returns a GLuint reference to it.
	program = glCreateProgram();
	glAttachShader(program, vertex_shader);		// This attaches our vertex shader to our program.
	glAttachShader(program, fragment_shader);	// This attaches our fragment shader to our program.

												// This links the program, using the vertex and fragment shaders to create executables to run on the GPU.
	glLinkProgram(program);
	// End of shader and program creation

	// Creates the view matrix using glm::lookAt.
	// First parameter is camera position, second parameter is point to be centered on-screen, and the third paramter is the up axis.
	view = glm::lookAt(glm::vec3(BoundarySizeX / 2.0f, 0.5f, 3.0f), glm::vec3(BoundarySizeX / 2.0f, 0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

	// Creates a projection matrix using glm::perspective.
	// First parameter is the vertical FoV (Field of View), second paramter is the aspect ratio, 3rd parameter is the near clipping plane, 4th parameter is the far clipping plane.
	proj = glm::perspective(45.0f, 800.0f / 800.0f, 0.1f, 100.0f);

	PV = proj * view;

	glm::mat4 translation = glm::translate(glm::vec3(0.0f, 0.0f, 0.0f));

	MVP = PV * translation;

	// This gets us a reference to the uniform variable in the vertex shader, which is called "color".
	// We're using this variable to change color during runtime, without changing the buffer values.
	// Only 2 parameters required: A reference to the shader program and the name of the uniform variable within the shader code.
	uniMVP = glGetUniformLocation(program, "MVP");

	// This is not necessary, but I prefer to handle my vertices in the clockwise order. glFrontFace defines which face of the triangles you're drawing is the front.
	// Essentially, if you draw your vertices in counter-clockwise order, by default (in OpenGL) the front face will be facing you/the screen. If you draw them clockwise, the front face 
	// will face away from you. By passing in GL_CW to this function, we are saying the opposite, and now the front face will face you if you draw in the clockwise order.
	// If you don't use this, just reverse the order of the vertices in your array when you define them so that you draw the points in a counter-clockwise order.
	glFrontFace(GL_CCW);

	// This is also not necessary, but more efficient and is generally good practice. By default, OpenGL will render both sides of a triangle that you draw. By enabling GL_CULL_FACE, 
	// we are telling OpenGL to only render the front face. This means that if you rotated the triangle over the X-axis, you wouldn't see the other side of the triangle as it rotated.
	glEnable(GL_CULL_FACE);
	//We are disabling hte cull face, because we wish to see both the front and back of the objects in wireframe mode for better understanding the depth.

	// Determines the interpretation of polygons for rasterization. The first parameter, face, determines which polygons the mode applies to.
	// The face can be either GL_FRONT, GL_BACK, or GL_FRONT_AND_BACK
	// The mode determines how the polygons will be rasterized. GL_POINT will draw points at each vertex, GL_LINE will draw lines between the vertices, and 
	// GL_FILL will fill the area inside those lines.
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}

#pragma endregion

#pragma region util_functions
// Functions called between every frame. game logic

// This runs once every physics timestep.
void update(float t)
{
	//Catergorize the particles into their respective grids.
	catergorizeParticles();

	//Each particles collects info on the particles surrounding them
	getNeighbors();

	//Update the densities at each particle location
	updateDensities();

	//update the acceleration of each particle
	if (start)
		updateVelocities();

	//Resolve collisions
	//findAndResolveCollisions();

	//Integrate the particle (update the 
	integrate(t);
}

// This runs once every frame to determine the FPS and how often to call update based on the physics step.
void checkTime()
{
	// Get the current time.
	time = glfwGetTime();

	// Get the time since we last ran an update.
	double dt = time - timebase;

	// If more time has passed than our physics timestep.
	if (dt > physicsStep)
	{

		timebase = time; // set new last updated time

						 // Limit dt
		if (dt > 0.25)
		{
			dt = 0.25;
		}
		accumulator += dt;

		// Update physics necessary amount
		while (accumulator >= physicsStep)
		{
			update(physicsStep);
			accumulator -= physicsStep;
		}
	}
}

// This function runs every frame
void renderScene()
{
	// Clear the color buffer and the depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Clear the screen to white
	glClearColor(0.2f, 0.2f, 0.2f, 1.0);

	// Tell OpenGL to use the shader program you've created.
	glUseProgram(program);

	glLineWidth(1.0f);

	glm::vec3 p;
	glm::mat4 m;
	glEnable(GL_POINT_SMOOTH);
	glUniformMatrix4fv(uniMVP, 1, GL_FALSE, glm::value_ptr(PV));
	glColor3f(1.0f, 1.0f, 1.0f);
	glPointSize(POINTSIZE);

	glBegin(GL_POINTS);

	for (int i = 0; i < Number_of_particels; i++)
	{
		glVertex3fv((float*)&particles[i].position);
	}
	glEnd();
}

// This function is used to handle key inputs.
// It is a callback funciton. i.e. glfw takes the pointer to this function (via function pointer) and calls this function every time a key is pressed in the during event polling.
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_SPACE && (action == GLFW_PRESS))
	{
		if (G.x >= 0)
		{

			view = glm::lookAt(glm::vec3(BoundarySizeX / 2.0f, 0.5f, 3.0f), glm::vec3(BoundarySizeX / 2.0f, 0.5f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f));

			PV = proj * view;

			G.x = -14.8f;
		}
		else
		{
			view = glm::lookAt(glm::vec3(BoundarySizeX / 2.0f, 0.5f, 3.0f), glm::vec3(BoundarySizeX / 2.0f, 0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

			PV = proj * view;

			G.x = 0;
		}

	}


	if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		if (G.y == 0)
		{
			G.y = -9.8f;
		}
		else
			G.y = 0.0f;
	}

	if (key == GLFW_KEY_LEFT_SHIFT && (action == GLFW_PRESS || action == GLFW_REPEAT))
	{
		start = true;
	}

}
#pragma endregion

void main()
{
	glfwInit();

	// Creates a window given (width, height, title, monitorPtr, windowPtr).
	// Don't worry about the last two, as they have to do with controlling which monitor to display on and having a reference to other windows. Leaving them as nullptr is fine.
	window = glfwCreateWindow(800, 800, "Fluid (SPH)", nullptr, nullptr);

	std::cout << "\n This program demonstrates implementation of angular friction \n\n\n\n\n\n\n\n\n\n";
	std::cout << "\n Press \"SHIFT\" to start simulation.";
	std::cout << "\n Use \"SPACE\" to toggle gravity in x - axis.";
	std::cout << "\n use \"W\" to toggle gravity in y - axis.";

	// Makes the OpenGL context current for the created window.
	glfwMakeContextCurrent(window);

	// Sets the number of screen updates to wait before swapping the buffers.
	// Setting this to zero will disable VSync, which allows us to actually get a read on our FPS. Otherwise we'd be consistently getting 60FPS or lower, 
	// since it would match our FPS to the screen refresh rate.
	// Set to 1 to enable VSync.
	glfwSwapInterval(0);

	// Initializes most things needed before the main loop
	init();

	// Sends the funtion as a funtion pointer along with the window to which it should be applied to.
	glfwSetKeyCallback(window, key_callback);

	setup();

	// Enter the main loop.
	while (!glfwWindowShouldClose(window))
	{
		// Call to update() which will update the gameobjects.
		checkTime();

		// Call the render function.
		renderScene();

		// Swaps the back buffer to the front buffer
		// Remember, you're rendering to the back buffer, then once rendering is complete, you're moving the back buffer to the front so it can be displayed.
		glfwSwapBuffers(window);

		// Checks to see if any events are pending and then processes them.
		glfwPollEvents();
	}

	// After the program is over, cleanup your data!
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	glDeleteProgram(program);
	// Note: If at any point you stop using a "program" or shaders, you should free the data up then and there.


	// Frees up GLFW memory
	glfwTerminate();
}













































/*

std::vector<GameObject *> grid[Grid_Size][Grid_Size][Grid_Size];
std::vector<GameObject *> neighbors[Number_of_particels];
std::vector<GameObject *> particles[Number_of_particels];



#pragma region program specific Data members
// We change this variable upon detecting collision
float timestep = 0.016666667f;

//the number of disvision used to make the structure of the circle
int NumberOfDivisions = 20;



//int lastState; //State for key input
//glm::vec3 boostforce=glm::vec3(0,4,0);
//float initVelocity = 0.045;

GameObject circle(MVP);

// vector of scene bodies
std::vector<GameObject*> bodies;
#pragma endregion




void shoot()
{
	//circle.totalForce += boostforce;
}




// Functions called between every frame.
#pragma region util_functions



void update()
{
	/*
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && lastState != 1)
	{
		shoot();
	}
	lastState = glfwGetKey(window, GLFW_KEY_SPACE);

	

	for each (GameObject *body in bodies)
		body->update(timestep, PV);
		*/
}
/*

// This function is used to handle key inputs.
// It is a callback funciton. i.e. glfw takes the pointer to this function (via function pointer) and calls this function every time a key is pressed in the during event polling.
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//Sets the current window to a close flag when ESC is pressed
	if (key == GLFW_KEY_ESCAPE && ((action == GLFW_PRESS) || action == GLFW_REPEAT))
	{
		glfwSetWindowShouldClose(window,1);
	}
	
}
#pragma endregion

void main()
{
	// Initializes most things needed before the main loop
	init();

	// Sends the funtion as a funtion pointer along with the window to which it should be applied to.
	glfwSetKeyCallback(window, key_callback);

	//Sets up bodies in the scene
	//circle.setupCircle(.05f, NumberOfDivisions);
	//circle.Position(glm::vec3(-0, -1, 0)); 
	//bodies.push_back(&circle);

	//std::cout << "Press Space to fire\n";


	// Enter the main loop.
	while (!glfwWindowShouldClose(window))
	{
		// Call to update() which will update the gameobjects.
		update();

		// Call the render function(s).
		renderScene();

		//Rendering each body after the scene
		for each (GameObject *body in bodies)
			renderBody(body);

		// Swaps the back buffer to the front buffer
		// Remember, you're rendering to the back buffer, then once rendering is complete, you're moving the back buffer to the front so it can be displayed.
		glfwSwapBuffers(window);

		// Checks to see if any events are pending and then processes them.
		glfwPollEvents();
	}

	//Cleans shaders and the program and frees up GLFW memory
	cleanup();

	return;
}

*/