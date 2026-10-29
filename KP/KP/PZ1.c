#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <locale.h>
#include <windows.h>
#define MAX_POINTS 1000000
#define PI 3.14159265358979323846


int calculate_Y(double x, double* result)
{
    double exponent, argument;

    if (!isfinite(x))
        return 0;

    argument = x * sin(PI / 4.0);
    exponent = x * cos(PI / 4.0);

    if (exponent > log(DBL_MAX) || exponent < log(DBL_MIN))
        return 0;

    *result = cos(argument) * exp(exponent);

    return isfinite(*result);
}

int calculate_V(double x, double* result)
{
    double phi, numerator, denominator;

    if (!isfinite(x))
        return 0;

    if (x < 1.0)
        phi = 2.0 + x;
    else {
        double s = sin(x);
        phi = s * s;
    }

    if (fabs(x) > sqrt(DBL_MAX - 5.0))
        return 0;

    denominator = x * x + 5.0;
    numerator = 2.8 * phi + 1.13 * cos(sqrt(fabs(x)));

    if (!isfinite(numerator) ||
        !isfinite(denominator) ||
        denominator == 0.0)
        return 0;

    *result = numerator / denominator;

    return isfinite(*result);
}

int read_interval(double* xn, double* xk, double* h,
    int* interval_type)
{
    printf("Введите xn, xk и шаг h: ");

    if (scanf("%lf %lf %lf", xn, xk, h) != 3) {
        printf("Ошибка ввода.\n");
        return 0;
    }

    if (!isfinite(*xn) || !isfinite(*xk) ||
        !isfinite(*h) || *xn >= *xk || *h <= 0.0) {
        printf("Ошибка: xn < xk, h > 0.\n");
        return 0;
    }

    printf("\nВыберите тип промежутка:\n");
    printf("1 — (xn, xk)\n");
    printf("2 — [xn, xk)\n");
    printf("3 — (xn, xk]\n");
    printf("4 — [xn, xk]\n");
    printf("Ваш выбор: ");

    if (scanf("%d", interval_type) != 1 ||
        *interval_type < 1 || *interval_type > 4) {
        printf("Неверный тип промежутка.\n");
        return 0;
    }

    if ((*xk - *xn) / *h > (double)MAX_POINTS) {
        printf("Слишком много точек. Увеличьте шаг h.\n");
        return 0;
    }

    return 1;
}

int is_left_closed(int type)
{
    return type == 2 || type == 4;
}

int is_right_closed(int type)
{
    return type == 3 || type == 4;
}

void tabulate(int function_number)
{
    double xn, xk, h, x, y;
    int type, i, count = 0;

    int (*function)(double, double*);

    function = (function_number == 1)
        ? calculate_Y
        : calculate_V;

    if (!read_interval(&xn, &xk, &h, &type))
        return;

    printf("\n%16s | %20s\n",
        "x",
        function_number == 1 ? "Y(x)" : "V(x)");

    printf("-----------------+----------------------\n");

    for (i = 0; i <= MAX_POINTS; ++i) {
        x = xn + (double)i * h;

        if (!isfinite(x) || x > xk)
            break;

        if (i == 0 && !is_left_closed(type))
            continue;

        if (x == xk && !is_right_closed(type))
            continue;

        if (!function(x, &y)) {
            printf("%16.8g | %s\n",
                x, "значение вне диапазона");
        }
        else {
            printf("%16.8g | %20.10g\n", x, y);
        }

        ++count;
    }

    if (count == 0)
        printf("На заданной сетке нет точек промежутка.\n");
}


