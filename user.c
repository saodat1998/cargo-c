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
#include <mysql/mysql.h>


GtkBuilder *builder;
GtkWidget *StartWindow;
GtkWidget *MainWindow;
GtkWidget *window_order;
GtkWidget *order_history;

#define BUFSIZE 1024


static char *host = "bxkkera27hebzuadghy0-mysql.services.clever-cloud.com";
static char *user = "upj9gcpm4xe3vaxx";
static char *password = "nByuvCegwIGu021UxDxK";
static char *dbname = "bxkkera27hebzuadghy0";
unsigned int port = 3306;
static char *unix_socket = NULL; // To specify connection type
unsigned int flag = 0; // To specify ODBS connection


static char *products[]={'\0'};
static int counter=0;
const gchar *USERNAME;
const gchar *PASSWORD;
const gchar *TITLE;
const gchar *TYPE;
const gchar *VOLUME;
const gchar *PICKUP;
const gchar *DESTINATION;
const gchar *PICKUPDATE;
const gchar *DESTINATIONDATE;
const gchar *STATUS;
const gchar *user_id_const;

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

MYSQL *conn;

int main (int argc, char *argv[])
{


    conn = mysql_init(NULL); //To prepare sturcture to connection

    if (!(mysql_real_connect(conn, host, user, password, dbname, port, unix_socket, flag)))
    {
      fprintf(stderr, "nError: %s [%d]\n", mysql_error(conn), mysql_errno(conn));
      exit(1);
    }
    printf("Connection Successfull\n\n");

    gtk_init(&argc, &argv);

    builder = gtk_builder_new_from_file ("user.glade");
    gtk_builder_connect_signals(builder, NULL);


    StartWindow  = GTK_WIDGET(gtk_builder_get_object(builder, "login"));
    window_order = GTK_WIDGET(gtk_builder_get_object(builder, "window_order"));
    MainWindow = GTK_WIDGET(gtk_builder_get_object(builder, "MainWindow"));
    order_history  = GTK_WIDGET(gtk_builder_get_object(builder, "order_history"));

    //user_id_const = NULL;
    gtk_widget_hide (GTK_WIDGET(StartWindow));
    gtk_widget_hide (GTK_WIDGET(MainWindow));
    gtk_widget_hide (GTK_WIDGET(window_order));
    gtk_widget_hide (GTK_WIDGET(order_history));

    gtk_widget_show(StartWindow);

    gtk_main();

    return 0;
}
  //Login window

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

//Login window
void submit_clicked(GtkButton *button, gpointer  entry1)
  {
    const char *query = "SELECT * FROM `user`";

    if (mysql_query(conn, query) != 0)
    {
    fprintf(stderr, "%s\n", mysql_error(conn));
    exit(-1);
    } else {

    MYSQL_RES *query_results = mysql_store_result(conn);
    if (query_results) { // make sure there *are* results..
          MYSQL_ROW row;

          while((row = mysql_fetch_row(query_results)) !=0)
          {

            if (strcmp(row[1], USERNAME) == 0 && strcmp(row[3], PASSWORD) == 0)
            {

              user_id_const = row[0];
              gtk_widget_show(MainWindow);
              gtk_widget_hide(StartWindow);
              break;
            }

          }
          mysql_free_result(query_results);
        }
      }


}

 void on_order_clicked()
 {
    gtk_widget_show(window_order);
    gtk_widget_hide(MainWindow);
 }

 void on_quit_clicked()
{
    gtk_widget_destroy(MainWindow);
}
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
gboolean pickUp(GtkEntry *p4, gpointer  pick_up)
    {
        PICKUP = gtk_entry_get_text ((p4));
        return FALSE;
    }
gboolean pickUpDate(GtkEntry *p5, gpointer  pick_up_date)
    {
        PICKUPDATE = gtk_entry_get_text ((p5));
        return FALSE;
    }
gboolean enteredDestination(GtkEntry *p6, gpointer  destination)
    {
        DESTINATION = gtk_entry_get_text ((p6));
        return FALSE;
    }
gboolean destinationDate(GtkEntry *p7, gpointer  destination_date)
    {
        DESTINATIONDATE = gtk_entry_get_text ((p7));
        return FALSE;
    }


void product_submit_clicked(GtkButton *b1, gpointer *user_data)
{
    int check = 0;
    printf("%s\n", user_id_const);
    const char *query = "INSERT INTO product (title, volume, type, pick_up, pick_up_time, destination, destination_time, status, client_id) VALUES (";
    char * new_str ;

    if((new_str = malloc(strlen(query)+500)) != NULL){
        new_str[0] = '\0';   // ensures the memory is an empty string
        strcat(new_str,query);
        strcat(new_str,"'");
        strcat(new_str,TITLE);
        strcat(new_str,"','");
        strcat(new_str, VOLUME);
        strcat(new_str,"','");
        strcat(new_str, TYPE);
        strcat(new_str,"','");
        strcat(new_str, PICKUP);
        strcat(new_str,"','");
        strcat(new_str, PICKUPDATE);
        strcat(new_str,"','");
        strcat(new_str, DESTINATION);
        strcat(new_str,"','");
        strcat(new_str, DESTINATIONDATE);
        strcat(new_str,"','0','10');");
    }
      printf("%s\n", new_str);
    if (mysql_query(conn, new_str) != 0)
    {
      fprintf(stderr, "%s\n", mysql_error(conn));
      exit(-1);
    } else {

    MYSQL_RES *query_results = mysql_store_result(conn);

    if (query_results) { // make sure there *are* results..
      MYSQL_ROW row;


      while((row = mysql_fetch_row(query_results)) !=0)
      {
        check = 1;
      }

       mysql_free_result(query_results);
    }
  }
    if(check){
      gtk_widget_hide(window_order);
      gtk_widget_show(order_history);
    }
}

void on_order_history_clicked(GtkButton *button,gpointer *admin_data)
{
  const char *query = "SELECT * FROM `product` where client_id='10'";

if (mysql_query(conn, query) != 0)
{
fprintf(stderr, "%s\n", mysql_error(conn));
exit(-1);
} else {

MYSQL_RES *query_results = mysql_store_result(conn);
if (query_results) { // make sure there *are* results..
      MYSQL_ROW row;

      while((row = mysql_fetch_row(query_results)) !=0)
      {

        GtkTreeIter iter;

        GtkTreeView *treeview_payment_admin = GTK_TREE_VIEW(admin_data);

        GtkListStore *liststore1 = GTK_LIST_STORE(gtk_tree_view_get_model(treeview_payment_admin));

        gtk_list_store_append(liststore1, &iter);
        gtk_list_store_set(liststore1, &iter, 0, row[1], 1,  row[2], 2, row[3], 3, row[4], 4, row[5], 5, row[6], 6, row[7], 7, row[8], 8, row[9], -1);


        gtk_widget_show(order_history);
        gtk_widget_hide(window_order);

      }

      mysql_free_result(query_results);
    }
  }

}

void on_payment_cancel_clicked()
{
    gtk_widget_show(MainWindow);
    gtk_widget_hide(order_history);
}

void on_table_back_clicked()
{
    gtk_widget_show(MainWindow);
    gtk_widget_hide(order_history);
}

void on_cancel_clicked() {
  gtk_widget_show(MainWindow);
  gtk_widget_hide(window_order);
}
