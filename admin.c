///gcc -export-dynamic -Wall `pkg-config --cflags --libs gtk+-3.0` admin.c -o admin -pthread `pkg-config --cflags --libs gtk+-3.0`

//  ./driver

//username: driver
//password: 1

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>


GtkBuilder *builder;
GtkWidget *window_admin;
GtkWidget *window_order;
GtkWidget *window_order_history;
GtkWidget *window_change_status;
GtkWidget *window_users_list;

#define PORT 8888
#define BUFSIZE 1024



static char *host = "bxkkera27hebzuadghy0-mysql.services.clever-cloud.com";
static char *user = "upj9gcpm4xe3vaxx";
static char *password = "nByuvCegwIGu021UxDxK";
static char *dbname = "bxkkera27hebzuadghy0";
unsigned int port = 3306;
static char *unix_socket = NULL; // To specify connection type
unsigned int flag = 0; // To specify ODBS connection

	void send_to_all(int j, int i, int sockfd, int nbytes_recvd, char *recv_buf, fd_set *master)
{
  if (FD_ISSET(j, master)){
    if (j != sockfd && j != i) {
      if (send(j, recv_buf, nbytes_recvd, 0) == -1) {
        perror("send");
      }
    }
  }
}

void send_recv(int i, fd_set *master, int sockfd, int fdmax)
{
  int nbytes_recvd, j;
  char recv_buf[BUFSIZE], buf[BUFSIZE];

  if ((nbytes_recvd = recv(i, recv_buf, BUFSIZE, 0)) <= 0) {
    if (nbytes_recvd == 0) {
      printf("socket %d hung up\n", i);
    }else {
      perror("recv");
    }
    close(i);
    FD_CLR(i, master);
  }else {
	recv_buf[nbytes_recvd] = '\0';

    printf("%s\n", recv_buf);

    for(j = 0; j <= fdmax; j++){
      send_to_all(j, i, sockfd, nbytes_recvd, recv_buf, master );
    }
	fflush(stdout);
  }
}

void connection_accept(fd_set *master, int *fdmax, int sockfd, struct sockaddr_in *client_addr)
{
  socklen_t addrlen;
  int newsockfd;

  addrlen = sizeof(struct sockaddr_in);
  if((newsockfd = accept(sockfd, (struct sockaddr *)client_addr, &addrlen)) == -1) {
    perror("accept");
    exit(1);
  }else {
    FD_SET(newsockfd, master);
    if(newsockfd > *fdmax){
      *fdmax = newsockfd;
    }
    printf("new connection from %s on port %d \n",inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port));
  }
}

void connect_request(int *sockfd, struct sockaddr_in *my_addr)
{
  int yes = 1;

  if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("Socket");
    exit(1);
  }

  my_addr->sin_family = AF_INET;
  my_addr->sin_port = htons(8888);
  my_addr->sin_addr.s_addr = INADDR_ANY;
  memset(my_addr->sin_zero, '0', sizeof my_addr->sin_zero);

  if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
    perror("setsockopt");
    exit(1);
  }

  if (bind(*sockfd, (struct sockaddr *)my_addr, sizeof(struct sockaddr)) == -1) {
    perror("Unable to bind");
    exit(1);
  }
  if (listen(*sockfd, 10) == -1) {
    perror("listen");
    exit(1);
  }
  printf("\nTCPServer Waiting for client on port 8888\n");
  fflush(stdout);
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




   pthread_t thread_glade;

	fd_set master;
  fd_set read_fds;
  int fdmax, i;
  int sockfd= 0;
  struct sockaddr_in my_addr, client_addr;

  FD_ZERO(&master);
  FD_ZERO(&read_fds);
  connect_request(&sockfd, &my_addr);
  FD_SET(sockfd, &master);

  fdmax = sockfd;




    gtk_init(&argc, &argv);

    builder = gtk_builder_new_from_file ("admin.glade");
    gtk_builder_connect_signals(builder, NULL);

    window_admin = GTK_WIDGET(gtk_builder_get_object(builder, "window_admin"));
    window_order = GTK_WIDGET(gtk_builder_get_object(builder, "window_order"));
    window_order_history = GTK_WIDGET(gtk_builder_get_object(builder, "window_order_history"));
		window_change_status = GTK_WIDGET(gtk_builder_get_object(builder, "window_change_status"));
		window_users_list = GTK_WIDGET(gtk_builder_get_object(builder, "window_users_list"));

    gtk_widget_hide (GTK_WIDGET(window_admin));
    gtk_widget_hide (GTK_WIDGET(window_order));
    gtk_widget_hide (GTK_WIDGET(window_order_history));
    gtk_widget_hide (GTK_WIDGET(window_change_status));
    gtk_widget_hide (GTK_WIDGET(window_users_list));


    gtk_widget_show(window_admin);

//    gtk_main();
pthread_create(&thread_glade, NULL, gtk_main, NULL);


  while(1){
    read_fds = master;
    if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
      perror("select");
      exit(4);
    }

    for (i = 0; i <= fdmax; i++){
      if (FD_ISSET(i, &read_fds)){
        if (i == sockfd)
          connection_accept(&master, &fdmax, sockfd, &client_addr);
        else
          send_recv(i, &master, sockfd, fdmax);
      }
    }
  }
pthread_join (thread_glade, NULL);
    return 0;

}


//menu admin

    const gchar *USERNAME;
    const gchar *PASSWORD;
