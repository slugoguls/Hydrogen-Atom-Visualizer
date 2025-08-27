#include "UIManager.h"

void UIManager::drawUI(QuantumNumbers& qn, bool& orbitalNeedsUpdate) {
    ImGui::Begin("Controls");
    ImGui::Text("Quantum Numbers");
    if (ImGui::SliderInt("n", &qn.n, 1, 4)) orbitalNeedsUpdate = true;
    if (qn.l >= qn.n) qn.l = qn.n - 1;
    if (ImGui::SliderInt("l", &qn.l, 0, qn.n > 0 ? qn.n - 1 : 0)) orbitalNeedsUpdate = true;
    if (qn.m > qn.l) qn.m = qn.l;
    if (qn.m < -qn.l) qn.m = -qn.l;
    if (ImGui::SliderInt("m", &qn.m, -qn.l, qn.l)) orbitalNeedsUpdate = true;
    ImGui::Separator();
    ImGui::Text("Spin (s)");
    if (ImGui::RadioButton("Up", &qn.s, 1)) orbitalNeedsUpdate = true;
    ImGui::SameLine();
    if (ImGui::RadioButton("Down", &qn.s, -1)) orbitalNeedsUpdate = true;
    ImGui::SameLine();
    if (ImGui::RadioButton("Both", &qn.s, 0)) orbitalNeedsUpdate = true;
    ImGui::End();
}
