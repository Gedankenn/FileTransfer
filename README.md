
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

## 🚀 Getting Started

For the brave souls who want to test it:

```bash
git clone https://github.com/Gedankenn/FileTransfer.git
cd FileTransfer
make help
