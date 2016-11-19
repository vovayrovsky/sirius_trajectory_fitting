#include "TXLib.h"

int main()
{
srand(time(NULL));
FILE* outt = fopen("DATA.dat", "w");
assert (outt);

double v0 = 5.2, new_g = 9.8;

printf ("In your universe G = %lg(g = %lg), v0 = %lg", (new_g*6356*6356)/(5.9726*pow(10,24)), new_g, v0);

double t = 0.0;
while (t < 2000)
    {
    double y_point = v0*t - new_g*t*t/2;

    fprintf (outt, "%lg %lg ", y_point + random(-0.01, +0.01), t);

    if (y_point < 0) break;

    t += 0.01;
    }

fprintf (outt, "%ld ", -1000);
fclose(outt);

return 0;
}
