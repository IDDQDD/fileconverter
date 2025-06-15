# FileConverter

**FileConverter** is a modular C++ server application for converting files between different formats using a plugin-based architecture.  
The server communicates with clients via the WebSocket protocol, allowing users to upload files and request conversions (e.g., from plain text to PDF).

## Features

- WebSocket server for real-time file conversion requests
- Plugin system: add new converters (as `.so` libraries) without recompiling the server
- Dynamic plugin loading (planned: hot-plug support)
- MIME type detection and flexible conversion logic
- Example plugin: `text2pdf` (converts plain text files to PDF)

## Usage

1. Start the server.
2. Connect with a WebSocket client and send a JSON message specifying the source MIME type and target format.
3. Upload the file to be converted.
4. Receive the converted file as a response.

## Adding Plugins

- Implement the `IConverterFactory` interface in a shared library.
- Place the `.so` file in the `plugins/` directory.
- The server will detect and use the new plugin automatically.

## Project Structure

- `backend/` — server source code
- `plugins/` — converter plugins (shared libraries)
- `config/` — configuration files

## License

This project is licensed under the GNU General Public License v3.0.