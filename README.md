
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
- [/] Actually sending the files  <-- Im Here
- [ ] Reliability layer (a.k.a "please don’t drop my packets")  
- [ ] Documentation that actually explains things (this README doesn’t count)  

---

## 🧩 How it's built

- **C (C99), GCC, Make**, Linux sockets: the transport is plain **TCP over IPv4**, through my [`mysocket`](https://github.com/Gedankenn/mysocket) wrapper (git submodule in `modules/mysocket`).
- `application/server.c` / `application/client.c`: server sends a directory tree, client receives it (default buffer: 4095 bytes).
- `application/file_transfer_protocol.c`: packet framing for the custom protocol.
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
./output/app -c <host> <port>     # client mode: connect and receive
./output/app -f <dir>             # list files in a directory
```

> ⚠️ Heads-up: the current `main` is mid-refactor and does **not** compile yet. `application/` still references file-handler functions removed in the last refactor.
