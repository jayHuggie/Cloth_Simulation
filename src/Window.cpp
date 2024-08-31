#include "Window.h"

// Window Properties
int Window::width;
int Window::height;
const char* Window::windowTitle = "Cloth Simulation";

// Objects to render
//Cube* Window::cube;

// Project 1
Skeleton* Window::skel;

// Project 2
Skin* Window::skin = nullptr;
bool Window::drawSkelFlag = false;

// Project 3
Character* Window::character = nullptr;

// Project 4
Cloth* Window::cloth = nullptr;

// Camera Properties
Camera* Cam;

// Interaction Variables
bool LeftDown, RightDown;
int MouseX, MouseY;

// The shader program id
GLuint Window::shaderProgram;

// Project 1 Default Skel
//std::string Window::skelFileName = "skelFile/test.skel";

// Project 1
std::string Window::skelFileName = "";

// Project 2
std::string Window::skinFileName = "";

// Project 3
std::string Window::animFileName = "";

//gui
std::vector<std::string> Window::jointsName;
std::vector<glm::vec3> Window::jointsRot;
std::vector<glm::vec3> Window::jointsRotMin;
std::vector<glm::vec3> Window::jointsRotMax;


// Constructors and desctructors
bool Window::initializeProgram() {
    // Create a shader program with a vertex shader and a fragment shader.
    shaderProgram = LoadShaders("shaders/shader.vert", "shaders/shader.frag");

    // Check the shader program.
    if (!shaderProgram) {
        std::cerr << "Failed to initialize shader program" << std::endl;
        return false;
    }

    return true;
}

bool Window::initializeObjects() {
    // Create a cube
    // cube = new Cube();
    // cube = new Cube(glm::vec3(-1, 0, -2), glm::vec3(1, 1, 1));
    
    // Project 1
    /*skel = new Skeleton(skelFileName, glm::mat4(1.0));

    return true;
    */

   // Project 2
   	if (!skelFileName.empty())
	{
		skel = new Skeleton(skelFileName, shaderProgram);

        // Project 3
        int numOfJoint = skel->getNumOfJoint();
		jointsRot.resize(numOfJoint);
		jointsRotMin.resize(numOfJoint);
		jointsRotMax.resize(numOfJoint);
		for (int i = 0; i < numOfJoint; i++)
		{
			std::string jointName = skel->getJointName(i);
			jointsName.push_back(jointName);
		}
	}
	
	if (!skinFileName.empty())
	{
		skin = new Skin(skinFileName, skel, shaderProgram);

        if (!animFileName.empty())
		{
			Animation* anim = new Animation(animFileName);
			character = new Character(skel, anim);
		}
	}
/*  Project 2
	int numOfJoint = skel->getNumOfJoint();
	jointsRot.resize(numOfJoint);
	jointsRotMin.resize(numOfJoint);
	jointsRotMax.resize(numOfJoint);
	for (int i = 0; i < numOfJoint; i++)
	{
		std::string jointName = skel->getJointName(i) + "\t\t";
		jointsName.push_back(jointName);
		skel->getJointRot(i, jointsRot[i]);
		skel->getJointRotMin(i, jointsRotMin[i]);
		skel->getJointRotMax(i, jointsRotMax[i]);
	}
*/
    // Project 4
	cloth = new Cloth(
		4.0f, // size 
		100.0, //mass 
		25, // N particles
		glm::vec3(-2.0f, 3.0f, 0.0f), //topleft
		glm::vec3(1.0f, 0.0f, 0.0f), // horizontal
		glm::vec3(0.0f, -1.0f, 0.0f), // vertical
		shaderProgram);

	return true;
}

void Window::cleanUp() {
    // Deallcoate the objects.
    // delete cube;
	if (skel) delete skel;
	if (skin) delete skin;
	if (cloth) delete cloth;

    // Delete the shader program.
    glDeleteProgram(shaderProgram);
}

// for the Window
GLFWwindow* Window::createWindow(int width, int height) {
    // Initialize GLFW.
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return NULL;
    }

    // 4x antialiasing.
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Create the GLFW window.
    GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);

    // Check if the window could not be created.
    if (!window) {
        std::cerr << "Failed to open GLFW window." << std::endl;
        glfwTerminate();
        return NULL;
    }

    // Make the context of the window.
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewInit();

    // Set swap interval to 1.
    glfwSwapInterval(0);

    // set up the camera
    Cam = new Camera();
    Cam->SetAspect(float(width) / float(height));

    // initialize the interaction variables
    LeftDown = RightDown = false;
    MouseX = MouseY = 0;

    // Call the resize callback to make sure things get drawn immediately.
    Window::resizeCallback(window, width, height);

    return window;
}

