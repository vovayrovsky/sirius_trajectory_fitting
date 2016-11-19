#include "TXLib.h"

// Обработка данных физического эксперимента c с помощью фиттинга функции методом Монте-Карло

using namespace std;

const int NUM_OF_PODBORS = 2500;

const double minV0      =  0.0 , maxV0      = 20.0;
const double minG       =  0.0 , maxG       = 20.0;
const double minALPH    = -1.0 , maxALPH    = 0.0;

struct Point
{
double t;
double y;
};

void    INIT();
double  calc_error(vector<Point> in, double calc_g, double calc_v0, double calc_alpha);
double  Theory(double g, double v0, double t, double alpha);
Point   Find_var(vector<Point> in);

void DrawParabula(double v0, double g, COLORREF color, double alpha, size_t size_, double Y_Scale, double X_Scale, double Y_Move, double X_Move);
void DrawInterface();
void DrawPoint(vector <Point> points, double Y_Scale, double X_Scale, double Y_Move, double X_Move);
void OutNum(double best_g, double best_v0, double best_alpha);
void DrawDeltaSurface(double error, double max_error, double g, double v0);
int  FindMax(vector <Point> points);

int main(int argc, char** argv)
{
if (argc < 2) return 1;
INIT();

FILE* in = fopen(argv[1], "r");
assert(in);

vector<Point> in_data;

Point buf = {};

while (true)
    {
    fscanf(in, "%lg ", &(buf.y));
    if ((int)buf.y == -1000) break;

    fscanf(in, "%lg ", &(buf.t));

    in_data.push_back(buf);
    }

buf = Find_var(in_data);

txSleep(200);
getch();

return 0;
}

