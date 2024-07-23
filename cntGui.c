/*
 * cntGui.c
 * menuConの治具用
 * vbox hboxで構成した画面の例　確認用
 * box型は画面サイズの可変に対応する
 * 
 * 
 * cd /home/nakaza0904/LPrg デレクトリに移動しておく
 * 
 * git clone https//:github.com/M-nakaza-pop/GTK_Sample GTK_Sample(デレクトリが作成される)
 * git clone -b timer_sample https::/github.com/M-nakaza-pop/GTK_sample.git TimerSample
 * 
 *  git diff		//差分
 *  git checkout [file]	//変更前に戻す
 * 
 * 	git add [file]
 * 	git commit -m "**を変更した"	//	ここまではLocal側
 * git push -u origin main		//初回のうまく行かないとき
 * git push							//2回目からは
 * 
 * 
 * gcc -W -Wall -o buttontest buttontest.c `pkg-config --libs --cflags gtk+-3.0`
 *	sudo ./buttontest  --> run
 * 
 * 
 * 
 * gtk_box_pack_start(GTK_BOX(hbox),label, TRUE, TRUE, 20);
 * 
 * 	TRUE TRUE
 *  TRUE FALSE
 *  FALSE FALSE の組み合わせ
 * 
 * 
 *  
 * 
 */


/***********************************************************
 * Includes
 ***********************************************************/
#include 	<gtk/gtk.h>
#include 	<string.h>		//文字列処理		
#include	<stdio.h>		//標準入出力
#include	<unistd.h>		//UNIX標準
#include	<stdlib.h>

#include	<wiringPi.h>	
#include	<wiringSerial.h>
#include	<termios.h>		//parity set


#define	GPIO18	18

#define	ENQ		0x05
#define	EOT		0x04
#define	ETX		0x03
#define	STX		0x02
#define ACK		0x06

#define adr0	34
#define adr1	35
#define	adr2	36
#define	rem		37

typedef	unsigned char	uint8_t;
typedef	unsigned int	uint16_t;

/***********************************************************
 * Global variables
 ***********************************************************/
 gchar		buf0[50];		/* 表示buff */ 
 gchar		buf1[50];
 gchar		buf2[50];
 

/* 
* https://scrapbox.io/imasaraC/曜日を求める
* localtime() 時間を表す構造体  include <time.h と被るのでtag名はtmmにしている 
* http://www1.cts.ne.jp/~clab/hsample/Time/Time4.html
*/
struct tmm{
	  
		gint tm_sec;		//秒
		gint tm_min;		//分
		gint tm_hour;	//時
		gint tm_mday;	//日
		gint tm_mon;		//月
		gint tm_year;	//西暦年
		gint tm_wday;	//曜日(日＝0)
		gint tm_yday;			
		gint tm_isday;	//サマータイムフラグ
};
#if 0
 
//struct tmm disp = {  0,55,16,4,5,2023,1,0,0}; 
//char* a[7]= {"日","月","火","水","木","金","土"};
//char* b[2]= {"*"," "};
//gint 	room= 101;		/* 部屋番号 */
 

//gint	ci_hour 	= 12;
//gint	ci_min		= 20;
//gint	stay_hour	= 2;
//gint	stay_min	= 10;     

 struct ymd{
	  
		gint day;		//日
		gint mon;		//月
		gint year;	//西暦年
		gint room1;
		gint room2;
		gint room3;
};
struct ymd regi = { 00, 00, 00, 0101, 0102, 0103}; 
struct ymd anni = { 11, 11, 11, 101, 102, 103}; 

//GtkWidget *label1; 
//GtkWidget *label2; 

guint	tickercount;

gint	raiten 	= 4;
gint 	point		=1024;
gint	total		=84830;

//int		seikyu[10]	= {3900,1000,1500,0,0,0,0,0,0,0};
//char *num1 = {"12345678"};
//GtkWidget *btn1,*btn2,*btn3;
#endif


char	rcvbuf[33] = {0};		

char	trsbuf[]={		0x01,0x02,0x03,0x04,0x05,0x06,
						0x07,0x08,0x09,0x10,0x11,0x12,
						0x13,0x14,0x15,0x16,0x17,0x18,
						0x19,0x20,0xfa,0xfa,0xfa,
						0xfa,0xfa,0xfa,0xfa,0xfa,
						0xfa,0xfa,0xfa,0xfa,0xfa,
						0xfa,0xfa,0xfa,0xfa,0xfa,
						0xfa,0xfa,0xfa,0xfa,0xfa,
						0xfa,0xfa,0xfa,0xfa,0xfa,
						0xfa,0xfa,0xfa,0xfa,0x00
					};
					
char	gcombuf[20] = {0};
char	sendflg;

char	moni[16] ={0};		//input 


GtkWidget *PGui[11];



struct	covtable{
	
	char	ascii;
	char	hex;
	};
struct covtable	aschex[]={
			
			{'0', 0x00},
			{'1', 0x01},
			{'2', 0x02},
			{'3', 0x03},
			{'4', 0x04},
			{'5', 0x05},
			{'6', 0x06},
			{'7', 0x07},
			{'8', 0x08},
			{'9', 0x09},
			{'A', 0x0A},
			{'B', 0x0B},
			{'C', 0x0C},
			{'D', 0x0D},
			{'E', 0x0E},
			{'F', 0x0F}	
	};


int	fd;

GtkWidget	*button[38];
GtkWidget	*label[38];
GtkWidget	*box1;


struct input{
	gchar	e;		// 
	gchar	y;		// 1= Y電文発行済みなのでEnq, 2=G電文あり
	char 	n[2];	// collor n[0] n[1]='\0'
	char 	r[5]; 	// 表示文字
	};
				
