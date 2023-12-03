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

extern bool camSpeedChanged;
extern float camSpeed;

extern VoxelGrid<clusterData> vGrid;

// reset
extern bool resetView;

void updateMenu();

} // namespace panel
