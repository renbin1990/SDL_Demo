#include <stdio.h>
#include <SDL.h>

//自定义消息类型
#define REFRESH_EVENT   (SDL_USEREVENT + 1)     // 请求画面刷新事件
#define QUIT_EVENT      (SDL_USEREVENT + 2)     // 退出事件
static const float ASPECT_RATIO = 16.f/12.f;

//定义分辨率
// YUV像素分辨率
#define YUV_WIDTH   320
#define YUV_HEIGHT  240
//定义YUV格式
#define YUV_FORMAT  SDL_PIXELFORMAT_IYUV

int s_thread_exit = 0;  // 退出标志 = 1则退出

int refresh_video_timer(){
    while (!s_thread_exit) {
        SDL_Event event;
        event.type = REFRESH_EVENT;
        SDL_PushEvent(&event);
        SDL_Delay(40);
    }

    s_thread_exit = 0;

    //push quit event
    SDL_Event event;
    event.type = QUIT_EVENT;
    SDL_PushEvent(&event);

    return 0;
}


/**
  * 播放YUV视频，命名错误
  * ffmpeg -i test_1920x1080.mp4 -t 20 -pix_fmt yuv420p -s 320x240 -y yuv420p_v.yuv
  * ffplay -pixel_format yuv420p -video_size 320x240 -framerate 15 yuv420p_v.yuv
  * */
#undef main
int main()
{
    //初始化 SDL
    if(SDL_Init(SDL_INIT_VIDEO))
    {
        fprintf( stderr, "Could not initialize SDL - %s\n", SDL_GetError());
        return -1;
    }

    //SDL相关初始化
    SDL_Window *window = NULL;          //窗口
    SDL_Renderer *renderer = NULL;      //渲染器
    SDL_Texture *texture = NULL;        //纹理
    SDL_Thread *timer_thread = NULL;    //线请求的刷新线程
    SDL_Rect rect;                      //矩形
    SDL_Event event;                    //事件
    uint32_t pixformat = YUV_FORMAT;    //视频格式 YUV420P，即是SDL_PIXELFORMAT_IYUV

    // 分辨率
    // 1. YUV的分辨率
    int video_width = YUV_WIDTH;
    int video_height = YUV_HEIGHT;
    // 2.显示窗口的分辨率
    int win_width = YUV_WIDTH;
    int win_height = YUV_WIDTH;

    //YUV文件读取信息
    FILE *video_fd = NULL;
    const char *yuv_path = "D:\\project\\musicVideo\\SDL_PlayPCM\\yuv420p_v.yuv";
    size_t  video_buff_len = 0;
    uint8_t *video_buf = NULL; //读取数据后先把放到buffer里面

    //计算文件长度  我们测试的文件是YUV420P格式 YUV420P比例是4:1:1
    uint32_t y_frame_len = video_width*video_height;
    uint32_t u_frame_len = video_width * video_height / 4;
    uint32_t v_frame_len = video_width * video_height / 4;
    //总长度
    uint32_t yuv_frame_len = y_frame_len + u_frame_len + v_frame_len;

    //创建窗口
    window = SDL_CreateWindow(
                "YUV PLAY TEST",
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                video_width,
                video_height,
                SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE
                );
    if(window == NULL)
    {
        fprintf(stderr, "SDL: could not create window, err:%s\n",SDL_GetError());
        goto _FAIL;
    }

   // 基于窗口创建渲染器
    renderer = SDL_CreateRenderer(window,-1,0);
    if(renderer == NULL)
    {
        fprintf(stderr, "SDL: could not create renderer, err:%s\n",SDL_GetError());
        goto _FAIL;
    }

    //基于渲染器创建纹理
    texture = SDL_CreateTexture(renderer,
                                pixformat,
                                SDL_TEXTUREACCESS_STREAMING,
                                video_width,
                                video_height
                                );
    if(texture == NULL)
    {
        fprintf(stderr, "SDL: could not create texture, err:%s\n",SDL_GetError());
        goto _FAIL;
    }

    //分配空间
    video_buf = (uint8_t*)malloc(yuv_frame_len);
    if(!video_buf)
    {
        fprintf(stderr, "Failed to alloce yuv frame space!\n");
        goto _FAIL;
    }

     // 打开YUV文件  以二进制读取
    video_fd = fopen(yuv_path,"rb");
    if( !video_fd )
    {
        fprintf(stderr, "Failed to open yuv file\n");
        goto _FAIL;
    }

    // 创建请求刷新线程
    timer_thread = SDL_CreateThread(
                refresh_video_timer,"refresh_video_timer",NULL
                );

    while (1)
    {
    // 收取SDL系统里面的事件
        SDL_WaitEvent(&event);
        switch (event.type) {
        case REFRESH_EVENT:   //页面刷新事件
            video_buff_len = fread(video_buf,1,yuv_frame_len,video_fd);
            if(video_buff_len <= 0 ){
                //说明视频结束了
                fprintf(stderr, "Failed to read data from yuv file!\n");
                goto _FAIL;
            }

            // 设置纹理的数据 video_width = 320， plane NULL表示整个矩形*纹理
            SDL_UpdateTexture(texture,NULL,video_buf,video_width);

            // 显示区域，可以通过修改w和h进行缩放
            rect.x = 0;
            rect.y = 0;
            float w_ratio = win_width * 1.0 /video_width;
            float h_ratio = win_height * 1.0 /video_height;
            // 320x240 怎么保持原视频的宽高比例
            int tempW = video_width * w_ratio;
            int tempH = video_height * h_ratio;
            float aspectRatio = (float)tempW/(float)tempH;
            if(aspectRatio == ASPECT_RATIO){
                rect.w = tempW;
                rect.h = tempH;
            }else if(aspectRatio > ASPECT_RATIO){
                rect.w = ASPECT_RATIO * tempH;
                rect.h = tempH;
            }else{
                rect.w = tempW;
                rect.h = (1.f / ASPECT_RATIO) * tempW;
            }


//            rect.w = video_width * w_ratio;
//            rect.h = video_height * h_ratio;

            // 清除当前显示
            SDL_RenderClear(renderer);
            // 将纹理的数据拷贝给渲染器  NULL表示整个矩形*纹理
            SDL_RenderCopy(renderer,texture,NULL,&rect);
            //显示
            SDL_RenderPresent(renderer);

            break;
        case SDL_WINDOWEVENT:  //窗口事件
            SDL_GetWindowSize(window,&win_width,&win_height);
            printf("SDL_WINDOWEVENT win_width:%d, win_height:%d\n",win_width,
                   win_height );
            break;
        case QUIT_EVENT:   //退出事件
            break;
        case SDL_QUIT:   //退出事件
            s_thread_exit = 1;
            break;
        default:
            break;
        }

    }


_FAIL:
    s_thread_exit = 1;      // 保证线程能够退出
    // 释放资源
    if(timer_thread)
        SDL_WaitThread(timer_thread, NULL); // 等待线程退出
    if(video_buf)
        free(video_buf);
    if(video_fd)
        fclose(video_fd);
    if(texture)
        SDL_DestroyTexture(texture);
    if(renderer)
        SDL_DestroyRenderer(renderer);
    if(window)
        SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}
