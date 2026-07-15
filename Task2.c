/*
 * Task 2: Memory Management Simulation (C version)

 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FRAMES 16
#define MAX_REFS   256

typedef struct {
    int page_faults;
    int page_hits;
} Stats;

/* ---------------- FIFO ---------------- */

Stats run_fifo(const int *refs, int n_refs, int num_frames, int page_size_bytes) {
    (void)page_size_bytes;
    Stats stats = {0, 0};
    int frames[MAX_FRAMES];
    int count = 0;       /* how many frames currently occupied */
    int front = 0;       /* index of the oldest page (for eviction) */

    printf("%-6s%-6s%-7s%-9s%s\n", "Step", "Page", "Event", "Evicted", "RAM contents");

    for (int i = 0; i < n_refs; i++) {
        int page = refs[i];
        int found = -1;
        for (int f = 0; f < count; f++) {
            int idx = (front + f) % num_frames;
            if (frames[idx] == page) { found = idx; break; }
        }

        int evicted = -1;
        const char *event;
        if (found != -1) {
            stats.page_hits++;
            event = "HIT";
        } else {
            stats.page_faults++;
            event = "FAULT";
            if (count >= num_frames) {
                evicted = frames[front];
                frames[front] = page;          /* overwrite the oldest slot */
                front = (front + 1) % num_frames;
            } else {
                frames[(front + count) % num_frames] = page;
                count++;
            }
        }

        printf("%-6d%-6d%-7s", i, page, event);
        if (evicted != -1) printf("%-9d", evicted); else printf("%-9s", "-");
        printf("[");
        for (int f = 0; f < count; f++) {
            int idx = (front + f) % num_frames;
            printf("%d%s", frames[idx], f == count - 1 ? "" : ", ");
        }
        printf("]\n");
    }
    return stats;
}

/* ---------------- LRU ---------------- */

Stats run_lru(const int *refs, int n_refs, int num_frames) {
    Stats stats = {0, 0};
    int frames[MAX_FRAMES];
    int last_used[MAX_FRAMES]; /* higher number = more recently used */
    int count = 0;

    printf("%-6s%-6s%-7s%-9s%s\n", "Step", "Page", "Event", "Evicted", "RAM contents");

    for (int i = 0; i < n_refs; i++) {
        int page = refs[i];
        int found = -1;
        for (int f = 0; f < count; f++) {
            if (frames[f] == page) { found = f; break; }
        }

        int evicted = -1;
        const char *event;
        if (found != -1) {
            stats.page_hits++;
            event = "HIT";
            last_used[found] = i;   /* mark as most-recently-used */
        } else {
            stats.page_faults++;
            event = "FAULT";
            if (count >= num_frames) {
                /* find the slot with the smallest last_used value */
                int lru_slot = 0;
                for (int f = 1; f < count; f++) {
                    if (last_used[f] < last_used[lru_slot]) lru_slot = f;
                }
                evicted = frames[lru_slot];
                frames[lru_slot] = page;
                last_used[lru_slot] = i;
            } else {
                frames[count] = page;
                last_used[count] = i;
                count++;
            }
        }

        printf("%-6d%-6d%-7s", i, page, event);
        if (evicted != -1) printf("%-9d", evicted); else printf("%-9s", "-");
        printf("[");
        for (int f = 0; f < count; f++) {
            printf("%d%s", frames[f], f == count - 1 ? "" : ", ");
        }
        printf("]\n");
    }
    return stats;
}

void print_summary(const char *name, Stats s) {
    int total = s.page_faults + s.page_hits;
    double hit_ratio = total ? (double)s.page_hits / total : 0.0;
    double miss_ratio = total ? (double)s.page_faults / total : 0.0;
    printf("\n--- %s summary ---\n", name);
    printf("Total memory accesses : %d\n", total);
    printf("Page faults           : %d\n", s.page_faults);
    printf("Page hits             : %d\n", s.page_hits);
    printf("Hit ratio              : %.2f%%\n", hit_ratio * 100);
    printf("Miss ratio             : %.2f%%\n", miss_ratio * 100);
}

void run_comparison(const int *refs, int n_refs, int num_frames, int page_size_bytes) {
    printf("\n======================================================================\n");
    printf("Page reference string: [");
    for (int i = 0; i < n_refs; i++) printf("%d%s", refs[i], i == n_refs - 1 ? "" : ", ");
    printf("]\n");
    printf("Number of frames (RAM capacity): %d\n", num_frames);
    printf("Configured page size: %d bytes\n", page_size_bytes);
    printf("======================================================================\n");

    printf("\n[FIFO] step-by-step trace:\n");
    Stats fifo_stats = run_fifo(refs, n_refs, num_frames, page_size_bytes);
    print_summary("FIFO", fifo_stats);

    printf("\n[LRU] step-by-step trace:\n");
    Stats lru_stats = run_lru(refs, n_refs, num_frames);
    print_summary("LRU", lru_stats);

    printf("\n--- Side-by-side comparison ---\n");
    printf("%-10s%-10s%-10s%-12s%s\n", "Algorithm", "Faults", "Hits", "Hit Ratio", "Miss Ratio");
    int ft = fifo_stats.page_faults + fifo_stats.page_hits;
    int lt = lru_stats.page_faults + lru_stats.page_hits;
    printf("%-10s%-10d%-10d%-12.2f%.2f\n", "FIFO", fifo_stats.page_faults, fifo_stats.page_hits,
           100.0 * fifo_stats.page_hits / ft, 100.0 * fifo_stats.page_faults / ft);
    printf("%-10s%-10d%-10d%-12.2f%.2f\n", "LRU", lru_stats.page_faults, lru_stats.page_hits,
           100.0 * lru_stats.page_hits / lt, 100.0 * lru_stats.page_faults / lt);
}

int main(void) {
    /* Test case 1: classic textbook reference string */
    int refs1[] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1};
    run_comparison(refs1, sizeof(refs1) / sizeof(int), 3, 4096);

    /* Test case 2: strong "locality" -- LRU should shine here */
    int refs2[] = {1, 2, 3, 4, 1, 2, 5, 1, 2, 3, 4, 5};
    run_comparison(refs2, sizeof(refs2) / sizeof(int), 4, 4096);

    /* Test case 3: sequential access, no repeats -- FIFO and LRU tie */
    int refs3[10];
    for (int i = 0; i < 10; i++) refs3[i] = i + 1;
    run_comparison(refs3, 10, 3, 4096);

    printf("\n=== ALL TEST CASES COMPLETE ===\n");
    return 0;
}