void plot_function(void)
{
    double xn, xk, h, x, y;
    double ymin = DBL_MAX;
    double ymax = -DBL_MAX;
    double px, py;
    int type, i, function_number;
    int count = 0;

    int (*function)(double, double*);

    FILE* file;

    const int width = 900;
    const int height = 600;
    const int margin = 60;

    printf("Какую функцию построить?\n");
    printf("1 — Y(x)\n");
    printf("2 — V(x)\n");
    printf("Ваш выбор: ");

    if (scanf("%d", &function_number) != 1 ||
        (function_number != 1 && function_number != 2)) {
        printf("Неверный выбор функции.\n");
        return;
    }

    if (!read_interval(&xn, &xk, &h, &type))
        return;

    function = (function_number == 1)
        ? calculate_Y
        : calculate_V;

    for (i = 0; i <= MAX_POINTS; i++) {
        x = xn + (double)i * h;

        if (!isfinite(x) || x > xk)
            break;

        if (i == 0 && !is_left_closed(type))
            continue;

        if (x == xk && !is_right_closed(type))
            continue;

        if (function(x, &y)) {
            if (y < ymin) ymin = y;
            if (y > ymax) ymax = y;
            count++;
        }
    }

    if (count == 0) {
        printf("Нет точек для построения графика.\n");
        return;
    }

    if (ymax == ymin) {
        ymin -= 1.0;
        ymax += 1.0;
    }

    {
        double delta = (ymax - ymin) * 0.05;
        ymin -= delta;
        ymax += delta;
    }

    file = fopen("graph.svg", "w");

    if (file == NULL) {
        printf("Не удалось создать файл graph.svg.\n");
        return;
    }

    fprintf(file,
        "<svg xmlns=\"http://www.w3.org/2000/svg\" "
        "width=\"900\" height=\"600\" viewBox=\"0 0 900 600\">\n");

    fprintf(file, "<rect width=\"900\" height=\"600\" fill=\"white\"/>\n");

    fprintf(file,
        "<text x=\"450\" y=\"30\" text-anchor=\"middle\" "
        "font-size=\"22\">%s</text>\n",
        function_number == 1 ? "Y(x)" : "V(x)");

    for (i = 0; i <= 10; i++) {
        px = margin + i * (width - 2 * margin) / 10.0;
        py = margin + i * (height - 2 * margin) / 10.0;

        fprintf(file,
            "<line x1=\"%.2f\" y1=\"60\" x2=\"%.2f\" y2=\"540\" "
            "stroke=\"#dddddd\"/>\n", px, px);

        fprintf(file,
            "<line x1=\"60\" y1=\"%.2f\" x2=\"840\" y2=\"%.2f\" "
            "stroke=\"#dddddd\"/>\n", py, py);

        fprintf(file,
            "<text x=\"%.2f\" y=\"560\" text-anchor=\"middle\" "
            "font-size=\"12\">%.3g</text>\n",
            px, xn + i * (xk - xn) / 10.0);

        fprintf(file,
            "<text x=\"50\" y=\"%.2f\" text-anchor=\"end\" "
            "font-size=\"12\">%.3g</text>\n",
            py, ymax - i * (ymax - ymin) / 10.0);
    }

    fprintf(file,
        "<rect x=\"60\" y=\"60\" width=\"780\" height=\"480\" "
        "fill=\"none\" stroke=\"black\"/>\n");

    fprintf(file,
        "<path d=\"");

    {
        int started = 0;

        for (i = 0; i <= MAX_POINTS; i++) {
            x = xn + (double)i * h;

            if (!isfinite(x) || x > xk)
                break;

            if (i == 0 && !is_left_closed(type))
                continue;

            if (x == xk && !is_right_closed(type))
                continue;

            if (!function(x, &y)) {
                started = 0;
                continue;
            }

            px = margin + (x - xn) / (xk - xn) *
                (width - 2 * margin);

            py = height - margin -
                (y - ymin) / (ymax - ymin) *
                (height - 2 * margin);

            fprintf(file, "%c %.2f %.2f ",
                started ? 'L' : 'M', px, py);

            started = 1;
        }
    }

    fprintf(file,
        "\" fill=\"none\" stroke=\"blue\" stroke-width=\"2\"/>\n");

    fprintf(file, "</svg>\n");

    fclose(file);

    printf("\nГрафик успешно построен!\n");
    printf("Файл сохранен: graph.svg\n");
    printf("Откройте graph.svg в любом браузере.\n");
}

int main(void)
{
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
    setlocale(LC_ALL, ".UTF - 8");
    int choice;
    double x, y;

    do {
        printf("\n========== МЕНЮ ==========\n");
        printf("1. Вычислить Y(x)\n");
        printf("2. Вычислить V(x)\n");
        printf("3. Табулировать Y(x)\n");
        printf("4. Табулировать V(x)\n");
        printf("5. Построить график функции\n");
        printf("6. Завершить работу\n");
        printf("Выберите пункт: ");

        if (scanf("%d", &choice) != 1) {
            printf("Ошибка ввода.\n");
            return 1;
        }

        switch (choice) {
        case 1:
            printf("Введите x: ");

            if (scanf("%lf", &x) != 1 ||
                !isfinite(x)) {
                printf("Некорректное значение x.\n");
                break;
            }

            if (calculate_Y(x, &y))
                printf("Y(%.10g) = %.15g\n", x, y);
            else
                printf("Невозможно вычислить Y(x): "
                    "выход за диапазон double.\n");

            break;

        case 2:
            printf("Введите x: ");

            if (scanf("%lf", &x) != 1 ||
                !isfinite(x)) {
                printf("Некорректное значение x.\n");
                break;
            }

            if (calculate_V(x, &y))
                printf("V(%.10g) = %.15g\n", x, y);
            else
                printf("Невозможно вычислить V(x).\n");

            break;

        case 3:
            tabulate(1);
            break;

        case 4:
            tabulate(2);
            break;

        case 5:
            plot_function();
            break;

        case 6:
            printf("Работа программы завершена.\n");
            break;

        default:
            printf("Нет такого пункта меню. "
                "Введите число от 1 до 6.\n");
        }

    } while (choice != 6);

    return 0;
}