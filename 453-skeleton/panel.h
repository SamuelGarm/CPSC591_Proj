#pragma once

#include <iosfwd>
#include <string>
#include <glm/gtx/vector_query.hpp>

#include "imgui/imgui.h"

#include "ClusterVoid.h"

namespace panel {

extern bool showPanel;
extern ImVec4 clear_color;

extern bool bgColourChanged;

// clipping planes
extern bool useClipping;
extern float xClipMin;
extern float xClipMax;
extern float yClipMin;
extern float yClipMax;
extern float zClipMin;
extern float zClipMax;
extern bool clippingChanged;
extern glm::vec3 minClipBounds;
extern glm::vec3 maxClipBounds;

extern glm::vec3 lightPos;
extern float ks;
extern float kd;
extern float ka;
extern float bodyCol;
extern float d;
extern float particleDiameter;
extern bool useSanders;

// Ray Trace Samples
extern int sample_count;
extern int max_path_length;
extern float light_emission;

//camera controls
extern bool camSpeedChanged;
extern float camSpeed;

extern int renderPipeline;
extern bool pipelineChanged;
extern VoxelGrid<clusterData> vGrid;
extern int xSize;
extern int ySize;
extern int zSize;
extern float clusterPercentage;
extern int clusterMode;
extern float meanRadius;
extern bool sampleNeighbours;
extern bool fixedRadius;
extern bool retraceImage;
// reset
extern bool resetView;

void updateMenu();

} // namespace panel
