//#include "ImGuiComponent.h"
//#include "imgui.h"
//#include "imgui_impl_sdl2.h"
//#include "imgui_impl_opengl3.h"
//
//void buki::ImguiComponent::Update(float dt)
//{
//	ImGui::Begin("My First Tool", &active, ImGuiWindowFlags_MenuBar);
//	if (ImGui::BeginMenuBar())
//	{
//		if (ImGui::BeginMenu("File"))
//		{
//			if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
//			if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
//			if (ImGui::MenuItem("Close", "Ctrl+W")) { active = false; }
//			ImGui::EndMenu();
//		}
//		ImGui::EndMenuBar();
//	}
//	ImGuiColorEditFlags imguiColorFlag = 0;
//	float colors[4] = {imguiColor.r, imguiColor.g, imguiColor.b, imguiColor.a};
//	// Edit a color stored as 4 floats
//	ImGui::ColorEdit4("Color", colors, imguiColorFlag);
//
//	// Generate samples and plot them
//	float samples[100];
//	for (int n = 0; n < 100; n++)
//		samples[n] = sinf(n * 0.2f + ImGui::GetTime() * 1.5f);
//	ImGui::PlotLines("Samples", samples, 100);
//
//	// Display contents in a scrolling region
//	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Important Stuff");
//	ImGui::BeginChild("Scrolling");
//	for (int n = 0; n < 50; n++)
//		ImGui::Text("%04d: Some text", n);
//	ImGui::EndChild();
//	ImGui::End();
//}
//
//void buki::ImguiComponent::Start()
//{
//}
//
//void buki::ImguiComponent::Destroy()
//{
//}
//
//json buki::ImguiComponent::Serialize()
//{
//    return json();
//}
//
//void buki::ImguiComponent::Deserialize(json _doc)
//{
//}
//
//void buki::ImguiComponent::Set()
//{
//}