void Window::resizeCallback(GLFWwindow* window, int width, int height) {
    Window::width = width;
    Window::height = height;
    // Set the viewport size.
    glViewport(0, 0, width, height);

    Cam->SetAspect(float(width) / float(height));
}

// update and draw functions
void Window::idleCallback() {
    // Perform any updates as necessary.
    Cam->Update();

    //cube->update();

    // Project 1
    // skel->update(glm::mat4(1.0));

    // Project 3
    if (character != nullptr) character->update();

    // Project 3
    // ELSE IF !!!! for fixed camera!!!!!
    else if (skel != nullptr)skel->update(glm::mat4(1.0));

	if (skin != nullptr)skin->update();

    // Project 4
    if (cloth != nullptr) cloth->Update();
	
}

void Window::displayCallback(GLFWwindow* window) {
    // Clear the color and depth buffers.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render the object.
    //cube->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);
    
    // Project 1
    //skel->draw(Cam->GetViewProjectMtx(), Window::shaderProgram);

    if (skin != nullptr)
	{
		skin->draw(Cam->GetViewProjectMtx());
	}
	if ((skin == nullptr || drawSkelFlag) && skel != nullptr)
	{
		skel->draw(Cam->GetViewProjectMtx());
	}
    if (cloth != nullptr)
	{
		cloth->Draw(Cam->GetViewProjectMtx());
	}

    // Gets events, including input such as keyboard and mouse or window resizing.
    glfwPollEvents();

    // Project 2 Imgui
    plotImGUI();

    // Swap buffers.
    glfwSwapBuffers(window);
}

// helper to reset the camera
void Window::resetCamera() {
    Cam->Reset();
    Cam->SetAspect(float(Window::width) / float(Window::height));
}

// callbacks - for Interaction
void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

    // Check for a key press.
    if (action == GLFW_PRESS) {
        switch (key) {
            case GLFW_KEY_ESCAPE:
                // Close the window. This causes the program to also terminate.
                glfwSetWindowShouldClose(window, GL_TRUE);
                break;

            case GLFW_KEY_R:
                resetCamera();
                break;

            case GLFW_KEY_Z:
                // Zoom in
                Cam->SetDistance(glm::max(Cam->GetDistance() - 1.0f, 1.0f)); // Adjust the 1.0f for desired zoom speed
                break;

            case GLFW_KEY_X:
                // Zoom out
                Cam->SetDistance(Cam->GetDistance() + 1.0f); // Adjust the 1.0f for desired zoom speed
                break;

            case GLFW_KEY_S:
			    drawSkelFlag ^= true;
			break;

            default:
                break;
        }
    }
}

void Window::mouse_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        LeftDown = (action == GLFW_PRESS);
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        RightDown = (action == GLFW_PRESS);
    }
}

void Window::cursor_callback(GLFWwindow* window, double currX, double currY) {
    int maxDelta = 100;
    int dx = glm::clamp((int)currX - MouseX, -maxDelta, maxDelta);
    int dy = glm::clamp(-((int)currY - MouseY), -maxDelta, maxDelta);

    MouseX = (int)currX;
    MouseY = (int)currY;

    // Move camera
    // NOTE: this should really be part of Camera::Update()
    if (LeftDown) {
        const float rate = 1.0f;
        Cam->SetAzimuth(Cam->GetAzimuth() + dx * rate);
        Cam->SetIncline(glm::clamp(Cam->GetIncline() - dy * rate, -90.0f, 90.0f));
    }
    if (RightDown) {
        const float rate = 0.005f;
        float dist = glm::clamp(Cam->GetDistance() * (1.0f - dx * rate), 0.01f, 1000.0f);
        Cam->SetDistance(dist);
    }
}

