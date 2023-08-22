#include <stdio.h>
#include <SDL.h>

#define FF_QUIT_EVENT (SDL_USEREVENT + 2) // 用户自定义事件

/**
*
* SDL事件
◼ 函数
• SDL_WaitEvent()：等待一个事件
• SDL_PushEvent()：发送一个事件
• SDL_PumpEvents()：将硬件设备产生的事件放入事件队列，用于读取事件，在调用该函数之前，必须调用SDL_PumpEvents搜集键盘等事件
• SDL_PeepEvents()：从事件队列提取一个事件
◼ 数据结构
• SDL_Event：代表一个事件
**/
#undef main
int main()
{
    int videoW = 640,videoH = 480;
//    printf("Hello World!\n");

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
                "TEST SDL_EVENT",
                SDL_WINDOWPOS_CENTERED,         //坐标位置
                SDL_WINDOWPOS_CENTERED,
                videoW,
                videoH,
                SDL_WINDOW_SHOWN | SDL_WINDOW_BORDERLESS// flags - see below
                );
    if(window == NULL){
        return -1;
    }

    renderer = SDL_CreateRenderer(
                    window,
                    -1,
                    0
                );
    if(renderer == NULL){
        return -1;
    }

    SDL_SetRenderDrawColor(renderer,0,0,255,255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);

    SDL_Event event;
    int b_exit = 0;
    for(;;)
    {
        SDL_WaitEvent(&event);
        switch (event.type)
        {
        case SDL_KEYDOWN:   //键盘事件
            switch (event.key.keysym.sym)
            {
            case SDLK_a:
                printf("key down a\n");
                break;
            case SDLK_s:
                printf("key down s\n");
                break;
            case SDLK_d:
                printf("key down d\n");
                break;
            case SDLK_q:
                printf("key down q and push quit event\n");
                SDL_Event event_q;
                event_q.type = FF_QUIT_EVENT;
                SDL_PushEvent(&event_q);
                break;
            default:
                printf("key down 0x%x\n", event.key.keysym.sym);
                break;
            }
            break;
        case SDL_MOUSEBUTTONDOWN:			/* 鼠标按下事件 */
            if (event.button.button == SDL_BUTTON_LEFT)
            {
                printf("mouse down left\n");
            }
            else if(event.button.button == SDL_BUTTON_RIGHT)
            {
                printf("mouse down right\n");
            }
            else
            {
                printf("mouse down %d\n", event.button.button);
            }
             break;
        case SDL_MOUSEMOTION:		/* 鼠标移动事件 */
            printf("mouse movie (%d,%d)\n", event.button.x, event.button.y);
            break;
        case FF_QUIT_EVENT:
            printf("receive quit event\n");
            b_exit = 1;
            break;
        default:
            break;
        }
        if(b_exit)
            break;
    }

//    SDL_Delay(5000);

    //销毁释放资源
    if(renderer != NULL){
        SDL_DestroyRenderer(renderer);
    }

    if(window != NULL){
        SDL_DestroyWindow(window);
    }

    SDL_Quit();

    return 0;
}