Point Find_var(vector<Point> in)
{
txSetFillColor(TX_BLACK);
txClear();
double calc_g  = random(minG, maxG);
double calc_v0 = random(minV0, maxV0);
double calc_alpha = random(minALPH, maxALPH) /*0.0*/;

double min_error = calc_error(in, calc_g, calc_v0, calc_alpha);
double min_g = calc_g, min_v0 = calc_v0, min_alpha = calc_alpha;

double max_error = calc_error(in, 20, 20, 0);

HDC Interface = txLoadImage("background.bmp");
assert(Interface);

HDC Image = txLoadImage("image.bmp");
assert(Image);

HDC Images[32] = {};

char str[128] = "video\\00000102.bmp";

txSetColor(RGB(255, 0, 0));
txSetFillColor(RGB(255, 0, 0));

for (int i = 0; i < 32; i++)
    {
    txRectangle(0, 0, i*(1920/32), 1280);

    sprintf(str, "video\\%05d.bmp", i + 102);

    Images[i] = txLoadImage(str);
    assert(Images[i]);
    }

txSetColor(TX_BLACK);
txSetFillColor(TX_BLACK);
txClear();

int num_of_max_y = FindMax(in);

double Y_Scale = 739/in[num_of_max_y].y/6;
double X_Scale = 739/in[in.size()-1].t;
double Y_Move  = 0;
double X_Move  = 0;

FILE* config = fopen("config.cfg", "r");

//printf("%x\n", config);

if (config != NULL)
    {
//    printf("bbb\n");
    fscanf(config, "%lg %lg %lg %lg", &Y_Scale, &X_Scale, &Y_Move, &X_Move);

//    printf("ccc\n");
    fclose(config);
    }

//printf("aaa");

double Alpha_ = 1.0;

bool alpha_blend = true;

if (txAlphaBlend(txDC(), 1, 1, 1, 1, Images[0], 0, 0, 0) == 0)
    alpha_blend = false;

while(!GetAsyncKeyState(VK_CONTROL) || !GetAsyncKeyState(VK_SHIFT) || !GetAsyncKeyState(VK_MENU) || !GetAsyncKeyState('Q'))
    txSleep(200);

bool video = true;

for (int i = 0; i < NUM_OF_PODBORS && !(GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState('C') && GetAsyncKeyState(VK_SHIFT)) ; i++)
    {
//    printf("%lg, %lg, %lg, %lg\r", Y_Scale, X_Scale, Y_Move, X_Move);
    txBegin();

    if (GetAsyncKeyState(VK_TAB))
        {
        switch(video)
            {
            case true:  video = false; break;
            case false: video = true ; break;
            }

        while(GetAsyncKeyState(VK_TAB))
            txSleep(20);

        txSleep(20);
        }

    if (GetAsyncKeyState(VK_CONTROL))
        {
        if (GetAsyncKeyState(VK_SHIFT))
            {
            if (GetAsyncKeyState(VK_UP))
                Y_Scale++;
            if (GetAsyncKeyState(VK_DOWN))
                Y_Scale--;

            if (GetAsyncKeyState(VK_LEFT))
                X_Scale--;
            if (GetAsyncKeyState(VK_RIGHT))
                X_Scale++;
            }
        else
            {
            if (GetAsyncKeyState(VK_UP))
                Y_Move++;
            if (GetAsyncKeyState(VK_DOWN))
                Y_Move--;

            if (GetAsyncKeyState(VK_LEFT))
                X_Move--;
            if (GetAsyncKeyState(VK_RIGHT))
                X_Move++;
            }
        }
    else
        {
        if (GetAsyncKeyState(VK_UP) && Alpha_ < 1.0)
            Alpha_ += 0.1;
        if (GetAsyncKeyState(VK_DOWN) && Alpha_ > 0.0)
            Alpha_ -= 0.1;
        }

    txSetColor(TX_BLACK, 0);
    txSetFillColor(TX_BLACK);
    txRectangle(73, 277, 813, 1015);

    double error = calc_error(in, calc_g, calc_v0, calc_alpha);

    DrawDeltaSurface(error, max_error, calc_g, calc_v0);

    min_g       = min_error > error ? calc_g     : min_g;
    min_v0      = min_error > error ? calc_v0    : min_v0;
    min_alpha   = min_error > error ? calc_alpha : min_alpha;
    min_error   = min_error > error ? error      : min_error;

//    max_error   = max_error < error ? error : max_error;

//    if (GetAsyncKeyState(VK_UP)    && calc_g < maxG)
//        calc_g += 0.1;
//    if (GetAsyncKeyState(VK_DOWN)  && calc_g > minG)
//        calc_g -= 0.1;
//
//    if (GetAsyncKeyState(VK_RIGHT) && calc_v0 < maxV0)
//        calc_v0 += 0.1;
//    if (GetAsyncKeyState(VK_LEFT)  && calc_v0 > minV0)
//        calc_v0 -= 0.1;
//
//    if (GetAsyncKeyState(VK_SPACE) && calc_alpha < maxALPH)
//        calc_alpha += 0.1;
//    if (GetAsyncKeyState(VK_SHIFT) && calc_alpha > minALPH)
//        calc_alpha -= 0.1;

    calc_v0     = random(minV0, maxV0);
    calc_g      = random(minG , maxG);
    calc_alpha  = random(minALPH , maxALPH);

//    printf ("%lg, %lg, %lg\r", calc_g, calc_v0, calc_alpha);

    if (GetAsyncKeyState(VK_ESCAPE))
        txBitBlt(txDC(), 73, 273, 742, 742, Image, 0, 0);
    else if (alpha_blend)
        txAlphaBlend(txDC(), 73, 273, 742, 742, Images[i%32], 0, 0, Alpha_);
    else if (video)
        txBitBlt(txDC(), 73, 273, 742, 742, Images[i%32], 0, 0);

    txSetColor(RGB(14, 237, 75), 1);
    txLine(73 + X_Move, 1015 - (739/6) - Y_Move, 811 + X_Move, 1015 - (739/6) - Y_Move);

    DrawParabula(min_v0 , min_g , RGB(128, 0, 0)    , min_alpha , 6, Y_Scale, X_Scale, Y_Move, X_Move);
    DrawParabula(calc_v0, calc_g, RGB(0  , 0, 128)  , calc_alpha, 6, Y_Scale, X_Scale, Y_Move, X_Move);
    DrawParabula(min_v0 , min_g , RGB(255, 124, 128), min_alpha , 2, Y_Scale, X_Scale, Y_Move, X_Move);
    DrawParabula(calc_v0, calc_g, RGB(0  , 176, 240), calc_alpha, 2, Y_Scale, X_Scale, Y_Move, X_Move);

    DrawPoint(in, Y_Scale, X_Scale, Y_Move, X_Move);

    txTransparentBlt(txDC(), 0, 0, 1920, 1080, Interface, 0, 0, RGB(69, 194, 75));

    txSetColor(RGB(192, 0, 0), 0);
    txSetFillColor(RGB(192, 0, 0));
    txRectangle (74, 149, 74 + (1778*i)  / NUM_OF_PODBORS, 157);

    OutNum(min_g, min_v0, min_alpha);

    while(GetAsyncKeyState (VK_SPACE)) txSleep(10);

    txSleep(0);
    txEnd();
    }

for (int i = 0; i < 32; i++)
    {
    txDeleteDC(Images[i]);
    }

getch();
txDeleteDC(Interface);

config = fopen("config.cfg", "w");

if (config != NULL)
    fprintf(config, "%lg %lg %lg %lg", Y_Scale, X_Scale, Y_Move, X_Move);

fclose(config);

return {min_g, min_v0};
}

