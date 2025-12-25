//
// My own implementation of splines (C0 continuous)
//
#include <raylib.h>
#include <vector>
#include <iostream>

Font font;

const float uiWidth = 300.0f;
const float windowWidth = 1100.0f;
const float windowHeight = 800.0f;
float dotRadius = 5.0f;
float pointRadius = 3.0f;
Color pointColor = LIME;

Color pointLineColor = {40, 40, 40, 255};
const Color uiColor = {29, 29, 29, 255};
const Color textColor = {145, 145, 145, 255};
const float margin = 16.0f;
const int fontSize = 20;

const float line = windowWidth-uiWidth;

constexpr int MAX_STEPS = 150;
constexpr int MIN_STEPS = 0;
int steps = 50;

Vector2 button1 = {line + margin, margin + fontSize*12.0f};
bool doDrawLines = true;
Vector2 button2 = {line + margin, margin + fontSize*6.0f};
bool doDrawControlPoints = true;
Vector2 button3 = {line + margin, margin + fontSize*8.0f};
bool doDrawSplineDots = true;
Vector2 button4 = {line + margin, margin + fontSize*14.0f};
bool doDrawSpline = true;
Vector2 button5 = {line + margin, margin + fontSize*16.0f};
Color splineColor = BLUE;

std::vector<Vector2> dots;

// This is for dragging the dots
float DistSquared(Vector2 p, Vector2 q)
{
    float dx = p.x - q.x;
    float dy = p.y - q.y;
    return dx*dx + dy*dy;
}
float Clamp(float v, float min, float max)
{
    return (v < min) ? min : (v > max) ? max : v;
}
Vector2 m;
Vector2* activePoint = nullptr;
bool isMouseInWindow;

bool InButton(float x, float y, float by)
{
    return x >= line && y >= by && y <= by + fontSize;
}

void initWindow(); 
void eventListener();
Vector2 Lerp(Vector2 point1, Vector2 point2, float t); // Lerping between 2 points
Vector2 BezierPoint(Vector2 a, Vector2 b, Vector2 c, Vector2 d, float t); // Lerping between 3 points
void DrawBezier(Vector2 a, Vector2 b, Vector2 c, Vector2 d);
void DragPoint(Vector2 &p);
void DrawUI();

int main(void)
{
    initWindow();
    font = LoadFont("src/UbuntuMono-Regular.ttf");

    while (!WindowShouldClose())
    {
        m = GetMousePosition();
        isMouseInWindow = m.x > 0 && m.x < GetScreenWidth() - uiWidth &&
                          m.y > 0 && m.y < GetScreenHeight();
        
        for (Vector2& p : dots)
        {
            DragPoint(p);
        }

        BeginDrawing();
        eventListener();
        ClearBackground(BLACK);

        // Lines for control points
        for (size_t i = 1; i < dots.size(); i++)
        {
            if (i + 1 != dots.size() - 1 && doDrawLines)
            {            
                if ((i % 3 == 0 && i <= dots.size()) || (i % 3 == 1 && i + 1 < dots.size()))
                    DrawLineEx(dots[i], dots[i + 1], 2, pointLineColor);

                if (i % 3 == 2 && i + 1 < dots.size())
                    DrawLineEx(dots[i], dots[i + 1], 2, ColorBrightness(pointLineColor, -0.5f));
            }
        }

        // Draw the Spline (this type of spline consists of cubic Béziers)
        if (dots.size() >= 4 && doDrawSpline)
        {
            for (size_t i = 1; i+3 < dots.size(); i+=3)
            {
                DrawBezier(dots[i], dots[i+1], dots[i+2], dots[i+3]);
            }
        }

        // Draw dots on top
        for (size_t i = 1; i < dots.size(); i++)
        {
            if (i % 3 == 1 && doDrawSplineDots)
                DrawCircleV(dots[i], dotRadius, splineColor);
            else if (i % 3 != 1 && i + 1 != dots.size() && doDrawControlPoints)
                DrawCircleV(dots[i], pointRadius, pointColor);
        }

        DrawUI();
        EndDrawing();
    }

    UnloadFont(font);
    CloseWindow();
    return 0;
}

void initWindow()
{
    InitWindow(windowWidth, windowHeight, "SPLINES - C^0 CONTINUITY");
    SetTargetFPS(90);
}

