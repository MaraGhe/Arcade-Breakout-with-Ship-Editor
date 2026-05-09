#include "components/simple_scene.h"
#include "utils/gl_utils.h"
#include "lab_m1/Breakout/breakoutEditor.h"
#include "lab_m1/Breakout/breakoutGame.h"

namespace m1
{
    class BreakoutMain : public gfxc::SimpleScene
    {
    public:
        struct ViewportSpace
        {
            ViewportSpace() : x(0), y(0), width(1), height(1) {}
            ViewportSpace(int x, int y, int width, int height)
                : x(x), y(y), width(width), height(height) {
            }
            int x;
            int y;
            int width;
            int height;
        };

        struct LogicSpace
        {
            LogicSpace() : x(0), y(0), width(1), height(1) {}
            LogicSpace(float x, float y, float width, float height)
                : x(x), y(y), width(width), height(height) {
            }
            float x;
            float y;
            float width;
            float height;
        };

    public:
        BreakoutMain();
        ~BreakoutMain();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        void SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor = glm::vec3(0), bool clear = true);

        void EditorUpdate();
        void GameUpdate(float deltaTimeSeconds);
        breakoutEditor::BreakoutEditor editorScreen;
        breakoutGame::BreakoutGame game;

    protected:
        float length;
        ViewportSpace viewSpace;
        LogicSpace logicSpace;
        glm::ivec2 res = window->GetResolution();
        float unit = 40;	// the side of a 1x1 square
        
        bool gameStarted = false;

        glm::mat3 modelMatrix;
    };
}