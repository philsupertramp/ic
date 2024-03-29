#ifndef IC_IMAGE_CLASSIFIER_H
#define IC_IMAGE_CLASSIFIER_H

#include <map>
#include <iostream>
#include <vector>
#include "../lib/json-parser/JsonFile.h"
#include "imgui.h"

class MyVec2 {
public:
  float x;
  float y;
public:
  MyVec2(): x(0), y(0) {}

  MyVec2(const float& a, const float& b): x(a), y(b){}
  friend std::ostream& operator<<(std::ostream& os, const MyVec2& data);
};
std::ostream& operator<<(std::ostream& os, const MyVec2& data)
{
  os << "[" << data.x << ", " << data.y << "]";
  return os;
}

template<>
void JsonFile::Set<MyVec2>(const std::string &key, std::vector<MyVec2> value) {
  std::stringstream sstream;
  auto elems = value.size();
  int curIndex = 0;
  sstream << "[";
  for(auto elem: value){
    sstream << elem;
    if(curIndex++ < elems-1) sstream << ",";
  }
  sstream << "]";
  return Set(key, sstream.str());
}

class ICRenderer
{
  std::vector<MyVec2> points;
  ImVec2 scrolling = ImVec2(0.0f, 0.0f);
  bool opt_enable_grid = true;
  bool opt_enable_snap_grid = false;
  bool opt_enable_context_menu = true;
  bool add_box = false;
  std::map<std::string, std::vector<MyVec2>> image_map;

public:
  ICRenderer(){};
  
  void Render(GLuint my_image_texture, int my_image_width, int my_image_height, const std::string& filename){

    ImGui::Begin("Tool");
    // Canvas
//  ImGui::BeginTabBar("Things");
//  if (ImGui::BeginTabItem("Canvas"))
    {

      ImGui::Text("pointer = %p", my_image_texture);
      ImGui::Text("size = %d x %d", my_image_width, my_image_height);
      ImGui::Checkbox("Enable grid", &opt_enable_grid);
      ImGui::Checkbox("Enable snap to grid", &opt_enable_snap_grid);
      ImGui::Checkbox("Enable context menu", &opt_enable_context_menu);
      if(ImGui::Button("Save")){
        auto myyfile = JsonFile("");
        myyfile.Set("bounding-box", points);
        myyfile.Write("Foo.json");
      }
      ImGui::Text("Mouse Left: drag to add lines,\nMouse Right: drag to scroll, click for context menu.");

    // Using InvisibleButton() as a convenience 1) it will advance the layout cursor and 2) allows us to use IsItemHovered()/IsItemActive()
    ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();      // ImDrawList API uses screen coordinates!
    ImVec2 canvas_sz = ImGui::GetContentRegionAvail();   // Resize canvas to what's available

      if (canvas_sz.x < 50.0f) canvas_sz.x = 50.0f;
      if (canvas_sz.y < 50.0f) canvas_sz.y = 50.0f;
      ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);

      // Draw border and background color
      ImGuiIO& io = ImGui::GetIO();
      ImDrawList* draw_list = ImGui::GetWindowDrawList();
      draw_list->AddImage((void*)(intptr_t)my_image_texture, canvas_p0,ImVec2(canvas_p0[0] + canvas_sz[0], canvas_p0[1] + canvas_sz[1]));

      // This will catch our interactions
      ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
      const bool is_hovered = ImGui::IsItemHovered(); // Hovered
      const bool is_active = ImGui::IsItemActive();   // Held
      const MyVec2 origin(canvas_p0.x + scrolling.x, canvas_p0.y + scrolling.y); // Lock scrolled origin
      const MyVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);

      // Add first and second point
      if (is_hovered && !add_box && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
      {
        points.push_back(mouse_pos_in_canvas);
        points.push_back(mouse_pos_in_canvas);
        add_box = true;
      }
      if (add_box)
      {
        points.back() = mouse_pos_in_canvas;
        if (!ImGui::IsMouseDown(ImGuiMouseButton_Left))
          add_box = false;
      }

      // Pan (we use a zero mouse threshold when there's no context menu)
      // You may decide to make that threshold dynamic based on whether the mouse is hovering something etc.
      const float mouse_threshold_for_pan = opt_enable_context_menu ? -1.0f : 0.0f;
      if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right, mouse_threshold_for_pan))
      {
        scrolling.x += io.MouseDelta.x;
        scrolling.y += io.MouseDelta.y;
      }

      // Context menu (under default mouse threshold)
      ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
      if (opt_enable_context_menu && ImGui::IsMouseReleased(ImGuiMouseButton_Right) && drag_delta.x == 0.0f && drag_delta.y == 0.0f)
        ImGui::OpenPopupOnItemClick("context");
      if (ImGui::BeginPopup("context"))
      {
        if (add_box)
          points.resize(points.size() - 2);
        add_box = false;
        if (ImGui::MenuItem("Remove one", NULL, false, points.size() > 0)) { points.resize(points.size() - 2); }
        if (ImGui::MenuItem("Remove all", NULL, false, points.size() > 0)) { points.clear(); }
        if (ImGui::MenuItem("Save", NULL, false, !points.empty())){ AddData(filename); }
        ImGui::EndPopup();
      }


      // Draw grid + all lines in the canvas
      draw_list->PushClipRect(canvas_p0, canvas_p1, true);
      if (opt_enable_grid)
      {
        const float GRID_STEP = 64.0f;
        for (float x = fmodf(scrolling.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
          draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 40));
        for (float y = fmodf(scrolling.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
          draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 40));
      }
      for (int n = 0; n < points.size(); n += 2) {
        draw_list->AddQuad(
            ImVec2(origin.x + points[n].x, origin.y + points[n].y),
            ImVec2(origin.x + points[n + 1].x, origin.y + points[n].y),
            ImVec2(origin.x + points[n + 1].x, origin.y + points[n + 1].y),
            ImVec2(origin.x + points[n].x, origin.y + points[n + 1].y),
            IM_COL32(255, 255, 0, 255)
        );
      }
      draw_list->PopClipRect();

    }
    ImGui::End();
  }

  void AddData(const std::string& name){
    image_map.insert({name, points});
    points.clear();
  }

  void Save(){

  }
};

#endif //IC_IMAGE_CLASSIFIER_H
