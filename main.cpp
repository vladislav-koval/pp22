#include <cstdlib>
#include <iostream>
#include <cstring>
#include <pthread.h>
#include <stack>
#include <unistd.h>

using namespace std;

int currentTask = 0; // Указатель на текущее задание
pthread_mutex_t mutex;
bool stackIsEmpty; // флаг-сигнал
stack<int> todo; // список задач

void task(int taskNum) {
    cout << "Doing task #" << taskNum << endl;
    usleep(1000);

    cout << "Task #" << taskNum << " done" << endl;
}

void *addTask(void *arg) {
    /* поток добавляющий задания */
    int err;
    while (true) {
        int taskNum = rand() % 10;
        err = pthread_mutex_lock(&mutex);
        if (err > 0)
            cout << "Cannot lock mutex" << endl;
        todo.push(taskNum);
        stackIsEmpty = false;
        err = pthread_mutex_unlock(&mutex);
        if (err > 0)
            cout << "Cannot lock mutex" << endl;
        cout << "Task #" << taskNum << " added\n";
        usleep(7 * 1000 * 1000);
    }
}

void *doTask(void *arg) {
    /* поток выполняющий задания */
    int err;
    int taskNum;
    while (true) {
        // заблокировали мьютекс
        err = pthread_mutex_lock(&mutex);
        if (err > 0)
            cout << "Cannot lock mutex" << endl;
        // проверили есть ли задачи
        if (todo.size() > 0) {
            taskNum = todo.top();
            todo.pop();
            // если задание есть
            // разблокировали мьютех
            err = pthread_mutex_unlock(&mutex);
            if (err > 0)
                cout << "Cannot unlock mutex" << endl;
            task(taskNum);
        } else {
            stackIsEmpty = true;
            // если задания нет
            // разблокировали мьютех
            err = pthread_mutex_unlock(&mutex);
            if (err > 0)
                cout << "Cannot unlock mutex" << endl;
            // ждем пока стек пустует (пока не получим сигнал)
            while (stackIsEmpty);
            // блокируем мьютекс
            err = pthread_mutex_lock(&mutex);
            if (err > 0)
                cout << "Cannot lock mutex" << endl;
            // забираем задание
            taskNum = todo.top();
            todo.pop();
            usleep(1000);
            // освобождаем мьютекс
            err = pthread_mutex_unlock(&mutex);
            if (err > 0)
                cout << "Cannot unlock mutex" << endl;
            task(taskNum);
        }
    }
}

int main() {
    const int threads_num = 2;
    pthread_t *threads; // Идентификаторы потоков
    threads = new pthread_t[threads_num];
    int err; // Код ошибки
    // инициализируем стэк
    for (int i = 0; i < 10; i++)
        todo.push(0);

    // Инициализируем мьютекс
    err = pthread_mutex_init(&mutex, NULL);
    if (err != 0)
        printf("Cannot initialize mutex");
    // Создаём потоки
    err = pthread_create(&threads[0], NULL, addTask, NULL);
    if (err != 0)
        printf("Cannot create thread");
    err = pthread_create(&threads[1], NULL, doTask, NULL);
    if (err != 0)
        printf("Cannot create thread");
    // Ждем завершения потоков
    for (int i = 0; i < threads_num; i++)
        pthread_join(threads[i], NULL);
    // Освобождаем ресурсы, связанные с мьютексом
    pthread_mutex_destroy(&mutex);
}
