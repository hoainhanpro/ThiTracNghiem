#include "../header/UserInterface.h"
#include <iostream>
#include "../header/Helper.h"

Vector2 global_mouse_pos;
Font font;
Image logo;
Stack<Scene> scene_stack;
Scene current_scene;
bool is_close_icon_pressed;

void Initialize()
{
    // SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WIDTH, HEIGHT, "Phần mềm trắc nghiệm");

    global_mouse_pos = {-1, -1};
    scene_stack.push(Exit);
    scene_stack.push(Login);
    is_close_icon_pressed = false;

    font = LoadFontEx(FONT_PATH, 30, 0, 8192);
    logo = LoadImage(LOGO_PATH);

    SetWindowIcon(logo);
    SetWindowMinSize(480, 640);
    SetTargetFPS(10);

    SetExitKey(KEY_NULL);
}

void Deinitialize()
{
    UnloadFont(font);
    UnloadImage(logo);
    CloseWindow();
}

Vector2 TopLeft()
{
    return {0, 0};
}

Vector2 TopCenter()
{
    return {(float)GetScreenWidth() / 2.0f, 0};
}

Vector2 TopRight()
{
    return {(float)GetScreenWidth(), 0};
}

Vector2 MidLeft()
{
    return {0, (float)GetScreenHeight() / 2.0f};
}

Vector2 MidCenter()
{
    return {(float)GetScreenWidth() / 2.0f, (float)GetScreenHeight() / 2.0f};
}

Vector2 MidRight()
{
    return {(float)GetScreenWidth(), (float)GetScreenHeight() / 2.0f};
}

Vector2 BotLeft()
{
    return {0, (float)GetScreenHeight()};
}

Vector2 BotCenter()
{
    return {(float)GetScreenWidth() / 2.0f, (float)GetScreenHeight()};
}

Vector2 BotRight()
{
    return {(float)GetScreenWidth(), (float)GetScreenHeight()};
}

Text::Text(std::string _data,
           Color _normal,
           Color _highlight,
           Font _font,
           int _size,
           int _spacing)
{
    data = _data;
    normal = _normal;
    highlight = _highlight;
    font = _font;
    size = _size;
    spacing = _spacing;
}

void Text::render(Vector2 (*location)(), int offset_x, int offset_y)
{
    Vector2 size_text = MeasureTextEx(font, data.c_str(), size, spacing);
    Vector2 default_location = location();
    Vector2 final_des;

    final_des.x = default_location.x - (size_text.x) / 2 + offset_x;
    final_des.y = default_location.y - (size_text.y) / 2 + offset_y;

    DrawTextEx(font, data.c_str(), final_des, size, spacing, normal);
}

Text &Text::operator=(const Text &other)
{
    data = other.data;
    normal = other.normal;
    highlight = other.highlight;
    font = other.font;
    size = other.size;
    spacing = other.spacing;
    return *this;
}

InputBox::InputBox(Rectangle _box,
                   Color _normal,
                   Color _hightlight,
                   Color _background,
                   Font _font,
                   bool _secure)
{
    box = _box;
    max_length = -1;
    text = {"", _normal, _hightlight, _font};
    clicked = false;
    background = _background;
    frames_count = 0;
    secure = _secure;
    secure_text = {"", _normal, _hightlight, _font};
}

void InputBox::render()
{ // Chỉ vẽ khung
    if (clicked)
        frames_count++;
    else
        frames_count = 0;

    if (clicked)
    {
        DrawRectangleRec(box, background);
        DrawRectangleLinesEx(box, 1, text.highlight);
    }
    else
        DrawRectangleLinesEx(box, 1, text.normal);

    Text display_text;
    if (secure)
        display_text = secure_text;
    else
        display_text = text;

    Color text_color;
    if (clicked)
        text_color = text.highlight;
    else
        text_color = text.normal;

    if (isBoxFull() && display_text.data.length() > max_length)
    {
        DrawTextEx(text.font, display_text.data.substr(display_text.data.length() - max_length).c_str(), {box.x + 5, box.y + 8}, text.size, text.spacing, text_color);
    }
    else
    {
        DrawTextEx(text.font, display_text.data.c_str(), {box.x + 5, box.y + 8}, text.size, text.spacing, text_color);
    }

    if (clicked)
    {
        if (isBoxFull() && display_text.data.length() > max_length)
        {
            if (((frames_count / 5) % 2) == 0)
                DrawText("_", (int)box.x + 8 + MeasureTextEx(text.font, display_text.data.substr(display_text.data.length() - max_length).c_str(), text.size, text.spacing).x, (int)box.y + 12, text.size, text.highlight);
        }
        else
        {
            if (((frames_count / 5) % 2) == 0)
                DrawText("_", (int)box.x + 8 + MeasureTextEx(text.font, display_text.data.c_str(), text.size, text.spacing).x, (int)box.y + 12, text.size, text.highlight);
        }
    }
}

void InputBox::run(Vector2 &mouse_pos)
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        mouse_pos = GetMousePosition();

    if (CheckCollisionPointRec(mouse_pos, box))
        clicked = true;
    else
        clicked = false;

    if (clicked)
    {
        int key = GetCharPressed();
        while (key > 0)
        {
            int length = 0;
            const char *utf8 = CodepointToUTF8(key, &length);
            std::string str_utf8(utf8);
            str_utf8.resize(length);
            if (length > 1)
            {
                uint64_t how_many_to_remove = find(text.data, viet_key[str_utf8][0]);
                if (how_many_to_remove == std::string::npos)
                    how_many_to_remove = text.data.length();
                text.data.resize(how_many_to_remove);
                text.text_layout.resize(how_many_to_remove);
            }
            text.data += str_utf8;
            secure_text.data += '*';
            text.text_layout.push_back(length);
            if (length > 1)
            {
                for (int i = 0; i < length - 1; i++)
                    text.text_layout.push_back(0);
            }

            if (isBoxFull() && max_length == std::string::npos)
                max_length = text.data.length();
            else if (!isBoxFull() && max_length != std::string::npos)
                max_length = std::string::npos;

            key = GetCharPressed();
        }

        if (IsKeyDown(KEY_BACKSPACE) && text.text_layout.size() > 0)
        {
            while (text.text_layout.back() == 0)
                text.text_layout.pop_back();
            for (int i = 0; i < text.text_layout.back(); i++)
                text.data.pop_back();
            text.text_layout.pop_back();

            secure_text.data.pop_back();
        }
    }

    render();
}

bool InputBox::isBoxFull()
{
    Text display = secure ? secure_text : text;
    if (box.width - SIZE_DASH_NORMAL * 2 <= MeasureTextEx(text.font, display.data.c_str(), text.size, text.spacing).x)
        return true;
    return false;
}

Button::Button(Rectangle _box,
               std::string _label,
               Color _normal,
               Color _hightlight,
               Font _font)
{
    box = _box;
    label = _label;
    normal = _normal;
    highlight = _hightlight;
    background = normal;
    font = _font;
}

void Button::render()
{
    Vector2 size = MeasureTextEx(font, label.c_str(), 30, 5);
    DrawRectangleRec(box, background);
    DrawRectangleLines(box.x, box.y, box.width, box.height, BLACK);
    DrawTextEx(font, label.c_str(), {box.x + box.width / 2 - size.x / 2, box.y + box.height / 2 - size.y / 2}, 30, 5, BLACK);
}

void Button::run(bool &clicked)
{
    if (CheckCollisionPointRec(GetMousePosition(), box))
    {
        background = highlight;
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            clicked = true;
        }
    }
    else
        background = normal;

    render();
}