struct input	status[]= {
	{0,0,"0","01"},{0,0,"0","02"},{0,0,"0","03"},{0,0,"0","04"},{0,0,"0","05"},{0,0,"0","06"},
	{0,0,"0","07"},{0,0,"0","08"},{0,0,"0","09"},{0,0,"0","10"},{0,0,"0","11"},{0,0,"0","12"},
	{0,0,"0","13"},{0,0,"0","14"},{0,0,"0","15"},{0,0,"3","--"},{0,0,"8","?01"},{0,0,"8","?02"},
	{0,0,"8","?03"},{0,0,"8","?04"},{0,0,"8","?05"},{0,0,"8","?06"},{0,0,"8","?07"},{0,0,"8","?08"},
	{0,0,"8","?09"},{0,0,"8","?10"},{0,0,"8","?11"},{0,0,"8","?12"},{0,0,"8","?13"},{0,0,"8","?14"},
	{0,0,"0","ENQ"},{0,0,"0","V"},{0,0,"0","Y"},{0,0,"0","G"},
	{0,0,"2","ERR"},{0,0,"2","adr1"},{0,0,"2","adr2"},{0,0,"2","00"}
	},*pst;
	
unsigned int	address =0x00;
char	adr[5];

/***********************************************************
 * Functions
 ***********************************************************/
void 			myCss(void);
void			quitGtk(GtkWidget *widget, GtkWidget *no_use);
GtkWidget		*create_window(gchar *title, gint width, gint height);
GtkWidget		*createButton(const gchar *label, const gchar *name, int horiz,int vertical);
GtkWidget		*create_Grid(const gchar *name);
GtkWidget		*createLabel(const gchar *title, const gchar *name);
static gint	ticker_interrupt(gint *const p);
//static gint	idlefunc(gpointer user_data);

GtkWidget 	*Gui10(void);

void	btn_clicked(GtkWidget *widget, gpointer data);


void	serialSeting(void);
char		serRead(char *buf);
void	stxComm(char *buf);
void	txFinish(unsigned int num);
void	startFa(void);
void	enqCommand(char	addr);
void	vCommand(char addr);
void	yCommand(char addr);
char	gCommand(void);
char	serialWrite(int fd,char *buf,int len);
char	makeLrc(char *buf,char lim);
char	hexascConvert(char type, unsigned char num);
void	iniLabel(void);
void	setGcombuf(char *buf);
char	btnColor(int i, char val);
char	btnSelect(int i, char *sw, char c);
char	inDisp(struct input *ptr, char *buf);
char	inBit(char *rbuf);
char	remDisp(struct input *ptr, char *rbuf);

void	testToggle(void);

/*******************************************************************************
* File Name    	:
* Version      	:
* Device(s)    	:
* Description  	:
* Creation Date	:
*******************************************************************************/
int main(int argc, char *argv[]){
	
    
    
    
	int	dispnum= 0;

	gtk_init(&argc, &argv);
	myCss();
	GtkWidget* window= create_window("TEST", 800, 480);

	PGui[0]= Gui10();


#if 1
	GtkWidget *disp= gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(disp),  PGui[0]);
	gtk_container_add(GTK_CONTAINER(disp),  PGui[1]);
	gtk_container_add(GTK_CONTAINER(disp),  PGui[2]);
	gtk_container_add(GTK_CONTAINER(disp),  PGui[3]);
	gtk_container_add(GTK_CONTAINER(disp),  PGui[4]);
	gtk_container_add(GTK_CONTAINER(disp),  PGui[5]);
	gtk_container_add(GTK_CONTAINER(disp),  PGui[6]);
	gtk_container_add(GTK_CONTAINER(disp),  PGui[7]);
	gtk_container_add(GTK_CONTAINER(disp),  PGui[8]);
	gtk_container_add(GTK_CONTAINER(disp),  PGui[9]);

#else
	GtkWidget *disp = gtk_fixed_new();
	gtk_fixed_put(GTK_FIXED(disp),  PGui[1], 0, 0);
	gtk_fixed_put(GTK_FIXED(disp),  PGui[2], 0, 0);
	gtk_fixed_put(GTK_FIXED(disp),  PGui[3], 0, 0);
	gtk_fixed_put(GTK_FIXED(disp),  PGui[4], 0, 0);
	gtk_fixed_put(GTK_FIXED(disp),  PGui[5], 0, 0);
	gtk_fixed_put(GTK_FIXED(disp),  PGui[6], 0, 0);
	gtk_fixed_put(GTK_FIXED(disp),  PGui[7], 0, 0);
	gtk_fixed_put(GTK_FIXED(disp),  PGui[8], 0, 0);
	gtk_fixed_put(GTK_FIXED(disp),  PGui[9], 0, 0);

#endif


	gtk_container_add(GTK_CONTAINER(window), disp);	/* 初期画面をウィンドウにパック */     
	gtk_widget_show_all(window);

	g_timeout_add((int)1,(GSourceFunc)ticker_interrupt,&dispnum);		//1mSでtimeout()を実行)
	//g_idle_add((GSourceFunc)idlefunc,NULL);
		
	serialSeting();
	iniLabel();
	
	sprintf(status[34].r,"%d",address);
	gtk_button_set_label(GTK_BUTTON(button[34]),status[34].r);
	setGcombuf(gcombuf);
	
	gtk_main();
}
/*******************************************************************************
* File Name    	:	
* Version      	:
* Device(s)    	:
* Description  	:
* Creation Date	:
*******************************************************************************/
static gint	ticker_interrupt(gint *const p){
	
#if 0
	
	gint	i;   
	
	if(++tickercount < 200) return 1;
		
	tickercount= 0;
	
	(*p)++;
	if(*p > 10) *p= 1;
	
	g_print("print= %d\n",*p);
	
	for(i=1; i< 11; i++){
		//gtk_widget_hide(PGui[i]);
	}
		
		//gtk_widget_show(PGui[9]);
#else
/*
 *	ticker_interruptを1mSEC設定
 *  
 */
	static	unsigned int	lcnt= 0;
	static  unsigned int	lcnt10= 0;
	static  unsigned int	lcnt100= 0;
	static  char			comflg= '\0';
	
	
	
	comflg|= serRead(rcvbuf);
	//puts(".");
	lcnt= ((lcnt+1) % 10);	// 1mS incとすると	同じ=if(++cunt10>9)	cunt10= 0;
							// 0-9で回転する
	// lcnt= ++lcnt;にするとerrer[wsequence-point]になる
	// lcntを２回更新するから,++で１回目、 =で２回目
	
	switch(lcnt){

		case 0:					// 10mSに1回回る
		
			break;
			
		case 1:					// 10mSが10回なので100mS毎に実行
			if(++lcnt10> 9){
				lcnt10= 0;
				
				
				
				status[adr0].n[0]= (0x04==comflg||0x06==comflg||'!'==comflg)?	 '6': '2';
				gtk_widget_set_name(button[adr0],status[adr0].n);
				if(0x04==comflg||0x06==comflg) comflg= 0;
				if('!'==comflg){

					puts("!");
					comflg= inDisp(status, rcvbuf);
					inBit(rcvbuf);
					remDisp(status, rcvbuf);
				}	
				if(sendflg=='t' && status[adr0].n[0]=='6'){
					sendflg= gCommand();
					puts("G電文");
				}
				else enqCommand(0);		//status[0].y == 1					
			}	 
			break;
			
		case 2:
			if((lcnt100=(lcnt100+ 1)% 1000)== 0){

				testToggle();
				//gCommand();				// 10mSが1000回なので10S毎に実行
			}	
			break;
			
		default:			
			break;
	}
#endif

	return 1;			//1継続　0停止
}
/*for ticker_interrupt*/