gboolean enteredUsername(GtkEntry *e1, gpointer  user)
    {
        USERNAME = gtk_entry_get_text ((e1));
        return FALSE;
    }
    gboolean enteredPassword(GtkEntry *e2, gpointer  user)
    {
        PASSWORD = gtk_entry_get_text ((e2));
        return FALSE;
    }

		const gchar *PRODUCT_ID;
		const gchar *CHANGED_STATUS;
		gboolean entered_product_id(GtkEntry *e1, gpointer user) {
			PRODUCT_ID = gtk_entry_get_text((e1));
			return FALSE;
		}
		gboolean entered_status(GtkEntry *e2, gpointer user) {
			CHANGED_STATUS = gtk_entry_get_text((e2));
			return FALSE;
		}

void on_status_clicked() {
	const char *query = "UPDATE product SET status=";
	char * new_str ;

	if((new_str = malloc(strlen(query)+500)) != NULL){
			new_str[0] = '\0';   // ensures the memory is an empty string
			strcat(new_str,query);
			strcat(new_str, CHANGED_STATUS);
			strcat(new_str, " WHERE ");
			strcat(new_str,"product_id=");
			strcat(new_str, PRODUCT_ID);
			strcat(new_str,";");
	}
	query = new_str;

	if (mysql_query(conn, new_str) != 0)
	{
		fprintf(stderr, "%s\n", mysql_error(conn));
		exit(-1);
	} else {

		MYSQL_RES *query_results = mysql_store_result(conn);
		mysql_free_result(query_results);

		gtk_widget_show(window_order);
		gtk_widget_hide(window_change_status);
	}
}

void on_change_status_menu_clicked() {
	gtk_widget_show(window_change_status);
	gtk_widget_hide(window_order);
}

void on_bottom_admin_apply_clicked()
{
	const char *query = "SELECT username, password FROM `user` where user_type='admin'";

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

					/* Do whatever you need to with 'f' */
					// printf(row[0]);
					if (strcmp(row[0], USERNAME) == 0 && strcmp(row[1], PASSWORD) == 0)
					{
						gtk_widget_show(window_order);
						gtk_widget_hide(window_admin);
						break;
					}

				}

				/* Free results when done */
				mysql_free_result(query_results);
			}
		}
}

void on_logout_clicked()
{
       gtk_widget_destroy(window_order);
}

//menu order
void on_bottom_apply_order_clicked()
{

    gtk_widget_show(window_order_history);
    gtk_widget_hide(window_order);
}


GtkTreeIter iter;

GtkTreeView *treeview_payment_admin;

GtkListStore *liststore2;

void on_bottom_history_clicked(GtkButton *button,gpointer *admin_data)
{
 // signal_cola=s;
 const char *query = "SELECT * FROM `product`";

 if (mysql_query(conn, query) != 0)
 {
 fprintf(stderr, "%s\n", mysql_error(conn));
 exit(-1);
 } else {
// GtkListStore *liststore2 = GTK_LIST_STORE(gtk_tree_view_get_model(0));
 MYSQL_RES *query_results = mysql_store_result(conn);
 if (query_results) { // make sure there *are* results..
			 MYSQL_ROW row;

			 while((row = mysql_fetch_row(query_results)) !=0)
			 {

				 /* Do whatever you need to with 'f' */
				 // printf(row[0]);


				 // GtkTreeIter iter;
				 //
				 // GtkTreeView *treeview_payment_admin = GTK_TREE_VIEW(admin_data);
				 //
				 // GtkListStore *liststore2 = GTK_LIST_STORE(gtk_tree_view_get_model(treeview_payment_admin));

				 treeview_payment_admin = GTK_TREE_VIEW(admin_data);

				 liststore2 = GTK_LIST_STORE(gtk_tree_view_get_model(treeview_payment_admin));


				 gtk_list_store_append(liststore2, &iter);
				 gtk_list_store_set(liststore2, &iter, 0, row[1], 1,  row[2], 2, row[3], 3, row[4], 4, row[5], 5, row[6], 6, row[7], 7, row[8], 8, row[9], 9, row[0], -1);

				 // gtk_list_store_remove (GTK_LIST_STORE(liststore2), &iter);

			 }

			 /* Free results when done */
			 mysql_free_result(query_results);
		 }
	 }
    gtk_widget_show(window_order_history);
    gtk_widget_hide(window_order);

    //     gtk_widget_show(window_order);
    // gtk_widget_hide(window_order_history);
}

void on_bottom_order_history_back_clicked()
{
		gtk_list_store_clear(liststore2);
    gtk_widget_show(window_order);
    gtk_widget_hide(window_order_history);
}

void on_users_list_clicked(GtkButton *button,gpointer *admin_data) {
	const char *query = "SELECT username FROM `user`";

  if (mysql_query(conn, query) != 0)
  {
  fprintf(stderr, "%s\n", mysql_error(conn));
  exit(-1);
  } else {
 // GtkListStore *liststore2 = GTK_LIST_STORE(gtk_tree_view_get_model(0));
  MYSQL_RES *query_results = mysql_store_result(conn);
  if (query_results) { // make sure there *are* results..
 			 MYSQL_ROW row;

 			 while((row = mysql_fetch_row(query_results)) !=0)
 			 {

 				 /* Do whatever you need to with 'f' */
 				 // printf(row[0]);


 				 GtkTreeIter iter1;

 				 GtkTreeView *treeview_username = GTK_TREE_VIEW(admin_data);

 				 GtkListStore *liststore1 = GTK_LIST_STORE(gtk_tree_view_get_model(treeview_username));


 				 gtk_list_store_append(liststore1, &iter);
 				 gtk_list_store_set(liststore1, &iter, 0, row[0], -1);

 				 // gtk_list_store_remove (GTK_LIST_STORE(liststore2), &iter);

 			 }

 			 /* Free results when done */
 			 mysql_free_result(query_results);
 		 }
 	 }
     gtk_widget_show(window_users_list);
     gtk_widget_hide(window_order);

}

void on_users_list_back() {
	gtk_widget_show(window_order);
	gtk_widget_hide(window_users_list);
}
