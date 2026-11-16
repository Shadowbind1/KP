#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <locale.h>
#include <time.h>

#define MAX_POINTS 1000000
#define PI 3.14159265358979323846

typedef double (*Function)(double);

double calculate_Y(double x);
double calculate_V(double x);
int is_left_closed(int type);
int is_right_closed(int type);
int read_interval(double* xn, double* xk, double* h, int* type);
void process_values(Function function, const char* name,
    const double* values, int count);
void plot_function(Function function, const char* name,
    double xn, double xk, double h, int type);

int main(void)
{
    int choice, function_number, mode;
    int count, i, type;
    double x, y, xmin, dx, xn, xk, h;
    double* values;
    Function function;
    const char* function_name;

    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    setlocale(LC_ALL, ".UTF-8");

    srand((unsigned int)time(NULL));

    do {
        printf("\n========== МЕНЮ ==========\n");
        printf("1. Вычислить Y(x)\n");
        printf("2. Вычислить V(x)\n");
        printf("3. Обработать набор аргументов\n");
        printf("4. Построить график функции\n");
        printf("5. Завершить работу\n");
        printf("Выберите пункт: ");

        if (scanf("%d", &choice) != 1) {
            printf("Ошибка ввода.\n");
            return 1;
        }

        switch (choice) {
        case 1:
        case 2:
            printf("Введите x: ");
            if (scanf("%lf", &x) != 1 || !isfinite(x)) {
                printf("Некорректное значение x.\n");
                break;
            }

            if (choice == 1) {
                y = calculate_Y(x);
                function_name = "Y";
            }
            else {
                y = calculate_V(x);
                function_name = "V";
            }

            if (isfinite(y))
                printf("%s(%.10g) = %.15g\n", function_name, x, y);
            else
                printf("Невозможно вычислить функцию для данного x.\n");
            break;

        case 3:
            printf("\nВыберите функцию:\n");
            printf("1 — Y(x)\n");
            printf("2 — V(x)\n");
            printf("Ваш выбор: ");

            if (scanf("%d", &function_number) != 1 ||
                (function_number != 1 && function_number != 2)) {
                printf("Неверный выбор функции.\n");
                break;
            }

            if (function_number == 1) {
                function = calculate_Y;
                function_name = "Y(x)";
            }
            else {
                function = calculate_V;
                function_name = "V(x)";
            }

            printf("\nВыберите способ задания аргументов:\n");
            printf("1. Ввести набор аргументов\n");
            printf("2. Десять аргументов по xmin и dx\n");
            printf("3. N случайных аргументов на промежутке\n");
            printf("Ваш выбор: ");

            if (scanf("%d", &mode) != 1 ||
                mode < 1 || mode > 3) {
                printf("Неверный способ задания аргументов.\n");
                break;
            }

            if (mode == 1) {
                printf("Введите количество аргументов: ");
                if (scanf("%d", &count) != 1 ||
                    count <= 0 || count > MAX_POINTS) {
                    printf("Некорректное количество аргументов.\n");
                    break;
                }

                values = (double*)malloc((size_t)count * sizeof(double));
                if (values == NULL) {
                    printf("Ошибка выделения памяти.\n");
                    break;
                }

                printf("Введите %d аргументов:\n", count);
                for (i = 0; i < count; ++i) {
                    if (scanf("%lf", &values[i]) != 1 ||
                        !isfinite(values[i])) {
                        printf("Некорректный аргумент.\n");
                        break;
                    }
                }

                if (i == count)
                    process_values(function, function_name, values, count);

                free(values);
            }
            else if (mode == 2) {
                printf("Введите xmin и dx: ");
                if (scanf("%lf %lf", &xmin, &dx) != 2 ||
                    !isfinite(xmin) || !isfinite(dx) || dx <= 0.0) {
                    printf("Некорректные xmin или dx.\n");
                    break;
                }

                values = (double*)malloc(10 * sizeof(double));
                if (values == NULL) {
                    printf("Ошибка выделения памяти.\n");
                    break;
                }

                for (i = 0; i < 10; ++i) {
                    values[i] = xmin + (double)i * dx;
                    if (!isfinite(values[i])) {
                        printf("Выход за диапазон double.\n");
                        break;
                    }
                }

                if (i == 10)
                    process_values(function, function_name, values, 10);

                free(values);
            }
            else {
                printf("Введите N, xn и xk: ");
                if (scanf("%d %lf %lf", &count, &xn, &xk) != 3 ||
                    count <= 0 || count > MAX_POINTS ||
                    !isfinite(xn) || !isfinite(xk) || xn >= xk) {
                    printf("Некорректные исходные данные.\n");
                    break;
                }

                values = (double*)malloc((size_t)count * sizeof(double));
                if (values == NULL) {
                    printf("Ошибка выделения памяти.\n");
                    break;
                }

                for (i = 0; i < count; ++i) {
                    values[i] = xn + (xk - xn) *
                        ((double)rand() / (double)RAND_MAX);
                }

                process_values(function, function_name, values, count);
                free(values);
            }
            break;

        case 4:
            printf("Какую функцию построить?\n");
            printf("1 — Y(x)\n");
            printf("2 — V(x)\n");
            printf("Ваш выбор: ");

            if (scanf("%d", &function_number) != 1 ||
                (function_number != 1 && function_number != 2)) {
                printf("Неверный выбор функции.\n");
                break;
            }

            if (function_number == 1) {
                function = calculate_Y;
                function_name = "Y(x)";
            }
            else {
                function = calculate_V;
                function_name = "V(x)";
            }

            if (read_interval(&xn, &xk, &h, &type))
                plot_function(function, function_name, xn, xk, h, type);
            break;

        case 5:
            printf("Работа программы завершена.\n");
            break;

        default:
            printf("Нет такого пункта меню. Введите число от 1 до 5.\n");
        }

    } while (choice != 5);

    return 0;
}