/*******************************************************************************
* File Name    	:	
* Version      	:
* Device(s)    	:
* Description  	:
* Creation Date	:
*******************************************************************************/
#if 0
static gint idlefunc(gpointer user_data){

	return	0;	



	serRead(rcvbuf);
	
	if(rxflg== '!'){
		
		rxflg= 0x00;
		
		//printf("rcv:%c",rcvbuf[1]);
		stxComm(trsbuf);
	};
//	g_print("idle\n");
				//falseの場合は単発、trueの場合は繰り返し呼ばれる
	return 1;	/* 常にtrueにした場合、かなりCPUを喰うので注意 */
}
#endif
/**********************************************************************/
//	Window終了で呼び出される関数
/**********************************************************************/
void	quitGtk(GtkWidget *widget, GtkWidget *no_use){
		gtk_main_quit();
}   
/*******************************************************************************
* File Name    	:	windowを作る関数 Create Window
* Version      	:
* Device(s)    	:
* Description  	:
* Creation Date	:
*******************************************************************************/
GtkWidget	*create_window(gchar *title, gint width, gint height){

		GtkWidget	*window;

		window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
		gtk_window_set_title(GTK_WINDOW(window), title);
		gtk_window_set_default_size(GTK_WINDOW(window), width, height);
		gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
		g_signal_connect(G_OBJECT(window), "destroy",
		G_CALLBACK(quitGtk),window);

		return window;
} 
/*******************************************************************************
* File Name    	:	画面3
* Description  	:
*******************************************************************************/
 GtkWidget* createLabel(const gchar *title, const gchar *name){   
	 
	GtkWidget *label = gtk_label_new(title);
	gtk_widget_set_name(label, name);			//CCSで使う
	
	return	label; 
 }