void LoginScene(DanhSachLopHoc &dslh, SinhVien *&sv)
{
    const int MAIN_SV_WIDTH = 480;
    const int MAIN_SV_HEIGHT = 640;
    SetWindowSize(MAIN_SV_WIDTH, MAIN_SV_HEIGHT);
    SetWindowPosition(GetMonitorWidth(0) / 2 - MAIN_SV_WIDTH / 2, GetMonitorHeight(0) / 2 - MAIN_SV_HEIGHT / 2);
    Texture2D login_logo = LoadTextureFromImage(logo);

    Text ID("ID:", BLACK, BLACK, font, 30, 5);
    Text Pass("Pass:", BLACK, BLACK, font, 30, 5);
    InputBox box1(Rectangle{(WIDTH / 2 - 225 / 2) - 8, (HEIGHT / 2 - 50 / 2) + 100, 240, 50}, BLACK, BLACK, WHITE, font);
    InputBox box2(Rectangle{(WIDTH / 2 - 225 / 2) - 8, (HEIGHT / 2 - 50 / 2) + 160, 240, 50}, BLACK, BLACK, WHITE, font, true);
    Button button1(Rectangle{(WIDTH / 2 - 150 / 2), (HEIGHT / 2 - 50 / 2) + 220, 150, 50}, "login", LIGHTGRAY, GRAY, font);

    bool is_button1_clicked = false;
    bool is_error = false;

    while (!is_close_icon_pressed)
    {
        BeginDrawing();
        ClearBackground(WHITE);
        DrawTexture(login_logo, MidCenter().x - 250 / 2 + 5, MidCenter().y - 50 / 2 - 240 + 50, WHITE);
        ID.render(MidCenter, -160, 100);
        Pass.render(MidCenter, -175, 160);
        box1.run(global_mouse_pos);
        box2.run(global_mouse_pos);
        button1.run(is_button1_clicked);
        if (is_error)
        {
            string err;
            if (box1.text.data == "" || box2.text.data == "")
            {
                err = "ID or Password is empty!";
            }
            else
            {
                err = "ID or Password is wrong!";
            }
            Rectangle close_button = Popup(err, "Error");
            if (CheckCollisionPointRec(global_mouse_pos, close_button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                is_error = false;
                box1.text.data = "";
                box2.text.data = "";
                box2.secure_text.data = "";
                box1.text.text_layout.clear();
                box2.text.text_layout.clear();
                box2.secure_text.text_layout.clear();
            }
        }
        EndDrawing();

        if (is_button1_clicked)
        {
            int res = login(dslh, sv, box1.text.data, box2.text.data);
            if (res == 2)
            {
                scene_stack.push(current_scene);
                scene_stack.push(Main_GV);
                break;
            }
            else if (res == 1)
            {
                scene_stack.push(current_scene);
                scene_stack.push(Main_SV);
                break;
            }
            else
            {
                is_error = true;
            }
        }

        if (IsKeyPressed(KEY_ESCAPE))
        {
            break;
        }

        if (WindowShouldClose())
            is_close_icon_pressed = true;

        is_button1_clicked = false;
    }

    UnloadTexture(login_logo);
}

Rectangle Popup(std::string message, std::string lable)
{
    Vector2 measure_msg = MeasureTextEx(font, (char *)message.c_str(), 30, 5);
    Vector2 measure_lable = MeasureTextEx(font, (char *)lable.c_str(), 30, 5);
    // measure.x = MidCenter().x - measure.x / 2;
    // measure.y = MidCenter().y - measure.y / 4;
    Rectangle main_popup = {MidCenter().x - 400 / 2, MidCenter().y - 300 / 2, 400, 300};
    Rectangle close_button = {main_popup.x + 350, main_popup.y, 50, 50};
    DrawRectangleRec(main_popup, WHITE);
    DrawRectangleRec(close_button, RED);
    DrawTextEx(font, "X", {close_button.x + 15, close_button.y + 10}, 30, 5, WHITE);
    DrawRectangleLines(main_popup.x, main_popup.y, main_popup.width, main_popup.height, BLACK);
    DrawLine(main_popup.x, main_popup.y + 50, main_popup.x + main_popup.width, main_popup.y + 50, BLACK);
    DrawLine(main_popup.x + 350, main_popup.y, main_popup.x + 350, main_popup.y + 50, BLACK);
    DrawTextEx(font, (char *)message.c_str(), Vector2{main_popup.x + main_popup.width / 2 - measure_msg.x / 2, main_popup.y + main_popup.height / 2 - measure_msg.y / 2}, 30, 5, BLACK);
    DrawTextEx(font, (char *)lable.c_str(), Vector2{main_popup.x + main_popup.width / 2 - measure_lable.x / 2, (main_popup.y * 2 + 50) / 2 - measure_lable.y / 2}, 30, 5, BLACK);

    return close_button;
}

void DrawTextBoxed(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint)
{
    DrawTextBoxedSelectable(font, text, rec, fontSize, spacing, wordWrap, tint, 0, 0, WHITE, WHITE);
}

void DrawTextBoxedSelectable(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint, int selectStart, int selectLength, Color selectTint, Color selectBackTint)
{
    int length = TextLength(text); // Total length in bytes of the text, scanned by codepoints in loop

    float textOffsetY = 0;    // Offset between lines (on line break '\n')
    float textOffsetX = 0.0f; // Offset X to next character to draw

    float scaleFactor = fontSize / (float)font.baseSize; // Character rectangle scaling factor

    // Word/character wrapping mechanism variables
    enum
    {
        MEASURE_STATE = 0,
        DRAW_STATE = 1
    };
    int state = wordWrap ? MEASURE_STATE : DRAW_STATE;

    int startLine = -1; // Index where to begin drawing (where a line begins)
    int endLine = -1;   // Index where to stop drawing (where a line ends)
    int lastk = -1;     // Holds last value of the character position

    for (int i = 0, k = 0; i < length; i++, k++)
    {
        // Get next codepoint from byte string and glyph index in font
        int codepointByteCount = 0;
        int codepoint = GetCodepoint(&text[i], &codepointByteCount);
        int index = GetGlyphIndex(font, codepoint);

        // NOTE: Normally we exit the decoding sequence as soon as a bad byte is found (and return 0x3f)
        // but we need to draw all of the bad bytes using the '?' symbol moving one byte
        if (codepoint == 0x3f)
            codepointByteCount = 1;
        i += (codepointByteCount - 1);

        float glyphWidth = 0;
        if (codepoint != '\n')
        {
            glyphWidth = (font.glyphs[index].advanceX == 0) ? font.recs[index].width * scaleFactor : font.glyphs[index].advanceX * scaleFactor;

            if (i + 1 < length)
                glyphWidth = glyphWidth + spacing;
        }

        // NOTE: When wordWrap is ON we first measure how much of the text we can draw before going outside of the rec container
        // We store this info in startLine and endLine, then we change states, draw the text between those two variables
        // and change states again and again recursively until the end of the text (or until we get outside of the container).
        // When wordWrap is OFF we don't need the measure state so we go to the drawing state immediately
        // and begin drawing on the next line before we can get outside the container.
        if (state == MEASURE_STATE)
        {
            // TODO: There are multiple types of spaces in UNICODE, maybe it's a good idea to add support for more
            // Ref: http://jkorpela.fi/chars/spaces.html
            if ((codepoint == ' ') || (codepoint == '\t') || (codepoint == '\n'))
                endLine = i;

            if ((textOffsetX + glyphWidth) > rec.width)
            {
                endLine = (endLine < 1) ? i : endLine;
                if (i == endLine)
                    endLine -= codepointByteCount;
                if ((startLine + codepointByteCount) == endLine)
                    endLine = (i - codepointByteCount);

                state = !state;
            }
            else if ((i + 1) == length)
            {
                endLine = i;
                state = !state;
            }
            else if (codepoint == '\n')
                state = !state;

            if (state == DRAW_STATE)
            {
                textOffsetX = 0;
                i = startLine;
                glyphWidth = 0;

                // Save character position when we switch states
                int tmp = lastk;
                lastk = k - 1;
                k = tmp;
            }
        }
        else
        {
            if (codepoint == '\n')
            {
                if (!wordWrap)
                {
                    textOffsetY += (font.baseSize + font.baseSize / 2) * scaleFactor;
                    textOffsetX = 0;
                }
            }
            else
            {
                if (!wordWrap && ((textOffsetX + glyphWidth) > rec.width))
                {
                    textOffsetY += (font.baseSize + font.baseSize / 2) * scaleFactor;
                    textOffsetX = 0;
                }

                // When text overflows rectangle height limit, just stop drawing
                if ((textOffsetY + font.baseSize * scaleFactor) > rec.height)
                    break;

                // Draw selection background
                bool isGlyphSelected = false;
                if ((selectStart >= 0) && (k >= selectStart) && (k < (selectStart + selectLength)))
                {
                    DrawRectangleRec((Rectangle){rec.x + textOffsetX - 1, rec.y + textOffsetY, glyphWidth, (float)font.baseSize * scaleFactor}, selectBackTint);
                    isGlyphSelected = true;
                }

                // Draw current character glyph
                if ((codepoint != ' ') && (codepoint != '\t'))
                {
                    DrawTextCodepoint(font, codepoint, (Vector2){rec.x + textOffsetX, rec.y + textOffsetY}, fontSize, isGlyphSelected ? selectTint : tint);
                }
            }

            if (wordWrap && (i == endLine))
            {
                textOffsetY += (font.baseSize + font.baseSize / 2) * scaleFactor;
                textOffsetX = 0;
                startLine = endLine;
                endLine = -1;
                glyphWidth = 0;
                selectStart += lastk - k;
                k = lastk;

                state = !state;
            }
        }

        if ((textOffsetX != 0) || (codepoint != ' '))
            textOffsetX += glyphWidth; // avoid leading spaces
    }
}

void MainSceneSV(SinhVien *&sv, DanhSachMonHoc &dsmh, DanhSachCauHoi &dsch, std::string &testing_subject, std::string &time_testing, std::string &number_of_question, CauHoi **&ptr)
{
    const int MAIN_SV_WIDTH = 1500;
    const int MAIN_SV_HEIGHT = 800;
    std::string msg = "Xin hãy nhập số";
    int page = 1;
    int index = 0;
    int page_in_da_thi = 1;
    SetWindowSize(MAIN_SV_WIDTH, MAIN_SV_HEIGHT);
    SetWindowPosition(GetMonitorWidth(0)/2 - MAIN_SV_WIDTH/2, GetMonitorHeight(0)/2 - MAIN_SV_HEIGHT/2);
    InputBox input_ma_mon{{25, 25, 200, 50}, BLACK, BLUE, LIGHTGRAY, font, false};
    InputBox box_thoi_gian_thi(Rectangle{MidCenter().x - 400 / 2 + 180, MidCenter().y - 300 / 2 + 110, 200, 50}, BLACK, BLACK, WHITE, font);
    InputBox box_so_cau_hoi(Rectangle{MidCenter().x - 400 / 2 + 180, MidCenter().y - 300 / 2 + 170, 200, 50}, BLACK, BLACK, WHITE, font);
    Rectangle box = {5, 100, 1490, 600};
    Button next{{BotRight().x - 110, BotRight().y - 90, 100, 50}, "Next", WHITE, BLUE, font};
    Button prev{{BotRight().x - 220, BotRight().y - 90, 100, 50}, "Prev", WHITE, BLUE, font};
    Button next_in_da_thi{{BotRight().x - 110, BotRight().y - 90, 100, 50}, "Next", WHITE, BLUE, font};
    Button prev_in_da_thi{{BotRight().x - 220, BotRight().y - 90, 100, 50}, "Prev", WHITE, BLUE, font};
    Button testing_button{{MidCenter().x - 100/2, MidCenter().y - 300 / 2 + 230, 100, 50}, "Thi", WHITE, BLUE, font};

    bool is_button_next_pressed = false;
    bool is_button_prev_pressed = false;
    bool press_flag_chua_thi = false;
    bool back_button_in_da_thi = false;
    bool is_button_next_pressed_da_thi = false;
    bool is_button_prev_pressed_da_thi = false;
    int ma_cau_hoi_duoc_chon;
    string ma_mon_duoc_chon = "";
    string so_cau_thi = "";
    string thoi_gian_thi = "";
    int scene = 1;

    global_mouse_pos = GetMousePosition();
    int offset = 0;
    bool error = false;

    while(!is_close_icon_pressed){

        BeginDrawing();
            ClearBackground(WHITE);

            switch(scene){
            case 1:
            {
                if(!press_flag_chua_thi){
                    next.run(is_button_next_pressed);
                    prev.run(is_button_prev_pressed);
                }
                else{
                    next.render();
                    prev.render();
                }

                if (is_button_next_pressed && page < (dsmh.getLength() - offset) / 7 + 1)
                    page++;
                if (is_button_prev_pressed && page > 1)
                    page--;

                DrawTextEx(font, std::to_string(page).c_str(), {(BotRight().x - 110 + BotRight().x - 220) / 2 - (MeasureTextEx(font, std::to_string(page).c_str(), 30, 3).x / 2) - 100, BotRight().y - 90 + 10}, 30, 3, BLACK);
                DrawTextEx(font, "/", {(BotRight().x - 110 + BotRight().x - 220) / 2 - (MeasureTextEx(font, "/", 30, 3).x / 2) - 87.5, BotRight().y - 90 + 10}, 30, 3, BLACK);
                DrawTextEx(font, std::to_string((dsmh.getLength() - offset) / 7 + 1).c_str(), {(BotRight().x - 110 + BotRight().x - 220) / 2 - (MeasureTextEx(font, std::to_string((dsmh.getLength() - offset) / 7 + 1).c_str(), 30, 3).x / 2) - 75, BotRight().y - 90 + 10}, 30, 3, BLACK);

                index = 7 * (page - 1);
                offset = 0;

                if(!press_flag_chua_thi)
                    input_ma_mon.run(global_mouse_pos);
                else{
                    input_ma_mon.render();
                }
                if (CheckCollisionPointRec(GetMousePosition(), input_ma_mon.box) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    page = 1;
                }
                DrawRectangleLinesEx(input_ma_mon.box, 3, BLACK);
                DrawRectangleLinesEx(box, 3, BLACK);
                if (input_ma_mon.text.data == "" && !input_ma_mon.clicked)
                    DrawTextEx(font, "Tìm kiếm", (Vector2){input_ma_mon.box.x + 50, input_ma_mon.box.y + 10}, 30, 0, BLACK);

                for (int i = 0; i < 8; i++)
                {
                    DrawLineEx(Vector2{5, 175 + 75 * i}, Vector2{1490, 175 + 75 * i}, 2, BLACK);
                }

                DrawTextEx(font, "STT", {(5 + 100) / 2 - (MeasureTextEx(font, "STT", 30, 3).x / 2), 124}, 30, 3, BLACK);
                DrawTextEx(font, "Mã môn", {(100 + 273) / 2 - (MeasureTextEx(font, "Mã môn", 30, 3).x / 2), 124}, 30, 3, BLACK);
                DrawTextEx(font, "Tên môn học", {786 - (MeasureTextEx(font, "Tên môn học", 30, 3).x / 2), 124}, 30, 3, BLACK);
                DrawTextEx(font, "Điểm", {(1300 + 1490) / 2 - (MeasureTextEx(font, "Điểm", 30, 3).x / 2), 124}, 30, 3, BLACK);
                DrawLineEx({100, 100}, {100, 700}, 2, BLACK);
                DrawLineEx({273, 100}, {273, 700}, 2, BLACK);
                DrawLineEx({1300, 100}, {1300, 700}, 2, BLACK);

                for (int place = 0; index < dsmh.getLength(); index++)
                {
                    if (input_ma_mon.text.data != "")
                    {
                        if (strstr(dsmh[index].ma_mon_hoc, input_ma_mon.text.data.c_str()) == nullptr &&
                            strstr(standardization(dsmh[index].ten_mon_hoc).c_str(), standardization(input_ma_mon.text.data).c_str()) == nullptr)
                        {
                            offset++;
                            continue;
                        }
                    }

                    if (place > 6)
                        place = 10000;

                    Rectangle rec = {5, 175 + 75 * place, 1490, 75};
                    DTPtr dt = nullptr;
                    dt = (*sv->diem)[string(dsmh[index].ma_mon_hoc)];
                    std::string output_mark;
                    if (dt == nullptr)
                    {
                        output_mark = "Chưa thi";
                    }
                    else
                    {
                        output_mark = std::to_string(dt->data.Diem).substr(0, std::to_string(dt->data.Diem).find(".") + 2);
                    }
                    DrawTextEx(font, std::to_string(index + 1 - offset).c_str(), {(5 + 100) / 2 - (MeasureTextEx(font, std::to_string(index + 1 - offset).c_str(), 30, 3).x / 2), 199 + 75 * (place)}, 30, 3, BLACK);
                    DrawTextEx(font, dsmh[index].ma_mon_hoc, Vector2{100 + 5, 199 + 75 * (place)}, 30, 3, BLACK);
                    DrawTextEx(font, (char *)dsmh[index].ten_mon_hoc.c_str(), Vector2{273 + 5, 199 + 75 * (place)}, 30, 3, BLACK);
                    DrawTextEx(font, (char *)output_mark.c_str(), Vector2{(1490 + 1300) / 2 - MeasureTextEx(font, (char *)output_mark.c_str(), 30, 3).x / 2, 199 + 75 * (place)}, 30, 3, BLACK);

                    if (CheckCollisionPointRec(GetMousePosition(), rec) && !press_flag_chua_thi)
                    {
                        DrawRectangleLinesEx(rec, 3, GREEN);

                        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                        {
                            if (output_mark == "Chưa thi")
                                press_flag_chua_thi = true;
                            else
                                scene = 2;
                            ma_mon_duoc_chon = dsmh[index].ma_mon_hoc;
                        }
                    }

                    place++;
                }

                if (press_flag_chua_thi)
                {
                    bool I_WANT_TO_TEST = false;
                    Vector2 measure_lable = MeasureTextEx(font, (char *)ma_mon_duoc_chon.c_str(), 30, 5);
                    Rectangle main_popup = {MidCenter().x - 400 / 2, MidCenter().y - 300 / 2, 400, 300};
                    Rectangle close_button = {main_popup.x + 350, main_popup.y, 50, 50};
                    DrawRectangleRec(main_popup, WHITE);
                    DrawRectangleRec(close_button, RED);
                    DrawTextEx(font, "X", {close_button.x + 15, close_button.y + 10}, 30, 5, WHITE);
                    DrawRectangleLinesEx(main_popup, 2, BLACK);
                    DrawLine(main_popup.x, main_popup.y + 50, main_popup.x + main_popup.width, main_popup.y + 50, BLACK);
                    DrawLine(main_popup.x + 350, main_popup.y, main_popup.x + 350, main_popup.y + 50, BLACK);
                    // DrawTextEx(font, (char *)message.c_str(), Vector2{main_popup.x + main_popup.width / 2 - measure_msg.x / 2, main_popup.y + main_popup.height / 2 - measure_msg.y / 2}, 30, 5, BLACK);
                    DrawTextEx(font, (char *)ma_mon_duoc_chon.c_str(), Vector2{main_popup.x + main_popup.width / 2 - measure_lable.x / 2, (main_popup.y * 2 + 50) / 2 - measure_lable.y / 2}, 30, 5, BLACK);

                    DrawTextEx(font, "Thời gian:", Vector2{main_popup.x + 20, main_popup.y + 120}, 30, 5, BLACK);
                    DrawTextEx(font, "Số câu:", Vector2{main_popup.x + 20, main_popup.y + 180}, 30, 5, BLACK);

                    box_thoi_gian_thi.run(global_mouse_pos);
                    box_so_cau_hoi.run(global_mouse_pos);
                    testing_button.run(I_WANT_TO_TEST);

                    if(I_WANT_TO_TEST){
                        if(is_number(box_so_cau_hoi.text.data) && is_number(box_thoi_gian_thi.text.data)){
                            testing_subject = ma_mon_duoc_chon;
                            time_testing = box_thoi_gian_thi.text.data;
                            number_of_question = box_so_cau_hoi.text.data;

                            DArray<CauHoi *> list_cau_hoi_thi;
                            dsch.getQuestionList(list_cau_hoi_thi, testing_subject);
                            CauHoi **cau_hoi_duoc_chon = new CauHoi *[stoi(number_of_question)];
                            srand(time(0));
                            if (stoi(time_testing) < 1 || stoi(time_testing) > 60)
                            {
                                error = true;
                                msg = "Thời gian không hợp lệ";
                            }
                            else if (list_cau_hoi_thi.size() < stoi(number_of_question))
                            {
                                error = true;
                                msg = "Số câu hỏi không đủ";
                            }
                            else
                            {
                                for (int i = 0; i < stoi(number_of_question); i++)
                                {
                                    int random_index = rand() % list_cau_hoi_thi.size();
                                    cau_hoi_duoc_chon[i] = list_cau_hoi_thi[random_index];
                                    list_cau_hoi_thi.remove(random_index);
                                }
                                is_close_icon_pressed = true;
                                scene_stack.push(Testing);
                                ptr = cau_hoi_duoc_chon;
                            }
                        }
                        else{
                            msg = "Xin nhập số";
                            error = true;
                        }
                    }

                    if (CheckCollisionPointRec(global_mouse_pos, close_button) && !error)
                    {
                        DrawRectangleRec(close_button, Fade(RED, 0.5f));

                        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                        {
                            press_flag_chua_thi = false;
                        }
                    }

                    if(error){
                        Rectangle close_button = Popup(msg, "Error");

                        if(CheckCollisionPointRec(global_mouse_pos, close_button)){
                            DrawRectangleRec(close_button, Fade(RED, 0.5f));

                            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                                error = false;
                            }
                        }
                    }
                }
                break;
            }
            case 2:
            {
                Rectangle scene_when_press_da_thi = {0, 0, MAIN_SV_WIDTH, MAIN_SV_HEIGHT};
                DrawRectangleRec(scene_when_press_da_thi, WHITE);

                int count = 0;
                int start = -1;
                for (int i = 0; i < sv->question_id.size(); i++)
                {
                    if (string(dsch.getQuestion(sv->question_id[i]).ma_mon_hoc) == ma_mon_duoc_chon)
                    {
                        count++;
                        if (start == -1)
                            start = i;
                    }
                }

                Button back_button = {{5, 5, 100, 50}, "Back", WHITE, GRAY, font};

                DrawRectangleLinesEx(box, 3, BLACK);
                for (int i = 0; i < 8; i++)
                {
                    DrawLineEx(Vector2{5, 175 + 75 * i}, Vector2{1490, 175 + 75 * i}, 2, BLACK);
                }

                back_button.run(back_button_in_da_thi);
                next_in_da_thi.run(is_button_next_pressed_da_thi);
                prev_in_da_thi.run(is_button_prev_pressed_da_thi);

                if (is_button_next_pressed_da_thi && page_in_da_thi < (count) / 7 + 1)
                    page_in_da_thi++;
                if (is_button_prev_pressed_da_thi && page_in_da_thi > 1)
                    page_in_da_thi--;


                DrawTextEx(font, std::to_string(page_in_da_thi).c_str(), {(BotRight().x - 110 + BotRight().x - 220) / 2 - (MeasureTextEx(font, std::to_string(page).c_str(), 30, 3).x / 2) - 100, BotRight().y - 90 + 10}, 30, 3, BLACK);
                DrawTextEx(font, "/", {(BotRight().x - 110 + BotRight().x - 220) / 2 - (MeasureTextEx(font, "/", 30, 3).x / 2) - 87.5, BotRight().y - 90 + 10}, 30, 3, BLACK);
                DrawTextEx(font, std::to_string((count) / 7 + 1).c_str(), {(BotRight().x - 110 + BotRight().x - 220) / 2 - (MeasureTextEx(font, std::to_string((dsmh.getLength() - offset) / 7 + 1).c_str(), 30, 3).x / 2) - 75, BotRight().y - 90 + 10}, 30, 3, BLACK);

                DrawTextEx(font, "STT", {(5 + 100) / 2 - (MeasureTextEx(font, "STT", 30, 3).x / 2), 124}, 30, 3, BLACK);
                DrawTextEx(font, "ID", {(100 + 273) / 2 - (MeasureTextEx(font, "ID", 30, 3).x / 2), 124}, 30, 3, BLACK);
                DrawTextEx(font, "Câu hỏi", {881 - (MeasureTextEx(font, "Câu hỏi", 30, 3).x / 2), 124}, 30, 3, BLACK);
                // DrawTextEx(font, "Điểm", {(1300 + 1490)/2 - (MeasureTextEx(font, "Điểm", 30, 3).x / 2), 124}, 30, 3, BLACK);
                DrawLineEx({100, 100}, {100, 700}, 2, BLACK);
                DrawLineEx({273, 100}, {273, 700}, 2, BLACK);
                // DrawLineEx({1300, 100}, {1300, 700}, 2, BLACK);

                for (int place = 0, index = 7 * (page_in_da_thi - 1), tempo = start + 7 * (page_in_da_thi - 1); tempo < count + start; tempo++, index++)
                {
                    // if ((tempo / ((start + 7))) == page_in_da_thi - 1)
                    // {
                        if (place > 6)
                            place = 10000;

                        Rectangle rec = {5, 175 + 75 * place, 1490, 75};
                        std::string question = dsch.getQuestion(sv->question_id[tempo]).noi_dung;

                        for (int i = 1; i < question.length(); i++)
                        {
                            if (MeasureTextEx(font, question.substr(0, i).c_str(), 30, 3).x + 273 > 1450)
                            {
                                question = question.substr(0, i - 1);
                                question += "...";
                                break;
                            }
                        }

                        DrawTextEx(font, std::to_string(index + 1).c_str(), {(5 + 100) / 2 - (MeasureTextEx(font, std::to_string(index + 1).c_str(), 30, 3).x / 2), 199 + 75 * (place)}, 30, 3, BLACK);
                        DrawTextEx(font, std::to_string(sv->question_id[tempo]).c_str(), Vector2{100 + 5, 199 + 75 * (place)}, 30, 3, BLACK);
                        DrawTextEx(font, question.c_str(), Vector2{273 + 5, 199 + 75 * (place)}, 30, 3, BLACK);

                        if (CheckCollisionPointRec(GetMousePosition(), rec))
                        {
                            DrawRectangleLinesEx(rec, 3, GREEN);

                            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                            {
                                scene = 3;
                                ma_cau_hoi_duoc_chon = sv->question_id[tempo];
                            }
                        }

                        place++;
                    // }
                }

                if (back_button_in_da_thi)
                {
                    back_button_in_da_thi = false;
                    scene = 1;
                }
                break;
            }
            case 3:
            {
                CauHoi question = dsch.getQuestion(ma_cau_hoi_duoc_chon);
                Rectangle detail_question_box = {0, 0, 1500, 800};
                DrawRectangleRec(detail_question_box, WHITE);

                bool close_detail_question = false;
                Button back_in_detail_question = Button({BotCenter().x - 200 / 2, BotCenter().y - 50 / 2 - 50, 200, 50}, "Back", WHITE, GRAY, font);
                back_in_detail_question.run(close_detail_question);

                Vector2 starting_point = MeasureTextEx(font, (std::to_string(question.Id) + ".").c_str(), 30, 3);
                DrawTextEx(font, (std::to_string(question.Id) + ".").c_str(), {25, 25}, 30, 3, BLACK);
                DrawRectangleLinesEx({23, 23, 1490 - 23, 800 - 30}, 2, BLACK);
                DrawTextBoxed(font, question.noi_dung.c_str(), {starting_point.x + 25 + 5, 25, 1490 - starting_point.x - 10, 150}, 30, 3, true, BLACK);

                DrawTextEx(font, "A. ", {starting_point.x - 10, 150 + 25 + 5}, 30, 3, BLACK);
                DrawTextBoxed(font, question.dap_an_a.c_str(), {starting_point.x + 25 + 5, 150 + 25 + 5, 1490 - starting_point.x - 10, 150}, 30, 3, false, BLACK);

                DrawTextEx(font, "B. ", {starting_point.x - 10, 300 + 25 + 5}, 30, 3, BLACK);
                DrawTextBoxed(font, question.dap_an_b.c_str(), {starting_point.x + 25 + 5, 300 + 25 + 5, 1490 - starting_point.x - 10, 150}, 30, 3, false, BLACK);

                DrawTextEx(font, "C. ", {starting_point.x - 10, 450 + 25 + 5}, 30, 3, BLACK);
                DrawTextBoxed(font, question.dap_an_c.c_str(), {starting_point.x + 25 + 5, 450 + 25 + 5, 1490 - starting_point.x - 10, 150}, 30, 3, false, BLACK);

                DrawTextEx(font, "D. ", {starting_point.x - 10, 600 + 25 + 5}, 30, 3, BLACK);
                DrawTextBoxed(font, question.dap_an_d.c_str(), {starting_point.x + 25 + 5, 600 + 25 + 5, 1490 - starting_point.x - 10, 150}, 30, 3, false, BLACK);

                if (close_detail_question)
                {
                    close_detail_question = false;
                    scene = 2;
                }
                break;
            }
            }
 
        EndDrawing();

        if (WindowShouldClose())
        {
            is_close_icon_pressed = true;
        }

        if(IsKeyPressed(KEY_ESCAPE)){
            break;
        }

        is_button_next_pressed = false;
        is_button_prev_pressed = false;
        is_button_next_pressed_da_thi = false;
        is_button_prev_pressed_da_thi = false;
    }
}

