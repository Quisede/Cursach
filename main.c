#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Windows.h >

//структура события
typedef struct {
    int day; //день
    int month; //месяц
    int year; //год
    char type[256]; //тип события (День Рождения, Праздник, Свадьба и др.)
    char description[256]; //описание события (краткое описание-пояснение, например, День Рождения Вадима, купить торт)
} Event;

//интерфейс используемых функций
int paste_db(Event **events, int* num_events);
void print_events(Event **events, int num_events);
int delete_db(Event **events, int* num_events);
int editor_db(Event **events, int num_events);
int search_db(Event **events, int num_events);
void delete_full_db(const char* filename, Event **events, int* num_events);

//печать меню
void menu_print() {
    system("cls");//отчистка консоли

    printf("******************************************\n");
    printf("*             КАЛЕНДАРЬ                  *\n");
    printf("******************************************\n");
    printf("Выберите нужный пункт:                   *\n");
    printf("1) Добавить событие                      *\n");
    printf("2) Вывести события                       *\n");
    printf("3) Удалить событие                       *\n");
    printf("4) Найти событие                         *\n");
    printf("5) Редактировать событие                 *\n");
    printf("6) Удалить всю базу данных               *\n");
    printf("7) Сохранить и выйти                     *\n");
    printf("******************************************\n");
}

//функция добавления записи в базу данных
int paste_db(Event** events, int* num_events) {
    Event* temp = realloc(*events, (*num_events + 1) * sizeof(Event));//увеличиваем размер памяти для массива структур
    if (temp == NULL) {
        printf("Ошибка выделения памяти при добавлении нового события\n");
        return 1;
    }
    *events = temp; // Обновляем указатель на новую память
    Event new_event; //структура нового события

    // Ввод данных события от пользователя
    do {
        printf("Введите день:\n");
        scanf("%d", &new_event.day);
        getchar();//для перехода к следующему запросу на ввод 
        if (new_event.day < 1 || new_event.day > 31) { //проверяем на правильность ввода
            printf("Введите другое число\n");
        }
    } while (new_event.day < 1 || new_event.day > 31);

    do {
        printf("Введите месяц:\n");
        scanf("%d", &new_event.month);
        getchar();
        if (new_event.month < 1 || new_event.month > 12) {
            printf("Введите другое число\n");
        }
    } while (new_event.month < 1 || new_event.month > 12);

    do {
        printf("Введите год:\n");
        scanf("%d", &new_event.year);
        getchar();
        if (new_event.year < 1 || new_event.year > 3000) {
            printf("Введите другое число \n");
        }
    } while (new_event.year < 1 || new_event.year > 3000);

    printf("Введите тип события : (День рождения, праздник, событие)\n");
    fgets(new_event.type, sizeof(new_event.type), stdin);
    new_event.type[strcspn(new_event.type, "\n")] = 0;//используем strcspn для удаления символа новой строки при занесении в базу данных

    printf("Введите описание события:\n");
    fgets(new_event.description, sizeof(new_event.description), stdin);
    new_event.description[strcspn(new_event.description, "\n")] = 0;

    // Добавляем заполненное событие в конец расширенного массива
    memcpy(&(*events)[*num_events], &new_event, sizeof(Event));
    (*num_events)++; // Увеличиваем счетчик событий

    //отчищаем консоль
    system("cls");
    printf("Событие успешно занесено в календарь\n\n");

    return 0;
}

//функция чтения
void print_events(Event** events, int num_events) {
    int print_count = 0;//счетчик выведенных на экран событий
    //проходимся по массиву структур и выводим информацию
    for (int i = 0; i < num_events; i++) {
        printf("\nДень: %d\n", (*events)[i].day);
        printf("Месяц: %d\n", (*events)[i].month);
        printf("Год: %d\n", (*events)[i].year);
        printf("Тип события: %s\n", (*events)[i].type);
        printf("Описание события: %s\n", (*events)[i].description);
        printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
        printf("\n");
        print_count++;

        // Выводим за раз 10 событий, если нужно продолжить, пользователь сообщит
        if ((print_count == 10) && ((i+1) != num_events)) {
            printf("Вывести еще записи? (1 - ДА, 2 - НЕТ)\n");
            int answer;//ответ для диалога с пользователем
            do {
                answer = getch();
            } while (answer != '1' && answer != '2');

            // Если пользователь отказался, очищаем экран и прерываем вывод
            if (answer == '2') {
                system("cls");
                printf("Вывод окончен\n\n");
                break;
            }
        }
    }
}