/*******************************************************************************
* File Name    	:
* Version      	:
* Device(s)    	:
* Description  	:
* Creation Date	:
*******************************************************************************/
GtkWidget	*create_Grid(const gchar *name){
    
		GtkWidget	*grid = gtk_grid_new ();	//マス目状配置
		
		gtk_grid_set_row_homogeneous(GTK_GRID(grid), FALSE);	//縦一杯(true)
		gtk_grid_set_column_homogeneous(GTK_GRID(grid), FALSE);	//横一杯(true)
		//gtk_grid_set_row_spacing(GTK_GRID(grid), 10);		//縦間隔
		//gtk_grid_set_column_spacing(GTK_GRID(grid), 10);	//横間隔
		gtk_widget_set_name(grid, name);
#if 1
		gtk_container_set_border_width(GTK_CONTAINER (grid), 10); //コンテナ周りの枠の幅
#endif
		g_object_set (grid, "margin", 0, NULL);
		
		return	grid;
}
/*******************************************************************************
* File Name    	:	CCS
* Description  	:
*******************************************************************************/
void myCss(void){

	GError *error = NULL;
	GtkCssProvider*	provider= gtk_css_provider_new();
	gtk_css_provider_load_from_path(provider,"./myCss.css",&error);
	if(error!= NULL){
		g_print("Error:\t%s_n",error->message);
		g_error_free(error);
		return;
	}
	gtk_style_context_add_provider_for_screen (gdk_screen_get_default (), 
                                               GTK_STYLE_PROVIDER (provider),
                                               GTK_STYLE_PROVIDER_PRIORITY_USER);
}
/*******************************************************************************
* File Name    	:	
* Version      	:
* Device(s)    	:
* Description  	:
* Creation Date	:
*******************************************************************************/
void	iniLabel(void){
	
	int	i;
	
	for(i=0; i< 30; i++){
		
		gtk_widget_set_name(button[i],status[i].n);
		
	}
}	
/*******************************************************************************
* File Name    	:
* Version      	:
* Device(s)    	:	
* Description  	:
* Creation Date	:
*******************************************************************************/
GtkWidget	*Gui10(void){
	
	
	unsigned int	i;
			
			
	
	GtkWidget *vbox= gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_widget_set_size_request(vbox,800,480);
		
	
	GtkWidget	*Alabel= gtk_label_new("出力指示");
	gtk_widget_set_name(Alabel,"label");			//CCSで使う
	GtkWidget *hbox1= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_name(hbox1,"box1");
	
	gtk_box_pack_start(GTK_BOX(hbox1),Alabel, TRUE, TRUE, 10);
	
	/*		2目		*/
	GtkWidget *hbox2= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_set_homogeneous(GTK_BOX(hbox2), TRUE);	 //	縦均一化
	gtk_widget_set_name(hbox2,"box2");
	
	for(i=0; i<10; i++){
		//button[i] = gtk_button_new_with_label(str[i]);
		button[i] = gtk_button_new_with_label(status[i].r);
		gtk_widget_set_name(button[i],status[i].n);
		gtk_box_pack_start(GTK_BOX(hbox2),button[i], TRUE, TRUE, 10);
		g_signal_connect(button[i], "clicked",G_CALLBACK(btn_clicked),status[i].r);
	}
	/*		3段目		*/
	GtkWidget *hbox3= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_set_homogeneous(GTK_BOX(hbox3), TRUE);	 //	縦均一化
	gtk_widget_set_name(hbox3,"box2");
	
	for(i=10; i<20; i++){
		button[i] = gtk_button_new_with_label(status[i].r);
		gtk_box_pack_start(GTK_BOX(hbox3),button[i], TRUE, TRUE, 10);
		g_signal_connect(button[i], "clicked",G_CALLBACK(btn_clicked),status[i].r);
	}
	/*		4段目		*/
	GtkWidget *hbox4= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_set_homogeneous(GTK_BOX(hbox4), TRUE);	 //	縦均一化
	gtk_widget_set_name(hbox4,"box2");
	
	for(i=20; i<30; i++){
		button[i] = gtk_button_new_with_label(status[i].r);
		gtk_box_pack_start(GTK_BOX(hbox4),button[i], TRUE, TRUE, 10);
		g_signal_connect(button[i], "clicked",G_CALLBACK(btn_clicked),status[i].r);
	}
	
	
	/*		5段目		*/
	GtkWidget *hbox5= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_set_homogeneous(GTK_BOX(hbox5), TRUE);	 //	縦均一化
	gtk_widget_set_name(hbox5,"box2");
	
	for(i=30; i<34; i++){
		button[i] = gtk_button_new_with_label(status[i].r);
		gtk_widget_set_name(button[i],"btn");
		gtk_box_pack_start(GTK_BOX(hbox5),button[i], TRUE, TRUE, 10);
		g_signal_connect(button[i], "clicked",G_CALLBACK(btn_clicked),status[i].r);
	}
	
	/*		6段目		*/
	GtkWidget *hbox6= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_box_set_homogeneous(GTK_BOX(hbox6), TRUE);	 //	縦均一化
	gtk_widget_set_name(hbox6,"box2");
	
#if 1
	i= 34;
	button[i] = gtk_button_new_with_label(status[i].r);
	gtk_widget_set_name(button[i],"btn");
	gtk_box_pack_start(GTK_BOX(hbox6),button[i], TRUE, TRUE, 10);
	g_signal_connect(button[i], "clicked",G_CALLBACK(btn_clicked),"34");
	
	i= 37;
	label[i]= gtk_label_new(status[i].r);
	gtk_widget_set_name(label[i],"label7");
	box1= gtk_event_box_new();
	gtk_widget_set_name(box1,"btn");
	gtk_container_add(GTK_CONTAINER(box1),label[i]);
	gtk_box_pack_start(GTK_BOX(hbox6),box1, TRUE, TRUE, 10);
	
#else
	for(i=34; i<38; i++){
		button[i] = gtk_button_new_with_label(status[i].r);
		gtk_widget_set_name(button[i],"btn");
		gtk_box_pack_start(GTK_BOX(hbox6),button[i], TRUE, TRUE, 10);
	}
#endif
	/*		7段目		*/
	GtkWidget *hbox7= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_name(hbox7,"box2");
		
	gtk_box_pack_start(GTK_BOX(vbox),hbox1, FALSE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox2, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox3, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox4, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox5, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox6, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(vbox),hbox7, TRUE, TRUE, 0);
	return vbox;
}
/*******************************************************************************
* File Name    	:
* Version      	:
* Device(s)    	:	
* Description  	:
* Creation Date	:
*******************************************************************************/
#if 1
GtkWidget *Gui07(void){
	
	GtkWidget *hbox[16];
	GtkWidget *label;
	GtkWidget *vbox= gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_widget_set_size_request(vbox,800,480);
	
	struct room{
		gchar n[2];
		gchar r[5];
	};
	struct room	useroom[18]= {
		
		{"0","0101"},{"1","0102"},{"0","0103"},{"0","0104"},{"0","0105"},{"0","0106"},
		{"0","0107"},{"1","0108"},{"0","0201"},{"0","0202"},{"0","0203"},{"0","0204"},
		{"0","0205"},{"1","0206"},{"0","0207"},{"0","0208"},{"0","0301"},{"0","0302"},
		};
	
		gint	i;
	
	GtkWidget *hbox1= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	
	label= gtk_label_new("◆利用部屋一覧◆");
	gtk_widget_set_name(label,"label3");			//CCSで使う
	GtkWidget *hbox1h= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_name(hbox1h,"box3");
	gtk_box_pack_start(GTK_BOX(hbox1h),label, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox1),hbox1h, TRUE, TRUE, 0);
	
	label= gtk_label_new("◆利用部屋◆");
	gtk_widget_set_name(label,"label2");
	GtkWidget *hbox2h= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_name(hbox2h,"box2");	
	gtk_box_pack_start(GTK_BOX(hbox2h),label, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox1),hbox2h, TRUE, TRUE, 0);
	
	gtk_box_pack_start(GTK_BOX(vbox),hbox1, FALSE, TRUE, 0);
	
	
	GtkWidget *hbox2= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	
	for(i= 0; i<8; i++){
		hbox[i]= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_name(hbox[i],useroom[i].n);
		label= gtk_label_new(useroom[i].r);	
		gtk_container_add(GTK_CONTAINER(hbox[i]),label);
		gtk_box_pack_start(GTK_BOX(hbox2),hbox[i], FALSE, TRUE, 20);
	}
	gtk_box_pack_start(GTK_BOX(vbox),hbox2, FALSE, TRUE, 0);
	

	GtkWidget *hbox3= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	for(i= 8; i<16; i++){
		hbox[i]= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_name(hbox[i],useroom[i].n);
		label= gtk_label_new(useroom[i].r);	
		gtk_container_add(GTK_CONTAINER(hbox[i]),label);
		gtk_box_pack_start(GTK_BOX(hbox3),hbox[i], FALSE, TRUE, 20);
	}
	gtk_box_pack_start(GTK_BOX(vbox),hbox3, FALSE, TRUE, 0);

	return vbox;
}	
#else