void TestingScene(SinhVien *&sv, DanhSachMonHoc &dsmh, CauHoi **&ptr, std::string &ma_mon_thi, std::string &time, std::string &number_of_question){
    SetWindowSize(1500, 800);

    time = standardization(time);
    number_of_question = standardization(number_of_question);
    is_close_icon_pressed = false;
    int second = stoi(time) * 60;

    int index = 0;
    int count = 0;

    Button test_button = {{MidCenter().x - 200 / 2, MidCenter().y + 100, 200, 50}, "Bắt đầu thi", WHITE, GRAY, font};
    bool is_test_button_pressed = false;
    int result[stoi(number_of_question)];
    for(int i = 0; i<stoi(number_of_question); i++){
        result[i] = -1;
    }

    while(!is_close_icon_pressed){
        BeginDrawing();
            ClearBackground(WHITE);

            if(!is_test_button_pressed){
                DrawTextEx(font, ("Mã môn thi: " + ma_mon_thi).c_str(), {MidCenter().x - MeasureTextEx(font, ("Mã môn thi: " + ma_mon_thi).c_str(), 30, 3).x / 2, MidCenter().y - 100}, 30, 3, BLACK);
                DrawTextEx(font, ("Tên môn thi: " + dsmh[(char *)ma_mon_thi.c_str()].ten_mon_hoc).c_str(), {MidCenter().x - MeasureTextEx(font, ("Tên môn thi: " + dsmh[(char *)ma_mon_thi.c_str()].ten_mon_hoc).c_str(), 30, 3).x / 2, MidCenter().y - 50}, 30, 3, BLACK);
                DrawTextEx(font, ("Thời gian: " + time + " phút").c_str(), {MidCenter().x - MeasureTextEx(font, ("Thời gian: " + time + "phút").c_str(), 30, 3).x / 2, MidCenter().y}, 30, 3, BLACK);
                DrawTextEx(font, ("Số câu thi: " + number_of_question).c_str(), {MidCenter().x - MeasureTextEx(font, ("Số câu thi: " + number_of_question).c_str(), 30, 3).x / 2, MidCenter().y + 50}, 30, 3, BLACK);
                test_button.run(is_test_button_pressed);
            }
            else{
                Rectangle detail_question_box = {0, 0, 1500, 800};
                DrawRectangleRec(detail_question_box, WHITE);

                bool next_question = false;
                bool prev_question = false;
                bool end_test = false;
                Button next_question_in_thi = Button({BotCenter().x - 200 / 2 + 150, BotCenter().y - 50 / 2 - 50, 200, 50}, "Next", WHITE, GRAY, font);
                Button prev_question_in_thi = Button({BotCenter().x - 200 / 2 - 150, BotCenter().y - 50 / 2 - 50, 200, 50}, "Back", WHITE, GRAY, font);
                Button end_test_in_thi = Button({BotRight().x - 200 / 2 - 150, BotCenter().y - 50 / 2 - 50, 200, 50}, "End", WHITE, GRAY, font);
                next_question_in_thi.run(next_question);
                prev_question_in_thi.run(prev_question);

                DrawRectangleLinesEx({23 + 10, BotLeft().y - 60 - 23, 100, 50}, 2, BLACK);
                DrawTextEx(font, TextFormat("%i:%i", second / 60, second % 60), {23 + 10 + 100/2 - MeasureTextEx(font, TextFormat("%i:%i", second / 60, second % 60), 30, 3).x/2, BotLeft().y - 60 - 23 + 50/2 - 15}, 30, 3, BLACK);
                if(count % 12 == 0){
                    second--;
                    count = 0;
                }
                count++;

                if(second == 0){
                    end_test = true;
                }

                if(next_question){
                    index++;
                    if(index == std::stoi(number_of_question)){
                        index = 0;
                    }
                    next_question = false;
                }
                if(prev_question){
                    index--;
                    if(index == -1){
                        index = std::stoi(number_of_question) - 1;
                    }
                    prev_question = false;
                }

                if(index == std::stoi(number_of_question) - 1){
                    end_test_in_thi.run(end_test);

                    if(end_test){
                        end_test = false;
                        float diem_thi = 0;
                        for(int i = 0; i < std::stoi(number_of_question); i++){
                            int dap_an_dung;
                            if(ptr[i]->dap_an == "A"){
                                dap_an_dung = 0;
                            }
                            else if(ptr[i]->dap_an == "B"){
                                dap_an_dung = 1;
                            }
                            else if(ptr[i]->dap_an == "C"){
                                dap_an_dung = 2;
                            }
                            else if(ptr[i]->dap_an == "D"){
                                dap_an_dung = 3;
                            }
                            if(result[i] == dap_an_dung){
                                diem_thi += 10.0f / std::stoi(number_of_question);
                            }

                            sv->question_id.push_back(ptr[i]->Id);
                        }
                        sv->diem->insertOrderDT(DiemThi{(char*)ma_mon_thi.c_str(), diem_thi});
                        scene_stack.push(Main_SV);
                        break;
                    }
                }


                CauHoi question = *ptr[index];
                Vector2 starting_point = MeasureTextEx(font, (std::to_string(index + 1) + ".").c_str(), 30, 3);
                if(CheckCollisionPointRec(GetMousePosition(), {23, 125 + 25 + 5, 1490 - starting_point.x, 125}) || result[index] == 0){
                    DrawRectangle(23, 125 + 25 + 5, 1490 - 23, 125, GREEN);

                    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && result[index] != 0){
                        result[index] = 0;
                    }
                }
                if(CheckCollisionPointRec(GetMousePosition(), {23, 250 + 25 + 5, 1490 - starting_point.x, 125}) || result[index] == 1){
                    DrawRectangle(23, 250 + 25 + 5, 1490 -23, 125, GREEN);

                    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && result[index] != 1){
                        result[index] = 1;
                    }
                }
                if(CheckCollisionPointRec(GetMousePosition(), {23, 375 + 25 + 5, 1490 - starting_point.x, 125}) || result[index] == 2){
                    DrawRectangle(23, 375 + 25 + 5, 1490 - 23, 125, GREEN);

                    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && result[index] != 2){
                        result[index] = 2;
                    }
                }
                if(CheckCollisionPointRec(GetMousePosition(), {23, 500 + 25 + 5, 1490 - starting_point.x, 125}) || result[index] == 3){
                    DrawRectangle(23, 500 + 25 + 5, 1490 - 23, 125, GREEN);

                    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && result[index] != 3){
                        result[index] = 3;
                    }
                }
                DrawTextEx(font, (std::to_string(index + 1) + ".").c_str(), {25, 25}, 30, 3, BLACK);
                DrawTextBoxed(font, question.noi_dung.c_str(), {starting_point.x + 25 + 5, 25, 1490 - starting_point.x - 10, 125}, 30, 3, true, BLACK);
                DrawRectangleLinesEx({23, 23, 1490 - 23, 800 - 30}, 2, BLACK);
                DrawLineEx({23, 125 + 25 + 5}, {1490, 125 + 25 + 5}, 2, BLACK);

                DrawTextEx(font, "A. ", {23, 125 + 25 + 5}, 30, 3, BLACK);
                DrawTextBoxed(font, question.dap_an_a.c_str(), {starting_point.x + 25 + 10, 125 + 25 + 5, 1490 - starting_point.x - 10, 125}, 30, 3, false, BLACK);
                DrawLineEx({23, 125 + 25 + 5 + 125}, {1490, 125 + 25 + 5 + 125}, 2, BLACK);

                DrawTextEx(font, "B. ", {23, 250 + 25 + 5}, 30, 3, BLACK);
                DrawTextBoxed(font, question.dap_an_b.c_str(), {starting_point.x + 25 + 10, 250 + 25 + 5, 1490 - starting_point.x - 10, 125}, 30, 3, false, BLACK);
                DrawLineEx({23, 250 + 25 + 5 + 125}, {1490, 250 + 25 + 5 + 125}, 2, BLACK);


                DrawTextEx(font, "C. ", {23, 375 + 25 + 5}, 30, 3, BLACK);
                DrawTextBoxed(font, question.dap_an_c.c_str(), {starting_point.x + 25 + 10, 375 + 25 + 5, 1490 - starting_point.x - 10, 125}, 30, 3, false, BLACK);
                DrawLineEx({23, 375 + 25 + 5 + 125}, {1490, 375 + 25 + 5 + 125}, 2, BLACK);

                DrawTextEx(font, "D. ", {23, 500 + 25 + 5}, 30, 3, BLACK);
                DrawTextBoxed(font, question.dap_an_d.c_str(), {starting_point.x + 25 + 10, 500 + 25 + 5, 1490 - starting_point.x - 10, 125}, 30, 3, false, BLACK);
                DrawLineEx({23, 500 + 25 + 5 + 125}, {1490, 500 + 25 + 5 + 125}, 2, BLACK);

            }

        EndDrawing();

        if (WindowShouldClose())
        {
            is_close_icon_pressed = true;
        }

        if(IsKeyPressed(KEY_ESCAPE)){
            scene_stack.push(Main_SV);
            break;
        }
    }
}

