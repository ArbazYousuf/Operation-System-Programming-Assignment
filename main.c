#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define numberOfReaders 5
#define numberOfWriters 2

int read_count = 0;
pthread_mutex_t read_write_mutex, lock_mutex;

void *reader(void *process_number) {

    // acquire the lock
    pthread_mutex_lock(&lock_mutex);

    // increment the read count can obtain read write lock if this is the first reader
    read_count++;
    if (read_count == 1) {
        pthread_mutex_lock(&read_write_mutex);
    }

    // release the lock
    pthread_mutex_unlock(&lock_mutex);

    // simulate reading of resource
    printf("Reader #%d reading the database\n", process_number);
    printf("Reader #%d exiting the database\n", process_number);

    // acquire the lock
    pthread_mutex_lock(&lock_mutex);

    // decrement read count and release read write lock if this is the last reader
    read_count--;
    if (read_count == 0) {
        pthread_mutex_unlock(&read_write_mutex);
    }

    // release the lock
    pthread_mutex_unlock(&lock_mutex);

    // thread safe exit
    pthread_exit(NULL);

    return NULL;
}

/***
 * Method to write to resourse
 * @param process_number the process number
 * @return NULL
 */
void *writer(void *process_number) {
    printf("Writer #%d waiting for database\n", process_number);

    // acquire the read write lock
    pthread_mutex_lock(&read_write_mutex);

    // simulate writing to resource
    printf("Writer #%d updating the database\n", process_number);
    printf("Writer #%d exiting the database\n", process_number);

    // release the read write lock
    pthread_mutex_unlock(&read_write_mutex);

    // thread safe exit
    pthread_exit(NULL);

    return NULL;
}

/***
 * Method to check the error code and exit if necessary
 * @param message the message to be displayed
 * @param error_code the error code
 */
void check_error_code_and_exit(char *message, int error_code) {
    // if the error code is non zero, display the appropriate message and exit
    if (error_code != 0) {
        printf("%s (error code = %d)\n", message, error_code);
        exit(EXIT_FAILURE);
    }
}

/***
 * Main function
 * @return error code
 */
int main() {
    // initialize the error code with 0 value
    int error_code = 0;

    pthread_t readers[numberOfReaders], writers[numberOfReaders];

    // initialize the lock mutex and check if the initialization was successful
    error_code = pthread_mutex_init(&lock_mutex, NULL);
    check_error_code_and_exit("Could not initialize lock mutex", error_code);

    // initialize the read and write mutex and check if the initialization was successful
    error_code = pthread_mutex_init(&read_write_mutex, NULL);
    check_error_code_and_exit("Could not initialize read write mutex", error_code);

    // loop to create and start the reader and writer process
    for (int i = 0, j = 0; i < numberOfReaders; i++) {
        // create and start a reader and check if it was successful
        error_code = pthread_create(&readers[i], NULL, reader, i);
        check_error_code_and_exit("Could not create a reader thread", error_code);

        // ensure that the reader thread is not killed when the main thread completes
        pthread_detach(readers[i]);

        // start a writer process for every 5th reader process
        if (i % 5 == 0) {
            // create and start a write process and check if it was successful
            error_code = pthread_create(&writers[j], NULL, writer, j);
            check_error_code_and_exit("Could not create a writer thread", error_code);

            // ensure that the writer thread is not killed when the main thread completes
            pthread_detach(writers[j++]);
        }
    }

    // thread safe exit of main thread
    pthread_exit(0);
}