//функция удаления события из базы данных
int delete_db(Event **events, int* num_events) {
    int answer = 0;//ответ для диалога с пользователем
    int month_s = 0;//месяц поиска
    int count_delete = 0;//счетчик кол-ва удаленных событий
    char input_type_s[256];//тип поиска
    char input_description_s[256];//описание поиска

    //ветки действий (диалог с пользователем и последовательный выбор действий)
    printf("Какую запись вы хотите удалить?\n");
    printf("Введите '1' для поиска по месяцам.\nВведите '2' для поиска по типу события.\nВведите '3' для поиска по описанию события.\nДля выхода в меню нажмите любую другую клавишу.\n");
    int choice = getch();//для считывания варианта поиска
    switch (choice) {
        //вводим данные для поиска нужной записи
    case '1':
        do {
            printf("Введите месяц:\n");
            scanf("%d", &month_s);
            getchar();
            if (month_s < 1 || month_s > 12) {
                printf("Неверное число. Введите заново.\n");//ограничение на ввод
            }
        } while (month_s < 1 || month_s > 12);
        choice = 1;
        break;
    case '2':
        printf("\nВведите тип события\n");
        fgets(input_type_s, sizeof(input_type_s), stdin);
        input_type_s[strcspn(input_type_s, "\n")] = 0;
        choice = 2;
        break;
    case '3':
        printf("\nВведите описание события\n");
        fgets(input_description_s, sizeof(input_description_s), stdin);
        input_description_s[strcspn(input_description_s, "\n")] = 0;
        choice = 3;
        break;
    }

    //ищем нужную запись в бд
    for (int i = 0; i < *num_events; i++) {
        if (((choice == 1 && month_s == (*events)[i].month) ||
            (choice == 2 && strcmp(input_type_s, (*events)[i].type) == 0) ||
            (choice == 3 && strcmp(input_description_s, (*events)[i].description) == 0)) && count_delete < 1) {
            //если нашли совпадение, выводим эту запись
            printf("\nСобытие найдено\n");
            printf("Дата: %d.%d.%d\n", (*events)[i].day, (*events)[i].month, (*events)[i].year);
            printf("Тип: %s\n", (*events)[i].type);
            printf("Описание: %s\n", (*events)[i].description);
            printf("Это нужная запись? (1 - ДА, 2 - НЕТ)\n");
            //диалог с пользователем, нужная ли запись была выведена?
            do {
                answer = getch();
            } while (answer != '1' && answer != '2');

            if (answer == '1') {
                count_delete++;
                //здесь выполняем непосредственно смещение в массиве структур, перезаписывая на место удаляемого события оставшуюся часть событий
                for (int j = i; j < *num_events - 1; j++) {
                    (*events)[j] = (*events)[j + 1];
                }
                (*num_events)--; // Уменьшаем количество событий
                // Изменяем размер блока памяти с использованием realloc
                Event* temp = realloc(*events, (*num_events) * sizeof(Event));
                if (temp == NULL) {
                    printf("Ошибка изменения размера памяти\n");
                    return 1; 
                }
                *events = temp; // Обновляем указатель на новую память
                break; // Если удалили запись, прерываем цикл
            }
            else if (answer == '2')
                continue;//если не то событие, то продолжаем поиск
        }
    }
    //в случае, если ничего не нашли по базе данных
    if (answer == 0) {
        printf("Событий не найдено.\n\n");
    }

    // Очистка консоли
    system("cls");
    //если удалили событие, то сообщаем об успешности
    if (count_delete > 0) {
        printf("Событие успешно удалено.\n\n");
        return 0;
    }
    else {
        printf("Событие не удалено. Попробуйте еще раз\n\n");
        return 1;
    }
}

