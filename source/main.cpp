#include "../header/raylib.h"
#include "../header/UserInterface.h"
#include "../header/Helper.h"

void SauKhiAnVaoNganHangCauHoi()
{
    const int screenWidth = 1500;
    const int screenHeight = 800;
    SetWindowPosition(GetMonitorWidth(0) / 2 - screenWidth / 2, GetMonitorHeight(0) / 2 - screenHeight / 2);
    SetWindowSize(screenWidth, screenHeight);
    while (!is_close_icon_pressed)
    {
        BeginDrawing();
        ClearBackground(WHITE);
        DrawRectangle(0, 0, 1500, 70, Fade(GRAY, 0.3f));
        DrawRectangle(0, 70, 1500, 100, Fade(SKYBLUE, 0.5f));
        DrawRectangle(0, 650, 1500, 50, Fade(GOLD, 0.6f));
        DrawTextEx(font, "Ngân hàng câu hỏi môn:", {5, 20}, 30, 3, Fade(BLUE, 0.9f));
        for (int i = 0; i < 4; i++)
        {
            DrawRectangle(1050 + (110 * i), 15, 90, 40, BLUE);
            DrawRectangleLines(1050 + (110 * i), 15, 90, 40, BLACK);
        }
        DrawTextEx(font, "Thêm", {1060, 19.5}, 30, 3, BLACK);
        DrawTextEx(font, "Sửa", {1180, 19.5}, 30, 3, BLACK);
        DrawTextEx(font, "Xóa", {1290, 19.5}, 30, 3, BLACK);
        // DrawTextEx(font,"")

        EndDrawing();
        if (WindowShouldClose())
        {
            is_close_icon_pressed = true;
        }

        if (IsKeyPressed(KEY_ESCAPE))
        {
            break;
        }
    }
}

int main()
{
    Initialize();

    DanhSachLopHoc dslh("../data/DANHSACHLOP.txt");
    DanhSachMonHoc dsmh("../data/DANHSACHMON.txt");
    DanhSachCauHoi dsch("../data/DANHSACHCAUHOI.txt");
    SinhVien *sv = nullptr;
    std::string testing_subject;
    std::string time;
    std::string number_of_questions;
    std::string ma_lop;
    CauHoi **ptr = nullptr;

        current_scene = scene_stack.pop();
        while (current_scene != Exit || is_close_icon_pressed)
        {
            switch (current_scene)
            {
            case Login:
                LoginScene(dslh, sv);
                current_scene = scene_stack.pop();
                break;

            case Main_SV:
                MainSceneSV(sv, dsmh, dsch, testing_subject, time, number_of_questions, ptr);
                current_scene = scene_stack.pop();
                break;

            case Main_GV:
                GiaoDienDanhSachLop(dslh, dsmh, dsch);
                current_scene = scene_stack.pop();
                break;

            case Testing:
                TestingScene(sv, dsmh, ptr, testing_subject, time, number_of_questions);
                current_scene = scene_stack.pop();
                break;

            case Exit:
                goto exit_tag;
            }
        }
       exit_tag:
    Deinitialize();
    return 0;
}