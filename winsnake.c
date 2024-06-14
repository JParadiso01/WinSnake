#include <Windows.h>
#include <stdio.h>
#define WIDTH  800
#define HEIGHT 800
const char g_szClassName[] = "myWindowClass";

RECT snakeStart    = {.left = 0, .top = 0, .right = 25, .bottom = 25};
RECT apple         = {.left = 104, .top = 154, .right = 121, .bottom = 171};
RECT collider      = {0};
RECT snake[100]    = {0};
RECT ScoreRect     = {.left = WIDTH-100, .top = 0, .right = WIDTH-10, .bottom = HEIGHT-100};
RECT GameOverRect  = {.left = WIDTH/4, .top = HEIGHT/4, .right = WIDTH-(WIDTH/4), .bottom = HEIGHT-(HEIGHT/4)};

int Hoffset = 25;
int Yoffset = 25;
int Hdir = 0;
int Ydir = 0;

int score = 0;
WINBOOL GameOver = 0;


WINBOOL CompareRect(RECT a, RECT b){
    return a.top == b.top && a.bottom == b.bottom && a.left == b.left && a.right == b.right;
}

void AddToSnake(RECT snake[100], RECT body){
    snake[score].top = body.top;
    snake[score].bottom = body.bottom;
    snake[score].left = body.left;
    snake[score].right = body.right;

}

void ClampSnake(RECT *snakeHead){
    if (snakeHead->right > WIDTH-25){
        GameOver = 1;
    }
    if (snakeHead->bottom > HEIGHT-50){
        GameOver = 1;
    }
    if (snakeHead->left < 0){
        GameOver = 1;
    }
    if (snakeHead->top < 0){
        GameOver = 1;
    }
}

void ClampApple(RECT *apple){
    if (apple->right > WIDTH-25){
        apple->right = WIDTH-25;
        apple->left  = WIDTH-50;
    }
    if (apple->bottom > HEIGHT-50){
        apple->bottom = HEIGHT-50;
        apple->top    = HEIGHT-75;
    }
    if (apple->left < 0){
        apple->left  = 0;
        apple->right = 25;
    }
    if (apple->top < 0){
        apple->top    = 0;
        apple->bottom = 25;
    }
}

void DrawSnake(HDC screen, RECT snakeHead){
    HBRUSH Brush = CreateSolidBrush(RGB(255, 0, 0));
    SelectObject(screen, Brush);
    Rectangle(screen,snakeHead.left,snakeHead.top,snakeHead.right,snakeHead.bottom);
}

void DrawBackground(HDC screen, HBRUSH color){
    SelectObject(screen, color);
    Rectangle(screen,0,0,WIDTH,HEIGHT);
}

void DrawApple(HDC screen, RECT apple){
    HBRUSH greenBrush = CreateSolidBrush(RGB(0, 255, 0));
    SelectObject(screen, greenBrush);
    Rectangle(screen,apple.left,apple.top,apple.right,apple.bottom);
}

// Step 4: the Window Procedure
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

    switch(msg)
    {   
        case WM_KEYDOWN:
            switch (wParam){
            case VK_RIGHT:
                if (Hdir == -1){
                    break;
                }
                Ydir = 0;
                Hdir = 1;
                break;
            case VK_LEFT:
                if (Hdir == 1){
                    break;
                }
                Ydir = 0;
                Hdir = -1;
                break;
            case VK_UP:
                if (Ydir == 1){
                    break;
                }
                Hdir = 0;
                Ydir = -1;
                break;
            case VK_DOWN:
                if (Ydir == -1){
                    break;
                }
                Hdir = 0;
                Ydir = 1;
                break;
            case 0x52:
                if (GameOver){
                    memset(snake, 0, sizeof(snake));
                    Hdir = 1;
                    Ydir = 0;
                    score = 0;
                    AddToSnake(snake, snakeStart);
                    GameOver = 0;
                }
            default:
                break;
            }
        break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
        break;
        case WM_DESTROY:
            PostQuitMessage(0);
        break;
        case WM_PAINT:
        HDC screen = GetDC(hwnd);
        char scoreText[64] = {0};
        if (!GameOver){
            RECT lastSnake = snake[score];
            RECT prevSnakePOS = snake[0];
            RECT temp;

            snake[0].left   += (Hoffset*Hdir);
            snake[0].top    += (Yoffset*Ydir);
            snake[0].right  += (Hoffset*Hdir);
            snake[0].bottom += (Yoffset*Ydir);

            for (int i = 1; i <= score; i++){
                temp = snake[i];
                snake[i] = prevSnakePOS;
                prevSnakePOS = temp;
            }

            ClampSnake(&snake[0]);

            HBRUSH bgColor = CreateSolidBrush(RGB(102, 204, 255));

            DrawBackground(screen, bgColor);
            for (int i = 0; i <= score; i++){
                if (i != 0){
                    if (CompareRect(snake[0],snake[i])){
                        GameOver = 1;
                    }
                }
                DrawSnake(screen, snake[i]);
            }

            //if snake Head and apple collide change apple coords
            //weird added numbers to make apple smaller than snake Head
            WINBOOL collide = IntersectRect(&collider, &snake[0], &apple);
            if (collide){
                score++;
                apple.left   = (25 * (rand() % (WIDTH/25))  - 25)  + 4;
                apple.top    = (25 * (rand() % (HEIGHT/25)) - 25) + 4;
                apple.right  = apple.left + 17;
                apple.bottom = apple.top  + 17;
                ClampApple(&apple);
                AddToSnake(snake, lastSnake);
            }
            DrawApple(screen, apple);
            sprintf(scoreText, "SCORE: %d", score);
            DrawText(screen, scoreText,strlen(scoreText), &ScoreRect,DT_CENTER);
        }
        else{
            HBRUSH bgColor = CreateSolidBrush(RGB(255, 255, 255));
            DrawBackground(screen, bgColor);

            sprintf(scoreText, "GAME OVER\n\nYou had a score: %d\n\nPress 'R' to play again!", score);
            DrawText(screen, scoreText,strlen(scoreText), &GameOverRect,DT_CENTER);
        }
        Sleep(200-(score*5));
        break;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }

    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    AddToSnake(snake, snakeStart);
    srand(0);
    WNDCLASSEX wc;
    HWND hwnd;
    MSG Msg;

    //Step 1: Registering the Window Class
    wc.cbSize        = sizeof(WNDCLASSEX);
    wc.style         = 0;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = g_szClassName;
    wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

    if(!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Step 2: Creating the Window
    hwnd = CreateWindowEx(
        WS_EX_CLIENTEDGE,
        g_szClassName,
        "Snake",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, WIDTH, HEIGHT,
        NULL, NULL, hInstance, NULL);

    if(hwnd == NULL)
    {
        MessageBox(NULL, "Window Creation Failed!", "Error!",
            MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Step 3: The Message Loop
    while(GetMessage(&Msg, NULL, 0, 0) > 0)
    {
    TranslateMessage(&Msg);
    DispatchMessage(&Msg);
    }
    return Msg.wParam;

}