//функция поиска по базе данных
int search_db(Event **events, int num_events) {
    int answer = 0;
    int month_s = 0;
    int count = 0;
    char input_type_s[256];
    char input_description_s[256];
    char input_surname_s[256];//для поиска по фамилии

    printf("\nКакую запись вы хотите найти?\n");
    printf("Введите '1' для поиска по месяцам.\nВведите '2' для поиска по типу события.\nВведите '3' для поиска по описанию события.\nВведите '4' для поиска по фамилии.\nДля выхода в меню нажмите любую другую клавишу.\n");
    int choice = getch();

    switch (choice) {
        //вводим данные для поиска
    case '1':
        do {
            printf("Введите месяц:\n");
            scanf("%d", &month_s);
            getchar();
            if (month_s < 1 || month_s > 12) {
                printf("Неверное число. Введите заново.\n");
            }
        } while (month_s < 1 || month_s > 12);
        choice = 1;
        break;
    case '2':
        printf("\nВведите тип события\n");
        fgets(input_type_s, sizeof(input_type_s), stdin);
        input_type_s[strcspn(input_type_s, "\n")] = 0;
        choice = 2;
        break;
    case '3':
        printf("\nВведите описание события\n");
        fgets(input_description_s, sizeof(input_description_s), stdin);
        input_description_s[strcspn(input_description_s, "\n")] = 0;
        choice = 3;
        break;
    case '4':
        printf("\nВведите фамилию\n");
        fgets(input_surname_s, sizeof(input_surname_s), stdin);
        input_surname_s[strcspn(input_surname_s, "\n")] = 0;
        choice = 4;
        break;
    }

    //проходимся по массиву структур и выводим события, которые удовлетворяют критериям поиска
    for (int i = 0; i < num_events; i++) {
        if ((choice == 1 && month_s == (*events)[i].month) ||
            ((choice == 2 && strcmp(input_type_s, (*events)[i].type) == 0) ||
                (choice == 3 && strcmp(input_description_s, (*events)[i].description) == 0) ||
                (choice == 4 && (strstr((*events)[i].type, input_surname_s) != NULL || strstr((*events)[i].description, input_surname_s) != NULL)))) {
            //выводим на экран нужные события
            printf("\nСобытие найдено\n");
            printf("Дата: %d.%d.%d\n", (*events)[i].day, (*events)[i].month, (*events)[i].year);
            printf("Тип: %s\n", (*events)[i].type);
            printf("Описание: %s\n", (*events)[i].description);
            count++;//счетчик количества выведенных событий
        }
    }
    if (count == 0)
        printf("Не было найдено ни одного события\n\n");

    printf("\nПоиск закончен\n\n");
    return 1;
}

//функция редактирования событий в бд
int editor_db(Event **events, int num_events) {
    int answer = 0;
    int month_s = 0;
    int count_redact = 0;//счетчик отредактированных событий
    char input_type_s[256];
    char input_description_s[256];

    printf("Какую запись вы хотите изменить?\n");
    printf("Введите '1' для поиска по месяцам.\nВведите '2' для поиска по типу события.\nВведите '3' для поиска по описанию события.\nДля выхода в меню нажмите любую другую клавишу.\n");
    int choice = getch();

    switch (choice) {
        //вводим данные для поиска нужного события
    case '1':
        do {
            printf("Введите месяц:\n");
            scanf("%d", &month_s);
            getchar();
            if (month_s < 1 || month_s > 12) {
                printf("Неверное число. Введите заново.\n");
            }
        } while (month_s < 1 || month_s > 12);
        choice = 1;
        break;
    case '2':
        printf("\nВведите тип события\n");
        fgets(input_type_s, sizeof(input_type_s), stdin);
        input_type_s[strcspn(input_type_s, "\n")] = 0;
        choice = 2;
        break;
    case '3':
        printf("\nВведите описание события\n");
        fgets(input_description_s, sizeof(input_description_s), stdin);
        input_description_s[strcspn(input_description_s, "\n")] = 0;
        choice = 3;
        break;
    }

    //проходимся по бд и ищем подходящее событие
    for (int i = 0; i < num_events; i++) {
        if (((choice == 1 && month_s == (*events)[i].month) ||
            (choice == 2 && strcmp(input_type_s, (*events)[i].type) == 0) ||
            (choice == 3 && strcmp(input_description_s, (*events)[i].description) == 0)) && count_redact < 1) {
            printf("\nСобытие найдено\n");
            //нужно красиво вывести это событие
            printf("Дата: %d.%d.%d\n", (*events)[i].day, (*events)[i].month, (*events)[i].year);
            printf("Тип: %s\n", (*events)[i].type);
            printf("Описание: %s\n", (*events)[i].description);
            printf("Это нужная запись? (1 - ДА, 2 - НЕТ)\n");
            int answer;
            do {
                answer = getch();
            } while (answer != '1' && answer != '2');

            //если нужное событие, то спрашиваем что хотим изменить и меняем
            if (answer == '1') {
                //проверка, то ли событие хотим изменить
                printf("\nЧто вы хотите изменить? (1 - ДАТА, 2 - ТИП СОБЫТИЯ, 3 - ОПИСАНИЕ СОБЫТИЯ\n");
                int choice = getch();
                //вводим новые данные для выбранного события
                count_redact++;
                switch (choice) {
                case '1':
                    do {
                        printf("Введите день:\n");
                        scanf("%d", &(*events)[i].day);
                        getchar();
                        if ((*events)[i].day < 1 || (*events)[i].day > 31) {
                            printf("Неверное число. Введите заново.\n");
                        }
                    } while ((*events)[i].day < 1 || (*events)[i].day > 31);

                    do {
                        printf("Введите месяц:\n");
                        scanf("%d", &(*events)[i].month);
                        getchar();
                        if ((*events)[i].month < 1 || (*events)[i].month > 12) {
                            printf("Неверное число. Введите заново.\n");
                        }
                    } while ((*events)[i].month < 1 || (*events)[i].month > 12);
                    do {
                        printf("Введите год:\n");
                        scanf("%d", &(*events)[i].year);
                        getchar();
                        if ((*events)[i].year < 1 || (*events)[i].year > 2100) {
                            printf("Неверное число. Введите заново.\n");
                        }
                    } while ((*events)[i].year < 1 || (*events)[i].year > 2100);
                    choice = 1;
                    break;
                case '2':
                    printf("\nВведите тип события\n");
                    fgets((*events)[i].type, sizeof((*events)[i].type), stdin);
                    (*events)[i].type[strcspn((*events)[i].type, "\n")] = 0;
                    choice = 2;
                    break;
                case '3':
                    printf("\nВведите описание события\n");
                    fgets((*events)[i].description, sizeof((*events)[i].description), stdin);
                    (*events)[i].description[strcspn((*events)[i].description, "\n")] = 0;
                    choice = 3;
                    break;
                }
            }
        }

    }
    if (answer == 0) {
        printf("\nСобытий на выбранный месяц не найдено.\n\n");
    }

    system("cls");
    if (count_redact > 0)
        printf("\nОперация редактирования проведена успешно\n\n");
    else
        printf("\nСобытие не отредактировано. Попробуйте еще раз\n\n");

    return 1;
}

