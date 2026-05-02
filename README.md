# USBImager CLI

A high-performance, zero-dependency, cross-platform command-line tool designed for automated raw disk image writing. This tool provides a professional-grade headless backend for ISO burning workflows.

## Key Features

- **Headless Architecture**: Designed exclusively for command-line and automated environments, eliminating unnecessary UI overhead.
- **Native Performance**: Leverages low-level OS APIs (Windows Raw Disk I/O) to achieve maximum hardware throughput.
- **Zero-Dependency Design**: Statically linked for maximum portability; requires no external libraries or runtimes.
- **Automation Ready**: Native JSON output for device enumeration allows seamless integration with Python, Node.js, and other backend services.
- **Built-in Privilege Management**: On Windows, the executable includes a manifest that automatically requests Administrator privileges.
- **Advanced Windows Optimization**:
  - Direct disk access using `FILE_FLAG_NO_BUFFERING`.
  - Memory-aligned buffering via `VirtualAlloc` to satisfy strict hardware alignment requirements.
  - Automatic 4KB sector padding for compatibility with Advanced Format (4Kn/512e) drives.
  - Automated volume locking and dismounting to ensure safe and exclusive device access.
- **Robust I/O**: Full 64-bit support for image files exceeding 4GB.

## Usage

### List Available Devices
Returns a structured JSON array of available physical disks.
```bash
usbimager-cli --list
```

### Write ISO to Disk
Writes the specified ISO image to the target physical disk index.
```bash
usbimager-cli --write <iso_file> <disk_id>
```
*Note: Administrative/Root privileges are required. On Windows, a UAC prompt will appear automatically.*

## Compilation

### Prerequisites
- **Windows**: We highly recommend [w64devkit](https://github.com/skeeto/w64devkit/releases) for a zero-installation, portable build environment that includes `gcc`, `make`, and `windres`.
- **Linux/macOS**: Standard `gcc` and `make`.

### Build Commands
Run the following commands from the project **root** directory:
```bash
make          # Build the CLI tool to the build/ directory
make package  # Create a distribution ZIP package with binary and docs
make clean    # Remove all intermediate objects and build artifacts
```

## Project Structure

- `src/`: Core C source files and Windows `manifest.xml`.
- `build/`: Compilation output and intermediate `.o` objects.
- `Makefile`: Unified cross-platform build configuration.
- `src/iso_burner.c`: CLI implementation and automation interface logic.
- `src/disks_win.c`: High-performance Windows disk I/O and device management.
- `src/stream_minimal.h/c`: Efficient raw stream processing engine.

## Credits

This tool is built upon the robust disk I/O foundation provided by the original [USBImager](https://gitlab.com/bztsrc/usbimager) project by **bzt**.

## License

MIT License. See `LICENSE` for details.
