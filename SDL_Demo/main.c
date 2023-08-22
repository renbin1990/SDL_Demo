#include <stdio.h>
#include <SDL.h>

#undef main
int main()
{
    printf("Hello World!\n");

    int run = 1;
    //声明窗口
    SDL_Window *window = NULL;
    //声明渲染器
    SDL_Renderer *renderer = NULL;
    //声明纹理
    SDL_Texture *texture = NULL;
    // 长方形，原点在左上角
    SDL_Rect rect;
    //设置宽高
    rect.w = 20,rect.h = 30;
    //设置窗口宽高
    int windowW = 320,windowH = 240;


    //初始化SDL函数
    SDL_Init(SDL_INIT_VIDEO);

    //创建window
    window = SDL_CreateWindow(
                "Test SDL",
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                windowW,
                windowH,
                SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
                );

    //检测是否出创建成功
    if(!window){
        printf("Can't create window, err:%s\n", SDL_GetError());
        return 1;
    }

    //创建渲染器
    renderer = SDL_CreateRenderer(window,-1,0);
    if(!renderer){
        return -1;
    }

    //创建纹理
    texture = SDL_CreateTexture(
                renderer,
                SDL_PIXELFORMAT_ABGR8888,
                SDL_TEXTUREACCESS_TARGET,
                windowW,
                windowH
                );
    if(!texture){
        return -1;
    }

    //创建窗口内数据
    int show_count = 0;
    while(run){
        rect.x = rand() % windowW;
        rect.y = rand() % windowH;

        // 设置渲染目标为纹理
        SDL_SetRenderTarget(renderer,texture);
        //设置纹理背景颜色
        SDL_SetRenderDrawColor(renderer,0,0,255,255);
        //清屏
        SDL_RenderClear(renderer);

        //绘制矩形
        SDL_RenderDrawRect(renderer,&rect);
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255); //长方形为白色
        SDL_RenderFillRect(renderer,&rect);


        SDL_SetRenderTarget(renderer, NULL); //恢复默认，渲染目标为窗口
        SDL_RenderCopy(renderer, texture, NULL, NULL); //拷贝纹理到CPU

        //输出到窗口上
        SDL_RenderPresent(renderer);
        SDL_Delay(300);
        if(show_count++ >=30){
            run = 0;
        }
    }


//    //显示10s
//    SDL_Delay(5000);

    //关闭窗口
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    //释放资源
    SDL_Quit();

    return 0;
}
