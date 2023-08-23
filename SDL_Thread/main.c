#include <stdio.h>
#include <SDL.h>

SDL_mutex *s_lock = NULL;
SDL_cond *s_cond = NULL;

int thread_work()
{
    SDL_LockMutex(s_lock);  //持有锁
    printf("                <============thread_work sleep\n");
    sleep(10);
    printf("                <============thread_work wait\n");

    //释放s_lock资源，并等待signal。之所以释放s_lock是让别的线程能够获取到s_lock
    //另一个线程(1)发送signal和(2)释放lock后，这个函数退出
    SDL_CondWait(s_cond,s_lock);
    printf("                <===========thread_work receive signal, continue to do ~_~!!!\n");
    printf("                <===========thread_work end\n");
    SDL_UnlockMutex(s_lock);
    return 0;
}

/**
◼ SDL线程创建：SDL_CreateThread
◼ SDL线程等待：SDL_WaitThead
◼ SDL互斥锁：SDL_CreateMutex/SDL_DestroyMutex
◼ SDL锁定互斥：SDL_LockMutex/SDL_UnlockMutex
◼ SDL条件变量(信号量)：SDL_CreateCond/SDL_DestoryCond
◼ SDL条件变量(信号量)等待/通知：SDL_CondWait/SDL_CondSingal
  * */
#undef main
int main()
{
    printf("Hello World!\n");
    s_lock = SDL_CreateMutex();
    s_cond = SDL_CreateCond();
    SDL_Thread *thread = SDL_CreateThread(
                    thread_work,
                    "thread_work",
                    NULL
                );

    if(thread == NULL){
        printf("  %s",SDL_GetError);
        return -1;
    }

    for(int i = 0;i< 2;i++)
    {
        sleep(2);
        printf("main execute =====>\n");
    }
    printf("main SDL_LockMutex(s_lock) before ====================>\n");
    SDL_LockMutex(s_lock); // 获取锁，但是子线程还拿着锁
    printf("main ready send signal====================>\n");
    printf("main SDL_CondSignal(s_cond) before ====================>\n");
    SDL_CondSignal(s_cond); // 发送信号，唤醒等待的线程
    printf("main SDL_CondSignal(s_cond) after ====================>\n");
    sleep(10);
    SDL_UnlockMutex(s_lock);// 释放锁，让其他线程可以拿到锁
    printf("main SDL_UnlockMutex(s_lock) after ====================>\n");

    SDL_WaitThread(thread, NULL);
    SDL_DestroyMutex(s_lock);
    SDL_DestroyCond(s_cond);

    return 0;
}
