#include <stdio.h>

#include <stdlib.h>

#include <iostream>

#include <string>

#include <fstream>

#include <string.h>

#include <cstring>

#include <vector>

using namespace std;

char globalPath[128];

char wordToFind[32];

void searchTxt(char *path, char *find) {

    ifstream file;

    file.open(path);

    while (file.good()) {

        char word[50];

        file >> word;

        if (file.good() && strcmp(word, find) == 0) {

            cout << "Word " << find << " found in " << path << endl;

            return;

        }

    }

}

void *searchInDir(void *arg) {

// int threadId = pthread_self();

// cout << "Thread id: " << threadId << endl;

    char *path = (char *) arg;

// cout << path <<endl;

    char cmd[64];

    char buffer[128];

// Получаем все файлы в директории

    sprintf(cmd, "ls -p %s | grep -v /", path);

    FILE *pipe = popen(cmd, "r");

    if (!pipe) throw runtime_error("popen() failed!");

    try {

        while (fgets(buffer, sizeof buffer, pipe) != NULL) {

            int len = strlen(buffer);

            buffer[len - 1] = '\0';

            char *end = buffer + len - 4;

            if (!strcmp(end, "txt")) {

// Если файл txt то ищем слово в нем

                char newPath[128];

                strcpy(newPath, path);

                strcat(newPath, "/");

                strcat(newPath, buffer);

                searchTxt(newPath, wordToFind);

            }

        }

    }

    catch (...) {

        pclose(pipe);

        throw;

    }

    pclose(pipe);

}

void searchDir(char *path, char *find) {

    vector<string> paths;

    char buffer[128];

    char cmd[128];

// Получаем все не пустые директории

    sprintf(cmd, "find %s -type d ! -empty", path);

    FILE *pipe = popen(cmd, "r");

    if (!pipe) throw runtime_error("popen() failed!");

    try {

        while (fgets(buffer, sizeof buffer, pipe) != NULL) {

// Формируем список путей

            pthread_t thread;

            int len = strlen(buffer);

            buffer[len - 1] = '\0';

            paths.push_back(buffer);

        }

    }

    catch (...) {

        pclose(pipe);

        throw;

    }

    pclose(pipe);

// Для каждой директории создаем свой поток

    pthread_t *threads;

    threads = new pthread_t[paths.size()];

    for (int i = 0; i < paths.size(); i++) {

        const char *c = paths[i].c_str();

        int err = pthread_create(&threads[i], NULL, searchInDir, (void *) c);

        if (err != 0)

            cout << "Cannot create thread " << endl;

    }

    for (int i = 0; i < paths.size(); i++)

        pthread_join(threads[i], NULL);

}

int main() {

    char buffer[128];

    string result = "";

    cout << "Enter directory\n";

    cin >> globalPath;

    cout << "Enter word to find\n";

    cin >> wordToFind;

    searchDir(globalPath, wordToFind);

}