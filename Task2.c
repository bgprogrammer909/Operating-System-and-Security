/*
=========================================================
Virtual Memory Management Simulator
---------------------------------------------------------
*/

#include <stdio.h>

#define MAX 100

/* Convert logical address to page number */
int getPageNumber(int address, int pageSize)
{
    return address / pageSize;
}

/* Display current frames */
void displayFrames(int frames[], int frameCount)
{
    int i;

    printf("Frames: ");

    for(i = 0; i < frameCount; i++)
    {
        if(frames[i] == -1)
            printf("[ ] ");
        else
            printf("[%d] ", frames[i]);
    }

    printf("\n");
}

/* Check whether page exists */
int search(int frames[], int frameCount, int page)
{
    int i;

    for(i=0;i<frameCount;i++)
    {
        if(frames[i]==page)
            return i;
    }

    return -1;
}

/* FIFO Page Replacement */
void FIFO(int pages[], int totalPages, int frameCount)
{
    int frames[MAX];

    int pointer = 0;

    int hits = 0;
    int faults = 0;

    int i;

    for(i=0;i<frameCount;i++)
        frames[i]=-1;

    printf("\n=====================================\n");
    printf("FIFO PAGE REPLACEMENT\n");
    printf("=====================================\n");

    for(i=0;i<totalPages;i++)
    {
        int page = pages[i];

        if(search(frames,frameCount,page)!=-1)
        {
            hits++;

            printf("Page %d -> HIT\n",page);
        }
        else
        {
            faults++;

            printf("Page %d -> FAULT\n",page);

            frames[pointer]=page;

            pointer=(pointer+1)%frameCount;
        }

        displayFrames(frames,frameCount);
    }

    printf("\nHits   : %d\n",hits);
    printf("Faults : %d\n",faults);

    printf("Hit Ratio  : %.2f\n",(float)hits/(hits+faults));
    printf("Miss Ratio : %.2f\n",(float)faults/(hits+faults));
}

/* LRU Page Replacement */
void LRU(int pages[], int totalPages, int frameCount)
{
    int frames[MAX];

    int lastUsed[MAX];

    int hits=0;
    int faults=0;

    int i,j;

    for(i=0;i<frameCount;i++)
    {
        frames[i]=-1;
        lastUsed[i]=-1;
    }

    printf("\n=====================================\n");
    printf("LRU PAGE REPLACEMENT\n");
    printf("=====================================\n");

    for(i=0;i<totalPages;i++)
    {
        int page=pages[i];

        int index=search(frames,frameCount,page);

        if(index!=-1)
        {
            hits++;

            lastUsed[index]=i;

            printf("Page %d -> HIT\n",page);
        }
        else
        {
            faults++;

            printf("Page %d -> FAULT\n",page);

            int empty=-1;

            for(j=0;j<frameCount;j++)
            {
                if(frames[j]==-1)
                {
                    empty=j;
                    break;
                }
            }

            if(empty!=-1)
            {
                frames[empty]=page;
                lastUsed[empty]=i;
            }
            else
            {
                int lru=0;

                for(j=1;j<frameCount;j++)
                {
                    if(lastUsed[j]<lastUsed[lru])
                        lru=j;
                }

                frames[lru]=page;
                lastUsed[lru]=i;
            }
        }

        displayFrames(frames,frameCount);
    }

    printf("\nHits   : %d\n",hits);
    printf("Faults : %d\n",faults);

    printf("Hit Ratio  : %.2f\n",(float)hits/(hits+faults));
    printf("Miss Ratio : %.2f\n",(float)faults/(hits+faults));
}

int main()
{
    int pageSize;
    int frameCount;

    int n;

    int addresses[MAX];

    int pages[MAX];

    int i;

    printf("Virtual Memory Simulator\n");

    printf("------------------------------\n");

    printf("Enter Page Size: ");
    scanf("%d",&pageSize);

    printf("Enter Number of Frames: ");
    scanf("%d",&frameCount);

    printf("Enter Number of Logical Addresses: ");
    scanf("%d",&n);

    printf("\nEnter Logical Addresses:\n");

    for(i=0;i<n;i++)
    {
        scanf("%d",&addresses[i]);

        pages[i]=getPageNumber(addresses[i],pageSize);
    }

    printf("\nPage Reference String:\n");

    for(i=0;i<n;i++)
        printf("%d ",pages[i]);

    printf("\n");

    FIFO(pages,n,frameCount);

    LRU(pages,n,frameCount);

    return 0;
}