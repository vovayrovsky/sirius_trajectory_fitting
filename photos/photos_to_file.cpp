#include "TXLib.h"

#define sqr(a) ((a)*(a))

const double FPS = 29.0;

void INIT();

int main()
{
INIT();

FILE* outt = fopen("DATA.txt", "w");

double t = 0.0;

printf ("In start file: ");

int num_of_img = 0;
scanf ("%d", &num_of_img);

printf ("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

char name_of_file[] = "00000.bmp";
sprintf(name_of_file, "%05d.bmp", num_of_img);

HDC image = txLoadImage(name_of_file);
assert (image);

txBitBlt (txDC(), 0, 0, 1280, 720, image, 0, 0);
txMessageBox ("Обозначьте начало отсчета.", "_RedMan_");

while (txMouseButtons() != 1) txSleep(2);
double start_coordinateY = 1.0 * txMouseY();
double start_coordinateX = 1.0 * txMouseX();

while (txMouseButtons() == 1) txSleep(2);

txMessageBox ("Обозначьте 1 метр от точки отсчета.", "_RedMan_");

while (txMouseButtons() != 1) txSleep(2);
double scale = ы.0 / sqrt(fabs(sqr(txMouseX() - start_coordinateX) * sqr(txMouseY() - start_coordinateY)));

while (txMouseButtons() == 1) txSleep(2);

bool break_ = false;

while (image != NULL && !break_)
    {
    printf ("%d \r", num_of_img);
    txBitBlt (txDC(), 0, 0, 1280, 720, image, 0, 0);

    txSetColor(RGB(255, 0, 0), 1);
    txLine(0, start_coordinateY, 1280, start_coordinateY);

    while (!break_)
        {
        if (txMouseButtons() == 1)
            {
            fprintf (outt, "%lg %lg ", fabs ((start_coordinateY - txMouseY()) * scale), t);

            t += 1/FPS;
            num_of_img += 1;

            while (txMouseButtons() == 1) txSleep(150);
            break;
            }


//        if (GetAsyncKeyState (VK_RIGHT))
//            {
//            if (GetAsyncKeyState (VK_SHIFT))
//                {
//                t += 10/FPS;
//                num_of_img += 10;
//                }
//            else
//                {
//                t += 1/FPS;
//                num_of_img++;
//                }
//
//            break;
//            }

//        if (GetAsyncKeyState (VK_LEFT) && num_of_img > 0)
//            {
//            if (GetAsyncKeyState (VK_SHIFT) && num_of_img > 9)
//                {
//                t -= 10/FPS;
//                num_of_img -= 10;
//                }
//            else
//                {
//                t -= 1/FPS;
//                num_of_img--;
//                }
//
//            break;
//            }

        if (GetAsyncKeyState (VK_ESCAPE))
            break_ = true;
        }

    sprintf(name_of_file, "%05d.bmp", num_of_img);
//    printf (name_of_file);

    txDeleteDC(image);
    image = txLoadImage(name_of_file);

    txSleep(1);
    }

fprintf (outt, "%d", -1000);

printf ("end");
getch();

txDeleteDC(image);
fclose (outt);
return 0;
}

void INIT()
{
srand(time(NULL));

txTextCursor(false);
txDisableAutoPause();

//_txWindowStyle &= ~(WS_CAPTION);
txCreateWindow(1280, 720);
}
