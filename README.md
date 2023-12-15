# Simple HTTP Server in C

This is a basic HTTP server implemented in C that handles GET and POST requests. The server listens on port `8080` and serves static files from the `www` directory. It supports serving HTML, PDF, and JPEG files, as well as executing PHP files.

## Features

- **GET Requests:** Handles GET requests for various file types (HTML, PDF, JPEG) from the `www` directory.
- **POST Requests:** Processes form data submitted to `/submit` and displays it back to the user.
- **Dynamic Content:** Executes PHP files and sends their output to the client.

## Usage

### Prerequisites

- C Compiler
- Access to the `www` directory containing web files
- PHP interpreter (if using PHP files)

### Running the Server

1. Compile the code using a C compiler.
2. Ensure the `www` directory contains the web files.
3. Run the compiled binary.

```shell
   $ gcc http_server.c -o http_server
   $ ./http_server
```

The server will start listening on port 8080.

## Directory Structure

```shell
├── http_server.c # Main C code for the HTTP server
└── www
    ├── index.html # Default HTML file
    ├── error.html # Error HTML file
    ├── example.pdf # Example PDF file
    ├── image.jpg # Example JPEG file
    ├── submit.php # Example PHP file for form submission
    └── ... # Other web files
```

## How It Works

- The server creates a socket and binds it to the specified port.
- It accepts incoming connections and handles GET and POST requests accordingly.
- GET requests serve static files by detecting the file type and sending the appropriate content.
- POST requests process form data and display it back to the user.
