#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "imgui.h"
#include "QuantumNumbers.h"

class UIManager {
public:
    void drawUI(QuantumNumbers& qn, bool& orbitalNeedsUpdate);
};

#endif // UI_MANAGER_H