GtkWidget *Gui07(void){
	
	GtkWidget *hbox[16];
	GtkWidget *label;
	GtkWidget *vbox= gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	gtk_widget_set_size_request(vbox,800,480);
	
	gchar *name[16]={"0","1","0","0","0","0","0","0","0","0","0","1","0","0","0","0"};
	gchar *room[16]={	"0101","0102","0103","0104","0105","0106","0107","0108",
						"0201","0202","0203","0204","0205","0206","0207","0208"
						};
	
	gint	i;
	
	GtkWidget *hbox1= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	
	label= gtk_label_new("◆利用部屋一覧◆");
	gtk_widget_set_name(label,"label3");			//CCSで使う
	GtkWidget *hbox1h= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_name(hbox1h,"box3");
	
	gtk_box_pack_start(GTK_BOX(hbox1h),label, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox1),hbox1h, TRUE, TRUE, 0);
	
	label= gtk_label_new("◆利用部屋◆");
	gtk_widget_set_name(label,"label2");
	GtkWidget *hbox2h= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	gtk_widget_set_name(hbox2h,"box2");	
	gtk_box_pack_start(GTK_BOX(hbox2h),label, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(hbox1),hbox2h, TRUE, TRUE, 0);
	
	gtk_box_pack_start(GTK_BOX(vbox),hbox1, FALSE, TRUE, 0);
	
	
	GtkWidget *hbox2= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	for(i= 0; i<8; i++){
		hbox[i]= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_name(hbox[i],name[i]);
		label= gtk_label_new(room[i]);	
		gtk_container_add(GTK_CONTAINER(hbox[i]),label);
		gtk_box_pack_start(GTK_BOX(hbox2),hbox[i], FALSE, TRUE, 20);
	}
	gtk_box_pack_start(GTK_BOX(vbox),hbox2, FALSE, TRUE, 0);
	

	GtkWidget *hbox3= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	for(i= 8; i<16; i++){
		hbox[i]= gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
		gtk_widget_set_name(hbox[i],name[i]);
		label= gtk_label_new(room[i]);	
		gtk_container_add(GTK_CONTAINER(hbox[i]),label);
		gtk_box_pack_start(GTK_BOX(hbox3),hbox[i], FALSE, TRUE, 20);
	}
	gtk_box_pack_start(GTK_BOX(vbox),hbox3, FALSE, TRUE, 0);

	return vbox;
}
#endif

/*******************************************************************************
* File Name    	:
* Version      	:
* Device(s)    	:
* Description  	:
* Creation Date	:
*******************************************************************************/
#if 0
char	outSet(char al, char bit){
	
	return al |= bit;
	
	}
char	outClr(char al, char bit){
	
	return al &= ~bit;
	}
	
char	btnColor(int i, char val){
	
	status[i].n[0]= (val)? '4': '1';	
	gtk_widget_set_name(button[i],status[i].n);
	return 'w';
	}
#endif
/*******************************************************************************
* File Name    	:
* Version      	:
* Device(s)    	:
* Description  	:
* Creation Date	:
*******************************************************************************/	
char	btnSelect(int i, char *sw, char c){
	
	sw[i]= (sw[i]+1) % 4;							// (++sw[i]) % 4; NOP,ON,BLNK,ONESHOTのリングカウンター
	
	if(sw[i]==0)	status[i].n[0]= '8';			//switchTABの色決めCSSに書いてある 7 
	else if(sw[i]==1)	status[i].n[0]= '2';
	else if(sw[i]==2)	status[i].n[0]= '4';
	else if(sw[i]==3)	status[i].n[0]= '5';
	gtk_widget_set_name(button[i],status[i].n);
	
	
	if((i& 0x01) ==0){
		
		c =hexascConvert('H', c);
		c &= 0x0C;
		if(sw[i]==1)	c |=0x01;
		else if(sw[i]==2)	c |=0x02;
		else if(sw[i]==3)	c |=0x03;
	}
	else if((i& 0x01) ==1){
		
		c =hexascConvert('H', c);
		c &= 0x03;
		if(sw[i]==1)	c |=0x04;
		else if(sw[i]==2)	c |=0x08;
		else if(sw[i]==3)	c |=0x0C;
	}
	//printf("hex:%x\n",c);
	c =hexascConvert('A', c);
	//printf("sta:%x\n",c);
	return	c;
}
/*******************************************************************************
* File Name    	:
* Version      	:
* Device(s)    	:
* Description  	:
* Creation Date	:
*******************************************************************************/
void	testToggle(void){
	
	static	char	t= '0';
	
	t= (t=='1')? '0':'1';		//ここでトグル
	
	//gcombuf[0]=STX;
	//gcombuf[1]=ADR;
	//gcombuf[2]=ADR;
	//gcombuf[3]=;
	//gcombuf[4]=;
	//gcombuf[5]=;
	//gcombuf[6]=;
	//gcombuf[7]=;
	//gcombuf[8]=;
	//gcombuf[9]=;
	gcombuf[10]= t;		//15
	//gcombuf[11]= t;		//13-14		  
	gcombuf[12]= ETX;
	gcombuf[13]= makeLrc(gcombuf,ETX);
	sendflg= 't';
	
}
/*******************************************************************************
* File Name    	:
* Version      	:
* Device(s)    	:
* Description  	:
* Creation Date	:
*******************************************************************************/
void	btn_clicked(GtkWidget *widget, gpointer data){
	
	/*
	 * 第1引数はアドレス
	 */
	//g_print("addr=%s",data);
	//stxComm();
	
	//g_print("adata=%d",ax);
	
	char	al;
	static	char	sw[15]= {0};
	int		ax;
	
	
	ax= atoi(data);			//文字から出力番号を作つ　dataはgpointer引数
	//g_print("adata=%d",ax);
	switch(ax){
		
		case 1:
			al= gcombuf[5];		//1-2			
			gcombuf[5]= btnSelect(0,sw,al);
			sendflg= 'w';
			//gcombuf[5] = (sw[0]= !sw[0])? outSet(al,0x01): outClr(al, 0x01);
			//sendflg= btnColor(0,sw[0]);
			break;
		
		case 2:
			al= gcombuf[5];		//1-2
			gcombuf[5] = btnSelect(1,sw,al);
			sendflg= 'w';
			break;
		
		case 3:
			al= gcombuf[4];
			gcombuf[4] = btnSelect(2,sw,al);
			sendflg= 'w';
			break;
		
		case 4:
			al= gcombuf[4];
			gcombuf[4] = btnSelect(3,sw,al);
			sendflg= 'w';
			break;
		
		case 5:
			al= gcombuf[7];
			gcombuf[7] = btnSelect(4,sw,al);
			sendflg= 'w';
			break;
		
		case 6:
			al= gcombuf[7];
			gcombuf[7] = btnSelect(5,sw,al);
			sendflg= 'w';
			break;
		
		case 7:
			al= gcombuf[6];
			gcombuf[6] = btnSelect(6,sw,al);
			sendflg= 'w';
			break;
		
		case 8:
			al= gcombuf[6];
			gcombuf[6] = btnSelect(7,sw,al);
			sendflg= 'w';
			break;
		
		
		case 9:
			al= gcombuf[9];
			gcombuf[9] = btnSelect(8,sw,al);
			sendflg= 'w';
			break;
		
		case 10:
			al= gcombuf[9];
			gcombuf[9] = btnSelect(9,sw,al);
			sendflg= 'w';
		break;
		
		case 11:
			al= gcombuf[8];
			gcombuf[8] = btnSelect(10,sw,al);
			sendflg= 'w';
			break;
		
		case 12:
			al= gcombuf[8];
			gcombuf[8] = btnSelect(11,sw,al);
			sendflg= 'w';
			break;
		
		case 13:
			al= gcombuf[11];
			gcombuf[11] = btnSelect(12,sw,al);
			sendflg= 'w';
			break;
		
		case 14:
			al= gcombuf[11];
			gcombuf[11] = btnSelect(13,sw,al);
			sendflg= 'w';
			break;
		
		case 15:
			al= gcombuf[10];
			gcombuf[10] = btnSelect(14,sw,al);
			sendflg= 'w';
			break;
		
		//case 34:
			//puts("address");
			//break;
			
		default:
		break;
	}
	
	
	 	
	if(widget== button[30]){
		
		enqCommand(0);
		puts("ENQ");		//モニター用としてターミナルに出力
	}
	else if(widget== button[31]){
		
		vCommand(0);
		puts("V電文");		//モニター用としてターミナルに出力

	}
	else if(widget== button[32]){
		
		yCommand(0);
		puts("Y電文");		//モニター用としてターミナルに出力
		
		status[adr0].y = 1;
	}
	else if(widget== button[33]){
		
		//startFa();
		//puts("FA電文");			//モニター用としてターミナルに出1力
		
		if(sendflg=='w'){
			
			gcombuf[12]= ETX;
			gcombuf[13]= makeLrc(gcombuf,ETX);
			sendflg= 't';
		}
		//puts("G電文");			//モニター用としてターミナルに出1力
	}	
	else if(widget== button[34]){
		
		address++;
		
		sprintf(status[34].r,"%d",address);

		puts(status[34].r);
		gtk_button_set_label(GTK_BUTTON(button[34]),status[34].r);
		setGcombuf(gcombuf);
	}
	
	
#if 0	 
	if(widget== button[0]){
		g_print("取消");
	}
	if(widget== button[1]){
		g_print("会計");
	}
#endif 
}

