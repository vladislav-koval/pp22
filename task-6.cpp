#include <iostream>

#include <math.h>

#include <pthread.h>

#include <stdio.h>

#include <vector>

using namespace std;

int redRes = 0;

typedef void *(mapFunc)(void *);

typedef void *(reduceFunc)(void *);

// typedef int (reduceFunc)(vector<int> arr);

struct mapData {

    vector<int> arr;

    int res;

};

void *map(void *arg) {

// int threadId = pthread_self();

// cout << "Thread id: " << threadId << endl;

    mapData *data = (mapData *) arg;

    vector<int> arr = (*data).arr;

    int res = 0;

    for (int i = 0; i < arr.size(); i++)

        res += arr[i];

    (*data).res = res;

}

void *reduce(void *arg) {

    mapData *data = (mapData *) arg;

    int mapRes = (*data).res;

    redRes += mapRes;

}

int mapReduse(vector<int> arr, mapFunc map, reduceFunc reduce, int threadQuant) {

    pthread_t *threads;

    threads = new pthread_t[threadQuant];

    mapData *data = new mapData[threadQuant];

    int err;

// Распределяем масив по тредам

    int pointer = 0;

    for (int i = 0; i < threadQuant; i++) {

        do {

            data[i].arr.push_back(arr[pointer]);

            pointer++;

        } while (pointer % (arr.size() / threadQuant) != 0);

    }

    if (pointer < arr.size()) {

        while (pointer != arr.size()) {

            data[threadQuant - 1].arr.push_back(arr[pointer]);

            pointer++;

        }

    }

// Создаем треды

    for (int i = 0; i < threadQuant; i++) {

        err = pthread_create(&threads[i], NULL, map, &data[i]);

        if (err != 0)

            cout << "Cannot create thread " << i << endl;

    }

    for (int i = 0; i < threadQuant; i++)

        pthread_join(threads[i], NULL);

    vector<int> mapRes;

    for (int i = 0; i < threadQuant; i++)

        mapRes.push_back(data[i].res);

    for (int i = 0; i < threadQuant; i++) {

        err = pthread_create(&threads[i], NULL, reduce, &data[i]);

        if (err != 0)

            cout << "Cannot create thread " << i << endl;

    }

    for (int i = 0; i < threadQuant; i++)

        pthread_join(threads[i], NULL);

}

int main() {

    vector<int> arr;

    int threadQuant, arrSize;

    cout << "Enter arrey size\n";

    cin >> arrSize;

    cout << "Enter quantity of threads\n";

    cin >> threadQuant;

    if (threadQuant > arrSize)

        threadQuant = arrSize;

    for (int i = 0; i < arrSize; i++)

        arr.push_back(i);

    mapReduse(arr, map, reduce, threadQuant);

    cout << "Answer:" << redRes << endl;

}