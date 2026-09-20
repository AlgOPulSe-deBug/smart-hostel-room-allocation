    #include <stdio.h>

    int heap[100], n=0;

    void swap(int *a, int *b)
    {
        int t = *a;
        *a = *b;
        *b = t;
    }
    void heapifyUp(int i)
    {
        while(i>0)
        {
            int p = (i-1)/2;
            if (heap[p] < heap[i])
            {
                swap(&heap[p], &heap[i]);
                i=p;
            }
            else
                break;
        }
        
    }
    void heapifyDown(int i)
    {
        while(1)
        {
            int l = 2*i+1;
            int r = 2*i+2;
            int largest = i;

            if (l<n && heap[l] > heap[largest])
                largest = l;
            if (r<n && heap[r] > heap[largest])
                largest = r;
            if(largest != i)
            {
                swap(&heap[i], &heap[largest]);
                i = largest;
            }
            else
                break;
        }
    }
    void Delete(int key)
    {
        int i;
        for(i =0;i<n;i++)
        {
            if (key == arr[i])
                break;
        }
        if(i == n)
        {
            printf("Element not found\n");
            return;
        }
        heap[i] = heap[n-1];
        n--;
        if (i>0 && heap[i]>heap[(i-1)/2])
            heapifyUp(i);
        else
            heapifyDown(i);

    }
    void deleteMax() 
    {
        heap[0] = heap[n - 1];
        n--;
        heapifyDown(0);
    }
    void inorder(int i)
    {
        
    }