// gcc -export-dynamic -Wall `pkg-config --cflags --libs gtk+-3.0` user.c -o user `pkg-config --cflags --libs gtk+-3.0`

//    ./user

// username: user
// password: 1234

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gdk/gdk.h>
#include <glib-object.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>


GtkBuilder *builder;
GtkWidget *StartWindow;
GtkWidget *MainWindow;
GtkWidget *window_order;
GtkWidget *window_payment;


#define BUFSIZE 1024

char cola[]="Coca-cola";
static char *products[]={'\0'};
static int counter=0;

void f(const char* s, char* res) {
    char* l = strchr(s, ' ');
    char* r = strchr(l + 1, ' ');
    memcpy(res, l + 1, r - l);
    res[r - l - 1] = '\0';
}


void send_recv(int i, int sockfd)
{
  char send_buf[BUFSIZE];
  char recv_buf[BUFSIZE];
  int nbyte_recvd;
  
  if (i == 0){

    fgets(send_buf, BUFSIZE, stdin);
    //send_buf=bus_num;
    if (strcmp(send_buf , "quit\n") == 0) {
      exit(0);
    }else
      send(sockfd, send_buf, strlen(send_buf), 0);
  }else {
    nbyte_recvd = recv(sockfd, recv_buf, BUFSIZE, 0);
    recv_buf[nbyte_recvd] = '\0';
    printf("%s\n" , recv_buf);
    fflush(stdout);
  }
}
    
    
void connect_request(int *sockfd, struct sockaddr_in *server_addr)
{
  if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("Socket");
    exit(1);
  }
  server_addr->sin_family = AF_INET;
  server_addr->sin_port = htons(8888);
  server_addr->sin_addr.s_addr = inet_addr("192.168.43.79");
  memset(server_addr->sin_zero, '0', sizeof server_addr->sin_zero);
  
  if(connect(*sockfd, (struct sockaddr *)server_addr, sizeof(struct sockaddr)) == -1) {
    perror("connect");
    exit(1);
  }
}

void send_product(char str[]){

  char sockfd, fdmax, i;
  struct sockaddr_in server_addr;
  fd_set master;
  fd_set read_fds;
 
  connect_request(&sockfd, &server_addr);
  FD_ZERO(&master);
        FD_ZERO(&read_fds);
        FD_SET(0, &master);
        FD_SET(sockfd, &master);
  fdmax = sockfd;
  //send(sockfd, argv[1], strlen(argv[1]), 0);

  for(i=0;i<counter;i++){
  send(sockfd, products[i], strlen(products[i]), 0);
  printf("%s\n", products[i]);
   }


  printf("client-quited\n");
  close(sockfd);

}
int main (int argc, char *argv[])
{

    gtk_init(&argc, &argv);
   
    builder = gtk_builder_new_from_file ("user.glade");
    gtk_builder_connect_signals(builder, NULL);


    StartWindow  = GTK_WIDGET(gtk_builder_get_object(builder, "login"));
    window_order = GTK_WIDGET(gtk_builder_get_object(builder, "window_order"));
    MainWindow = GTK_WIDGET(gtk_builder_get_object(builder, "MainWindow"));
    window_payment  = GTK_WIDGET(gtk_builder_get_object(builder, "window_payment"));


    gtk_widget_hide (GTK_WIDGET(StartWindow));
    gtk_widget_hide (GTK_WIDGET(MainWindow));
    gtk_widget_hide (GTK_WIDGET(window_payment));
    gtk_widget_hide (GTK_WIDGET(window_order));


    gtk_widget_show(window_order);
   
    gtk_main();

    return 0;
}


                                                            //Starting window
    const gchar *USERNAME;
    const gchar *PASSWORD;
    const gchar *TITLE;
    const gchar *TYPE;
    const gchar *VOLUME;
    const gchar *STATUS;
    
gboolean enteredUsername(GtkEntry *e1, gpointer user)
    {
        USERNAME = gtk_entry_get_text ((e1));
        return FALSE;
    }
    gboolean enteredPassword(GtkEntry *e2, gpointer user)
    {
        PASSWORD = gtk_entry_get_text ((e2));
        return FALSE;
    }

                                    //Starting window
void submit_clicked(GtkButton *b1, gpointer  entry1)

    {
        if (strcmp(USERNAME,"user") == 0 && strcmp(PASSWORD,"1") == 0)
        {
      
        gtk_widget_hide(StartWindow);
        gtk_widget_show (window_order);                
        
         }  
        
    }
 void on_move_payment_clicked()
 {
    gtk_widget_show(window_payment);
    gtk_widget_hide(MainWindow);
 }

 void on_quit_clicked()
{
    gtk_widget_destroy(MainWindow);
}
                                                            //menu drink
 
/*void on_pepsi_toggled(GtkToggleButton *togglebutton,  gpointer *user_data)
{
    gboolean button_state;
    
    button_state = gtk_toggle_button_get_active(togglebutton);
    GtkTreeIter iter;
    GtkTreeView *treeview_payment = GTK_TREE_VIEW(user_data);
    GtkListStore *liststore1 = GTK_LIST_STORE(gtk_tree_view_get_model(treeview_payment));

    if (button_state) {
    gtk_list_store_append(liststore1, &iter);
    gtk_list_store_set(liststore1, &iter, 0, "Pepsi Cola", 1, 1.0,2, 10.0 ,3, 10.0, -1);
    products[counter++]="Pepsi Cola";
     
    }
    else {
    gtk_list_store_remove (GTK_LIST_STORE(liststore1), &iter);
    }
}*/

  
                                                //Menu Payment
gboolean enteredTitle(GtkEntry *p1, gpointer  product_title)
    {
        TITLE = gtk_entry_get_text ((p1));
        return FALSE;
    }
gboolean enteredType(GtkEntry *p2, gpointer  product_type)
    {
        TYPE = gtk_entry_get_text ((p2));
        return FALSE;
    }
gboolean enteredVolume(GtkEntry *p3, gpointer  product_vol)
{
    VOLUME = gtk_entry_get_text ((p3));
    return FALSE;
}
// gboolean enteredVolType(GtkEntry *p4, gpointer  user_data4)
// {
//     VOLUME = strncat(VOLUME, gtk_entry_get_text ((p4)), 5);
//     return FALSE;
// }
    
void product_submit_clicked(GtkButton *b1, gpointer *user_data)
{
    GtkTreeIter iter;
    GtkTreeView *treeview_payment = GTK_TREE_VIEW((user_data));
    GtkListStore *liststore1 = GTK_LIST_STORE(gtk_tree_view_get_model(treeview_payment));
       
   // gtk_list_store_remove (GTK_LIST_STORE(liststore1), &iter);
    //printf("%s\n", VOLUME);
    gtk_list_store_append(liststore1, &iter);
    //gtk_list_store_set(liststore1, &iter, 0, "Pepsi Cola", 1, "sdsdsd", 2, "10.0" ,3, "10.0", -1);
   gtk_list_store_set(liststore1, &iter, 0,  TITLE, 1, TYPE, 2, VOLUME, 3, "on hold", -1);
  
    gtk_widget_hide(window_order);
    gtk_widget_show(window_payment);
}


void on_payment_cancel_clicked()
{
    gtk_widget_show(MainWindow);
    gtk_widget_hide(window_payment);
}

void on_payment_back_clicked()
{
    gtk_widget_show(MainWindow);
    gtk_widget_hide(window_payment);
}

