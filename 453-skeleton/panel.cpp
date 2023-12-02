#include "panel.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <array>
#include <string>

namespace panel {

// default values
bool showPanel = false;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

// clipping planes
bool useClipping = false;
extern glm::vec3 minClipBounds = glm::vec3(0,0,0);
extern glm::vec3 maxClipBounds = glm::vec3(10,10,10);
float xClipMin = 0;
float xClipMax = 10;
float yClipMin = 0;
float yClipMax = 10;
float zClipMin = 0;
float zClipMax = 10;

bool clippingChanged = false;

// reset
bool resetView = false;

void updateMenu() {
  using namespace ImGui;
  float olds[] = { xClipMin , xClipMax, yClipMin, yClipMax, zClipMin, zClipMax };
  bool oldCb = useClipping;

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  if (showPanel && Begin("panel", &showPanel, ImGuiWindowFlags_MenuBar)) {
    if (BeginMenuBar()) {
      if (BeginMenu("File")) {
        if (MenuItem("Close", "(P)")) {
          showPanel = false;
        }
        // add more if you would like...
        ImGui::EndMenu();
      }
      EndMenuBar();
    }

    Spacing();
    if (CollapsingHeader("Background Color")) { // Clear
      ColorEdit3("Clear color", (float *)&clear_color);
    }

    Spacing();
    if (CollapsingHeader("Clipping")) {
      Checkbox("Use Clipping", &useClipping);
      DragFloatRange2("X Clipping", &xClipMin, &xClipMax, 1, minClipBounds.x, maxClipBounds.x, "%.0f");
      DragFloatRange2("Y Clipping", &yClipMin, &yClipMax, 1, minClipBounds.y, maxClipBounds.y, "%.0f");
      DragFloatRange2("Z Clipping", &zClipMin, &zClipMax, 1, minClipBounds.z, maxClipBounds.z, "%.0f");
    }

    Spacing();
    Separator();
    resetView = Button("Reset view");

    Spacing();
    Separator();
    Text("Application average %.3f ms/frame (%.1f FPS)",
         1000.0f / GetIO().Framerate, GetIO().Framerate);

    End();
  }
  float news[] = { xClipMin , xClipMax, yClipMin, yClipMax, zClipMin, zClipMax };
  for (int i = 0; i < 6; i++) {
    if (olds[i] != news[i]) {
      clippingChanged = true;
      break;
    }
  }
  if (oldCb != useClipping)
    clippingChanged = true;
  ImGui::Render();	// Render the ImGui window
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // Some middleware thing
}

} // namespace panel
