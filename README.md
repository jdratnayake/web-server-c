# Simple HTTP Server in C

This project is a lightweight HTTP server written in C that handles GET and POST requests. It listens on port `8080` and serves static files from the `www` directory, including HTML, PDF, and JPEG files. Additionally, it supports executing PHP files for dynamic content generation.

## Features

- **GET Requests:** Handles GET requests for various file types (HTML, PDF, JPEG) from the `www` directory.
- **POST Requests:** Processes form data submitted to `/submit` and displays it back to the user.
- **Dynamic Content:** Executes PHP files and sends their output to the client.

## Prerequisites

- **C Compiler**: GCC or any compatible C compiler (e.g., `gcc` on Linux/macOS, `MinGW` on Windows).
- **`www` Directory**: A directory named `www` with web files (HTML, PDF, JPEG, PHP, etc.).
- **PHP Interpreter**: Required for executing PHP files (e.g., `php` CLI installed and accessible in your PATH).

## Installation

1. Clone the Repository:

```bash
  git https://github.com/jdratnayake/web-server-c
  cd web-server-c
```

2. Prepare the `www` Directory

   - Ensure the `www` directory exists in the project root and contains your web files (e.g., `index.html`, `example.pdf`).

3. Compile the Server:

```bash
  gcc http_server.c -o http_server
```

## Usage

1. Start the Server:

   ```bash
   ./http_server
   ```

   - The server will listen on `http://localhost:8080`.

2. Accessing the Server:

- Open a web browser or use a tool like `curl`:
  - `http://localhost:8080/index.html` (serves the default page).
  - `http://localhost:8080/example.pdf` (serves a PDF file).
  - `http://localhost:8080/submit.php` (executes a PHP script).

3. Testing POST Requests:
   - Submit form data to`/submit` using a POST request:
   ```bash
   curl -X POST http://localhost:8080/submit -d "name=John&age=30"
   ```

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

- **Socket Setup:** Creates a socket, binds it to port 8080, and listens for incoming connections.
- **Request Handling:**
  - GET: Identifies file types in the www directory and serves them with appropriate MIME types.
  - POST: Processes form data sent to /submit and returns it to the client.
- **PHP Integration:** Executes PHP files via the PHP interpreter and streams the output to the client.

## Dependencies

- **C Standard Library:** For socket programming and file operations.
- **PHP CLI:** Optional, required only for serving PHP files.
