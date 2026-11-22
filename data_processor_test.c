#include <stdio.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#define MAX_EVENTS 10
#define MAX_BATCHES 10
#define MAX_DIFFERENCE_MS 5000

/*
 * demo of data processing module - acts as a middleware for formatting user inputs and forwarding them to network module
 * prepares body for HTTP POST requests using basic grouping rule:
 *      max. MAX_BATCHES events in one request
 *                      OR
 *      new batch when no new inputs for MAX_DIFFERENCE_MS period
 */

typedef struct {
    int event_id;
    char event_type[20];

    time_t timestamp;
    int milliseconds;
} Event;

typedef struct {
    int batch_id;
    Event events[MAX_EVENTS];
    int event_count;
} Batch;

void get_current_timestamp(Event *event) {
    FILETIME ft;
    GetSystemTimePreciseAsFileTime(&ft);

    ULARGE_INTEGER time;
    time.LowPart = ft.dwLowDateTime;
    time.HighPart = ft.dwHighDateTime;

    unsigned long long milliseconds =
        (time.QuadPart - 116444736000000000ULL) / 10000ULL;

    event->timestamp = (time_t)(milliseconds / 1000);
    event->milliseconds = (int)(milliseconds % 1000);
}

void add_event(Event *event, int id, const char *type) {
    event->event_id = id;

    snprintf(
        event->event_type,
        sizeof(event->event_type),
        "%s",
        type
    );

    get_current_timestamp(event);
}

void format_timestamp(const Event *event, char *buffer, size_t size) {
    struct tm local_time;

    localtime_s(&local_time, &event->timestamp);

    snprintf(
        buffer,
        size,
        "%02d:%02d:%02d.%03d %02d-%02d-%04d",
        local_time.tm_hour,
        local_time.tm_min,
        local_time.tm_sec,
        event->milliseconds,
        local_time.tm_mday,
        local_time.tm_mon + 1,
        local_time.tm_year + 1900
    );
}


long long timestamp_difference_ms(const Event *a, const Event *b) {
    long long seconds =
        (long long)difftime(
            a->timestamp,
            b->timestamp
        );

    return seconds * 1000LL + (a->milliseconds - b->milliseconds);
}


void print_json(const Batch *batch) {

    printf("{\n");

    printf("  \"batch_id\": %d,\n",
           batch->batch_id);

    printf("  \"event_count\": %d,\n",
           batch->event_count);

    printf("  \"events\": [\n");

    for (int i = 0; i < batch->event_count; i++) {

        char timestamp[64];

        format_timestamp(
            &batch->events[i],
            timestamp,
            sizeof(timestamp)
        );

        printf("    {\n");

        printf("      \"event_id\": %d,\n",
               batch->events[i].event_id);

        printf("      \"event_type\": \"%s\",\n",
               batch->events[i].event_type);

        printf("      \"timestamp\": \"%s\"\n",
               timestamp);

        printf("    }%s\n",
               i < batch->event_count - 1
                   ? ","
                   : "");
    }

    printf("  ]\n");
    printf("}\n");
}


int main(void) {

    Event events[13];

    add_event(&events[0], 1, "VK_F12");
    Sleep(400);
    add_event(&events[1], 2, "VK_BACK");
    Sleep(500);
    add_event(&events[2], 3, "VK_SPACE");
    Sleep(100);
    add_event(&events[3], 4, "VK_SHIFT");
    Sleep(500);
    add_event(&events[4], 5, "O");
    Sleep(600);
    add_event(&events[5], 6, "S");
    Sleep(700);
    add_event(&events[6], 7, "2");
    Sleep(400);
    add_event(&events[7], 8, "0");
    Sleep(800);
    add_event(&events[8], 9, "2");
    Sleep(900);
    add_event(&events[9], 10, "6");
    /*
     * over 5 secs of no inputs OR over MAX_EVENTS events -> new batch
     */
    Sleep(100);
    add_event(&events[10], 11, "A");
    Sleep(700);
    add_event(&events[11], 12, "B");
    Sleep(5100);
    add_event(&events[12], 13, "C");
    Sleep(200);

    int event_count = sizeof(events) / sizeof(events[0]);

    Batch batches[MAX_BATCHES];

    int batch_count = 0;

    Batch current = {0};

    current.batch_id = 1;


    for (int i = 0; i < event_count; i++) {

        if (current.event_count > 0) {

            Event *previous = &current.events[current.event_count - 1];

            long long difference =
                timestamp_difference_ms(
                    &events[i],
                    previous
                );

            /*
             * over 5 secs of no inputs OR over MAX_EVENTS inputs -> new batch
             */
            if (current.event_count == MAX_EVENTS || difference > MAX_DIFFERENCE_MS) {
                batches[batch_count++] = current;

                current = (Batch){0};

                current.batch_id = batch_count + 1;
            }
        }

        current.events[current.event_count++] = events[i];
    }

    //add leftovers
    if (current.event_count > 0) {
        batches[batch_count++] = current;
    }


    printf(
        "Created %d batches.\n\n",
        batch_count
    );


    for (int i = 0; i < batch_count; i++) {

        printf(
            "=== BATCH %d ===\n",
            batches[i].batch_id
        );

        print_json(&batches[i]);

        printf("\n");
    }

    return 0;
}