void DrawDeltaSurface(double error, double max_error, double g, double v0)
{
txSetColor(RGB (47, 85, 151), 0);
txSetFillColor(RGB (47, 85, 151));

//printf("%lg, %lg, %lg, %lg, %lg", max_error, 0.25*max_error, 0.05*max_error, 0.015*max_error, 0.002*max_error);
//getch();

if (error / max_error < 0.25)
    {
    txSetColor(RGB (0, 176, 240), 0);
    txSetFillColor(RGB (0, 176, 240));
    }

if (error / max_error < 0.05)
    {
    txSetColor(RGB (112, 173, 71), 0);
    txSetFillColor(RGB (112, 173, 71 ));
    }

if (error / max_error < 0.015)
    {
    txSetColor(RGB (255, 217, 102), 0);
    txSetFillColor(RGB (255, 217, 102));
    }

if (error / max_error < 0.002)
    {
    txSetColor(RGB (255, 0, 0), 0);
    txSetFillColor(RGB (255, 0, 0));
    }

//if (1386 + (739/fabs(maxG - minG)) * g > 1116 && 1386 + (739/fabs(maxG - minG)) * g < 1855)
    txCircle (1117 + (739/fabs(maxG - minG)) *  (g - minG), 1017 - (739/fabs(maxV0 - minV0)) * (v0 - minV0), 5);
//
//    txEllipse  (1386 + (739/100) * g - (739/100), 1017 - (742/500) * v0 - 0.5*(742/500),
//                1386 + (739/100) * g + (739/100), 1017 - (742/500) * v0 + 0.5*(742/500));

txSetColor(0, 1);

char buf[15] = "";
sprintf (buf, "%d", (int) maxV0);
txTextOut (1118, 250, buf);

sprintf (buf, "%d", (int) minV0);
txTextOut (1100, 1030, buf);

sprintf (buf, "%d", (int) minG);
txTextOut (1021, 975, buf);

sprintf (buf, "%d", (int) maxG);
txTextOut (1860, 985, buf);
}

int FindMax(vector <Point> points)
{
int max_y = 0;
for (u_int i = 0; i < points.size(); i++)
    {
    max_y = points[i].y > points[max_y].y ? i : max_y;
    }

return max_y;
}

void OutNum(double best_g, double best_v0, double best_alpha)
{
char out_best_g [10]    = "";
char out_best_v0[10]    = "";
char out_best_alpha[10] = "";

sprintf(out_best_g      , "%lg",  best_g);
sprintf(out_best_v0     , "%lg",  best_v0);
sprintf(out_best_alpha  , "%lg",  best_alpha);

txSetColor(RGB(153, 255, 153), 1);
txSelectFont("Lucida Console", 25, 20, 5);

txDrawText(863, 440, 1054, 497, out_best_g);

txDrawText(863, 578, 1054, 635, out_best_v0);

txDrawText(863, 720, 1054, 777, out_best_alpha);

//txSetColor (RGB(190, 12, 10), 7);
//txSelectFont("Calibri", 38, 25, 50);
//txDrawText(1250, 10, 1920, 200, "Юровский Владимир,\n Лицей \"Вторая Школа\",\n       Москва");
}

void DrawPoint(vector <Point> points, double Y_Scale, double X_Scale, double Y_Move, double X_Move)
{
for (u_int i = 0; i < points.size(); i++)
    if (1015 - points[i].y*Y_Scale - 739/6 - Y_Move > 277 && 1015 - points[i].y*Y_Scale - 739/6 - Y_Move < 1015 &&
        73 + points[i].t*X_Scale + X_Move < 811)
        {
        txSetColor(RGB(128, 0, 0), 1);
        txSetFillColor(RGB(128, 0, 0));
        txCircle(73 + points[i].t*X_Scale + X_Move, 1015 - points[i].y*Y_Scale - 739/6 - Y_Move, 6);

        txSetColor(RGB(255, 128, 128), 1);
        txSetFillColor(RGB(255, 0, 0));
        txCircle(73 + points[i].t*X_Scale + X_Move, 1015 - points[i].y*Y_Scale - 739/6 - Y_Move, 4);
        }
}

void DrawParabula(double v0, double g, COLORREF color, double alpha, size_t size_, double Y_Scale, double X_Scale, double Y_Move, double X_Move)
{
txSetColor    (color, size_);
txSetFillColor(color);

int x       = 0, y_     = 0;
int x_old   = x, y_old  = y_;

for (double i = 0; i < 1000; i += 0.001)
    {
    x_old = x;
    y_old = y_;

    double y = Theory(g, v0, i, alpha);

    x  = 73 + i*X_Scale + X_Move;
    y_ = 1015 - y*Y_Scale - 739/6 - Y_Move;


    if (i == 0.0) continue;

    txLine(x, y_, x_old, y_old);

    if (y_ <= 0 || y_ >= 1080) break;
    if (x  >  811) break;
    }
}

#define sqr(a) ((a)*(a))

double Theory(double g, double v0, double t, double alpha)
{
return v0*t - g*t*t/2 - v0*alpha*t*t/2;
}

double calc_error(vector<Point> in, double calc_g, double calc_v0, double calc_alpha)
{
double answ = 0;

for (u_int i = 0; i < in.size(); i++)
    {
    assert(in.size() - i);

    answ += sqr(Theory(calc_g, calc_v0, in[i].t, calc_alpha) - in[i].y);
    }

return answ;
}

void INIT()
{
srand(time(NULL));

txTextCursor(false);
txDisableAutoPause();

_txWindowStyle &= ~(WS_CAPTION);
txCreateWindow(1920, 1080);
}