/*******************************************************************************
* File Name    	:
* Version      	:
* Device(s)    	:
* Description  	:
* Creation Date	:
*******************************************************************************/
void	serialSeting(void){
	
	struct  termios ttyparam;
	
	fd= serialOpen("/dev/ttyAMA0",19200);		// RAW mode  

	if(fd< 0){							// Err= -1
		printf("can not open serialport");
	}

	tcgetattr(fd, &ttyparam);
	
	/*
	ttyparam.c_cflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
	ttyparam.c_cflag &= ~OPOST;
	ttyparam.c_cflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
	*/
	
    ttyparam.c_cflag |= PARENB;       // Enable Parity -even
        								// PARENB 出力にパリティを付加し、入力のパリティチェックを行う。
										// PARODD 入力および出力を奇数パリティとする。

	ttyparam.c_iflag = 0;
	ttyparam.c_oflag = 0;
	
	/* non canonical mode setting */
	ttyparam.c_lflag = 0;
	ttyparam.c_cc[VTIME] = 0;
	ttyparam.c_cc[VMIN]  = 0;
	
	tcflush(fd,TCIFLUSH);
    tcsetattr(fd, TCSANOW,&ttyparam);
        

	if(wiringPiSetupGpio() < 0){
		printf("ERROR:WiringPi initialize");
	}
	pinMode(GPIO18,OUTPUT);
	digitalWrite(GPIO18, 0);
	
	startFa();
}
/*******************************************************************************
* File Name    	:
* Version      	:
* Device(s)    	:
* Description  	:
* Creation Date	:
*******************************************************************************/
void	startFa(void){
	
	digitalWrite(GPIO18, 1);
	
	serialWrite(fd,trsbuf,20);
	//write(fd,trsbuf,20);		//tsrbuf[2][4]が出ない
	//serialPuts(fd,trsbuf);	//tsrbuf[2][4]が出ない
	txFinish(15);
	delay(5);
	digitalWrite(GPIO18, 0);
}
/***************************************************************************
* Function Name:
* Description  :
* Arguments    : Code
* Return Value : None
****************************************************************************/
#if 0 

char	serialWrite(int fd,char *buf,int len){
	
		int	i;
		
		for(i=0; i<len; i++){
			serialPutchar(fd,buf[i]);
			delay(1);
		}
		return 0;
}

#else

char	serialWrite(int fd,char *buf,int len){
	
		int	i;
		char	*p= buf;
		
		for(i= 0; i< len; i++,p++){
			serialPutchar(fd,*p);
			delay(1);
		}
		return 0;
}