//функция ПОЛНОГО УДАЛЕНИЯ базы данных (реализована защита от случайного нажатия)
void delete_full_db(const char* filename, Event **events, int* num_events) {
    printf("\nВы точно хотите УДАЛИТЬ ВСЮ БАЗУ ДАННЫХ? (1 - ДА, 2 - НЕТ)\n");
    int answer;
    do {
        answer = getch();
    } while (answer != '1' && answer != '2');

    system("cls");
    //если да, удаляем файл с базой данных, отчищаем массив структур, обнуляем счетчик событий
    if (answer == '1') {
        int status = remove(filename);
        if (status == 0) {
            if (*events != NULL) {
                free(*events);
                *events = NULL;
            }
            *num_events = 0; // Сбрасываем счетчик
            printf("База данных полностью удалена.\n\n");
        }
        else {
            printf("Ошибка при удалении базы данных.\n\n");
        }
    }
    else
        printf("Отмена удаления\n\n");
}

//основная функция(обработка клавиш и вызов соответствующих функций
int main(void) {
    SetConsoleCP(1251);  //установка кодовой страницы win-cp 1251 в поток ввода
    SetConsoleOutputCP(1251); //установка кодовой страницы win-cp 1251 в поток вывода

    Event *events = NULL;//массив структур
    int num_events = 0;//счетчик количества событий
    const char *filename = "database.dat";

    // Открытие файла для чтения и определение количества элементов
    FILE* file = fopen(filename, "rb");
    if (file) {
        // Определяем размер файла и количество событий
        fseek(file, 0, SEEK_END);
        long filesize = ftell(file);//определяем размер данных в базе данных
        fseek(file, 0, SEEK_SET);

        num_events = filesize / sizeof(Event); //определяем начальное количество событий
        events = (Event*)malloc(filesize); // выделяем память под считываемые события

        // Считываем данные из файла
        fread(events, sizeof(Event), num_events, file);

        fclose(file);//закрываем файл
    }
    else {
        printf("Файл не найден. Будет создан новый файл.\n\n");
        // Создаем новый файл
        file = fopen(filename, "wb");
        if (file) {
            fclose(file);
        }
        else {
            fprintf(stderr, "Ошибка создания файла.\n\n");
            return 1;
        }

        // Выделяем память под максимально возможное количество событий
        events = (Event*)malloc(1 * sizeof(Event));
    }

    char key;//хранит данные о нажатой клавише
    do {
        menu_print();//выводим меню
        key = getch();//считываем символ без ожидания нажатия Enter

        switch (key) {
        case '1':
            system("cls");//отчистка консольного окна
            paste_db(&events, &num_events);
            break;
        case '2':
            system("cls");
            print_events(&events, num_events);
            break;
        case'3':
            system("cls");
            delete_db(&events, &num_events);
            break;

        case '4':
            system("cls");
            search_db(&events, num_events);
            break;
        case '5':
            system("cls");
            editor_db(&events, num_events);
            break;
        case '6':
            system("cls");
            delete_full_db(filename, & events, &num_events);
            break;
            return 0;
        }
        system("pause");
    } while (key != '7'); //выход из календаря

    // Сохраняем данные перед выходом
    file = fopen(filename, "wb");
    if (file) {
        fwrite(events, sizeof(Event), num_events, file);
        fclose(file);
    }

    // Освобождаем выделенную память
    free(events);

    return 0;
}
