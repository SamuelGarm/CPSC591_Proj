#include "panel.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <array>
#include <string>

namespace panel {

// default values
bool showPanel = false;
ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);

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

int renderPipeline = 0;
glm::vec3 lightPos = glm::vec3(0, 0, 1);
float ks = 0.3;
float kd = 0.4;
float ka = 0.2;
float bodyCol = 0.7;
float particleDiameter = 1500; //measured in Å
bool useSanders = false;

bool clippingChanged = false;
bool bgColourChanged = false;
bool camSpeedChanged = false;
bool pipelineChanged = false;
float camSpeed = 0.3f;

extern VoxelGrid<clusterData> vGrid = setupGrid(0,0,0);
//extern int xSize = vGrid.getDimensions().x;
//extern int ySize = vGrid.getDimensions().y;
//extern int zSize = vGrid.getDimensions().z;
extern int xSize = 0;
extern int ySize = 0;
extern int zSize = 0;
extern float clusterPercentage = 0.75f;
extern int clusterMode = 0;
extern float meanRadius = 3.f;
extern bool sampleNeighbours = true;

// Variables to index inspections
int inspectX = 0;
int inspectY = 0;
int inspectZ = 0;

// reset
bool resetView = false;

void updateMenu() {
  using namespace ImGui;
  float olds[] = { xClipMin , xClipMax, yClipMin, yClipMax, zClipMin, zClipMax };
  int oldPipeline = renderPipeline;
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
        if (ColorEdit3("BG Colour", (float*)&clear_color)) {
            bgColourChanged = true;
        }
    }

    Spacing();
    if (CollapsingHeader("Clipping")) {
      Checkbox("Use Clipping", &useClipping);
      DragFloatRange2("X Clipping", &xClipMin, &xClipMax, 1, minClipBounds.x, maxClipBounds.x, "%.0f");
      DragFloatRange2("Y Clipping", &yClipMin, &yClipMax, 1, minClipBounds.y, maxClipBounds.y, "%.0f");
      DragFloatRange2("Z Clipping", &zClipMin, &zClipMax, 1, minClipBounds.z, maxClipBounds.z, "%.0f");
    }

    if (CollapsingHeader("Grating Rendering Settings")) {
      Spacing();
      DragFloat3("Light Pos", (float*)&lightPos);

      Spacing();
      SliderFloat("kd", &kd, 0, 10);
      SliderFloat("ks", &ks, 0, 10);
      SliderFloat("ka", &ka, 0, 10);

      Spacing();
      SliderFloat("Body color (black to white)", &bodyCol, 0, 1);

      Checkbox("Use Sanders", &useSanders);
      SliderFloat("Particle Diameter (Å)", &particleDiameter, 1500, 4000);
    }


    Spacing();
    const char* items[] = { "Orientation", "grating" };
    Combo("RenderingPipeline", &renderPipeline, items, 2);

    Spacing();
    const char* clustGenNames[] = {"Linear Random","Exp Radius"};
    if (CollapsingHeader("Cluster Control")) {
        Text("Size: ");
        InputInt("X", &xSize);
        InputInt("Y", &ySize);
        InputInt("Z", &zSize);
        InputFloat("Cluster Percentage", &clusterPercentage);
        if (Button("Generate")) {
            //vGrid.~VoxelGrid();
            //vGrid = setupGrid(xSize, ySize, zSize);
            setAllVoid(vGrid);
            vGrid.setVoidRatio(clusterPercentage);
            if (clusterMode == 0) {
                distributeVoidClusters(vGrid, sampleNeighbours);
            }
            else if (clusterMode == 1) {
                distributeVoidClusterV2(vGrid, meanRadius);
            }
            trimVGrid(vGrid);
            pipelineChanged = true;
        }
        Text("Cluster Generation Type: ");
        RadioButton(clustGenNames[0], &clusterMode, 0); ImGui::SameLine();
        RadioButton(clustGenNames[1], &clusterMode, 1);
        if (clusterMode == 1) {
            InputFloat("Mean Radius", &meanRadius);
        }
        Checkbox("Sample Neighbours", &sampleNeighbours);
    }

    Spacing();
    if (CollapsingHeader("Cluster Info")) {
        Text("Size: x:%0.f, y:%0.f, z:%0.f", vGrid.getDimensions().x, vGrid.getDimensions().y, vGrid.getDimensions().z);
        float totalNumCells = vGrid.getDimensions().x * vGrid.getDimensions().y * vGrid.getDimensions().z;
        Text("Total Voxels: %.f", totalNumCells);
        Text("Input Cluster Percentage: %f", vGrid.getVoidRatio());
        glm::vec3 actualRatios = getClusterRatios(vGrid);
        Text("Actual Cluster Percentage: %f", actualRatios.x / (actualRatios.x + actualRatios.y));
        Text("Cluster Count: %0.f", actualRatios.x);
        Text("Void Count: %0.f", actualRatios.y);
        Text("Empty Count: %0.f", actualRatios.z);

        Separator();

        // I tried using InputInt3 but could not figure out the array inputs
        Text("Inspect Cell at:");
        InputInt("X", &inspectX);
        SameLine;
        InputInt("Y", &inspectY);
        SameLine;
        InputInt("Z", &inspectZ);

        // Range check if the input exists in the voxel grid
        if (inspectX >= 0 && inspectX < vGrid.getDimensions().x &&
            inspectY >= 0 && inspectY < vGrid.getDimensions().y &&
            inspectZ >= 0 && inspectZ < vGrid.getDimensions().z) {
                //if it exists, display the information about that voxel
                Text("::Voxel Index -> %d, %d, %d", inspectX, inspectY, inspectZ);
                if (vGrid.at(inspectX, inspectY, inspectZ).material == 0) {
                    Text("::Type -> Cluster");
                }
                else if (vGrid.at(inspectX, inspectY, inspectZ).material == 1) {
                    Text("::Type -> Void");
                }
                else if (vGrid.at(inspectX, inspectY, inspectZ).material == 2) {
                    Text("::Type -> Empty");
                }
                Text("::Orientation -> %f, %f, %f"
                    , vGrid.at(inspectX, inspectY, inspectZ).orientation.x
                    , vGrid.at(inspectX, inspectY, inspectZ).orientation.y
                    , vGrid.at(inspectX, inspectY, inspectZ).orientation.z);
        }
        else {
            Text("Out of range");
        }

        // This stuff below is commented out because if you make a large grid,
        // it'll lag the entire simulation heavily, I opted for creating an input inspection
        // as shown above
        
        //for (int i = 0; i < vGrid.getDimensions().x; i++) {
        //    for (int j = 0; j < vGrid.getDimensions().y; j++) {
        //        for (int k = 0; k < vGrid.getDimensions().z; k++) {
        //            Text("Voxel at: %d, %d, %d :: Type-> %d :: Orientation-> %f, %f, %f", i,j,k 
        //                ,vGrid.at(i,j,k).material
        //                ,vGrid.at(i, j, k).orientation.x
        //                ,vGrid.at(i, j, k).orientation.y
        //                ,vGrid.at(i, j, k).orientation.z);
        //        }
        //    }
        //}
    }

    Spacing();
    Separator();
    if (InputFloat("Camera Speed", &camSpeed)) {
        camSpeedChanged = true;
    }
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
  if (oldPipeline != renderPipeline)
    pipelineChanged = true;
  ImGui::Render();	// Render the ImGui window
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); // Some middleware thing
}

} // namespace panel