/* Project 1 ~ 3
void Window::plotImGUI()
{
	// Start a new ImGui frame. (To begin rendering the GUI elements)
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	//bool show_demo_window = true;
	//ImGui::ShowDemoWindow(&show_demo_window);

    // Define window flags for the ImGui window. These flags disable the title bar and prevent resizing.
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoResize;

	// Create a new ImGui window named "DOFs" (Degrees of Freedom) with the specified flags.
	ImGui::Begin("DOFs", NULL, window_flags);
    // Set the position and size of the ImGui window to the top-right corner of the application window.
	ImGui::SetWindowPos(ImVec2(Window::width - 190, 0));
	ImGui::SetWindowSize(ImVec2(190, Window::height));

    // Project 3

    // Determine the number of joints if a skeleton is present.
	int numOfJoint = 0;
	if(skel != nullptr) numOfJoint = skel->getNumOfJoint();

    // Iterate through all the joints in the skeleton.
	for (int i = 0; i < numOfJoint; i++)
	{
        // Create a collapsible tree node for each joint using its name.
		if (ImGui::TreeNode(jointsName[i].c_str()))
		{
			glm::vec3 rot, rotMin, rotMax;
			skel->getJointRot(i, rot);
			skel->getJointRotMin(i, rotMin);
			skel->getJointRotMax(i, rotMax);

            // Fetch and display draggable sliders for rotation in X, Y, Z axes.
            // The sliders are constrained by the joint's minimum and maximum rotation values.
            // Updating a slider will modify the joint's rotation accordingly.
            if (ImGui::DragFloat("RotX", &jointsRot[i].x, 0.005f, jointsRotMin[i].x, jointsRotMax[i].x, "%.3f", ImGuiSliderFlags_AlwaysClamp))
            {
                skel->setJointRot(i, jointsRot[i]); // Update the skeleton's joint rotation when the slider is changed.
            }
            if (ImGui::DragFloat("RotY", &jointsRot[i].y, 0.005f, jointsRotMin[i].y, jointsRotMax[i].y, "%.3f", ImGuiSliderFlags_AlwaysClamp))
            {
                skel->setJointRot(i, jointsRot[i]); // As above, for Y axis.
            }
            if (ImGui::DragFloat("RotZ", &jointsRot[i].z, 0.005f, jointsRotMin[i].z, jointsRotMax[i].z, "%.3f", ImGuiSliderFlags_AlwaysClamp))
            {
                skel->setJointRot(i, jointsRot[i]); // As above, for Z axis.
            }

            ImGui::TreePop(); // End the tree node for this joint.
		}
	}

	ImGui::End(); // End the ImGui window.

    // Render the ImGui elements that have been defined.
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
*/


