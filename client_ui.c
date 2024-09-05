#include <gtk/gtk.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to send a message and receive a response
void send_message(GtkWidget *widget, gpointer data) {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr;

    // Get the message from the input field
    GtkEntry *entry = GTK_ENTRY(data);
    const gchar *message = gtk_entry_get_text(entry);

    // Get the current time for the timestamp
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[50];
    strftime(time_str, sizeof(time_str), "%H:%M:%S", t);

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        return;
    }

    // Set server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Send message to server
    sendto(sockfd, message, strlen(message), 0, (const struct sockaddr *)&server_addr, sizeof(server_addr));

    // Receive response from server
    int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, NULL);
    buffer[n] = '\0';

    // Get the message log text buffer and append the message with timestamp
    GtkTextBuffer *log_buffer = GTK_TEXT_BUFFER(g_object_get_data(G_OBJECT(widget), "log_buffer"));
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(log_buffer, &end);

    // Append sent message to the log
    gtk_text_buffer_insert_with_tags_by_name(log_buffer, &end, time_str, -1, "time", NULL);
    gtk_text_buffer_insert_with_tags_by_name(log_buffer, &end, " You: ", -1, "you", NULL);
    gtk_text_buffer_insert_with_tags_by_name(log_buffer, &end, message, -1, "message", NULL);
    gtk_text_buffer_insert(log_buffer, &end, "\n", -1);

    // Append received response to the log
    gtk_text_buffer_get_end_iter(log_buffer, &end);
    gtk_text_buffer_insert_with_tags_by_name(log_buffer, &end, time_str, -1, "time", NULL);
    gtk_text_buffer_insert_with_tags_by_name(log_buffer, &end, " Server: ", -1, "server", NULL);
    gtk_text_buffer_insert_with_tags_by_name(log_buffer, &end, buffer, -1, "message", NULL);
    gtk_text_buffer_insert(log_buffer, &end, "\n", -1);

    // Clear the input field
    gtk_entry_set_text(entry, "");

    // Scroll to the bottom of the text view
    GtkTextView *text_view = GTK_TEXT_VIEW(g_object_get_data(G_OBJECT(widget), "text_view"));
    GtkAdjustment *adj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(gtk_widget_get_parent(GTK_WIDGET(text_view))));
    gtk_adjustment_set_value(adj, gtk_adjustment_get_upper(adj) - gtk_adjustment_get_page_size(adj));

    close(sockfd);
}

// Main function to create the UI
int main(int argc, char *argv[]) {
    GtkWidget *window;
    GtkWidget *entry;
    GtkWidget *send_button;
    GtkWidget *header_bar;
    GtkWidget *main_box;
    GtkWidget *scrolled_window;
    GtkWidget *text_view;
    GtkTextBuffer *log_buffer;
    GtkCssProvider *provider;

    gtk_init(&argc, &argv);

    // Load CSS
    provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "window { background-color: #f0f0f0; }"
        "headerbar { background-color: #3498db; color: white; }"
        "button { background-color: #2980b9; color: white; border-radius: 5px; }"
        "button:hover { background-color: #3498db; }"
        "entry { border-radius: 5px; }"
        "textview { font-family: 'Sans'; font-size: 12px; }"
        "textview text { background-color: white; color: #333333; }"  // Changed text color to dark gray
        , -1, NULL);
    gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
                                              GTK_STYLE_PROVIDER(provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    // Create a new window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "UDP Chat Client");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 500);
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

    // Set window icon (make sure icon.png is present in the same directory)
    gtk_window_set_icon_from_file(GTK_WINDOW(window), "icon.png", NULL);

    // Connect the window close event
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a header bar
    header_bar = gtk_header_bar_new();
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(header_bar), TRUE);
    gtk_header_bar_set_title(GTK_HEADER_BAR(header_bar), "UDP Chat Client");
    gtk_header_bar_set_subtitle(GTK_HEADER_BAR(header_bar), "Connected to localhost:8080");
    gtk_window_set_titlebar(GTK_WINDOW(window), header_bar);

    // Create a main box to contain the elements
    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(window), main_box);
    gtk_container_set_border_width(GTK_CONTAINER(main_box), 10);

    // Create a scrolled window and text view for the message log
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_vexpand(scrolled_window, TRUE);
    gtk_box_pack_start(GTK_BOX(main_box), scrolled_window, TRUE, TRUE, 0);

    text_view = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(text_view), FALSE);
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);

    log_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

    // Create text tags for styling
    gtk_text_buffer_create_tag(log_buffer, "time", "foreground", "#888888", NULL);
    gtk_text_buffer_create_tag(log_buffer, "you", "weight", PANGO_WEIGHT_BOLD, "foreground", "#2980b9", NULL);
    gtk_text_buffer_create_tag(log_buffer, "server", "weight", PANGO_WEIGHT_BOLD, "foreground", "#27ae60", NULL);
    gtk_text_buffer_create_tag(log_buffer, "message", "foreground", "#333333", NULL);

    // Create an entry widget for message input
    entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry), "Type your message here...");
    gtk_box_pack_start(GTK_BOX(main_box), entry, FALSE, FALSE, 0);

    // Create a button to send the message
    send_button = gtk_button_new_with_label("Send");
    gtk_widget_set_can_default(send_button, TRUE);
    gtk_widget_set_margin_top(send_button, 5);
    gtk_box_pack_start(GTK_BOX(main_box), send_button, FALSE, FALSE, 0);

    // Store references to the message log buffer and text view
    g_object_set_data(G_OBJECT(send_button), "log_buffer", log_buffer);
    g_object_set_data(G_OBJECT(send_button), "text_view", text_view);

    // Connect the send button to the send_message function
    g_signal_connect(send_button, "clicked", G_CALLBACK(send_message), entry);

    // Connect the entry "activate" signal to send_message function (for Enter key)
    g_signal_connect(entry, "activate", G_CALLBACK(send_message), entry);

    // Show all widgets
    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}