
# AutoCon

AutoCon is a C-based network mapping tool that:

- Detects the local subnet automatically
- Runs an Nmap scan against the network
- Parses Nmap XML output into a readable report
- Optionally performs `dig` DNS lookups when run with the `dig` argument
- Uploads generated output files to MEGA using Python helper scripts

To have this automatically run when connecting to wifi you have to add the network_dis.sh script to /etc/NetworkManager/dispatcher.d/.

---

## Requirements

### Operating System

This project was developed for:

- Kali Linux
- Debian-based Linux distributions

It may work on other Linux systems, but the install commands below assume `apt`.

---

## System Dependencies

Install the required system packages:

```bash
sudo apt update
sudo apt install -y \
  gcc \
  make \
  pkg-config \
  libxml2-dev \
  nmap \
  dnsutils \
  python3 \
  python3-pip