#endif
/***************************************************************************
* Function Name:
* Description  :
* Arguments    : Code
* Return Value : None
****************************************************************************/
char	serRead(char *rbuf){

			char	rcv=0;		
	static	int		i= 0;
	static	char	rxflg=0;
	

	if(serialDataAvail(fd)){
		
		rcv= (char)serialGetchar(fd);		//serialGetcharがintだから
		rbuf[i]= rcv;
		
		//printf("%d=%x\n",i,rbuf[i]);
		printf("rcv=%x\n",rbuf[i]);
		fflush(stdout);
		
		if(0x04==rcv|| 0x06==rcv)	return rcv;
		if('!'==rxflg){
			rxflg= '\0';	
			return	'!';
		}
		if(0x03==rcv&& 0x02==rxflg)	rxflg='!';	
		if(0x02==rcv){
			i= 0;
			rbuf[i]= rxflg= rcv;
		}
		if(++i > 32) i= 0;
	}
	return	0;
}//for serRead
/***************************************************************************
* Function Name:
* Description  :
* Arguments    : Code
* Return Value : None
****************************************************************************/
void	yCommand(char addr){
	
	if(status[34].r[1]=='\0'){
		trsbuf[1]='0';
		trsbuf[2]=status[34].r[0];
	}else{
		trsbuf[1]=status[34].r[0];
		trsbuf[2]=status[34].r[1];
	}
		
	digitalWrite(GPIO18, 1);
	delay(5);
	serialFlush(fd);
	trsbuf[0]=STX;
	//trsbuf[1]='0';	//status[34].r[0];
	//trsbuf[2]='0';	//status[34].r[1];	
	trsbuf[3]='Y';
	trsbuf[4]=0x03;
	trsbuf[5]=(trsbuf[1]^trsbuf[2]^trsbuf[3]^trsbuf[4]);
	//trsbuf[5]=0x5A;
	trsbuf[6]=0x00;				//終端用'\0'
	//serialPuts(fd,trsbuf);
	serialWrite(fd,trsbuf,6);
	txFinish(6);
	//delay(1);
	digitalWrite(GPIO18, 0);
}
/***************************************************************************
* Function Name:
* Description  :
* Arguments    : Code
* Return Value : None
****************************************************************************/
void	vCommand(char addr){

		if(status[34].r[1]=='\0'){
		trsbuf[1]='0';
		trsbuf[2]=status[34].r[0];
	}else{
		trsbuf[1]=status[34].r[0];
		trsbuf[2]=status[34].r[1];
	}
	
	digitalWrite(GPIO18, 1);
	delay(5);
	serialFlush(fd);
	trsbuf[0]=STX;
	//trsbuf[1]='0';	//status[34].r[0];
	//trsbuf[2]='0';	//status[34].r[1];	
	trsbuf[3]='V';				//0x56
	trsbuf[4]=0x03;
	trsbuf[5]=(trsbuf[1]^trsbuf[2]^trsbuf[3]^trsbuf[4]);
	//trsbuf[5]=0x55;
	trsbuf[6]=0x00;				//終端用'\0'
	//serialPuts(fd,trsbuf);
	serialWrite(fd,trsbuf,6);
	txFinish(6);
	//delay(1);
	digitalWrite(GPIO18, 0);
}
/***************************************************************************
* Function Name:
* Description  :
* Arguments    : Code
* Return Value : None
****************************************************************************/
void	enqCommand(char addr){
	
	if(status[34].r[1]=='\0'){
		trsbuf[1]='0';
		trsbuf[2]=status[34].r[0];
	}else{
		trsbuf[1]=status[34].r[0];
		trsbuf[2]=status[34].r[1];
	}

	
	digitalWrite(GPIO18, 1);
	delay(5);
	serialFlush(fd);
	trsbuf[0]=ENQ;				//ENQ
	//trsbuf[1]='0';	//status[34].r[0];
	//trsbuf[2]='0';	//status[34].r[1];	
	trsbuf[3]='\0';				//終端用'\0'
	//write(fd,trsbuf,5);
	//serialPuts(fd,trsbuf);
	serialWrite(fd,trsbuf,3);
	txFinish(3);
	//delay(1);
	digitalWrite(GPIO18, 0);	
}
/***************************************************************************
* Function Name:
* Description  :
* Arguments    : Code
* Return Value : None
****************************************************************************/
void	setGcombuf(char *buf){
	
	if(status[34].r[1]=='\0'){
		buf[1]='0';
		buf[2]=status[34].r[0];
	}else{
		buf[1]=status[34].r[0];
		buf[2]=status[34].r[1];
	}
	
	buf[0]=STX;
	//buf[1]='0';	//status[34].r[0];
	//buf[2]='0';	//status[34].r[1];	
	buf[3]='G';
	buf[4]='0';		//3-4	
	buf[5]='0';		//1-2
	buf[6]='0';		//7-8
	buf[7]='0';		//5-6
	buf[8]='0';		//11-12
	buf[9]='0';		//9-10
	buf[10]='0';	//15-16
	buf[11]='0';	//13-14
	buf[12]=ETX;
	buf[13]=	makeLrc(gcombuf,ETX);

}//setGcombuf()
/***************************************************************************
* Function Name:
* Description  :
* Arguments    : Code
* Return Value : None
****************************************************************************/
char	gCommand(void){

	digitalWrite(GPIO18, 1);
	delay(5);
	serialFlush(fd);
	
	//serialPuts(fd,trsbuf);
	serialWrite(fd,gcombuf,14);
	txFinish(14);
	//delay(1);
	digitalWrite(GPIO18, 0);
	
	return '\0';
}//for gCommand
/***************************************************************************
* Function Name:
* Description  : 小文字は子機電文
* Arguments    : Code
* Return Value : None
****************************************************************************/
void	stxComm(char *buf){

	digitalWrite(GPIO18, 1);
	delay(5);
	serialFlush(fd);
	*buf++ = STX;
	*buf++ ='F';
	*buf++ ='F';	
	*buf++='x';
	*buf++='0';
	*buf++='0';
	*buf++=0x03;
	*buf++=0x7b;
	*buf++=0x00;
	//serialPuts(fd,trsbuf);
	serialWrite(fd,trsbuf,8);
	txFinish(8);
	//delay(1);
	digitalWrite(GPIO18, 0);
}//for stxComm
/***************************************************************************
* Function Name:
* Description  :
* Arguments    : num送信文字数
* Return Value : None
****************************************************************************/
void	txFinish(unsigned int num){

	unsigned int	j;
	int				i;

	for(j= 0;j< 50;j++){		// 50msec

		delay(1);
		i=serialDataAvail(fd);
		//printf("rcv:%d",i);
		if(i >= num) break;
		

	}
	serialFlush(fd);

}//for txFinish 