void Window::plotImGUI()
{
    // Initialize a new ImGui frame.
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Configure ImGui window flags to omit the title bar and prevent resizing.
    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoResize;

    // Create an ImGui window named "Coefficients".
    ImGui::Begin("Coefficients", NULL, window_flags);
    // Position and size the ImGui window.
    ImGui::SetWindowPos(ImVec2(Window::width - 260, 0));
    ImGui::SetWindowSize(ImVec2(260, Window::height));

    // Display the current frames per second (FPS).
    std::string fps = "FPS: " + std::to_string(cloth->GetFPS());
    ImGui::Text(fps.c_str());

    // Create a collapsible tree node for translating fixed points of the cloth.
    if (ImGui::TreeNode("Fixed Points"))
    {
        // Small translation shift for moving fixed points.
        float posShift = 0.05f;
        
        // Buttons for translating fixed points along the X axis.
        if (ImGui::Button("\t+\t###XP"))
        {
            cloth->TranslateFixedParticles(0, posShift);
        }
        ImGui::SameLine();
        if (ImGui::Button("\t-\t###XM"))
        {
            cloth->TranslateFixedParticles(0, -posShift);
        }
        ImGui::SameLine();
        ImGui::Text("Axis X");

        // Repeat for Y axis translation.
        if (ImGui::Button("\t+\t###YP"))
        {
            cloth->TranslateFixedParticles(1, posShift);
        }
        ImGui::SameLine();
        if (ImGui::Button("\t-\t###YM"))
        {
            cloth->TranslateFixedParticles(1, -posShift);
        }
        ImGui::SameLine();
        ImGui::Text("Axis Y");

        // Repeat for Z axis translation.
        if (ImGui::Button("\t+\t###ZP"))
        {
            cloth->TranslateFixedParticles(2, posShift);
        }
        ImGui::SameLine();
        if (ImGui::Button("\t-\t###ZM"))
        {
            cloth->TranslateFixedParticles(2, -posShift);
        }
        ImGui::SameLine();
        ImGui::Text("Axis Z");

        // Small rotation shift for rotating fixed points.
        float rotShift = 0.02f;

        // Buttons for rotating fixed points around the X axis.
        if (ImGui::Button("\t+\t###RXP"))
        {
            cloth->RotateFixedParticles(0, rotShift);
        }
        ImGui::SameLine();
        if (ImGui::Button("\t-\t###RXM"))
        {
            cloth->RotateFixedParticles(0, -rotShift);
        }
        ImGui::SameLine();
        ImGui::Text("Rot X");

        // Repeat for Y axis rotation.
        if (ImGui::Button("\t+\t###RYP"))
        {
            cloth->RotateFixedParticles(1, rotShift);
        }
        ImGui::SameLine();
        if (ImGui::Button("\t-\t###RYM"))
        {
            cloth->RotateFixedParticles(1, -rotShift);
        }
        ImGui::SameLine();
        ImGui::Text("Rot Y");

        // Repeat for Z axis rotation.
        if (ImGui::Button("\t+\t###RZP"))
        {
            cloth->RotateFixedParticles(2, rotShift);
        }
        ImGui::SameLine();
        if (ImGui::Button("\t-\t###RZM"))
        {
            cloth->RotateFixedParticles(2, -rotShift);
        }
        ImGui::SameLine();
        ImGui::Text("Rot Z");

        // Provide input fields for directly manipulating the position of each fixed point.
        int numOfFixedPoints = cloth->GetFixedParticleNum();
        for (int i = 0; i < numOfFixedPoints; i++)
        {
            glm::vec3 pos = cloth->GetFixedParticlePos(i);
            std::string name = "Pos " + std::to_string(i);
            if (ImGui::InputFloat3(name.c_str(), glm::value_ptr(pos)))
            {
                cloth->SetFixedParticlePos(i, pos);
            }
        }
        ImGui::TreePop(); // Close the tree node.
    }

// Cloths Coefficients Section
if (ImGui::TreeNode("Cloth Coefficients"))
{
    // Retrieve and allow editing of the total mass of the cloth.
    float clothTotalMass = cloth->GetMass();
    if (ImGui::DragFloat("Mass", &clothTotalMass))
    {
        // Ensure the mass is positive and update the cloth simulation.
        cloth->SetMass(abs(clothTotalMass));
    }

    // Retrieve and allow editing of the gravity acceleration affecting the cloth.
    float g = cloth->GetGravityAcce();
    if (ImGui::DragFloat("g", &g))
    {
        // Ensure gravity acceleration is positive and update the cloth simulation.
        cloth->SetGravityAcce(abs(g));
    }

    // Retrieve and allow editing of the ground position (Y-axis) for collision detection.
    float groundPos = cloth->GetGroundPos();
    if (ImGui::DragFloat("Ground", &groundPos))
    {
        // Update the ground position in the cloth simulation.
        cloth->SetGroundPos(groundPos);
    }

    ImGui::TreePop(); // End of Cloth Coefficients section.
}

// SpringDamper Coefficients Section
if (ImGui::TreeNode("SpringDamper Coefficients"))
{
    // Retrieve and allow editing of the spring constant for the cloth's spring dampers.
    float k_spring = cloth->GetSpringConst();
    if (ImGui::DragFloat("Spring Constant", &k_spring))
    {
        // Ensure the spring constant is positive and update the cloth simulation.
        cloth->SetSpringConst(abs(k_spring));
    }

    // Retrieve and allow editing of the damping constant for the cloth's spring dampers.
    float k_damper = cloth->GetDampingConst();
    if (ImGui::DragFloat("Damping Constant", &k_damper))
    {
        // Ensure the damping constant is positive and update the cloth simulation.
        cloth->SetDampingConst(abs(k_damper));
    }

    ImGui::TreePop(); // End of SpringDamper Coefficients section.
}

// Aerodynamic Coefficients Section
if (ImGui::TreeNode("Aerodynamic Coefficients"))
{
    // Retrieve and allow editing of the wind velocity vector affecting the cloth.
    glm::vec3 windVelocity = cloth->GetWindVelocity();
    if (ImGui::InputFloat3("V_wind", glm::value_ptr(windVelocity)))
    {
        // Update the wind velocity in the cloth simulation.
        cloth->SetWindVelocity(windVelocity);
    }

    // Retrieve and allow editing of the fluid density (e.g., air density) for aerodynamic calculations.
    // "rho" represents the density of a fluid.
    
    float rho = cloth->GetFluidDensity();
    if (ImGui::DragFloat("rho", &rho))
    {
        // Ensure fluid density is positive and update the cloth simulation.
        cloth->SetFluidDensity(abs(rho));
    }

    // Retrieve and allow editing of the drag coefficient for aerodynamic force calculations.
    // "C_d" stands for the drag coefficient. It is a dimensionless number that quantifies 
    // the drag or resistance of an object in a fluid environment, such as air or water. 
    
    float C_d = cloth->GetDragConst();
    if (ImGui::DragFloat("C_d", &C_d))
    {
        // Ensure the drag coefficient is positive and update the cloth simulation.
        cloth->SetDragConst(abs(C_d));
    }

    ImGui::TreePop(); // End of Aerodynamic Coefficients section.
}

    // Finalize the ImGui frame rendering.
    ImGui::End();

    // Render ImGui draw data to the screen.
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}