double calculate_Y(double x)
{
    double exponent, argument, result;

    if (!isfinite(x))
        return NAN;

    argument = x * sin(PI / 4.0);
    exponent = x * cos(PI / 4.0);

    if (!isfinite(exponent) ||
        exponent > log(DBL_MAX) || exponent < log(DBL_MIN))
        return NAN;

    result = cos(argument) * exp(exponent);

    if (!isfinite(result))
        return NAN;

    return result;
}

double calculate_V(double x)
{
    double phi, numerator, denominator, result;

    if (!isfinite(x))
        return NAN;

    if (x < 1.0)
        phi = 2.0 + x;
    else {
        double s = sin(x);
        phi = s * s;
    }

    if (fabs(x) > sqrt(DBL_MAX - 5.0))
        return NAN;

    denominator = x * x + 5.0;
    numerator = 2.8 * phi + 1.13 * cos(sqrt(fabs(x)));

    if (!isfinite(numerator) ||
        !isfinite(denominator) || denominator == 0.0)
        return NAN;

    result = numerator / denominator;

    if (!isfinite(result))
        return NAN;

    return result;
}

int is_left_closed(int type)
{
    return type == 2 || type == 4;
}

int is_right_closed(int type)
{
    return type == 3 || type == 4;
}

int read_interval(double* xn, double* xk, double* h, int* type)
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

    if (scanf("%d", type) != 1 || *type < 1 || *type > 4) {
        printf("Неверный тип промежутка.\n");
        return 0;
    }

    if ((*xk - *xn) / *h > (double)MAX_POINTS) {
        printf("Слишком много точек. Увеличьте шаг h.\n");
        return 0;
    }

    return 1;
}

void process_values(Function function, const char* name,
    const double* values, int count)
{
    int i;
    double y;

    printf("\n%16s | %20s\n", "x", name);
    printf("-----------------+----------------------\n");

    for (i = 0; i < count; ++i) {
        y = function(values[i]);

        if (isfinite(y))
            printf("%16.8g | %20.10g\n", values[i], y);
        else
            printf("%16.8g | %s\n",
                values[i], "значение вне диапазона");
    }
}

void plot_function(Function function, const char* name,
    double xn, double xk, double h, int type)
{
    const int width = 61;
    const int height = 21;
    char graph[21][61];
    double x, y, ymin = DBL_MAX, ymax = -DBL_MAX;
    double delta;
    int i, j, column, row, count = 0;

    for (i = 0; i < height; ++i)
        for (j = 0; j < width; ++j)
            graph[i][j] = ' ';

    for (i = 0; i <= MAX_POINTS; ++i) {
        x = xn + (double)i * h;

        if (!isfinite(x) || x > xk)
            break;

        if (i == 0 && !is_left_closed(type))
            continue;

        if (x == xk && !is_right_closed(type))
            continue;

        y = function(x);
        if (isfinite(y)) {
            if (y < ymin) ymin = y;
            if (y > ymax) ymax = y;
            ++count;
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

    delta = (ymax - ymin) * 0.05;
    ymin -= delta;
    ymax += delta;

    for (i = 0; i < height; ++i) {
        y = ymax - (double)i * (ymax - ymin) / (height - 1);
        if (ymin <= 0.0 && ymax >= 0.0) {
            row = (int)((ymax / (ymax - ymin)) * (height - 1));
            if (i == row)
                for (j = 0; j < width; ++j)
                    graph[i][j] = '-';
        }
    }

    if (xn <= 0.0 && xk >= 0.0) {
        column = (int)((0.0 - xn) / (xk - xn) * (width - 1));
        for (i = 0; i < height; ++i)
            graph[i][column] = '|';
    }

    for (i = 0; i <= MAX_POINTS; ++i) {
        x = xn + (double)i * h;

        if (!isfinite(x) || x > xk)
            break;

        if (i == 0 && !is_left_closed(type))
            continue;

        if (x == xk && !is_right_closed(type))
            continue;

        y = function(x);
        if (!isfinite(y))
            continue;

        column = (int)((x - xn) / (xk - xn) * (width - 1));
        row = (int)((ymax - y) / (ymax - ymin) * (height - 1));

        if (column >= 0 && column < width &&
            row >= 0 && row < height)
            graph[row][column] = '*';
    }

    printf("\nГрафик функции %s\n", name);
    printf("Y: от %.6g до %.6g\n", ymin, ymax);

    for (i = 0; i < height; ++i) {
        printf("%10.4g |", ymax -
            (double)i * (ymax - ymin) / (height - 1));
        for (j = 0; j < width; ++j)
            putchar(graph[i][j]);
        putchar('\n');
    }

    printf("           +-------------------------------------------------------------\n");
    printf("           %.6g%*s%.6g\n", xn, width - 15, "", xk);
    printf("* — значения функции\n");
}
