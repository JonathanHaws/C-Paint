#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cmath>

double mouseScroll = 0.0;
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) { mouseScroll += yoffset;}

int main() {

    #pragma region Creating Window
        if (!glfwInit()) { std::cout << "Failed to initialize GLFW" << std::endl; return -1;}
        GLFWwindow* window = glfwCreateWindow(1280, 720, "Paint", NULL, NULL);
        if (!window) { std::cout << "Failed to create window" << std::endl; glfwTerminate(); return -1;}
        glfwMakeContextCurrent(window);
        #pragma endregion

    #pragma region Setup Variables
        glfwSetScrollCallback(window, scroll_callback);
        double mouseX = 0.0, mouseY = 0.0;
        double lastMouseX = 0.0, lastMouseY = 0.0;
        bool mouseLeft = false, mouseMiddle = false, mouseRight = false;
        bool lastMouseLeft = mouseLeft;
        float zoom = 1;
        double scrollYOffset = 0.0;
        bool z = false; 
        bool lastZ = false;

        struct Color { float r, g, b; };
        Color brush = {0.1f, 0.1f, 0.1f};
        int brushSize = 5;
        int canvasWidth = 500;
        int canvasHeight = 500;
        std::vector<std::vector<Color>> canvas(canvasWidth, std::vector<Color>(canvasHeight, {0.9f, 0.9f, 0.9f}));
        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        float canvasX = windowWidth / 2 - canvasWidth / 2, canvasY = windowHeight / 2 - canvasHeight / 2;

        struct PixelChange { int x, y; Color color; };
        struct Stroke { std::vector<PixelChange> changes; };
        std::vector<Stroke> history;

        #pragma endregion

    while (!glfwWindowShouldClose(window)) {
    
        #pragma region Input
            glfwPollEvents();
            lastMouseX = mouseX, lastMouseY = mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);
            lastMouseLeft = mouseLeft;
            mouseLeft = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT); 
            mouseMiddle = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;
            mouseRight = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
            lastZ = z;
            z = glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS;
            bool ctrl = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS;

            #pragma endregion

        #pragma region Undo
            if (z && ctrl && !lastZ) {
                if (!history.empty()) {
                    Stroke& lastStroke = history.back();
                    for (auto changeIt = lastStroke.changes.rbegin(); changeIt != lastStroke.changes.rend(); ++changeIt) {
                        const PixelChange& change = *changeIt;
                        canvas[change.x][change.y] = change.color;
                    }
                    history.pop_back();
                }
            }

            if (history.size() > 100) { history.erase(history.begin()); } 
            #pragma endregion

        #pragma region Drawing Canvas

            #pragma region Setup Context
                int width, height;
                glfwGetWindowSize(window, &width, &height);
                glViewport(0, 0, width, height);
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glOrtho(0, width / zoom, height/ zoom, 0, -1, 1); 
                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
                glClearColor(0.2f, 0.2f, 0.2f, 0.2f);
                glClear(GL_COLOR_BUFFER_BIT);
                glTranslatef(canvasX, canvasY, 0); 
                glPointSize(ceil(zoom));
                #pragma endregion

            glBegin(GL_POINTS);
            for (int x = 0; x < canvas.size(); x++) {
                for (int y = 0; y < canvas[x].size(); y++) {
                    glColor3f(canvas[x][y].r, canvas[x][y].g, canvas[x][y].b);
                    glVertex2i(x, y);
                }
            }
            glEnd();
            #pragma endregion

        #pragma region Painting
            if (mouseLeft) {
                int startX = (int)((lastMouseX / zoom) - canvasX), startY = (int)((lastMouseY / zoom) - canvasY);
                int endX = (int)((mouseX / zoom) - canvasX), endY = (int)((mouseY / zoom) - canvasY);
                float distance = std::sqrt(std::pow(endX - startX, 2) + std::pow(endY - startY, 2));
                int num_steps = std::ceil(distance);

                if(!lastMouseLeft) { history.push_back(Stroke());} // New stroke
                Stroke& currentStroke = history.back();

                for (int step = 0; step <= num_steps; ++step) {
                    float t = static_cast<float>(step) / num_steps;
                    int x = static_cast<int>(startX + t * (endX - startX));
                    int y = static_cast<int>(startY + t * (endY - startY));
                    for (int offsetX = -brushSize / 2; offsetX <= brushSize / 2; ++offsetX) {
                        for (int offsetY = -brushSize / 2; offsetY <= brushSize / 2; ++offsetY) {
                            int xWithOffset = x + offsetX;
                            int yWithOffset = y + offsetY;
                            if (xWithOffset >= 0 && xWithOffset < canvas.size() && yWithOffset >= 0 && yWithOffset < canvas[xWithOffset].size()) {
                                currentStroke.changes.push_back({xWithOffset, yWithOffset, canvas[xWithOffset][yWithOffset]});
                                canvas[xWithOffset][yWithOffset] = brush;
                                
                            }
                        }
                    }
                }
            }
            #pragma endregion

        #pragma region View Controls
            if (mouseMiddle) { 
                canvasX += (mouseX - lastMouseX) / zoom;
                canvasY += (mouseY - lastMouseY) / zoom;
            }
            #pragma endregion

        #pragma region Zoom with Scroll Wheel
            if (mouseScroll != 0) {
                float oldZoom = zoom;
                zoom += mouseScroll * 0.2f * zoom;
                zoom = std::max(0.1f, zoom); // Prevent zoom from going negative
                mouseScroll = 0;
                canvasX -= (mouseX / oldZoom) - (mouseX / zoom);
                canvasY -= (mouseY / oldZoom) - (mouseY / zoom);
            }
            #pragma endregion
    
        glfwSwapBuffers(window);
    
    }
    glfwTerminate(); 
    return 0;
}