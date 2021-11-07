#include <filesystem>
#include <vector>
#include <iostream>
#include <imgui.h>

#define BIT(x) (1 << x)

std::pair<bool, uint32_t> DirectoryTreeViewRecursive(const std::filesystem::path& path, uint32_t* count, int* selection_mask);


class DirectoryRenderer
{
  std::string path;
  std::vector<std::string> entries;
  uint32_t count = 0;
  int selection_mask = 0;
public:
  int selected = -1;


  explicit DirectoryRenderer(const std::string& directoryPath)
      : path(directoryPath)
  {
    for (const auto& entry : std::filesystem::recursive_directory_iterator(directoryPath)) {
      entries.push_back((entry.path()));
    }
    count = entries.size();
  }

  const char * getSelected(){
    if(selected >= 0) {
      return entries[selected].c_str();
    } else {
      return nullptr;
    }
  }

  void Render(){
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0.0f, 0.0f });

    ImGui::Begin("Directory");

    if (ImGui::CollapsingHeader(path.c_str()))
    {
      uint32_t a_count = count;
      auto clickState = DirectoryTreeViewRecursive(path, &a_count, &selection_mask);

      if (clickState.first)
      {
        if(selected == clickState.second - 1){
          selected = -1;
        } else {
          selected = clickState.second - 1;
        }
        // Update selection state
        // (process outside of tree loop to avoid visual inconsistencies during the clicking frame)
        if (ImGui::GetIO().KeyCtrl) {
          selection_mask ^= BIT(clickState.second);          // CTRL+click to toggle
        }
        else { //if (!(selection_mask & (1 << clickState.second))) // Depending on selection behavior you want, may want to preserve selection when clicking on item that is part of the selection
          selection_mask = BIT(clickState.second);           // Click to single-select
        }
      }
    }
    ImGui::End();

    ImGui::PopStyleVar();
  }
};