/***************************************************************************
* Function Name:
* Description  :
* Arguments    : 
* Return Value : None
****************************************************************************/
char	makeLrc(char *buf,char lim){
	
	char	*p= buf;
	char	tmp;
	
	tmp= *(++p);
	
	do{
		p++;
		tmp^=*p;
	}while(*p!=lim);
	
	return tmp;
	
	
#if 0	
	char	tmp;
	char	i= 1;
	
	tmp= buf[i];
	
	do{
		i++;
		tmp^= buf[i];
	}while(buf[i] != lim);
	
	return	tmp;
#endif

}

/***************************************************************************
* Function Name:
* Description  :
* Arguments    : type= 'H' return(hex) type='A' return (ascii)
* Return Value : None
****************************************************************************/
char	hexascConvert(char type, uint8_t num){
	
	int		i;
	char	ans= 0x10;
		
	if(type=='A'){	
		
		for(i= 0; i< 16; i++){
			if(aschex[i].hex== num){
				ans= aschex[i].ascii;
				break;
			}
		}
	}
	else if(type=='H'){
		
		for(i= 0; i< 16; i++){
			if(aschex[i].ascii== num){
				ans= aschex[i].hex;
				break;
			}
		}
	}
	return ans;
}

/***************************************************************************
* Function Name:
* Description  :
* Arguments    : 
* Return Value : None
****************************************************************************/
char	inDisp(struct input *ptr, char *rbuf){
	
	char	al;
	int		i=16;
	
	//printf("buf:%x",rbuf[3]);
	if('g'!= rbuf[3])	return '\0';
	
		//printf("buf[9]=%x",rbuf[9]);
		//fflush(stdout);
		
		//al= rbuf[9];
		al =hexascConvert('H',rbuf[9]);
		ptr[i++].n[0]= (al& 0x01)? '2':'0';
		ptr[i++].n[0]= ((al>>1)& 0x01)? '2':'0';
		ptr[i++].n[0]= ((al>>2)& 0x01)? '2':'0';
		ptr[i++].n[0]= ((al>>3)& 0x01)? '2':'0';
		
		//al= rbuf[8];
		al =hexascConvert('H',rbuf[8]);
		ptr[i++].n[0]= (al& 0x01)? '2':'0';
		ptr[i++].n[0]= ((al>>1)& 0x01)? '2':'0';
		ptr[i++].n[0]= ((al>>2)& 0x01)? '2':'0';
		ptr[i++].n[0]= ((al>>3)& 0x01)? '2':'0';
		
		//al= rbuf[11];
		al =hexascConvert('H',rbuf[11]);
		ptr[i++].n[0]= (al& 0x01)? '2':'0';
		ptr[i++].n[0]= ((al>>1)& 0x01)? '2':'0';
		ptr[i++].n[0]= ((al>>2)& 0x01)? '2':'0';
		ptr[i++].n[0]= ((al>>3)& 0x01)? '2':'0';
		
		//al= rbuf[10];
		al =hexascConvert('H',rbuf[10]);
		ptr[i++].n[0]= (al& 0x01)? '2':'0';
		ptr[i++].n[0]= ((al>>1)& 0x01)? '2':'0';
		
		//printf("i8:%x",ptr[24].n[0]);
		//printf("i9:%x",ptr[25].n[0]);
		//printf("i10:%x",ptr[26].n[0]);
		
		return '\0';
}

/***************************************************************************
* Function Name:
* Description  :
* Arguments    : 
* Return Value : None
****************************************************************************/
char	inBit(char *rbuf){


	int		i= 16;
	
	if('g'!= rbuf[3])	return '\0';
	
	if(rbuf[5]& 0x01)	gtk_widget_set_name(button[i],status[i].n);			/* 1*/
	if(rbuf[5]& 0x02)	gtk_widget_set_name(button[i+1],status[i+1].n);		/* 2*/
	if(rbuf[5]& 0x04)	gtk_widget_set_name(button[i+2],status[i+2].n);		/* 3*/
	if(rbuf[5]& 0x08)	gtk_widget_set_name(button[i+3],status[i+3].n);		/* 4*/
	
	if(rbuf[4]& 0x01)	gtk_widget_set_name(button[i+4],status[i+4].n);		/* 5*/
	if(rbuf[4]& 0x02)	gtk_widget_set_name(button[i+5],status[i+5].n);		/* 6*/
	if(rbuf[4]& 0x04)	gtk_widget_set_name(button[i+6],status[i+6].n);		/* 7*/
	if(rbuf[4]& 0x08)	gtk_widget_set_name(button[i+7],status[i+7].n);		/* 8*/
	
	if(rbuf[7]& 0x01)	gtk_widget_set_name(button[i+8],status[i+8].n);		/* 9*/
	if(rbuf[7]& 0x02)	gtk_widget_set_name(button[i+9],status[i+9].n);		/*10*/
	if(rbuf[7]& 0x04)	gtk_widget_set_name(button[i+10],status[i+10].n);	/*11*/
	if(rbuf[7]& 0x08)	gtk_widget_set_name(button[i+11],status[i+11].n);	/*12*/
	
	if(rbuf[6]& 0x01)	gtk_widget_set_name(button[i+12],status[i+12].n);	/*13*/
	if(rbuf[6]& 0x02)	gtk_widget_set_name(button[i+13],status[i+13].n);	/*14*/
	
	return '\0';		
}
/***************************************************************************
* Function Name:
* Description  :
* Arguments    : 
* Return Value : None
****************************************************************************/
char	remDisp(struct input *ptr, char *rbuf){
	
	if('g'!= rbuf[3])	return '\0';
	
	puts("R");
	
	ptr[rem].r[0]= rbuf[14];
	ptr[rem].r[1]= rbuf[15];
	ptr[rem].r[2]= '\0';
	
	gtk_label_set_text(GTK_LABEL(label[37]),status[37].r);
	
	return '\0';
}

