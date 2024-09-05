# UDP Client-Server Chat Application

This project is a simple UDP-based client-server chat application featuring a graphical user interface (GUI) built with GTK+ 3.0. The client allows users to send and receive messages from the server in real-time, simulating a chat-like experience.

## Features

- **Client-Server Architecture**: Communicates over the UDP protocol.
- **Graphical User Interface**: The client includes a message input field, send button, and a scrollable chat log.
- **Real-Time Messaging**: Messages are displayed in real time with timestamps for an interactive experience.
- **Custom Icon Support**: The client window can display a custom icon (`icon.png`).
- **Responsive UI**: The interface is designed to be responsive and user-friendly.

## Screenshots

Here are some screenshots showcasing the chat application:

- Chat Interface
- Server View

(Note: Add actual screenshots here)

## Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/0504ankitsharma/UDP-Client-Server-Chat-Application
   cd UDP-Client-Server-Chat-Application
   ```

2. Compile the code:

   For the client:
   ```bash
   gcc -o client_ui client_ui.c `pkg-config --cflags --libs gtk+-3.0` -Wall
   ```

   For the server:
   ```bash
   gcc -o server server.c -Wall
   ```

## Usage

1. Run the server:
   ```bash
   ./server
   ```

2. Run the client:
   ```bash
   ./client_ui
   ```

After running both the client and server, you can send messages via the client interface, and they will be displayed in real-time along with the server's response.

Ensure that the `icon.png` file is in the same directory as the `client_ui` binary for the custom icon to appear.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## Support

If you encounter any problems or have any questions, please open an issue in the GitHub repository.