void GiaoDienDanhSachSinhVien(DanhSachLopHoc &dslh, string a, DanhSachSinhVien &dsmh);
void inBangDiemDanhSachSinhVien(DanhSachMonHoc &dsmh, DanhSachLopHoc &dslh, string ma_lop_muon_in, string ma_mon ,float &cur_page);
void GiaoDienDanhSachLop(DanhSachLopHoc &dslh, DanhSachMonHoc &dsmh, DanhSachCauHoi &dsch)
{
    const int screenWidth = 1500;
    const int screenHeight = 800;
    int page = 1;
    int index = 0;
    //  int *nk = new int[MAX_DSL];
    DArray<bool> press_Another_Row_By_Row_Correction_Button;
    bool press_flag = false;
    bool press_Delete_Button = false;
    bool press_Correction_Button = false;
    bool press_Row_By_Row_Correction_Button = false;
    bool press_Class_To_See_Member_Button = false;
    bool press_Add_Save_Button = false;
    bool press_Delete_Save_Button = false;
    bool press_Are_You_Sure_Button = false;
    bool press_Correction_Save_Button = false;
    bool press_Student_List_Button = false;
    bool press_Menu_Button = false;
    bool press_Cau_Hoi_Button = false;
    string ma_lop = "";
    string ten_lop = "";
    string nien_khoa = "";
    float cur_page = 0, max_item = 10;
    InputBox box_ma_lop(Rectangle{500, 230, 250, 50}, BLACK, BLACK, WHITE, font);
    InputBox box_ten_lop(Rectangle{500, 320, 600, 50}, BLACK, BLACK, WHITE, font);
    InputBox box_nien_khoa(Rectangle{550, 410, 190, 50}, BLACK, BLACK, WHITE, font);
    InputBox box_ma_lop_khi_xoa(Rectangle{695, 320, 250, 50}, BLACK, BLACK, WHITE, font);
    InputBox box_in_danh_sach(Rectangle{10, 10, 320, 40}, BLACK, BLACK, WHITE, font);
    Button next{{850, 705, 180, 45}, "Next", WHITE, BLUE, font};
    Button prev{{480, 705, 180, 45}, "Prev", WHITE, BLUE, font};
    int offset = 0;
    global_mouse_pos = GetMousePosition();
    SetWindowPosition(GetMonitorWidth(0) / 2 - screenWidth / 2, GetMonitorHeight(0) / 2 - screenHeight / 2);
    SetWindowSize(screenWidth, screenHeight);
    bool is_button_next_pressed = false;
    bool is_button_prev_pressed = false;

    while (!is_close_icon_pressed)
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRectangle(0, 0, 1500, 100, Fade(GRAY, 0.5f));
        DrawRectangle(10, 10, 320, 40, WHITE); // Vẽô: Nhập từ khóa
        DrawRectangleLines(10, 10, 320, 40, BLACK);
        DrawRectangle(1350, 10, 120, 40, DARKBLUE); // Ô menu
        DrawRectangleLines(1350, 10, 120, 40, BLACK);
        DrawTextEx(font, "Môn Học", {1357.5, 15}, 30, 1, WHITE);
        DrawTextEx(font, "Nhập niên khóa ", {70, 15}, 30, 3, GRAY);
        for (int i = 0; i < 3; i++)
        {
            if (i != 2)
            {
                DrawRectangle(10 + (150 * i), 55, 80, 40, DARKBLUE); // ô thêm, xóa
                DrawRectangleLines(10 + (150 * i), 55, 80, 40, BLACK);
            }
            else if (i == 2)
            {
                DrawRectangle(10 + (150 * i), 55, 300, 40, DARKBLUE); // ô hiệu chỉnh
                DrawRectangleLines(10 + (150 * i), 55, 300, 40, BLACK);
            }
        }
        DrawTextEx(font, "Thêm", {13.5, 60}, 30, 3, WHITE);
        DrawTextEx(font, "Xóa", {174.5, 60}, 30, 3, WHITE);
        DrawTextEx(font, "Hiệu chỉnh thông tin", {325, 60}, 30, 3, WHITE);
        for (int j = 1; j < 13; j++)
        {
            if (j == 1)
            {
                for (int i = 0; i < 3; i++)
                {
                    DrawRectangle(0, 100, 80, 50, BLUE); // ô STT
                    DrawRectangleLines(0, 100 * j, 80, 50, BLACK);
                    if (i == 0)
                    { // mã lớp
                        DrawRectangle(80 + (260 * i), 100 * j, 260, 50, BLUE);
                        DrawRectangleLines(80 + (260 * i), 100 * j, 260, 50, BLACK);
                    }
                    else if (i == 1)
                    { // Tên lớp
                        DrawRectangle(80 + (260 * i), 100 * j, 700, 50, BLUE);
                        DrawRectangleLines(80 + (260 * i), 100 * j, 700, 50, BLACK);
                    }
                    else
                    { // Niên khóa
                        DrawRectangle(80 + (480 * i), 100 * j, 480, 50, BLUE);
                        DrawRectangleLines(80 + (480 * i), 100 * j, 480, 50, BLACK);
                    }
                }
            }
            else if (j >= 3)
            {
                for (int i = 0; i < 3; i++)
                {
                    if (i == 0)
                    { // mã lớp
                        DrawRectangleLines(0, 50 * j, 80, 50, BLACK);
                        DrawRectangleLines(80 + (480 * i), 50 * j, 260, 50, BLACK);
                    }
                    else if (i == 1)
                    { // tên lớp
                        DrawRectangleLines(80 + (260 * i), 50 * j, 700, 50, BLACK);
                    }
                    else
                    { // niên khóa
                        DrawRectangleLines(80 + (480 * i), 50 * j, 480, 50, BLACK);
                    }
                }
            }
        }

        DrawTextEx(font, "STT", {15, 110}, 30, 3, BLACK);
        DrawTextEx(font, "Mã lớp", {165, 110}, 30, 3, BLACK);
        DrawTextEx(font, "Tên lớp", {640, 110}, 30, 3, BLACK);
        DrawTextEx(font, "Niên khóa", {1210, 110}, 30, 3, BLACK);
        int count = 0;
        box_in_danh_sach.run(global_mouse_pos);
        next.run(is_button_next_pressed);
        prev.run(is_button_prev_pressed);
        if (is_button_next_pressed && page < (dslh.getSoLuong() - offset) / 10 + 1)
            page++;
        if (is_button_prev_pressed && page > 1)
            page--;
        index = 10 * (page - 1);
        offset = 0;
        if (CheckCollisionPointRec(GetMousePosition(), box_in_danh_sach.box) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            page = 1;
        }
        for (int place = 0; index < dslh.getSoLuong(); index++)
        {

            if (box_in_danh_sach.text.data != "")
            {
                if (strstr((char *)dslh[index]->getNienKhoa().c_str(), (char *)box_in_danh_sach.text.data.c_str()) == nullptr)
                {
                    offset++;
                    continue;
                }
            }

            if (place > 9)
                place = 100000;

            DrawTextEx(font, to_string(index + 1).c_str(), {30, 160 + 50 * place}, 30, 3, BLACK);                        // STT
            DrawTextEx(font, (char *)dslh[index]->getMaLop().c_str(), Vector2{90, 160 + 50 * place}, 30, 3, BLACK);      // Mã lớp
            DrawTextEx(font, (char *)dslh[index]->getTenLop().c_str(), Vector2{350, 160 + 50 * place}, 30, 3, BLACK);    // tên lớp
            DrawTextEx(font, (char *)dslh[index]->getNienKhoa().c_str(), Vector2{1200, 160 + 50 * place}, 30, 3, BLACK); // niên khóa

            if (CheckCollisionPointRec(GetMousePosition(), {80, 50 * (place + 3) - 500 * (place / 10), 260, 50}))
            {
                DrawRectangleLinesEx({80, 50 * (place + 3) - 500 * (place / 10), 260, 50}, 3, RED);
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !press_Student_List_Button)
                {
                    press_Student_List_Button = true;
                    ma_lop = dslh[index]->getMaLop();
                }
            }

            if (press_Student_List_Button)
            {
                GiaoDienDanhSachSinhVien(dslh, ma_lop, dsmh, dsch);
                if(IsKeyPressed(KEY_ESCAPE))
                {
                    press_Student_List_Button = false;
                }
                if (CheckCollisionPointRec(GetMousePosition(), {1350, 10, 120, 40}))
                {
                    DrawRectangleLinesEx({1350, 10, 120, 40}, 3, YELLOW);
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                    {
                        press_Student_List_Button = false;
                    }
                }
            }
            place++;
        }
        Rectangle rec4 = {310, 55, 300, 40};

        if (CheckCollisionPointRec(GetMousePosition(), rec4))
        {
            DrawRectangleLinesEx(rec4, 3, YELLOW);
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !press_Correction_Button)
            {
                press_Correction_Button = true;
            }
        }
        if (press_Correction_Button)
        {
            DrawRectangle(700, 55, 50, 40, RED);
            DrawTextEx(font, "X", {710, 60}, 30, 3, YELLOW);
            for (int place = 0, index = 0; index < dslh.getSoLuong(); index++)
            {
                if (place > 9)
                    place = 100000;
                Rectangle rec3 = {0, 50 * (place + 3), 80, 50};
                press_Another_Row_By_Row_Correction_Button.push_back(false);
                if (CheckCollisionPointRec(GetMousePosition(), rec3))
                {
                    DrawRectangleLinesEx(rec3, 3, YELLOW);
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !press_Another_Row_By_Row_Correction_Button[index])
                    {
                        press_Another_Row_By_Row_Correction_Button[index] = true;
                        ma_lop = dslh[place]->getMaLop();
                        ten_lop = dslh[place]->getTenLop();
                        nien_khoa = dslh[place]->getNienKhoa();
                    }
                }
                if (press_Another_Row_By_Row_Correction_Button[index])
                {
                    Rectangle main_popup = {350, 150, 800, 400};
                    Rectangle close_button = {main_popup.x + 750, main_popup.y, 50, 50};
                    Rectangle save_button = {700, 500, 100, 40};
                    DrawRectangleRec(main_popup, WHITE);
                    DrawRectangleRec(close_button, RED);
                    DrawRectangleRec(save_button, Fade(YELLOW, 0.5f));
                    DrawRectangleLinesEx(main_popup, 3, BLACK);
                    DrawRectangleLinesEx(close_button, 3, BLACK);
                    DrawRectangleLinesEx(save_button, 3, BLACK);
                    DrawTextEx(font, "X", {close_button.x + 15, close_button.y + 10}, 30, 5, WHITE);
                    DrawTextEx(font, "Mã lớp:", Vector2{main_popup.x + 20, main_popup.y + 90}, 30, 5, BLACK);
                    DrawTextEx(font, "Tên lớp:", Vector2{main_popup.x + 20, main_popup.y + 180}, 30, 5, BLACK);
                    DrawTextEx(font, "Niên khóa: ", Vector2{main_popup.x + 20, main_popup.y + 270}, 30, 5, BLACK);
                    DrawTextEx(font, "Lưu", Vector2{725, 505}, 30, 3, BLACK);
                    if (!box_ma_lop.clicked && box_ma_lop.text.data == "")
                        DrawTextEx(font, (char *)ma_lop.c_str(), Vector2{510, 240}, 30, 3, BLACK);
                    if (!box_ten_lop.clicked && box_ten_lop.text.data == "")
                        DrawTextEx(font, (char *)ten_lop.c_str(), Vector2{510, 330}, 30, 3, BLACK);
                    if (!box_nien_khoa.clicked && box_nien_khoa.text.data == "")
                        DrawTextEx(font, (char *)nien_khoa.c_str(), Vector2{560, 420}, 30, 3, BLACK);

                    box_ma_lop.run(global_mouse_pos);
                    box_ten_lop.run(global_mouse_pos);
                    box_nien_khoa.run(global_mouse_pos);
                    if (CheckCollisionPointRec(GetMousePosition(), save_button))
                    {
                        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !press_Correction_Save_Button)
                        {
                            press_Correction_Save_Button = true;
                        }
                    }
                    if (press_Correction_Save_Button)
                    {
                        if (box_ma_lop.text.data != "")
                        {
                            if (dslh.searchClass(box_ma_lop.text.data) == -1)
                            {
                                dslh[place]->setMaLop(box_ma_lop.text.data);
                                ma_lop = dslh[place]->getMaLop();
                            }
                            box_ma_lop.text.data = "";
                        }
                        if (box_ten_lop.text.data != "")
                        {
                            dslh[place]->setTenLop(box_ten_lop.text.data);
                            ten_lop = dslh[place]->getTenLop();
                            box_ten_lop.text.data = "";
                        }
                        if (box_nien_khoa.text.data != "")
                        {
                            dslh[place]->setNienKhoa(box_nien_khoa.text.data);
                            nien_khoa = dslh[place]->getNienKhoa();
                            box_nien_khoa.text.data = "";
                        }
                        if (CheckCollisionPointRec(global_mouse_pos, save_button))
                        {
                            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                            {
                                press_Correction_Save_Button = false;
                            }
                        }
                    }
                    if (CheckCollisionPointRec(global_mouse_pos, close_button))
                    {
                        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                        {
                            press_Another_Row_By_Row_Correction_Button[index] = false;
                        }
                    }
                }

                place++;
            }
            if (CheckCollisionPointRec(global_mouse_pos, {700, 55, 50, 40}))
            {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    press_Correction_Button = false;
                }
            }
        }

        Rectangle rec = {10, 55, 80, 40};
        if (CheckCollisionPointRec(GetMousePosition(), rec))
        {
            DrawRectangleLinesEx(rec, 3, YELLOW);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !press_flag)
            {
                press_flag = true;
            }
        }
        if (press_flag)
        {

            Rectangle main_popup = {350, 150, 800, 400};
            Rectangle close_button = {main_popup.x + 750, main_popup.y, 50, 50};
            Rectangle save_button = {700, 500, 100, 40};
            DrawRectangleRec(main_popup, WHITE);
            DrawRectangleRec(close_button, RED);
            DrawRectangleRec(save_button, Fade(YELLOW, 0.5f));
            DrawRectangleLinesEx(main_popup, 3, BLACK);
            DrawRectangleLinesEx(close_button, 3, BLACK);
            DrawRectangleLinesEx(save_button, 3, BLACK);
            DrawTextEx(font, "X", {close_button.x + 15, close_button.y + 10}, 30, 5, WHITE);
            DrawTextEx(font, "Mã lớp:", Vector2{main_popup.x + 20, main_popup.y + 90}, 30, 5, BLACK);
            DrawTextEx(font, "Tên lớp:", Vector2{main_popup.x + 20, main_popup.y + 180}, 30, 5, BLACK);
            DrawTextEx(font, "Niên khóa: ", Vector2{main_popup.x + 20, main_popup.y + 270}, 30, 5, BLACK);
            DrawTextEx(font, "Lưu", Vector2{725, 505}, 30, 3, BLACK);
            box_ma_lop.run(global_mouse_pos);
            box_ten_lop.run(global_mouse_pos);
            box_nien_khoa.run(global_mouse_pos);
            if (CheckCollisionPointRec(GetMousePosition(), save_button))
            {
                DrawRectangleLinesEx(save_button, 3, BLUE);
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !press_Add_Save_Button)
                {
                    press_Add_Save_Button = true;
                }
            }
            if (press_Add_Save_Button)
            {
                if (dslh.searchClass(box_ma_lop.text.data) < 0)
                {
                    if(box_ma_lop.text.data != "" && box_ten_lop.text.data != "" && box_nien_khoa.text.data != ""){
                    dslh.insert(new Lop{box_ma_lop.text.data, box_ten_lop.text.data, box_nien_khoa.text.data});
                    }
                }else{
                    DrawTextEx(font, "Mã lớp bị trùng", {MidCenter().x,MidCenter().y},30,3,RED);
                    
                    press_Add_Save_Button = false;
                }
                box_ma_lop.text.data = "";
                box_ten_lop.text.data = "";
                box_nien_khoa.text.data = "";
                press_Add_Save_Button = false;
                press_flag = false;
            }

            if (CheckCollisionPointRec(global_mouse_pos, close_button))
            {
                DrawRectangleRec(close_button, Fade(RED, 0.5f));
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    press_flag = false;
                }
            }
        }
        Rectangle rec2 = {160, 55, 80, 40};
        if (CheckCollisionPointRec(GetMousePosition(), rec2))
        {
            DrawRectangleLinesEx(rec2, 3, YELLOW);

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !press_Delete_Button)
            {
                press_Delete_Button = true;
            }
        }
        
        if (press_Delete_Button)
        {
            Rectangle main_popup = {570, 240, 375, 200};
            Rectangle close_button = {main_popup.x + 325, main_popup.y, 50, 50};
            Rectangle save_button = {715, 395, 100, 40};
            DrawRectangleRec(main_popup, WHITE);
            DrawRectangleRec(close_button, RED);
            DrawRectangleRec(save_button, Fade(YELLOW, 0.5f));
            DrawRectangleLinesEx(main_popup, 3, BLACK);
            DrawRectangleLinesEx(close_button, 3, BLACK);
            DrawRectangleLinesEx(save_button, 3, BLACK);
            DrawTextEx(font, "X", {close_button.x + 15, close_button.y + 10}, 30, 5, WHITE);
            DrawTextEx(font, "Mã lớp:", Vector2{main_popup.x + 10, main_popup.y + 90}, 30, 5, BLACK);
            DrawTextEx(font, "Lưu", Vector2{740, 400}, 30, 3, BLACK);
            box_ma_lop_khi_xoa.run(global_mouse_pos);
            if (CheckCollisionPointRec(GetMousePosition(), save_button))
            {
                DrawRectangleLinesEx(save_button, 3, BLUE);
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !press_Delete_Save_Button)
                {
                    press_Delete_Save_Button = true;
                }
            }
            if (press_Delete_Save_Button)
            {
                DrawRectangle(600, 220, 375, 200, Fade(BLUE, 0.5f));
                DrawRectangle(660, 300, 100, 40, YELLOW);
                DrawRectangle(820, 300, 100, 40, YELLOW);
                DrawTextEx(font, "Bạn có chắc chắn không?", {615, 260}, 30, 3, BLACK);
                DrawTextEx(font, "Có", {695, 305}, 30, 3, BLACK);
                DrawTextEx(font, "Không", {830, 305}, 30, 3, BLACK);

                if (CheckCollisionPointRec(GetMousePosition(), {660, 300, 100, 40}))
                {

                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !press_Are_You_Sure_Button)
                    {
                        press_Are_You_Sure_Button = true;
                    }
                }
                if (press_Are_You_Sure_Button)
                {
                    int count = 0;
                    dslh.removeClass(box_ma_lop_khi_xoa.text.data);
                    box_ma_lop_khi_xoa.text.data = "";
                    press_Are_You_Sure_Button = false;
                    press_Delete_Save_Button = false;
                }
            }
            if (CheckCollisionPointRec(global_mouse_pos, {820, 300, 100, 40}))
            {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    press_Delete_Save_Button = false;
                }
            }

            if (CheckCollisionPointRec(global_mouse_pos, close_button))
            {
                DrawRectangleRec(close_button, Fade(RED, 0.5f));

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    press_Delete_Button = false;
                }
            }
        }
        if(CheckCollisionPointRec(GetMousePosition(),{1350, 10, 120, 40})){
            DrawRectangleLinesEx({1350, 10, 120, 40}, 3, YELLOW);
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !press_Menu_Button){
                press_Menu_Button = true;
            }
        }
        if(press_Menu_Button){
            GiaoDienDanhSachMon(dsmh,dslh, dsch);
                if(IsKeyPressed(KEY_ESCAPE))
                {
                    press_Menu_Button = false;
                }
        }
            DrawRectangle(1350, 55, 120, 40, DARKBLUE); // Nut Cau Hoi
        DrawRectangleLines(1350, 55, 120, 40, BLACK);
        DrawTextEx(font, "Câu hỏi", {1362.5, 60}, 30, 1, WHITE);
                if(CheckCollisionPointRec(GetMousePosition(),{1350, 55, 120, 40})){
            DrawRectangleLinesEx({1350, 55, 120, 40}, 3, YELLOW);
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !press_Cau_Hoi_Button){
                press_Cau_Hoi_Button = true;
            }
        }
        if(press_Cau_Hoi_Button){
            //Ti add them Giao Dien Cau Hoi
            press_Cau_Hoi_Button = false;
            SauKhiAnVaoNganHangCauHoi(dsch);
        }


        EndDrawing();

        if (WindowShouldClose())
        {
            is_close_icon_pressed = true;
        }

        if (IsKeyPressed(KEY_ESCAPE))
        {
            break;
        }
        is_button_prev_pressed = false;
        is_button_next_pressed = false;
    }
}
void in_SinhVien_1_Lop(SVPtr FirstSV, float &cur_page, Font font, string malop, DanhSachLopHoc &dslh);
void GiaoDienDanhSachSinhVien(DanhSachLopHoc &dslh, string a, DanhSachMonHoc &dsmh, DanhSachCauHoi &dsch)
{
    const int screenWidth = 1500;
    const int screenHeight = 800;
    int page = 0;
    int index = 0;
    float line = 145;
    int item_per_page = 0;
    string ma_lop = "";
    SVPtr sv = dslh[(char *)a.c_str()]->getDSSV()->getFirst();
    bool press_Add_Button = false;
    bool press_Add_Save_Button = false;
    bool press_Delete_Button = false;
    bool press_Delete_Save_Button = false;
    bool press_Are_You_Sure_Button = false;
    bool press_Back_Button = false;
    bool press_Mon_Hoc_Button = false;
    bool press_Show_Subject_Button = false;
    InputBox box_add(Rectangle{10, 10, 90, 40}, BLACK, BLACK, WHITE, font);
    InputBox box_nien_khoa(Rectangle{550, 410, 190, 50}, BLACK, BLACK, WHITE, font);
    InputBox box_MSSV(Rectangle{480, 185, 210, 40}, BLACK, BLACK, WHITE, font);
    InputBox box_Ho_Ten(Rectangle{500, 275, 550, 40}, BLACK, BLACK, WHITE, font);
    InputBox box_Phai(Rectangle{540, 365, 100, 40}, BLACK, BLACK, WHITE, font);
    InputBox box_Password(Rectangle{540, 455, 150, 40}, BLACK, BLACK, WHITE, font);
    InputBox box_MSSV_Xoa(Rectangle{695, 320, 240, 50}, BLACK, BLACK, WHITE, font);
    InputBox box_Ma_Mon(Rectangle{695, 320, 240, 50}, BLACK, BLACK, WHITE, font);
    SetWindowPosition(GetMonitorWidth(0) / 2 - screenWidth / 2, GetMonitorHeight(0) / 2 - screenHeight / 2);
    SetWindowSize(screenWidth, screenHeight);
    bool is_button_next_pressed = false;
    bool is_button_prev_pressed = false;
    float cur_page = 0;
    while (!is_close_icon_pressed)
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);
        DrawRectangle(480, 750, 180, 45, Fade(BLUE, 0.2f));
        DrawRectangle(850, 750, 180, 45, Fade(BLUE, 0.2f));
        DrawTextEx(font, "Trang trước", {490, 755}, 30, 3, BLACK);
        DrawTextEx(font, "Trang sau", {875, 755}, 30, 3, BLACK);
        DrawRectangle(0, 0, 1500, 100, Fade(GRAY, 0.5f)); // vẽ bản màu xám
        // DrawRectangle(950, 10, 120, 40, DARKBLUE);
        // DrawRectangleLines(950, 10, 120, 40, BLACK);
        // DrawTextEx(font, "BACK", {972.5, 15}, 30, 3, WHITE);
        DrawRectangle(10, 10, 90, 40, DARKBLUE);
        DrawRectangleLines(10, 10, 90, 40, BLACK);
        DrawTextEx(font, "Thêm", {20, 15}, 30, 3, WHITE);
        DrawRectangle(140, 10, 90, 40, DARKBLUE);
        DrawRectangleLines(140, 10, 90, 40, BLACK);
        DrawTextEx(font, "Xóa", {160, 15}, 30, 3, WHITE);
        DrawRectangle(0, 100, 55, 40, DARKBLUE);     // STT
        DrawRectangle(55, 100, 250, 40, DARKBLUE);   // MSSV
        DrawRectangle(305, 100, 995, 40, DARKBLUE);  // Họ tên
        DrawRectangle(1300, 100, 200, 40, DARKBLUE); // Giới tín
        DrawRectangle(10, 55, 150, 40, DARKBLUE); // ô điểm
        DrawRectangleLines(10, 55, 150, 40, BLACK);
        DrawTextEx(font, "Xem điểm",{17.5,60},30,3,WHITE);

        for (int i = 0; i <= 15; i++)
        {
            if (i == 0)
            {
                DrawRectangleLines(0, 100, 55, 40, BLACK);     // STT
                DrawRectangleLines(55, 100, 250, 40, BLACK);   // MSSV
                DrawRectangleLines(305, 100, 995, 40, BLACK);  // Họ tên
                DrawRectangleLines(1300, 100, 200, 40, BLACK); // Giới tính
            }
            else if (i >= 1)
            {
                DrawRectangleLines(0, 100 + 40 * i, 55, 40, BLACK);     // STT
                DrawRectangleLines(55, 100 + 40 * i, 250, 40, BLACK);   // MSSV
                DrawRectangleLines(305, 100 + 40 * i, 995, 40, BLACK);  // Họ tên
                DrawRectangleLines(1300, 100 + 40 * i, 200, 40, BLACK); // Giới tính
            }
        }
        DrawTextEx(font, "STT", {0, 105}, 30, 3, BLACK);
        DrawTextEx(font, "MSSV", {140, 105}, 30, 3, BLACK);
        DrawTextEx(font, "Họ và tên", {760, 105}, 30, 3, BLACK);
        DrawTextEx(font, "Phái", {1370, 105}, 30, 3, BLACK);
        in_SinhVien_1_Lop(sv, cur_page, font, a, dslh);

        if (CheckCollisionPointRec(GetMousePosition(), {10, 10, 90, 40}))
        {
            DrawRectangleLinesEx({10, 10, 90, 40}, 3, YELLOW);
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !press_Add_Button)
            {
                press_Add_Button = true;
            }
        }
        if (press_Add_Button)
        {
            Rectangle main_popup = {350, 150, 800, 400};
            Rectangle close_button = {main_popup.x + 750, main_popup.y, 50, 50};
            Rectangle save_button = {700, 500, 100, 40};
            DrawRectangleRec(main_popup, WHITE);
            DrawRectangleRec(close_button, RED);
            DrawRectangleRec(save_button, Fade(YELLOW, 0.5f));
            DrawRectangleLinesEx(close_button, 3, BLACK);
            DrawRectangleLinesEx(save_button, 3, BLACK);
            DrawRectangleLinesEx(main_popup, 3, BLACK);
            DrawTextEx(font, "X", {close_button.x + 15, close_button.y + 10}, 30, 5, WHITE);
            DrawTextEx(font, "MSSV:", Vector2{main_popup.x + 20, main_popup.y + 40}, 30, 5, BLACK);
            DrawTextEx(font, "Họ tên:", Vector2{main_popup.x + 20, main_popup.y + 130}, 30, 5, BLACK);
            DrawTextEx(font, "Giới tính: ", Vector2{main_popup.x + 20, main_popup.y + 220}, 30, 5, BLACK);
            DrawTextEx(font, "Mật khẩu: ", Vector2{main_popup.x + 20, main_popup.y + 310}, 30, 5, BLACK);
            DrawTextEx(font, "Lưu", Vector2{725, 505}, 30, 3, BLACK);
            box_MSSV.run(global_mouse_pos);
            box_Ho_Ten.run(global_mouse_pos);
            box_Phai.run(global_mouse_pos);
            box_Password.run(global_mouse_pos);

            if (CheckCollisionPointRec(GetMousePosition(), save_button))
            {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !press_Add_Save_Button)
                {
                    press_Add_Save_Button = true;
                }
            }
            if (press_Add_Save_Button)
            {
                string ho, ten;
                DArray<int> question_id;
                int count = 0;
                for (SVPtr ex = sv; ex != NULL; ex = ex->next)
                {
                    if (ex->sv_data.MASV == box_MSSV.text.data)
                    {
                        count++;
                    }
                }
                ho = box_Ho_Ten.text.data.substr(0, box_Ho_Ten.text.data.find(' '));
                ten = box_Ho_Ten.text.data.substr(box_Ho_Ten.text.data.find(' ') + 1, box_Ho_Ten.text.data.size());
                if (box_MSSV.text.data != "" && box_Ho_Ten.text.data != "" && box_Phai.text.data != "" && box_Password.text.data != "")
                {
                    if (count == 0)
                    {
                        dslh[(char *)a.c_str()]->getDSSV()->insertOrderSV(SinhVien{box_MSSV.text.data, ho, ten, box_Phai.text.data, box_Password.text.data, question_id});
                        box_Ho_Ten.text.data = "";
                        box_MSSV.text.data = "";
                        box_Phai.text.data = "";
                        box_Password.text.data = "";
                    }
                }

                if (CheckCollisionPointRec(global_mouse_pos, save_button))
                {
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                    {
                        press_Add_Save_Button = false;
                    }
                }
            }
            if (CheckCollisionPointRec(global_mouse_pos, close_button))
            {
                DrawRectangleRec(close_button, Fade(RED, 0.5f));

                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    press_Add_Button = false;
                }
            }
        }
        if (CheckCollisionPointRec(GetMousePosition(), {140, 10, 90, 40}))
        {
            DrawRectangleLinesEx({140, 10, 90, 40}, 3, YELLOW);
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !press_Delete_Button)
            {
                press_Delete_Button = true;
            }
        }
        if (press_Delete_Button)
        {
            Rectangle main_popup = {570, 240, 375, 200};
            Rectangle close_button = {main_popup.x + 325, main_popup.y, 50, 50};
            Rectangle save_button = {715, 395, 100, 40};
            DrawRectangleRec(main_popup, WHITE);
            DrawRectangleRec(close_button, RED);
            DrawRectangleRec(save_button, Fade(YELLOW, 0.5f));
            DrawRectangleLinesEx(main_popup, 3, BLACK);
            DrawRectangleLinesEx(close_button, 3, BLACK);
            DrawRectangleLinesEx(save_button, 3, BLACK);
            DrawTextEx(font, "X", {close_button.x + 15, close_button.y + 10}, 30, 5, WHITE);
            DrawTextEx(font, "MSSV:", Vector2{main_popup.x + 10, main_popup.y + 90}, 30, 5, BLACK);
            DrawTextEx(font, "Lưu", Vector2{740, 400}, 30, 3, BLACK);
            box_MSSV_Xoa.run(global_mouse_pos);
            if (CheckCollisionPointRec(GetMousePosition(), save_button))
            {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !press_Delete_Save_Button)
                {
                    press_Delete_Save_Button = true;
                }
            }
            if (press_Delete_Save_Button)
            {
                DrawRectangle(600, 220, 375, 200, SKYBLUE);
                DrawRectangleLines(600, 220, 375, 200, BLACK);
                DrawRectangle(660, 300, 100, 40, YELLOW);
                DrawRectangle(820, 300, 100, 40, YELLOW);
                DrawTextEx(font, "Bạn có chắc chắn không?", {615, 260}, 30, 3, BLACK);
                DrawTextEx(font, "Có", {695, 305}, 30, 3, BLACK);
                DrawTextEx(font, "Không", {830, 305}, 30, 3, BLACK);
                if (CheckCollisionPointRec(GetMousePosition(), {660, 300, 100, 40}))
                {
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !press_Are_You_Sure_Button)
                    {
                        press_Are_You_Sure_Button = true;
                    }
                }
                if (press_Are_You_Sure_Button)
                {
                    dslh[(char *)a.c_str()]->getDSSV()->deleteSV(box_MSSV_Xoa.text.data);
                    box_MSSV_Xoa.text.data = "";
                    press_Delete_Save_Button = false;
                    press_Are_You_Sure_Button = false;
                }
                if (CheckCollisionPointRec(global_mouse_pos, {820, 300, 100, 40}))
                {
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                    {
                        press_Delete_Save_Button = false;
                    }
                }
            }
            if (CheckCollisionPointRec(global_mouse_pos, close_button))
            {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    press_Delete_Button = false;
                }
            }
        }
        // if (CheckCollisionPointRec(GetMousePosition(), {950, 10, 120, 40}))
        // {
        //         DrawRectangleLinesEx({950, 10, 120, 40}, 3, YELLOW);
        //     if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !press_Back_Button)
        //     {
        //         press_Back_Button = true;
        //     }
        // }
        // if(press_Back_Button){
        //     GiaoDienDanhSachLop(dslh, dsmh, dsch);
        //     press_Back_Button = false;
        // }
        if(CheckCollisionPointRec(GetMousePosition(),{10, 55, 150, 40})){
            DrawRectangleLinesEx({10, 55, 150, 40}, 3, YELLOW);
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !press_Mon_Hoc_Button){
                press_Mon_Hoc_Button = true;
            }
        }
        if(press_Mon_Hoc_Button){
            GiaoDienDanhSachMonSV(dsmh, dslh,a, dsch);
        if(IsKeyPressed(KEY_ESCAPE))
                {
                    press_Mon_Hoc_Button = false;
                }
            
        }
        EndDrawing();
        if (WindowShouldClose())
        {
            is_close_icon_pressed = true;
        }

        if (IsKeyPressed(KEY_ESCAPE))
        {
            break;
        }
        is_button_prev_pressed = false;
        is_button_next_pressed = false;
    }
}
void in_SinhVien_1_Lop(SVPtr FirstSV, float &cur_page, Font font, string malop, DanhSachLopHoc &dslh)
{
    const int screenWidth = 1500;
    const int screenHeight = 800;
    float line = 145, page = 0;
    int item_per_page = 0;
    int index = 0;
    bool is_button_next_pressed = false;
    bool is_button_prev_pressed = false;
    int dem = 0;
    for (SVPtr q = FirstSV; q != nullptr; q = q->next)
    {
        string ho_ten = q->sv_data.HO + " " + q->sv_data.TEN;
        DrawTextEx(font, (char *)q->sv_data.MASV.c_str(), Vector2{60 + screenWidth * (page - cur_page), line}, 30, 1, BLACK);
        DrawTextEx(font, (char *)ho_ten.c_str(), Vector2{310 + screenWidth * (page - cur_page), line}, 30, 1, BLACK);
        if ((index + 1) < 10)
        {
            DrawTextEx(font, (char *)to_string(index + 1).c_str(), {20 + screenWidth * (page - cur_page), line}, 30, 3, BLACK);
        }
        else
        {
            DrawTextEx(font, (char *)to_string(index + 1).c_str(), {10 + screenWidth * (page - cur_page), line}, 30, 3, BLACK);
        }
        if (q->sv_data.Phai == "Nam")
        {
            DrawTextEx(font, (char *)q->sv_data.Phai.c_str(), Vector2{1370 + screenWidth * (page - cur_page), line}, 30, 3, BLACK);
        }
        else
        {
            DrawTextEx(font, (char *)q->sv_data.Phai.c_str(), Vector2{1380 + screenWidth * (page - cur_page), line}, 30, 3, BLACK);
        }
        line += 40;
        item_per_page++;
        if (item_per_page == 15)
        {
            page++;
            line = 145;
            item_per_page = 0;
        }
        index++;
        dem++;
    }
    if (IsKeyPressed(KEY_LEFT))
    {
        cur_page--;
        if (cur_page < 0)
        {
            cur_page = 0;
        }
    }
if (IsKeyPressed(KEY_RIGHT))
    {
        if (cur_page < dem / 15)
            cur_page++;
    }
    if (CheckCollisionPointRec(GetMousePosition(), {850, 750, 180, 45}))
    {
        DrawRectangleLinesEx({850, 750, 180, 45}, 3, YELLOW);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !is_button_next_pressed)
        {
            if (cur_page < dem / 15)
                cur_page++;
            is_button_next_pressed = true;
        }
    }
    if (CheckCollisionPointRec(GetMousePosition(), {480, 750, 180, 45}))
    {
        DrawRectangleLinesEx({480, 750, 180, 45}, 3, YELLOW);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !is_button_prev_pressed)
        {
            cur_page--;
            if (cur_page < 0)
            {
                cur_page = 0;
            }
            is_button_prev_pressed = true;
        }
    }
}
void inDanhSachMon(DanhSachMonHoc &dsmh, float &cur_page, int max_item, bool is_button_next_pressed, bool is_button_prev_pressed)
{
    for (int place = cur_page * max_item; place < dsmh.getLength() && place < (cur_page + 1) * max_item; place++)
    {
        DrawTextEx(font, to_string(place + 1).c_str(), {30, 160 + 50 * place - 500 * (place / 10)}, 30, 3, BLACK);
        DrawTextEx(font, dsmh[place].ma_mon_hoc, Vector2{90, 160 + 50 * place - 500 * (place / 10)}, 30, 3, BLACK);
        DrawTextEx(font, (char *)dsmh[place].ten_mon_hoc.c_str(), Vector2{350, 160 + 50 * place - 500 * (place / 10)}, 30, 3, BLACK);
    }
    if (CheckCollisionPointRec(GetMousePosition(), {850, 705, 180, 45}))
    {
        DrawRectangleLinesEx({850, 705, 180, 45}, 3, YELLOW);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !is_button_next_pressed)
        {
            cur_page++;
            is_button_next_pressed = true;
        }
    }
    if (CheckCollisionPointRec(GetMousePosition(), {480, 705, 180, 45}))
    {
        DrawRectangleLinesEx({480, 705, 180, 45}, 3, YELLOW);
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !is_button_prev_pressed)
        {
            cur_page--;
            if (cur_page < 0)
            {
                cur_page = 0;
            }
            is_button_prev_pressed = true;
        }
    }
}
void GiaoDienDanhSachMon(DanhSachMonHoc &dsmh, DanhSachLopHoc &dslh, DanhSachCauHoi &dsch)
{
    const int screenWidth = 1500;
    const int screenHeight = 800;
    float cur_page = 0;
    int max_item = 10;
    string ma_mon = "";
    string ten_mon = "";
    InputBox box_Ma_Mon(Rectangle{500, 275, 250, 40}, BLACK, BLACK, WHITE, font);
    InputBox box_Ten_Mon(Rectangle{540, 380, 550, 40}, BLACK, BLACK, WHITE, font);
    InputBox box_Ma_Mon_Xoa(Rectangle{705, 320, 230, 50}, BLACK, BLACK, WHITE, font);
    Button next{{850, 705, 180, 45}, "Next", WHITE, BLUE, font};
    Button prev{{480, 705, 180, 45}, "Prev", WHITE, BLUE, font};
    global_mouse_pos = GetMousePosition();
    bool is_button_next_pressed = false;
    bool is_button_prev_pressed = false;
    bool press_Add_Button = false;
    bool press_Add_Save_Button = false;
    bool press_Delete_Button = false;
    bool press_Delete_Save_Button = false;
    bool press_Are_You_Sure_Button = false;
    bool press_Correction_Button = false;
    bool press_Correction_Save_Button = false;
    bool press_Back_Button = false;
    bool press_Logout_Button = false;
    DArray<bool> press_Row_By_Row_Correction_Button;
    SetWindowPosition(GetMonitorWidth(0) / 2 - screenWidth / 2, GetMonitorHeight(0) / 2 - screenHeight / 2);
    // SetWindowSize(screenWidth, screenHeight);
    while (!is_close_icon_pressed)
    {
        BeginDrawing();
        ClearBackground(WHITE);

        DrawRectangle(480, 705, 180, 45, Fade(BLUE, 0.2f));
        DrawRectangle(850, 705, 180, 45, Fade(BLUE, 0.2f));
        DrawTextEx(font, "Trang trước", {490, 712.5}, 30, 3, BLACK);
        DrawTextEx(font, "Trang sau", {875, 712.5}, 30, 3, BLACK);
        DrawRectangle(0, 0, 1500, 100, Fade(GRAY, 0.5f));
        // DrawRectangle(1150, 10, 120, 40, DARKBLUE);
        // DrawRectangleLines(1150, 10, 120, 40, BLACK);
        // DrawTextEx(font, "BACK", {1172.5, 15}, 30, 3, WHITE);

        for (int i = 0; i < 3; i++)
        {
            if (i != 2)
            {
                DrawRectangle(10 + (150 * i), 10, 80, 40, DARKBLUE); // ô thêm, xóa
                DrawRectangleLines(10 + (150 * i), 10, 80, 40, BLACK);
            }
            else if (i == 2)
            {
                DrawRectangle(10 + (150 * i), 10, 300, 40, DARKBLUE); // ô hiệu chỉnh
                DrawRectangleLines(10 + (150 * i), 10, 300, 40, BLACK);
            }
        }
        DrawTextEx(font, "Thêm", {13.5, 15}, 30, 3, WHITE);
        DrawTextEx(font, "Xóa", {174.5, 15}, 30, 3, WHITE);
        DrawTextEx(font, "Hiệu chỉnh thông tin", {325, 15}, 30, 3, WHITE);
        for (int j = 1; j < 13; j++)
        {
            if (j == 1)
            {
                for (int i = 0; i < 3; i++)
                {
                    DrawRectangle(0, 100, 80, 50, BLUE);
                    DrawRectangleLines(0, 100 * j, 80, 50, BLACK);
                    if (i == 0)
                    {
                        DrawRectangle(80 + (260 * i), 100 * j, 260, 50, BLUE);
                        DrawRectangleLines(80 + (260 * i), 100 * j, 260, 50, BLACK);
                    }
                    else if (i == 1)
                    {
                        DrawRectangle(80 + (260 * i), 100 * j, 1160, 50, BLUE);
                        DrawRectangleLines(80 + (260 * i), 100 * j, 1160, 50, BLACK);
                    }
                }
            }
            else if (j >= 3)
            {
                for (int i = 0; i < 3; i++)
                {
                    if (i == 0)
                    {
                        DrawRectangleLines(0, 50 * j, 80, 50, BLACK);
                        DrawRectangleLines(80 + (480 * i), 50 * j, 260, 50, BLACK);
                    }
                    else if (i == 1)
                    {
                        DrawRectangleLines(80 + (260 * i), 50 * j, 1160, 50, BLACK);
                    }
                }
            }
        }
        DrawTextEx(font, "STT", {15, 110}, 30, 3, BLACK);
        DrawTextEx(font, "Mã môn", {155, 110}, 30, 3, BLACK);
        DrawTextEx(font, "Tên môn học", {840, 110}, 30, 3, BLACK);
        inDanhSachMon(dsmh, cur_page, max_item, is_button_next_pressed, is_button_prev_pressed);
        if (CheckCollisionPointRec(GetMousePosition(), {10, 10, 80, 40}))
        {
            DrawRectangleLinesEx({10, 10, 80, 40}, 3, YELLOW);
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !press_Add_Button)
            {
                press_Add_Button = true;
            }
        }
        if (press_Add_Button)
        {
            Rectangle main_popup = {350, 150, 800, 400};
            Rectangle close_button = {main_popup.x + 750, main_popup.y, 50, 50};
            Rectangle save_button = {700, 500, 100, 40};
            DrawRectangleRec(main_popup, WHITE);
            DrawRectangleRec(close_button, RED);
            DrawRectangleRec(save_button, Fade(YELLOW, 0.5f));
            DrawRectangleLinesEx(close_button, 3, BLACK);
            DrawRectangleLinesEx(save_button, 3, BLACK);
            DrawRectangleLinesEx(main_popup, 3, BLACK);
            DrawTextEx(font, "X", {close_button.x + 15, close_button.y + 10}, 30, 5, WHITE);
            DrawTextEx(font, "Mã môn:", Vector2{main_popup.x + 20, main_popup.y + 130}, 30, 5, BLACK);
            DrawTextEx(font, "Tên môn:", Vector2{main_popup.x + 20, main_popup.y + 240}, 30, 5, BLACK);
            DrawTextEx(font, "Lưu", Vector2{725, 505}, 30, 3, BLACK);
            box_Ma_Mon.run(global_mouse_pos);
            box_Ten_Mon.run(global_mouse_pos);
            if (CheckCollisionPointRec(GetMousePosition(), save_button))
            {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !press_Add_Save_Button)
                {
                    press_Add_Save_Button = true;
                }
            }
            if (press_Add_Save_Button)
            {
                if (box_Ma_Mon.text.data != "" && box_Ten_Mon.text.data != "")
                {
                    if (dsmh.search((char *)box_Ma_Mon.text.data.c_str()) < 0)
                    {
                        dsmh.insert(MonHoc{(char *)box_Ma_Mon.text.data.c_str(), box_Ten_Mon.text.data});
                    }
                }
                box_Ma_Mon.text.data = box_Ten_Mon.text.data = "";
                press_Add_Save_Button = false;
            }
            if (CheckCollisionPointRec(global_mouse_pos, close_button))
            {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    press_Add_Button = false;
                }
            }
        }
        if (CheckCollisionPointRec(GetMousePosition(), {160, 10, 80, 40}))
        {
            DrawRectangleLinesEx({160, 10, 80, 40}, 3, YELLOW);
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !press_Delete_Button)
            {
                press_Delete_Button = true;
            }
        }
        if (press_Delete_Button)
        {
            Rectangle main_popup = {570, 240, 375, 200};
            Rectangle close_button = {main_popup.x + 325, main_popup.y, 50, 50};
            Rectangle save_button = {715, 395, 100, 40};
            DrawRectangleRec(main_popup, WHITE);
            DrawRectangleRec(close_button, RED);
            DrawRectangleRec(save_button, Fade(YELLOW, 0.5f));
            DrawRectangleLinesEx(main_popup, 3, BLACK);
            DrawRectangleLinesEx(close_button, 3, BLACK);
            DrawRectangleLinesEx(save_button, 3, BLACK);
            DrawTextEx(font, "X", {close_button.x + 15, close_button.y + 10}, 30, 5, WHITE);
            DrawTextEx(font, "Mã môn:", Vector2{main_popup.x + 10, main_popup.y + 90}, 30, 5, BLACK);
            DrawTextEx(font, "Lưu", Vector2{740, 400}, 30, 3, BLACK);
            box_Ma_Mon_Xoa.run(global_mouse_pos);
            if (CheckCollisionPointRec(GetMousePosition(), save_button))
            {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !press_Delete_Save_Button)
                {
                    press_Delete_Save_Button = true;
                }
            }
            if (press_Delete_Save_Button)
            {
                DrawRectangle(600, 220, 375, 200, SKYBLUE);
                DrawRectangleLines(600, 220, 375, 200, BLACK);
                DrawRectangle(660, 300, 100, 40, YELLOW);
                DrawRectangle(820, 300, 100, 40, YELLOW);
                DrawTextEx(font, "Bạn có chắc chắn không?", {615, 260}, 30, 3, BLACK);
                DrawTextEx(font, "Có", {695, 305}, 30, 3, BLACK);
                DrawTextEx(font, "Không", {830, 305}, 30, 3, BLACK);
                if (CheckCollisionPointRec(global_mouse_pos, {660, 300, 100, 40}))
                {
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !press_Are_You_Sure_Button)
                    {
                        press_Are_You_Sure_Button = true;
                    }
                }
                if (press_Are_You_Sure_Button)
                {
                    dsmh.remove((char *)box_Ma_Mon_Xoa.text.data.c_str());
                    box_Ma_Mon_Xoa.text.data = "";
                    press_Delete_Save_Button = false;
                    press_Are_You_Sure_Button = false;
                }
                if (CheckCollisionPointRec(global_mouse_pos, {820, 300, 100, 40}))
                {
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                    {
                        press_Delete_Save_Button = false;
                    }
                }
            }
            if (CheckCollisionPointRec(global_mouse_pos, {close_button}))
            {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    press_Delete_Button = false;
                }
            }
        }
        for (int place = cur_page * max_item; place < dsmh.getLength() && place < (cur_page + 1) * max_item; place++)
        {
            press_Row_By_Row_Correction_Button.push_back(false);
        }
        for (int place = cur_page * max_item; place < dsmh.getLength() && place < (cur_page + 1) * max_item; place++)
        {

            Rectangle rec3 = {0, 50 * (place + 3) - (50 * (place / 10)) * 10, 80, 50};
            Rectangle rec4 = {310, 10, 300, 40};
            if (CheckCollisionPointRec(GetMousePosition(), rec4))
            {
                DrawRectangleLinesEx(rec4, 3, YELLOW);
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !press_Correction_Button)
                {
                    press_Correction_Button = true;
                }
            }
            if (press_Correction_Button)
            {
                DrawRectangle(700, 55, 40, 40, RED);
                DrawTextEx(font, "X", {710, 60}, 30, 3, YELLOW);
                if (CheckCollisionPointRec(GetMousePosition(), rec3))
                {
                    DrawRectangleLinesEx(rec3, 3, RED);
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !press_Row_By_Row_Correction_Button[place])
                    {
                        press_Row_By_Row_Correction_Button[place] = true;
                        ma_mon = dsmh[place].ma_mon_hoc;
                        ten_mon = dsmh[place].ten_mon_hoc;
                    }
                }
                if (press_Row_By_Row_Correction_Button[place])
                {

                    Rectangle main_popup = {350, 150, 800, 400};
                    Rectangle close_button = {main_popup.x + 750, main_popup.y, 50, 50};
                    Rectangle save_button = {700, 500, 100, 40};
                    DrawRectangleRec(main_popup, WHITE);
                    DrawRectangleRec(close_button, RED);
                    DrawRectangleRec(save_button, Fade(YELLOW, 0.5f));
                    DrawRectangleLinesEx(close_button, 3, BLACK);
                    DrawRectangleLinesEx(save_button, 3, BLACK);
                    DrawRectangleLinesEx(main_popup, 3, BLACK);
                    DrawTextEx(font, "X", {close_button.x + 15, close_button.y + 10}, 30, 5, WHITE);
                    DrawTextEx(font, "Mã môn:", Vector2{main_popup.x + 20, main_popup.y + 130}, 30, 5, BLACK);
                    DrawTextEx(font, "Tên môn:", Vector2{main_popup.x + 20, main_popup.y + 240}, 30, 5, BLACK);
                    DrawTextEx(font, "Lưu", Vector2{725, 505}, 30, 3, BLACK);
                    DrawTextEx(font, (char *)ma_mon.c_str(), {box_Ma_Mon.box.x + 10, box_Ma_Mon.box.y + 5}, 30, 5, BLACK);
                    DrawTextEx(font, (char *)ten_mon.c_str(), {550, box_Ten_Mon.box.y + 5}, 30, 5, BLACK);
                    box_Ma_Mon.run(global_mouse_pos);
                    box_Ten_Mon.run(global_mouse_pos);
                    if (CheckCollisionPointRec(GetMousePosition(), save_button))
                    {
                        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON && !press_Correction_Save_Button))
                        {
                            press_Correction_Save_Button = true;
                        }
                    }
                    if (press_Correction_Save_Button)
                    {
                        if (box_Ma_Mon.text.data != "")
                        {
                            strcpy(dsmh[place].ma_mon_hoc, (char *)box_Ma_Mon.text.data.c_str());
                        }
                        if (box_Ten_Mon.text.data != "")
                        {
                            dsmh[place].ten_mon_hoc = box_Ten_Mon.text.data;
                        }
                        box_Ma_Mon.text.data = box_Ten_Mon.text.data = "";
                        press_Correction_Save_Button = false;
                    }
                    if (CheckCollisionPointRec(global_mouse_pos, close_button))
                    {
                        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                        {
                            press_Row_By_Row_Correction_Button[place] = false;
                        }
                    }
                }
                if (CheckCollisionPointRec(GetMousePosition(), {700, 55, 40, 40}))
                {
                    DrawRectangleLinesEx({700, 55, 40, 40}, 3, YELLOW);
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                    {
                        press_Correction_Button = false;
                    }
                }
            }
        }
        // if(CheckCollisionPointRec(GetMousePosition(),{1150, 10, 120, 40})){
        //     DrawRectangleLinesEx({1150, 10, 120, 40}, 3, YELLOW);
        //     if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !press_Back_Button){
        //         press_Back_Button = true;
        //     }
        // }
        // if(press_Back_Button){
            
        //     GiaoDienDanhSachLop(dslh, dsmh, dsch);
        //     if(IsKeyPressed(KEY_ESCAPE)){
        //     press_Back_Button = false;
        //     }
        // }
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
void GiaoDienDanhSachMonSV(DanhSachMonHoc &dsmh, DanhSachLopHoc &dslh, string ma_lop, DanhSachCauHoi &dsch)
{
    const int screenWidth = 1500;
    const int screenHeight = 800;
    float cur_page = 0;
    int max_item = 10;
    string ma_mon = "";
    Button next{{850, 705, 180, 45}, "Next", WHITE, BLUE, font};
    Button prev{{480, 705, 180, 45}, "Prev", WHITE, BLUE, font};
    global_mouse_pos = GetMousePosition();
    bool is_button_next_pressed = false;
    bool is_button_prev_pressed = false;
    bool press_Xem_Diem_pressed = false;
    bool press_Back_Button = false;
    bool press_Diem_Thi_Button = false;
    

    SetWindowPosition(GetMonitorWidth(0) / 2 - screenWidth / 2, GetMonitorHeight(0) / 2 - screenHeight / 2);
    while (!is_close_icon_pressed)
    {
        BeginDrawing();
        ClearBackground(WHITE);

        DrawRectangle(480, 705, 180, 45, Fade(BLUE, 0.2f));
        DrawRectangle(850, 705, 180, 45, Fade(BLUE, 0.2f));
        DrawTextEx(font, "Trang trước", {490, 712.5}, 30, 3, BLACK);
        DrawTextEx(font, "Trang sau", {875, 712.5}, 30, 3, BLACK);
        DrawRectangle(0, 0, 1500, 100, Fade(GRAY, 0.5f));
        // DrawRectangle(1150, 10, 120, 40, DARKBLUE);
        // DrawRectangleLines(1150, 10, 120, 40, BLACK);
        // DrawTextEx(font, "BACK", {1172.5, 15}, 30, 3, WHITE);


        for (int j = 1; j < 13; j++)
        {
            if (j == 1)
            {
                for (int i = 0; i < 3; i++)
                {
                    DrawRectangle(0, 100, 80, 50, BLUE);
                    DrawRectangleLines(0, 100 * j, 80, 50, BLACK);
                    if (i == 0)
                    {
                        DrawRectangle(80 + (260 * i), 100 * j, 260, 50, BLUE);
                        DrawRectangleLines(80 + (260 * i), 100 * j, 260, 50, BLACK);
                    }
                    else if (i == 1)
                    {
                        DrawRectangle(80 + (260 * i), 100 * j, 1160, 50, BLUE);
                        DrawRectangleLines(80 + (260 * i), 100 * j, 1160, 50, BLACK);
                    }
                }
            }
            else if (j >= 3)
            {
                for (int i = 0; i < 3; i++)
                {
                    if (i == 0)
                    {
                        DrawRectangleLines(0, 50 * j, 80, 50, BLACK);
                        DrawRectangleLines(80 + (480 * i), 50 * j, 260, 50, BLACK);
                    }
                    else if (i == 1)
                    {
                        DrawRectangleLines(80 + (260 * i), 50 * j, 1160, 50, BLACK);
                    }
                }
            }
        }
        DrawTextEx(font, "STT", {15, 110}, 30, 3, BLACK);
        DrawTextEx(font, "Mã môn", {155, 110}, 30, 3, BLACK);
        DrawTextEx(font, "Tên môn học", {840, 110}, 30, 3, BLACK);
        inDanhSachMon(dsmh, cur_page, max_item, is_button_next_pressed, is_button_prev_pressed);
        // if(CheckCollisionPointRec(GetMousePosition(),{1150, 10, 120, 40})){
        //     DrawRectangleLinesEx({1150, 10, 120, 40}, 3, YELLOW);
        //     if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !press_Back_Button){
        //         press_Back_Button = true;
        //     }
        // }
        // if(press_Back_Button){
        //     GiaoDienDanhSachLop(dslh, dsmh, dsch);
        //     if(IsKeyPressed(KEY_ESCAPE)){
        //     press_Back_Button = false;
        //     }
        // }
        for(int place = cur_page * max_item; place < dsmh.getLength() && place < (cur_page + 1) * max_item; place++){
                Rectangle rec3 = {80, 50 * (place + 3) - (50 * (place / 10)) * 10, 260, 50};
                if(CheckCollisionPointRec(GetMousePosition(),rec3)){
                    DrawRectangleLinesEx(rec3, 3, YELLOW);
                    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !press_Diem_Thi_Button){
                        press_Diem_Thi_Button = true;
                        ma_mon = dsmh[place].ma_mon_hoc;
                    }
                }
                if(press_Diem_Thi_Button){
                    inBangDiemDanhSachSinhVien(dsmh,dslh, ma_lop, ma_mon, cur_page, dsch);
                    if(IsKeyPressed(KEY_ESCAPE)){
                        press_Diem_Thi_Button = false;
                    }
                }
        }
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
void inBangDiemDanhSachSinhVien(DanhSachMonHoc &dsmh, DanhSachLopHoc &dslh, string ma_lop_muon_in, string ma_mon ,float &cur_page, DanhSachCauHoi &dsch)
{
    const int screenWidth = 1500;
    const int screenHeight = 800;
    SetWindowPosition(GetMonitorWidth(0) / 2 - screenWidth / 2, GetMonitorHeight(0) / 2 - screenHeight / 2);
    SetWindowSize(screenWidth, screenHeight);
    bool is_button_next_pressed = false;
    bool is_button_prev_pressed = false;
    int item_per_page = 0;
    float line = 145;
    int page = 0;
    int index = 0;
    bool press_Back_Button = false;
    SVPtr sv = dslh[ma_lop_muon_in]->getDSSV()->getFirst();
    while (!is_close_icon_pressed)
    {
        item_per_page = 0;
        line = 145;
        page = 0;
        index = 0;
        int dem = -0;
        BeginDrawing();
        ClearBackground(WHITE);
        DrawRectangle(0, 0, 1500, 100, Fade(GRAY, 0.5f)); // vẽ bản màu xám
        // DrawRectangle(10, 10, 90, 40, DARKBLUE);
        // DrawRectangleLines(10, 10, 90, 40, BLACK);
        // DrawTextEx(font, "BACK", {15, 15}, 30, 3, WHITE);
        DrawRectangle(0, 100, 55, 40, DARKBLUE);     // STT
        DrawRectangle(55, 100, 250, 40, DARKBLUE);   // MSSV
        DrawRectangle(305, 100, 995, 40, DARKBLUE);  // Họ tên
        DrawRectangle(1300, 100, 200, 40, DARKBLUE); // Giới tín
        DrawRectangle(480, 750, 180, 45, Fade(BLUE, 0.2f));
        DrawRectangle(850, 750, 180, 45, Fade(BLUE, 0.2f));
        DrawTextEx(font, "Trang trước", {490, 755}, 30, 3, BLACK);
        DrawTextEx(font, "Trang sau", {875, 755}, 30, 3, BLACK);
        //Viết tên môn header
        DrawRectangle(10, 55, 300, 40, Fade(SKYBLUE, 0.2f));
        DrawRectangleLines(10, 55, 300, 40, BLACK);
        DrawTextEx(font, "Mã môn:", {15, 60}, 30, 4, BLACK);
        DrawTextEx(font, (char*)ma_mon.c_str(), {140, 60}, 30, 3, RED);
        for (int i = 0; i <= 15; i++)
        {
            if (i == 0)
            {
                DrawRectangleLines(0, 100, 55, 40, BLACK);     // STT
                DrawRectangleLines(55, 100, 250, 40, BLACK);   // MSSV
                DrawRectangleLines(305, 100, 995, 40, BLACK);  // Họ tên
                DrawRectangleLines(1300, 100, 200, 40, BLACK); // Giới tính
            }
            else if (i >= 1)
            {
                DrawRectangleLines(0, 100 + 40 * i, 55, 40, BLACK);     // STT
                DrawRectangleLines(55, 100 + 40 * i, 250, 40, BLACK);   // MSSV
                DrawRectangleLines(305, 100 + 40 * i, 995, 40, BLACK);  // Họ tên
                DrawRectangleLines(1300, 100 + 40 * i, 200, 40, BLACK); // Giới tính
            }
        }
        DrawTextEx(font, "STT", {0, 105}, 30, 3, BLACK);
        DrawTextEx(font, "MSSV", {140, 105}, 30, 3, BLACK);
        DrawTextEx(font, "Họ và tên", {760, 105}, 30, 3, BLACK);
        DrawTextEx(font, "Điểm", {1370, 105}, 30, 3, BLACK);
        for (SVPtr ptr = sv; ptr != nullptr; ptr = ptr->next)
        {
            DTPtr dsdt = ptr->sv_data.diem->getFirst();
            for (; dsdt != nullptr; dsdt = dsdt->next)
            {
                if (strcmp(dsdt->data.Mamh, (char *)ma_mon.c_str()) == 0)
                {

                    break;
                }
            }
            string ho_ten = ptr->sv_data.HO + " " + ptr->sv_data.TEN;
            DrawTextEx(font, (char *)ho_ten.c_str(), Vector2{310 + screenWidth * (page - cur_page), line}, 30, 1, BLACK);
            DrawTextEx(font, ptr->sv_data.MASV.c_str(), Vector2{60 + screenWidth * (page - cur_page), line}, 30, 3, BLACK);
            DrawTextEx(font, (to_string(index + 1)).c_str(), Vector2{20 + screenWidth * (page - cur_page), line}, 30, 3, BLACK);
            if (dsdt != nullptr)
            {
                string output_mark = to_string(dsdt->data.Diem).substr(0, std::to_string(dsdt->data.Diem).find(".") + 2);
                DrawTextEx(font, (char*) output_mark.c_str(), Vector2{1375 + screenWidth * (page - cur_page), line}, 30, 3, BLACK);
            }
            else
            {
                DrawTextEx(font, "Chưa thi", Vector2{1340 + screenWidth * (page - cur_page), line}, 30, 3, BLACK);
            }
            index++;
            item_per_page++;
            line += 40;
            dem++;
            if (item_per_page == 15)
            {
                page++;
                line = 145;
                item_per_page = 0;
            }
            
        }
        if(CheckCollisionPointRec(GetMousePosition(), {480, 750, 180, 45})){
            DrawRectangleLinesEx({480, 750, 180, 45}, 3, YELLOW);
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                cur_page--;
                if (cur_page < 0){
                    cur_page = 0;
                }
            }
        }
        if(CheckCollisionPointRec(GetMousePosition(),{850, 750, 180, 45})){
            DrawRectangleLinesEx({850, 750, 180, 45}, 3, YELLOW);
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                if (cur_page < dem / 15)
                    cur_page++;
            }
        }
         if (IsKeyPressed(KEY_LEFT))
    {
        cur_page--;
        if (cur_page < 0)
        {
            cur_page = 0;
        }
    }
if (IsKeyPressed(KEY_RIGHT))
    {
        if (cur_page < dem / 15)
            cur_page++;
    }
    // if(CheckCollisionPointRec(GetMousePosition(),{10, 10, 90, 40})){
    //     DrawRectangleLinesEx({10, 10, 90, 40}, 3, YELLOW);
    //     if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !press_Back_Button){
    //         press_Back_Button = true;
    //     }
    // }
    // if(press_Back_Button){
    //     GiaoDienDanhSachMonSV(dsmh, dslh, ma_lop_muon_in, dsch);
    //     if(IsKeyPressed(KEY_ESCAPE)){
    //         press_Back_Button = false;
    //     }
    // }
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
void SpecialInputBox(Text &text, Vector2 &mouse_pos, Rectangle box, bool &clicked){
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        mouse_pos = GetMousePosition();

    if (CheckCollisionPointRec(mouse_pos, box))
        clicked = true;
    else
        clicked = false;

    if (clicked)
    {
        DrawRectangleRec({box.x - 5, box.y - 5, box.width + 5, box.height + 5}, LIGHTGRAY);
        int key = GetCharPressed();
        while (key > 0)
        {
            int length = 0;
            const char *utf8 = CodepointToUTF8(key, &length);
            std::string str_utf8(utf8);
            str_utf8.resize(length);
            if (length > 1)
            {
                uint64_t how_many_to_remove = find(text.data, viet_key[str_utf8][0]);
                if (how_many_to_remove == std::string::npos)
                    how_many_to_remove = text.data.length();
                text.data.resize(how_many_to_remove);
                text.text_layout.resize(how_many_to_remove);
            }
            text.data += str_utf8;
            text.text_layout.push_back(length);
            if (length > 1)
            {
                for (int i = 0; i < length - 1; i++)
                    text.text_layout.push_back(0);
            }

            key = GetCharPressed();
        }

        if (IsKeyDown(KEY_BACKSPACE) && text.text_layout.size() > 0)
        {
            while (text.text_layout.back() == 0)
                text.text_layout.pop_back();
            for (int i = 0; i < text.text_layout.back(); i++)
                text.data.pop_back();
            text.text_layout.pop_back();
        }
    }

    DrawRectangleLinesEx({box.x - 5, box.y - 5, box.width + 5, box.height + 5}, 3, BLACK);
    DrawTextBoxed(font, text.data.c_str(), box, 30, 3, true, BLACK);
}
void SauKhiAnVaoNganHangCauHoi(DanhSachCauHoi &dsch){
    const int screenWidth = 1500;
    const int screenHeight = 800;
    SetWindowPosition(GetMonitorWidth(0)/2 - screenWidth/2, GetMonitorHeight(0)/2 - screenHeight/2);
    SetWindowSize(screenWidth, screenHeight);
    Button back_btn_in_NgHCH = {{BotLeft().x + 50, BotLeft().y - 90, 100, 50}, "Back", WHITE, BLUE, font};
    bool is_back_btn_in_NgHCH_pressed = false;

    Button next_page_btn = {{BotRight().x - 110, BotRight().y - 90, 100, 50}, "Next", WHITE, BLUE, font};
    bool is_next_page_btn_pressed = false;

    Button previous_page_btn = {{BotRight().x - 220, BotRight().y - 90, 100, 50}, "Prev", WHITE, BLUE, font};
    bool is_previous_page_btn_pressed = false;

    Button add_btn = {{250, 25, 100, 50}, "Add", WHITE, BLUE, font};
    bool is_add_btn_pressed = false;

    InputBox search_ma_mon = {{25, 25, 200, 50}, BLACK, BLUE, LIGHTGRAY, font};

    InputBox input_ma_mon = {{25 + MeasureTextEx(font, "Mã môn: ", 30, 3).x, 25, 200, 50}, BLACK, BLUE, LIGHTGRAY, font};
    InputBox input_dap_an = {{25 + MeasureTextEx(font, "Đáp án: ", 30, 3).x + input_ma_mon.box.width + 200, 25, 200, 50}, BLACK, BLUE, LIGHTGRAY, font};

    int page = 1;
    Queue<Node*> q;
    int max_page = 10000;
    int offset = 0;

    int scene_in_NgHCH = 0;
    Text text_noi_dung;
    Text text_cau_a;
    Text text_cau_b;
    Text text_cau_c;
    Text text_cau_d;

    Node *CauHoiDangChon = nullptr;

    while(!is_close_icon_pressed){
        BeginDrawing();
            ClearBackground(WHITE);
            switch(scene_in_NgHCH){
                case 0:{
                    q.push(dsch.getRoot());

                    back_btn_in_NgHCH.run(is_back_btn_in_NgHCH_pressed);
                    next_page_btn.run(is_next_page_btn_pressed);
                    previous_page_btn.run(is_previous_page_btn_pressed);
                    search_ma_mon.run(global_mouse_pos);
                    add_btn.run(is_add_btn_pressed);
                    DrawRectangleLinesEx(search_ma_mon.box, 3, BLACK);
                    DrawRectangleLinesEx(add_btn.box, 3, BLACK);

                    if(is_add_btn_pressed){
                        scene_in_NgHCH = 1;
                        is_add_btn_pressed = false;
                    }

                    DrawRectangleLinesEx({5, 100, 1490, 600}, 3, BLACK);
                    for (int i = 0; i < 8; i++)
                    {
                        DrawLineEx(Vector2{5, 175 + 75 * i}, Vector2{1490, 175 + 75 * i}, 2, BLACK);
                    }

                    DrawTextEx(font, "STT", {(5 + 100) / 2 - (MeasureTextEx(font, "STT", 30, 3).x / 2), 124}, 30, 3, BLACK);
                    DrawTextEx(font, "Mã môn", {(100 + 273) / 2 - (MeasureTextEx(font, "Mã môn", 30, 3).x / 2), 124}, 30, 3, BLACK);
                    DrawTextEx(font, "ID", {(273 + 473) / 2 - (MeasureTextEx(font, "ID", 30, 3).x / 2), 124}, 30, 3, BLACK);
                    DrawTextEx(font, "Câu hỏi", {(473 + 1300) / 2 - (MeasureTextEx(font, "ID", 30, 3).x / 2), 124}, 30, 3, BLACK);
                    DrawTextEx(font, "Đáp án đúng", {(1300 + 1490) / 2 - (MeasureTextEx(font, "Đáp án đúng", 30, 3).x / 2), 124}, 30, 3, BLACK);
                    DrawLineEx({100, 100}, {100, 700}, 2, BLACK);
                    DrawLineEx({273, 100}, {273, 700}, 2, BLACK);
                    DrawLineEx({473, 100}, {473, 700}, 2, BLACK);
                    DrawLineEx({1300, 100}, {1300, 700}, 2, BLACK);

                    // int index = 7 * (page - 1);
                    if (is_next_page_btn_pressed && page < max_page)
                    {
                        page++;
                        // is_next_page_btn_pressed = false;
                    }
                    if (is_previous_page_btn_pressed && page > 1)
                    {
                        page--;
                        // is_previous_page_btn_pressed = false;
                    }
                    offset = 0;

                    for (int place = 0, index = 1; !q.empty(); index++)
                    {
                        Node *temp = q.pop();
                        if (temp->left != nullptr)
                        {
                            q.push(temp->left);
                        }
                        if (temp->right != nullptr)
                        {
                            q.push(temp->right);
                        }
                        if (q.empty())
                        {
                            if ((float(index - offset) / 7.0f) == int(float(index - offset) / 7.0f))
                                max_page = (index - offset) / 7;
                            else
                                max_page = (index - offset) / 7 + 1;
                            // if(max_page - 1 <= )
                        }

                        if (search_ma_mon.text.data != "")
                        {
                            if (strstr(temp->data.ma_mon_hoc, search_ma_mon.text.data.c_str()) == nullptr)
                            {
                                offset++;
                                continue;
                            }
                        }

                        if (is_next_page_btn_pressed || is_previous_page_btn_pressed)
                        {
                            place = 0;
                            is_next_page_btn_pressed = is_previous_page_btn_pressed = false;
                        }

                        std::string question = temp->data.noi_dung;
                        for (int i = 1; i < question.length(); i++)
                        {
                            if (MeasureTextEx(font, question.substr(0, i).c_str(), 30, 3).x + 473 > 1260)
                            {
                                question = question.substr(0, i - 1);
                                question += "...";
                                break;
                            }
                        }

                        if (index - offset >= 7 * (page - 1) + 1 && index - offset <= 7 * page)
                        {
                            DrawTextEx(font, std::to_string(index - offset).c_str(), {(5 + 100) / 2 - (MeasureTextEx(font, std::to_string(index - offset).c_str(), 30, 3).x / 2), 199 + 75 * (place % 7)}, 30, 3, BLACK);
                            DrawTextEx(font, (char *)(temp->data.ma_mon_hoc), Vector2{100 + 5, 199 + 75 * (place % 7)}, 30, 3, BLACK);
                            DrawTextEx(font, std::to_string(temp->data.Id).c_str(), Vector2{273 + 5, 199 + 75 * (place % 7)}, 30, 3, BLACK);
                            DrawTextEx(font, question.c_str(), Vector2{473 + 5, 199 + 75 * (place % 7)}, 30, 3, BLACK);
                            DrawTextEx(font, temp->data.dap_an.c_str(), Vector2{(1300 + 1490) / 2 - MeasureTextEx(font, temp->data.dap_an.c_str(), 30, 3).x / 2, 199 + 75 * (place % 7)}, 30, 3, BLACK);
                            if (CheckCollisionPointRec(GetMousePosition(), {5, 175 + 75 * (place % 7), 1490, 75}))
                            {
                                DrawRectangleLinesEx({5, 175 + 75 * (place % 7), 1490, 75}, 2, GREEN);
                                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                                {
                                    CauHoiDangChon = temp;
                                    scene_in_NgHCH = 2;
                                }
                            }
                        }


                        place++;
                    }

                    DrawTextEx(font, std::to_string(page).c_str(), {(BotRight().x - 110 + BotRight().x - 220) / 2 - (MeasureTextEx(font, std::to_string(page).c_str(), 30, 3).x / 2) - 110, BotRight().y - 90 + 10}, 30, 3, BLACK);
                    DrawTextEx(font, "/", {(BotRight().x - 110 + BotRight().x - 220) / 2 - (MeasureTextEx(font, "/", 30, 3).x / 2) - 95, BotRight().y - 90 + 10}, 30, 3, BLACK);
                    DrawTextEx(font, std::to_string((max_page * 7 + 1) / 7).c_str(), {(BotRight().x - 110 + BotRight().x - 220) / 2 - (MeasureTextEx(font, std::to_string((max_page * 7 + 1) / 7).c_str(), 30, 3).x / 2) - 76, BotRight().y - 90 + 10}, 30, 3, BLACK);

                    if (is_back_btn_in_NgHCH_pressed)
                    {
                        scene_stack.push(Main_GV);
                        goto back_label;
                        break;
                    }
                    break;
                }

                case 1:{
                    Rectangle close_button = {TopRight().x - 50, TopRight().y + 10, 40, 40};
                    DrawRectangleRec(close_button, RED);
                    DrawRectangleLinesEx(close_button, 2, BLACK);
                    DrawTextEx(font, "X", {TopRight().x - 50 + 40 / 2 - MeasureTextEx(font, "X", 30, 3).x / 2, TopRight().y + 10 + 40 / 2 - MeasureTextEx(font, "X", 30, 3).y / 2}, 30, 3, BLACK);

                    Rectangle save_button = {BotCenter().x - 100/2, BotCenter().y - 60, 100, 50};
                    DrawRectangleRec(save_button, GREEN);
                    DrawRectangleLinesEx(save_button, 2, BLACK);
                    DrawTextEx(font, "Save", {BotCenter().x - MeasureTextEx(font, "Save", 30, 3).x / 2, BotCenter().y - 60/2 - MeasureTextEx(font, "Lưu", 30, 3).y / 2}, 30, 3, BLACK);

                    bool is_special_noi_dung_clicked = false;
                    bool is_special_cau_a_clicked = false;
                    bool is_special_cau_b_clicked = false;
                    bool is_special_cau_c_clicked = false;
                    bool is_special_cau_d_clicked = false;
                    Rectangle noi_dung_text_box = {50, 100, 1490 - 80, 200};
                    Rectangle cau_a = {50, 350, 1490 - 80, 70};
                    Rectangle cau_b = {50, 450, 1490 - 80, 70};
                    Rectangle cau_c = {50, 550, 1490 - 80, 70};
                    Rectangle cau_d = {50, 650, 1490 - 80, 70};

                    DrawTextEx(font, "Mã môn: ", {25, TopLeft().y + 35}, 30, 3, BLACK);
                    input_ma_mon.run(global_mouse_pos);
                    DrawTextEx(font, "Đáp án: ", {25 + 400, TopLeft().y + 35}, 30, 3, BLACK);
                    input_dap_an.run(global_mouse_pos);

                    SpecialInputBox(text_noi_dung, global_mouse_pos, noi_dung_text_box, is_special_noi_dung_clicked);
                    SpecialInputBox(text_cau_a, global_mouse_pos, cau_a, is_special_cau_a_clicked);
                    DrawTextEx(font, "A. ", {15, 350}, 30, 3, BLACK);
                    SpecialInputBox(text_cau_b, global_mouse_pos, cau_b, is_special_cau_b_clicked);
                    DrawTextEx(font, "B. ", {15, 450}, 30, 3, BLACK);
                    SpecialInputBox(text_cau_c, global_mouse_pos, cau_c, is_special_cau_c_clicked);
                    DrawTextEx(font, "C. ", {15, 550}, 30, 3, BLACK);
                    SpecialInputBox(text_cau_d, global_mouse_pos, cau_d, is_special_cau_d_clicked);
                    DrawTextEx(font, "D. ", {15, 650}, 30, 3, BLACK);

                    if(CheckCollisionPointRec(GetMousePosition(), close_button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                        scene_in_NgHCH = 0;
                        break;
                    }

                    if(CheckCollisionPointRec(GetMousePosition(), save_button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                        if(text_noi_dung.data != "" && text_cau_a.data != "" && text_cau_b.data != "" && text_cau_c.data != "" && text_cau_d.data != "" && input_ma_mon.text.data != "" && input_dap_an.text.data != ""){
                            CauHoi cau_hoi;

                            strncpy(cau_hoi.ma_mon_hoc, input_ma_mon.text.data.c_str(), 15);
                            cau_hoi.noi_dung = text_noi_dung.data;
                            cau_hoi.dap_an_a = text_cau_a.data;
                            cau_hoi.dap_an_b = text_cau_b.data;
                            cau_hoi.dap_an_c = text_cau_c.data;
                            cau_hoi.dap_an_d = text_cau_d.data;
                            cau_hoi.dap_an = input_dap_an.text.data;
                            dsch.insert(cau_hoi);
                            scene_in_NgHCH = 0;
                            break;
                        }
                    }
                    break;
                }
                case 2:{
                    CauHoi question = CauHoiDangChon->data;
                    Rectangle detail_question_box = {0, 0, 1500, 800};
                    DrawRectangleRec(detail_question_box, WHITE);

                    bool close_detail_question = false;
                    Button back_in_detail_question = Button({BotCenter().x - 200 / 2, BotCenter().y - 50 / 2 - 50, 200, 50}, "Back", WHITE, GRAY, font);
                    back_in_detail_question.run(close_detail_question);

                    Vector2 starting_point = MeasureTextEx(font, (std::to_string(question.Id) + ".").c_str(), 30, 3);
                    DrawTextEx(font, (std::to_string(question.Id) + ".").c_str(), {25, 25}, 30, 3, BLACK);
                    DrawRectangleLinesEx({23, 23, 1490 - 23, 800 - 30}, 2, BLACK);
                    DrawTextBoxed(font, question.noi_dung.c_str(), {starting_point.x + 25 + 5, 25, 1490 - starting_point.x - 10, 150}, 30, 3, true, BLACK);

                    DrawTextEx(font, "A. ", {starting_point.x - 10, 150 + 25 + 5}, 30, 3, BLACK);
                    DrawTextBoxed(font, question.dap_an_a.c_str(), {starting_point.x + 25 + 5, 150 + 25 + 5, 1490 - starting_point.x - 10, 150}, 30, 3, false, BLACK);

                    DrawTextEx(font, "B. ", {starting_point.x - 10, 300 + 25 + 5}, 30, 3, BLACK);
                    DrawTextBoxed(font, question.dap_an_b.c_str(), {starting_point.x + 25 + 5, 300 + 25 + 5, 1490 - starting_point.x - 10, 150}, 30, 3, false, BLACK);

                    DrawTextEx(font, "C. ", {starting_point.x - 10, 450 + 25 + 5}, 30, 3, BLACK);
                    DrawTextBoxed(font, question.dap_an_c.c_str(), {starting_point.x + 25 + 5, 450 + 25 + 5, 1490 - starting_point.x - 10, 150}, 30, 3, false, BLACK);

                    DrawTextEx(font, "D. ", {starting_point.x - 10, 600 + 25 + 5}, 30, 3, BLACK);
                    DrawTextBoxed(font, question.dap_an_d.c_str(), {starting_point.x + 25 + 5, 600 + 25 + 5, 1490 - starting_point.x - 10, 150}, 30, 3, false, BLACK);

                    if (close_detail_question)
                    {
                        close_detail_question = false;
                        scene_in_NgHCH = 0;
                    }
                    break;
                }
            }
         EndDrawing();
         if (WindowShouldClose())
        {
            is_close_icon_pressed = true;
        }

        if(IsKeyPressed(KEY_ESCAPE)){
            break;
        }

        is_next_page_btn_pressed = false;
        is_previous_page_btn_pressed = false;
        is_back_btn_in_NgHCH_pressed = false;
    }
    back_label:
    return;
}