void eventListener()
{
    if (IsKeyPressed(KEY_C))
        dots.clear();
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && isMouseInWindow)
    {
        dots.push_back({m.x, m.y-30});
        dots.push_back(m);
        dots.push_back({m.x, m.y+30});
    }
    if (m.x >= line &&
        m.y >= button1.y && m.y <= button1.y + fontSize)
    {
        steps += GetMouseWheelMove();
        steps = Clamp(steps, MIN_STEPS-1, MAX_STEPS+1);
    }
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        if (InButton(m.x, m.y, button2.y)) doDrawLines = !doDrawLines;
        if (InButton(m.x, m.y, button3.y)) doDrawControlPoints = !doDrawControlPoints;
        if (InButton(m.x, m.y, button4.y)) doDrawSplineDots = !doDrawSplineDots;
        if (InButton(m.x, m.y, button5.y)) doDrawSpline = !doDrawSpline;
    }
}

Vector2 Lerp(Vector2 point1, Vector2 point2, float t)
{
    return
    {
        point1.x + (point2.x - point1.x) * t,
        point1.y + (point2.y - point1.y) * t,
    };
}

Vector2 BezierPoint(Vector2 a, Vector2 b, Vector2 c, Vector2 d, float t)
{
    Vector2 q0 = Lerp(a, b, t);
    Vector2 q1 = Lerp(b, c, t);
    Vector2 q2 = Lerp(c, d, t);

    Vector2 r0 = Lerp(q0, q1, t);
    Vector2 r1 = Lerp(q1, q2, t);

    return Lerp(r0, r1, t);
}

void DrawBezier(Vector2 a, Vector2 b, Vector2 c, Vector2 d)
{
    Vector2 prev = a;

    for (int i = 1; i <= steps; i++)
    {
        float t = (float)i / steps;
        Vector2 point = BezierPoint(a, b, c, d, t);

        DrawLineEx(prev, point, 5, splineColor);
        prev = point;
    }
}

void DragPoint(Vector2& p)
{
    bool mouseOverDot = DistSquared(m, p) <= (dotRadius*2)*(dotRadius*2);
    
    if (activePoint == nullptr && mouseOverDot &&
        isMouseInWindow && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        activePoint = &p;
    }
    if (activePoint == &p)
    {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            p.x = Clamp(m.x, 0.0f, (float)GetScreenWidth() - uiWidth);
            p.y = Clamp(m.y, 0.0f, (float)GetScreenHeight());
        }
        else
            activePoint = nullptr;
    }
}

void DrawUI()
{
    DrawRectangle(line, 0, windowWidth, windowHeight, uiColor);

    DrawTextEx(font, "C: Clear Canvas", {line + margin, margin}, fontSize, 0.0f, textColor);
    DrawTextEx(font, "RMB: Place Dots", {line + margin, margin + fontSize}, fontSize, 0.0f, textColor);
    DrawTextEx(font, "LMB: Move Dots", {line + margin, margin + fontSize*2.0f}, fontSize, 0.0f, textColor);
    DrawLineEx({line, margin+fontSize*3.5f}, {windowWidth, margin + fontSize*3.5f}, 2.0f, BLACK);

    DrawTextEx(font, "Control Points:", {line + margin, margin + fontSize*4.0f}, fontSize, 0.0f, textColor);
    DrawTextEx(font, "Draw Lines?", {line + margin, margin + fontSize*5.0f}, fontSize, 0.0f, textColor);
    DrawTextEx(font, doDrawLines ? "yes" : "no", button2, fontSize, 0.0f, splineColor);
    DrawTextEx(font, "Draw Dots?", {line + margin, margin + fontSize*7.0f}, fontSize, 0.0f, textColor);
    DrawTextEx(font, doDrawControlPoints ? "yes" : "no", button3, fontSize, 0.0f, splineColor);

    DrawLineEx({line, margin+fontSize*9.5f}, {windowWidth, margin + fontSize*9.5f}, 2.0f, BLACK);

    DrawTextEx(font, "Spline:", {line + margin, margin + fontSize*10.0f}, fontSize, 0.0f, textColor);
    DrawTextEx(font, "Curvature:", {line + margin, margin + fontSize*11.0f}, fontSize, 0.0f, textColor);
    DrawTextEx(font, std::to_string(steps).c_str(), button1, fontSize, 0.0f, splineColor);
    DrawTextEx(font, "Draw Dots?", {line + margin, margin + fontSize*13.0f}, fontSize, 0.0f, textColor);
    DrawTextEx(font, doDrawSplineDots ? "yes" : "no", button4, fontSize, 0.0f, splineColor);
    DrawTextEx(font, "Draw Curve?", {line + margin, margin + fontSize*15.0f}, fontSize, 0.0f, textColor);
    DrawTextEx(font, doDrawSpline ? "yes" : "no", button5, fontSize, 0.0f, splineColor);

    DrawLineEx({line, margin + fontSize*17.5f}, {windowWidth, margin + fontSize*17.5f}, 2.0f, BLACK);

    DrawTextEx(font, "splines", {line + margin, windowHeight - margin - fontSize/1.4f}, 
               fontSize, 0.0f, ColorBrightness(uiColor, -0.4f));
}
