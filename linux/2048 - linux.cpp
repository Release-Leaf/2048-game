#include<stdio.h>
#include<chrono>
#include<thread>
#include<random>
#include<termios.h>
#include<unistd.h>
#include<signal.h>
#include<locale.h>
struct termios orig_term;
void set_raw_mode(){
    struct termios t;
    tcgetattr(STDIN_FILENO,&orig_term);
    t=orig_term;
    t.c_lflag&=~(ICANON|ECHO);
    tcsetattr(STDIN_FILENO,TCSANOW,&t);
}
void reset_mode(){
    tcsetattr(STDIN_FILENO,TCSANOW,&orig_term);
}
int _getch(){
    char c;
    ssize_t k=read(STDIN_FILENO,&c,1);
    return(unsigned char)c;
}
void Sleep(int ms){usleep(ms*1000);}
#define uidi std::uniform_int_distribution<int>
#define fori(i) for(int i=0;i<4;i++)
#define ui64 unsigned long long
uint32_t x[4][4],tmp[5];
ui64 score,best;
std::random_device rd;
std::mt19937 gen(rd());
uidi dist(0,9);//随机数
bool fileok(){
    FILE*f=fopen("2048.sav","rb");
    if(f){
        fclose(f);
        return 1;
    }
    return 0;
}
void save(const char*name="2048.sav"){
    FILE*f=fopen(name,"wb");
    if(!f)return;
    fwrite(&score,sizeof score,1,f);
    fwrite(x,sizeof x,1,f);
    fwrite(&best,sizeof best,1,f);
    fclose(f);
}
void load(){
    FILE*f=fopen("2048.sav","rb");
    if(!f)return;
    int k=fread(&score,sizeof score,1,f);
    k=fread(x,sizeof x,1,f);
    k=fread(&best,sizeof best,1,f);
    fclose(f);
    save("2048.sav_old");//存档备份
}
inline void clear_tmp(){for(int i=0;i<4;i++)tmp[i]=0;}
void create(){//生成空格子的数字
    int cnt=0;
    for(int i=0;i<4;i++)for(int j=0;j<4;j++)if(!x[i][j])cnt++;
    if(!cnt)return;
    int pos=uidi(1,cnt)(gen);
    cnt=0;
    for(int i=0;i<4;i++)for(int j=0;j<4;j++){
        if(!x[i][j])cnt++;
        if(cnt==pos){
            int k=dist(gen);
            x[i][j]=k<9?2:4;
            return;
        }
    }
}
bool turn_on(){//上
    bool flag=1,lq=1;
    fori(j){
        clear_tmp();
        for(int i=0,k=0;i<4;i++){
            if(x[i][j]){
                if(lq&&k&&tmp[k-1]==x[i][j])tmp[k-1]*=2,lq=0,score+=tmp[k-1];
                else tmp[k++]=x[i][j],lq=1;
            }
        }
        fori(i){
            if(flag&&x[i][j]!=tmp[i])flag=0;
            x[i][j]=tmp[i];
        }
    }
    return flag;
}
bool turn_down(){//下
    bool flag=1,lq=1;
    fori(j){
        clear_tmp();
        for(int i=3,k=0;i>=0;i--){
            if(x[i][j]){
                if(lq&&k&&tmp[k-1]==x[i][j])tmp[k-1]*=2,lq=0,score+=tmp[k-1];
                else tmp[k++]=x[i][j],lq=1;
            }
        }
        for(int i=3;i>=0;i--){
            if(flag&&x[i][j]!=tmp[3-i])flag=0;
            x[i][j]=tmp[3-i];
        }
    }
    return flag;
}
bool turn_left(){//左
    bool flag=1,lq=1;
    fori(i){
        clear_tmp();
        for(int j=0,k=0;j<4;j++){
            if(x[i][j]){
                if(lq&&k&&tmp[k-1]==x[i][j])tmp[k-1]*=2,lq=0,score+=tmp[k-1];
                else tmp[k++]=x[i][j],lq=1;
            }
        }
        fori(j){
            if(flag&&x[i][j]!=tmp[j])flag=0;
            x[i][j]=tmp[j];
        }
    }
    return flag;
}
bool turn_right(){//右
    bool flag=1,lq=1;
    fori(i){
        clear_tmp();
        for(int j=3,k=0;j>=0;j--){
            if(x[i][j]){
                if(lq&&k&&tmp[k-1]==x[i][j])tmp[k-1]*=2,lq=0,score+=tmp[k-1];
                else tmp[k++]=x[i][j],lq=1;
            }
        }
        for(int j=3;j>=0;j--){
            if(flag&&x[i][j]!=tmp[3-j])flag=0;
            x[i][j]=tmp[3-j];
        }
    }
    return flag;
}
inline int getlen(ui64 n){
    if(n<10)return 1;
    if(n<100)return 2;
    if(n<1000)return 3;
    if(n<10000)return 4;
    if(n<100000)return 5;
    if(n<1000000)return 6;
    return 6+getlen(n/1000000);
}
const char*get_color(uint32_t val){//获取数字对应的颜色字符串
    if(val==0)return"\033[48;5;187m";//空格子
    switch(val){
        case 2:return"\033[48;5;230m\033[38;5;238m";
        case 4:return"\033[48;5;229m\033[38;5;238m";
        case 8:return"\033[48;5;216m\033[38;5;15m";
        case 16:return"\033[48;5;209m\033[38;5;15m";
        case 32:return"\033[48;5;203m\033[38;5;15m";
        case 64:return"\033[48;5;196m\033[38;5;15m";
        case 128:return"\033[48;5;221m\033[38;5;15m";
        case 256:return"\033[48;5;220m\033[38;5;15m";
        case 512:return"\033[48;5;214m\033[38;5;15m";
        case 1024:return"\033[48;5;208m\033[38;5;15m";
        case 2048:return"\033[48;5;202m\033[38;5;15m";
        case 4096:return"\033[48;5;198m\033[38;5;15m";
        case 8192:return"\033[48;5;127m\033[38;5;15m";
        case 16384:return"\033[48;5;21m\033[38;5;15m";
        case 32768:return"\033[48;5;28m\033[38;5;15m";
        case 65536:return"\033[48;5;226m\033[38;5;238m";
        default:return"\033[48;5;240m\033[38;5;15m";//超大数字
    }
}
#define null_str len+=sprintf(buf+len,"\033[48;5;230m|%s               ",get_color(x[i][j]))
#define clear_enter len+=sprintf(buf+len,"\033[48;5;230m|\n")
void print(){
    char buf[8192];
    int len=0;
    best=(best<score?score:best);
    len+=sprintf(buf+len,"\033[0m\033[3J\033[H");
    int nlen=getlen(score);
    len+=sprintf(buf+len,"\033[48;5;227mSCORES:%llu%*s",score,27-nlen,"");
    nlen=getlen(best);
    len+=sprintf(buf+len,"BEST:%llu%*s\033[48;5;230m\n",best,26-nlen,"");
    fori(i){
        fori(j)len+=sprintf(buf+len,"+---------------");
        len+=sprintf(buf+len,"+\n");
        fori(j)null_str;clear_enter;
        fori(j)null_str;clear_enter;
        fori(j){
            if(x[i][j]==0)null_str;
            else{
                nlen=getlen(x[i][j]);
                len+=sprintf(buf+len,"\033[48;5;230m|%s%*s%u%*s",get_color(x[i][j]),(15-nlen)>>1,""/*左空格*/,x[i][j],(16/*16=15+1向上取整*/-nlen)>>1,""/*右空格*/);
            }
        }
        clear_enter;
        fori(j)null_str;clear_enter;
        fori(j)null_str;clear_enter;
    }
    fori(j)len+=sprintf(buf+len,"+---------------");
    len+=sprintf(buf+len,"+");
    printf("%s",buf);
    fflush(stdout);
}
bool turn(int c){
    if(c==72)return turn_on();
    if(c==80)return turn_down();
    if(c==75)return turn_left();
    if(c==77)return turn_right();
    return 1;
}
void signal_handler(int sig) {
    switch(sig) {
        case SIGINT:case SIGTERM:case SIGHUP:
            save();exit(0);
            break;
        default:break;
    }
}
bool gameover(){
    fori(i)fori(j){
        if(!x[i][j])return 0;
        if(i>0&&x[i][j]==x[i-1][j])return 0;
        if(j>0&&x[i][j]==x[i][j-1])return 0;
    }
    return 1;
}
int getch(){
    int c=_getch();
    if(c==27){
        c=_getch();
        if(c==91){
            c=_getch();
            switch(c){
                case 65:return 72;
                case 66:return 80;
                case 67:return 77;
                case 68:return 75;
                default:return 27;
            }
        }
        return 27;
    }
    return c;
}
int main(){
    setlocale(LC_ALL, "en_US.UTF-8");
    set_raw_mode();
    if(fileok()){
        load();
    }else create(),create();
    print();
    while(1){
        if(gameover()){
            puts("\033[0m\nGameOver\n按任意键重开...");
            _getch();
            fori(i)fori(j)x[i][j]=0;score=0;
            create();create();print();
        }
        Sleep(50);//防熊孩子
        int c=getch();
        if(c==27)break;
        if(!turn(c)){
            create();
            print();
        }
    }
    save();
    return 0;
}