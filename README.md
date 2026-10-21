
# 🚧 File Transfer Protocol Over Ethernet (Work in Progress) 🚧

Welcome to the **brave attempt** of reinventing the wheel —  
a custom **File Transfer Protocol over Ethernet** written in C.  

Yes, we *know* FTP, TFTP, HTTP, NFS, SMB, scp, rsync… already exist.  
But where’s the fun in that? 🤷‍♂️

---

## ✨ What’s this?

This project is an **experimental protocol** for transferring files directly over Ethernet.  
It’s raw, it’s fresh, and it’s probably broken in more ways than I’d like to admit.  
The idea is to build a minimal and fast way of sending binary files across machines,  
without all the legacy baggage.

---

## 🛠️ Current Status

- [x] Basic packet sending and receiving  
- [x] File serialization and binary reads  
- [x] Actually sending the files (single file or a whole directory tree, verified with sha256)
- [ ] Reliability layer (a.k.a "please don’t drop my packets")  
- [ ] Documentation that actually explains things (this README doesn’t count)  

---

## 🧩 How it's built

- **C (C99), GCC, Make**, Linux sockets: the transport is plain **TCP over IPv4**, through my [`mysocket`](https://github.com/Gedankenn/mysocket) wrapper (git submodule in `modules/mysocket`).
- `application/server.c` / `application/client.c`: the server sends a file or a directory tree to one client and exits; the client writes it into a destination directory.
- `application/file_transfer_protocol.c`: the wire format. A header (`FTP1`, entry count, total size) is followed by one entry per directory/file (type, relative path, size) and, for files, the content streamed in 64 KiB chunks. Integers are big-endian; paths with `..` or a leading `/` are rejected by the client.
- `modules/file_handler.c`: file/directory metadata and binary reads.
- `modules/utils.c`: helpers, including finding the machine's IP (uses libcurl).
- `modules/test/test.c`: test program (`make test` / `make runtest`).

## 🚀 Getting Started

For the brave souls who want to test it (requires `gcc`, `make` and the libcurl development package):

```bash
git clone --recurse-submodules https://github.com/Gedankenn/FileTransfer.git
cd FileTransfer
make help      # list targets (release, debug, test, runtest, testapp, testfile, clean)
make           # build output/app
```

Usage, from `main.c`:

```bash
./output/app -s <port> <path>     # server mode: serve <path>
./output/app -c <host> <port> [dest]  # client mode: receive into dest (default: current dir)
./output/app -f <dir>             # list files in a directory
```

Example on one machine:

```bash
./output/app -s 1337 ~/Documents/report.pdf      # or a directory
./output/app -c 127.0.0.1 1337 /tmp/received     # in another terminal
```

The server serves one client per run. There is no encryption or authentication yet; use it only on a trusted network.
